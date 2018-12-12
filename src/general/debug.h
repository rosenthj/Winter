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
 * debug.h
 *
 *  Created on: Nov 1, 2016
 *      Author: Jonathan Rosenthal
 */

#ifndef GENERAL_DEBUG_H_
#define GENERAL_DEBUG_H_

#include <string>
#include <iostream>
#include <cstdlib>

namespace debug {

template<typename... Args>
inline void PrintFunction(const std::string function_name, Args... args);

}

namespace debug_internal {

void AddFunction(const std::string function_name);
int GetFunctionLevel();

void Print(const std::string message, const std::string tag = "");

template<typename T>
void PrintFunctionHelper(T arg) {
  std::cout << arg << ")" << std::endl;
}

template<typename T, typename... Args>
void PrintFunctionHelper(T arg, Args... args) {
  std::cout << arg << ", ";
  PrintFunctionHelper(args...);
}

template<typename... Args>
void EnterFunction(std::string function_name, Args... args) {
  std::cout << std::string(GetFunctionLevel(), '-') << "->";
  AddFunction(function_name);
  debug::PrintFunction(function_name, args...);
}

void ExitFunction();

}

namespace debug {

const bool kDebug = true;
#ifdef NDEBUG
const bool kAssertions = false;
#else
const bool kAssertions = true;
#endif

const bool kMain = false;
const bool kUci = false;

//Prints a command line string if condition is met. By default there
//is an incrementing counter used to tag messages.
inline void Print(const std::string message, const bool condition = true,
    const std::string tag = "") {
  if (kDebug && condition) {
    debug_internal::Print(message, tag);
  }
}

template<typename... Args>
inline void PrintFunction(const std::string function_name, Args... args) {
  if (kDebug) {
    std::cout << function_name << "(";
    debug_internal::PrintFunctionHelper(args...);
  }
}

template<typename... Args>
inline void EnterFunction(const bool active, const std::string function_name, Args... args) {
  if (active && kDebug) {
    debug_internal::EnterFunction(function_name, args...);
  }
}

inline void ExitFunction(const bool active = true) {
  if (active && kDebug) {
    debug_internal::ExitFunction();
  }
}

void Error(const std::string message, bool exit=true);

inline void Assertion(const bool condition, const std::string msg = "No message specified!") {
  if (kAssertions) {
    if (!condition) {
      std::cout << "Assertion Failure! " << msg  << std::endl;
      std::exit(1);
    }
  }
}

}


#endif
