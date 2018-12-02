/*
 * search_thread.cc
 *
 *  Created on: Nov 3, 2018
 *      Author: jonathan
 */

#include "search_thread.h"
#include "general/types.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <mutex>

namespace {
std::mt19937_64 rng;
}

namespace search {

ThreadPool Threads;

Thread::Thread() : stl_thread(&Thread::idle_loop, this) {
  id = 1;//This should be immediately set to something else. It is set here only to guarantee non-zero for helpers.
  clear_killers_and_counter_moves();
}

Thread::~Thread() {
  assert(!search_flag);
  exit_flag = true;
  condition_variable.notify_one();
  stl_thread.join();
}

void Thread::perturb_root_moves() {
  for (int i = moves.size()-1; i > 0; i--) {
    if (rng() % 2) {
      std::swap(moves[i-1], moves[i]);
    }
  }
}

void Thread::start_searching() {
  std::lock_guard<std::mutex> lock(mutex);
  search_flag = true;
  condition_variable.notify_one();
}

// Wait until we know the search is over or we need to exit.
void Thread::wait_actions_over() {
  std::unique_lock<std::mutex> lock(mutex);
  condition_variable.wait(lock, [&]{ return !search_flag || exit_flag; });
}

void Thread::idle_loop() {
  while (!exit_flag) {

    if (search_flag) {
      // The main search function is in search.cc
      search();
    }

    // Idle loop until search or exit flags are triggered
    std::unique_lock<std::mutex> lock(mutex);
    search_flag = false;
    condition_variable.notify_one();
    condition_variable.wait(lock, [&]{ return search_flag || exit_flag; });
    lock.unlock();//We need to release the lock to start searching
  }

  //ensure nobody is waiting on us
  condition_variable.notify_one();
}


ThreadPool::ThreadPool() {
  main_thread = new Thread();
  main_thread->id = 0;
}


void ThreadPool::set_num_threads(size_t num_threads) {
  assert(num_threads > 0);
  size_t num_helpers = num_threads - 1;

  //Create new threads if we have too few
  while(helpers.size() < num_helpers) {
    helpers.push_back(new Thread());
    helpers.back()->id = helpers.size();
  }

  //Kill helper threads if we have too many
  while(helpers.size() > num_helpers) {
    delete helpers.back();
    helpers.pop_back();
  }
}

}
