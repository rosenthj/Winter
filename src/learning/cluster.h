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
 * clustering.h
 *
 *  Created on: Nov 13, 2018
 *      Author: Jonathan Rosenthal
 */

#ifndef SRC_LEARNING_CLUSTER_H_
#define SRC_LEARNING_CLUSTER_H_

#include "../general/types.h"
#include "linear_algebra.h"
#include "machine_learning.h"
#include "../board.h"
#include <array>

namespace cluster {

Vec<double, kPhaseVecLength> GetBoardPhaseVec(const Board &board);


template <size_t k>
struct ClusterModel {
  virtual ~ClusterModel() {};
  virtual Vec<double, k> GetWeightedProbabilities(const Board &board) const {
    return Vec<double, k>(1.0 / k);
  }
  virtual void LoadFromParams(const std::vector<double> &params) { }
  virtual void SaveHardCode(std::string file_name) const { }
  virtual void SetModel(ClusterModel<k>* other) { }
};

template<size_t length>
struct Gaussian {
  double pdf(const Vec<double, length> &sample) const {
    //double prob = 1;
    float exponent = 0;
    Vec<double, length> dif = sample - mu;
    exponent = -((sigma_inv.dot(dif)).dot(dif)) / 2;
    return std::exp(exponent) / sqrt_det_sigma_times_divisor;
  }

  static double divisor() {
    return std::pow(std::sqrt(2 * std::acos(-1)), length);
  }
  //Run this between changing sigma and calling pdf.
  void set_sigma_inv() {
    sigma_inv = SymmetricSquareMatrixInv(sigma);
    sqrt_det_sigma_times_divisor = std::sqrt(divisor() * Determinant(sigma));
  }
  Vec<double, length> mu;

  SqMatrix<double, length> sigma;
  SqMatrix<double, length> sigma_inv;
  double sqrt_det_sigma_times_divisor;
};

// Normalized Fuzzy C-Means
template<size_t k, size_t length>
struct NormFuzzyCMeans : ClusterModel<k> {
  Vec<double, k> GetWeightedProbabilities(const Board &board) const override;
  void LoadFromParams(const std::vector<double> &params) override;
  void SaveHardCode(std::string file_name) const override;
  void SetModel(ClusterModel<k>* other) override;

  std::array<Vec<double, length>, k> centroids;
  ml::Normalizer<length> normalizer;
};

// The classic tapered model found in all state of the art non NN based engines.
struct TaperedModel : ClusterModel<2> {
// TODO implement
};

}

#endif /* SRC_LEARNING_CLUSTER_H_ */
