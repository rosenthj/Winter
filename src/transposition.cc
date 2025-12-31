/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2018 Jonathan Rosenthal
 *
 *  Winter is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Winter is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * transposition.cc
 *
 *  Created on: Dec 10, 2016
 *      Author: Jonathan Rosenthal
 */

#include "transposition.h"
#include "net_evaluation.h"
#include <array>
#include <cassert>
#include <cstring>
#include <cstdlib>            // for aligned_alloc, free
#include <optional>
#include <vector>
#include <thread>

#if defined(__linux__)
  #include <sys/mman.h>       // for madvise/MADV_HUGEPAGE
#endif

namespace {

Score score_to_tt_score(const Score score, const int32_t num_made_moves) {
  if (score.is_mate_score()) {
    if (score.is_disadvantage()) {
      return WDLScore{0, score.loss + num_made_moves};
    }
    return WDLScore{score.win + num_made_moves, 0};
  }
  return score;
}

Score tt_score_to_score(Score score, int32_t num_made_moves) {
  if (score.is_mate_score()) {
    if (score.is_disadvantage()) {
      return WDLScore{0, score.loss - num_made_moves};
    }
    return WDLScore{score.win - num_made_moves, 0};
  }
  return score;
}

}

namespace table {

struct alignas(64) EntryBucket : public std::array<Entry, 4>{};

// In addition to the main table, a subtable is used for improved PV entry redundancy.
// PV entries are stored in both and TT size is sum of size of main table and PV table.

namespace {
  
EntryBucket* _table = nullptr;
size_t _table_size = 0;
uint8_t current_generation = 0;

void* AlignedAlloc(size_t alignment, size_t size) {
#if defined(_WIN32)
    return _aligned_malloc(size, alignment);
#else
    return std::aligned_alloc(alignment, size);
#endif
}

void AlignedFree(void* ptr) {
#if defined(_WIN32)
    _aligned_free(ptr);
#else
    std::free(ptr);
#endif
}

}

void UpdateGeneration() {
  current_generation += (0x1 << 2);
}

void SetTableSize(const int32_t MB_total_int) {
  const size_t MB_total = static_cast<size_t>(MB_total_int);
  const size_t bytes = MB_total << 20;
  
  // Calculate new size (bucket count)
  size_t new_byte_size = bytes - (bytes % sizeof(EntryBucket));
  size_t new_bucket_count = new_byte_size / sizeof(EntryBucket);

  // Only reallocate if size actually changed
  if (new_bucket_count == _table_size && _table != nullptr) {
    return;
  }

  // Free old table
  if (_table) {
    AlignedFree(_table);
    _table = nullptr;
  }

  _table_size = new_bucket_count;
  
  size_t alignment = 2 * 1024 * 1024; 
    
  // Fallback to 4096 if size is small
  if (new_byte_size < alignment) alignment = 4096;

  _table = static_cast<EntryBucket*>(AlignedAlloc(alignment, new_byte_size));
  
  if (!_table) {
    std::cerr << "Failed to allocate Hash Table!" << std::endl;
    exit(EXIT_FAILURE);
  }
  
#ifdef MADV_HUGEPAGE
  madvise(_table, new_byte_size, MADV_HUGEPAGE);
#endif

  ClearTable();
}

size_t HashFunction(const HashType hash) {
  assert(_table_size != 0);
  return hash % _table_size;
}

bool ValidateHash(const Entry &entry, const HashType hash){
  HashType best_move_cast = entry.get_best_move();
  return entry.hash == (hash ^ best_move_cast);
}

std::optional<Entry> GetMainEntryIdx(const HashType hash, const size_t bucket) {
  for (size_t i = 0; i < 3; ++i) {
    if (ValidateHash(_table[bucket][i], hash)) {
      return _table[bucket][i];
    }
  }
  return {};
}

std::optional<Entry> GetEntry(const HashType hash) {
  size_t idx = HashFunction(hash);
  std::optional<Entry> entry = GetMainEntryIdx(hash, idx);
  Entry entry_pv = _table[idx][3];

  if (!ValidateHash(entry_pv, hash)) {
    return entry;
  }
  if (!entry.has_value()) {
    return entry_pv;
  }
  if (entry->depth > entry_pv.depth) {
    return entry;
  }
  return entry_pv;
}

size_t GetIdxToReplace(const HashType hash, size_t idx) {
  if (ValidateHash(_table[idx][0], hash)) {
    return 0;
  }
  int worst_score = 1024 + static_cast<int>(_table[idx][0].depth)
      - 512 * (_table[idx][0].get_generation() != current_generation);
  size_t worst_idx = 0;

  for (size_t i = 1; i <= 2; ++i) {
    if (ValidateHash(_table[idx][i], hash)) {
      return i;
    }
    int score = 1024 + static_cast<int>(_table[idx][i].depth)
          - 512 * (_table[idx][i].get_generation() != current_generation);
    if (score < worst_score) {
      worst_score = score;
      worst_idx = i;
    }
  }
  return worst_idx;
}

void SaveEntry(const Board &board, const Move best_move, const Score score,
               const Depth depth, const uint8_t bound) {
  HashType hash = board.get_hash();
  size_t idx = HashFunction(hash);
  size_t index = GetIdxToReplace(hash, idx);

  assert(idx < _table_size);
  assert(index < 3);
  assert(score.is_valid());

  HashType best_move_cast = best_move;
  hash ^= best_move_cast;
  
  if (_table[idx][index].hash != hash || _table[idx][index].depth <= depth + 2) {
    Entry entry;
    entry.hash = hash;
    entry.set_score(score, board);
    entry.set_best_move(best_move);
    entry.set_gen_and_bound(bound);
    assert(entry.get_generation() == current_generation);
    entry.depth = depth;
    _table[idx][index] = entry;
  }
}

void SavePVEntry(const Board &board, const Move best_move, const Score score, const Depth depth) {
  HashType hash = board.get_hash();
  size_t idx = HashFunction(hash);
  size_t index = GetIdxToReplace(hash, idx); // HashFunction(hash);
  size_t index_pv = 3;

  assert(index < _table_size);

  HashType best_move_cast = best_move;
  Entry entry;
  entry.hash = hash ^ best_move_cast;
  entry.set_score(score, board);
  entry.set_best_move(best_move);
  entry.depth = depth;
  entry.set_gen_and_bound(kExactBound);
  _table[idx][index] = entry;
  _table[idx][index_pv] = entry;
}

void ClearTable() {
  current_generation = 0;
  
  // Figure out how many threads to use
  constexpr size_t kMinMBPerThread = 32;
  const size_t total_bytes = _table_size * sizeof(EntryBucket);
  const size_t total_mb = total_bytes >> 20;
  size_t max_threads_by_mem = std::max<size_t>(1, total_mb / kMinMBPerThread);
  
  size_t thread_count = std::min(search::GetNumThreads(), max_threads_by_mem);
  
  // Use main thread in case we only want one thread.
  if (thread_count == 1) {
    std::memset(_table, 0, _table_size * sizeof(EntryBucket));
    return;
  }
  
  // Multithreaded initialization
  std::vector<std::thread> threads;
  threads.reserve(thread_count);
  
  const size_t total_buckets = _table_size;
  const size_t chunk_size = total_buckets / thread_count;

  for (size_t i = 0; i < thread_count; ++i) {
    threads.emplace_back([i, thread_count, chunk_size, total_buckets]() {
      const size_t start = i * chunk_size;
      // Ensure the last thread picks up any remainder from integer division
      const size_t end = (i == thread_count - 1) ? total_buckets : start + chunk_size;

      if (start < end) {
        std::memset(&_table[start], 0, (end - start) * sizeof(EntryBucket));
      }
    });
  }

  for (auto& t : threads) {
    t.join();
  }
}

void Entry::set_score(const Score score_new, const Board &board) {
  Score score = score_to_tt_score(score_new, board.get_num_made_moves());
  win = score.win;
  loss = score.loss;
}

Score Entry::get_score(const Board &board) const {
  Score score = WDLScore{win, loss};
  return tt_score_to_score(score, board.get_num_made_moves());
}

void Entry::set_gen() {
  gen_and_bound = current_generation | get_bound();
}

void Entry::set_gen_and_bound(uint8_t bound) {
  gen_and_bound = current_generation | bound;
}

size_t GetHashfull() {
  size_t result = 0;
  for (size_t i = 0; i < 1000; ++i) {
    result += (_table[i][0].get_generation() == current_generation);
  }
  return result;
}

}

