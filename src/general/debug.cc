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
 * debug.cc
 *
 *  Created on: Nov 1, 2016
 *      Author: Jonathan Rosenthal
 */

#include "debug.h"
#include <vector>

namespace {

int counter = 0;
std::vector<std::string> entered_functions;

}

namespace debug_internal {

void AddFunction(std::string function_name) {
  entered_functions.emplace_back(function_name);
}

int GetFunctionLevel() {
  return entered_functions.size();
}


void Print(const std::string message, const std::string tag) {
  if (tag.compare("") == 0) {
    std::cout << "DEBUG " << (counter++) << ": " << message << std::endl;
  }
  else {
    std::cout << "DEBUG " << tag << ": " << message << std::endl;
  }
}

void ExitFunction() {
  std::string function_name = entered_functions.back();
  entered_functions.pop_back();
  std::cout << "<-" << std::string(entered_functions.size(), '-') << function_name << "(...)" << std::endl;
}

}

namespace debug {

void Error(const std::string message, bool exit) {
  std::cout << "ERROR: " << message << std::endl;
  if (exit) {
    std::exit(1);
  }
}

}


