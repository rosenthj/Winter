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
 *      Author: Jonathan
 */

#include "bookkeeping.h"
#include <vector>
#include <iostream>

namespace {

std::vector<int> NWD1NodeRequirements(128, 0);
int max_node_requirement = 0;

}

namespace bookkeeping {

void IncrementNWD1NodesToFailHigh(int num_searched_nodes) {
  max_node_requirement = std::max(num_searched_nodes, max_node_requirement);
  NWD1NodeRequirements[num_searched_nodes] += 1;
}

void PrintNWD1NodeRequirements() {
  double total = 0, avg = 0;
  for (int i = 0; i <= max_node_requirement; i++) {
    total += NWD1NodeRequirements[i];
    avg += i * NWD1NodeRequirements[i];
  }
  for (int i = 0; i <= max_node_requirement; i++) {
    std::cout << i << ":" << (NWD1NodeRequirements[i] / total) << " ";
  }
  std::cout << "\nAverage wasted nodes:" << (avg / total) << std::endl;
}

void ResetNWD1NodeRequirements() {
  for (int i = 0; i <= max_node_requirement; i++) {
    NWD1NodeRequirements[i] = 0;
  }
  max_node_requirement = 0;
}

}
