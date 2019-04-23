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
 * bookkeeping.cc
 *
 *  Created on: May 23, 2017
 *      Author: Jonathan Rosenthal
 */

#include "bookkeeping.h"
#include "../board.h"
#include <vector>
#include <iostream>

namespace {

const int kPV = 0;
const int kNW = 1;

template<int num>
struct Counter {
  std::vector<Counter<num-1> > vec;
  Counter<num-1>& operator[](std::size_t idx) {
    while(vec.size() <= idx) {
      vec.emplace_back();
    }
    return vec[idx];
  }
  const size_t size() {
    return vec.size();
  }
  void clear() {
    for (Counter<num-1> counter: vec) {
      counter.clear();
    }
    vec.clear();
  }
  double sum() {
    double sum = 0;
    for (Counter<num-1> counter: vec) {
      sum += counter.sum();
    }
    return sum;
  }
};

template<>
struct Counter<1> {
  std::vector<double> vec;
  double& operator[](std::size_t idx) {
    while(vec.size() <= idx) {
      vec.emplace_back();
    }
    return vec[idx];
  }
  const size_t size() {
    return vec.size();
  }
  void clear() {
    vec.clear();
  }
  double sum() {
    double sum = 0;
    for (double counter: vec) {
      sum += counter;
    }
    return sum;
  }
};

template<bool relative>
void PrintCounter(Counter<2> counter, std::ostream &out = std::cout) {
  for (size_t i = 0; i < counter.size(); i++) {
    out << "Counter " << i << ": ";
    double sum = 1;
    if (relative) {
      sum = counter[i].sum();
    }
    if (sum > 0) {
      for (size_t j = 0; j < counter[i].size(); j++) {
        if (j != 0) {
          out << ", ";
        }
        out << j << ":" << (counter[i][j] / sum);
      }
    }
    out << std::endl;
  }
}

void PrintCounterBinary(Counter<2> counter, std::ostream &out = std::cout) {
  for (size_t i = 0; i < std::min(counter.size(), (size_t) 16); i++) {
    out << "Counter " << i << ": ";

    for (size_t j = 0; j < counter[i].size(); j += 2) {
      if (j != 0) {
        out << ", ";
      }
      double sum = counter[i][j] + counter[i][j+1];
      out << (j/2) << ":" << (counter[i][j] / sum);
    }
    out << std::endl;
  }
}

}

namespace bookkeeping {
Counter<2> counters;
Counter<2> improve_alpha_counter;
Counter<4> fh_nw_counter;
Counter<2> current_counter;

void inc_counter(int counter, int index) {
  counters[counter][index]++;
}

/*void inc_counter_old(int counter, int index) {
  while (counters.size() <= counter) {
    counters.emplace_back();
  }
  while (counters[counter].size() <= index) {
    counters[counter].emplace_back(0);
  }
  counters[counter][index]++;
}*/

template<bool relative>
void print_counters() {
  std::cout << "Generic Counters" << std::endl;
  PrintCounter<relative>(counters);
  std::cout << "\nPV Alpha Improvements by Tree Depth and Move Number" << std::endl;
  PrintCounterBinary(improve_alpha_counter);
  std::cout << "\nNW Fail High Expected High Without TT Entry by Tree Depth and Move Number" << std::endl;
  PrintCounterBinary(fh_nw_counter[0][1]);
  std::cout << "\nNW Fail High Expected Low Without TT Entry by Tree Depth and Move Number" << std::endl;
  PrintCounterBinary(fh_nw_counter[0][0]);
  std::cout << "\nNW Fail High Expected High With TT Entry by Tree Depth and Move Number" << std::endl;
  PrintCounterBinary(fh_nw_counter[1][1]);
  std::cout << "\nNW Fail High Expected Low With TT Entry by Tree Depth and Move Number" << std::endl;
  PrintCounterBinary(fh_nw_counter[1][0]);
  std::cout << "\nCurrent LMR Conditions Counter" << std::endl;
  PrintCounterBinary(current_counter);
}

void print_counters() {
  print_counters<false>();
}

void print_relative_counters() {
  print_counters<true>();
}

void reset_counters() {
  counters.clear();
  improve_alpha_counter.clear();
  fh_nw_counter.clear();
}

void log_info(const Board &board, const InfoContainer &info) {
  if (info.trigger == Trigger::kFailHigh) {
    if (info.NodeType == kPV) {
      improve_alpha_counter[info.depth][info.move_number * 2]++;
    }
    else if (info.tt_entry != kNullMove) {
      fh_nw_counter[1][info.expected_node][info.depth][info.move_number * 2]++;
      current_counter[info.depth][info.move_number * 2]++;
    }
    else {
      fh_nw_counter[0][info.expected_node][info.depth][info.move_number * 2]++;
      current_counter[info.depth][info.move_number * 2]++;
    }
  }

  if (info.trigger == Trigger::kImproveAlpha) {
    assert(info.NodeType == kPV);
    improve_alpha_counter[info.depth][info.move_number * 2]++;
  }

  if (info.trigger == Trigger::kLessEqualAlpha) {
    if (info.NodeType == kPV) {
      improve_alpha_counter[info.depth][info.move_number * 2 + 1]++;
    }
    else {
      fh_nw_counter[info.tt_entry != kNullMove][info.expected_node][info.depth][info.move_number * 2 + 1]++;
      current_counter[info.depth][info.move_number * 2 + 1]++;
    }
  }

  if (info.trigger == Trigger::kReturnAlpha && info.NodeType == kNW && info.expected_node < 2) {

  }
}

}
