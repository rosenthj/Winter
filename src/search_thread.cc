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
  //std::cout << "Deleting thread!" << std::endl;
  exit_flag = true;
  condition_variable.notify_one();
  stl_thread.join();
  //std::cout << "Thread deleted!" << std::endl;
}

void Thread::perturb_root_moves() {
  for (int i = 1; i < moves.size(); i++) {
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

void Thread::wait_actions_over() {
  std::unique_lock<std::mutex> lock(mutex);
  condition_variable.wait(lock, [&]{ return !search_flag || exit_flag; });
}

void Thread::idle_loop() {
  while (!exit_flag) {

    if (search_flag) {
      search();
    }

    std::unique_lock<std::mutex> lock(mutex);
    search_flag = false;
    //std::cout << "Idle loop lock aquired!" << std::endl;

    condition_variable.notify_one();

    //std::cout << "CV waiting!" << std::endl;
    condition_variable.wait(lock, [&]{ return search_flag || exit_flag; });

    lock.unlock();//We need to release the lock to start searching
    //std::cout << "Idle loop lock released!" << std::endl;
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
