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
 * cluster.cc
 *
 *  Created on: Nov 14, 2018
 *      Author: Jonathan Rosenthal
 */

#include "cluster.h"
#include "linear_algebra.h"
#include "../board.h"
#include "../general/settings.h"
#include "../general/types.h"
#include "../general/bit_operations.h"

#include <cmath>
#include <vector>
#include <fstream>

namespace cluster {

Vec<double, kPhaseVecLength> GetBoardPhaseVec(const Board &board) {
  Vec<double,kPhaseVecLength> vec(0);
//  if (settings::kExperimental) {
  for (PieceType pt = kKnight; pt <= kQueen; pt++) {
    vec[pt - 1] += board.get_piecetype_count(pt);
  }

  return vec;
}

template<size_t k, size_t length>
Vec<double, k> NormFuzzyCMeans<k, length>::GetWeightedProbabilities(const Board &board) const {
  const Vec<double,kPhaseVecLength> sample = normalizer.normalize(GetBoardPhaseVec(board));
  const double kEpsilon = 0.00001;
  Vec<double, k> distances;
  for (size_t i = 0; i < k; i++) {
    Vec<double, length> diff =  centroids[i] - sample;
    distances[i] = diff.normL2() + kEpsilon;
  }

  // Calculate distance power (2 / (m-1))
  // TODO Parametrize m in NFCM template class
  // distances = distances * distances;

  Vec<double, k> weighted_probabilities(0);
  for (size_t i = 0; i < k; i++) {
    for (size_t j = 0; j < k; j++) {
      weighted_probabilities[i] += distances[i] / distances[j];
    }
    weighted_probabilities[i] = 1 / weighted_probabilities[i];
  }
  return weighted_probabilities;
}

template<size_t k, size_t length>
void NormFuzzyCMeans<k, length>::LoadFromParams(const std::vector<double> &params) {
  int c = 0;
  for (size_t i = 0; i < length; i++) {
    normalizer.means[i] = params[c++];
    normalizer.std_dev[i] = params[c++];
  }
  for (size_t i = 0; i < k; i ++) {
    for (size_t j = 0; j < length; j++) {
      centroids[i][j] = params[c++];
    }
  }
}

template<size_t k, size_t length>
void NormFuzzyCMeans<k, length>::SaveHardCode(std::string file_name) const {
  std::ofstream file(file_name);
  file << "const std::array<double, (" << (2*length) << "+ (" << k << "*" << length  << ")> NFCM_params = {" << std::endl;
  file << "    // Means and Standard Deviations" << std::endl;
  for (size_t i = 0; i < length; i++) {
    file << "    " << normalizer.means[i] << ", " << normalizer.std_dev[i] << "," << std::endl;
  }
  file << "    // Centroids" << std::endl;
  for (size_t i = 0; i < k; i ++) {
    file << "    ";
    for (size_t j = 0; j < length; j++) {
      file << centroids[i][j];
      if (j+1 < length || i + 1 < k) {
        file << ", ";
      }
    }
    file << std::endl;
  }
  file << "};" << std::endl;
  file.flush();
  file.close();
}

template<size_t k, size_t length>
void NormFuzzyCMeans<k, length>::SetModel(ClusterModel<k>* other) {
  cluster::NormFuzzyCMeans<k, kPhaseVecLength>* nfcm_p =
      static_cast<cluster::NormFuzzyCMeans<k, kPhaseVecLength>* >(other);
  centroids = nfcm_p->centroids;
  normalizer = nfcm_p->normalizer;
}

template struct NormFuzzyCMeans<settings::kNumClusters, kPhaseVecLength>;

}
