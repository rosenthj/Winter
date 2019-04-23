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
 * machine_learning.h
 *
 *  Created on: Mar 8, 2018
 *      Author: Jonathan Rosenthal
 */

#ifndef SRC_LEARNING_MACHINE_LEARNING_H_
#define SRC_LEARNING_MACHINE_LEARNING_H_

#include "linear_algebra.h"
#include <vector>

namespace ml {

std::vector<double> Wrap(const double value);

enum ActivatedLoss {
  kSigmoidCrossEntropy
};

template<size_t len>
struct Normalizer {
  Vec<double, len> means;
  Vec<double, len> std_dev;
  Vec<double, len> normalize(const Vec<double, len> &sample) const {
    return (sample - means) / std_dev;
  }
  Vec<double, len> unnormalize(const Vec<double, len> &sample) const {
    return (sample * std_dev) + means;
  }
};

template<ActivatedLoss loss>
struct Regressor {
  std::vector<double> weights;
  std::vector<double> gradient(const std::vector<double> &features, const std::vector<double> &targets);
};

template<ActivatedLoss loss>
struct Optimizer {
  double nu;
  size_t counter = 0;
  Regressor<loss> regressor;
  virtual void set_num_features(size_t num_features) {
    regressor.weights = std::vector<double>(num_features, 0);
  };
  virtual void step(const std::vector<double> &features, std::vector<double> target) {};
  virtual ~Optimizer() {};
};

template<ActivatedLoss loss>
struct SGD : public Optimizer<loss> {
  void step(const std::vector<double> &features, std::vector<double> target) override;
};

template<ActivatedLoss loss>
struct Adam : public Optimizer<loss> {
  double beta1 = 0.99;
  double beta2 = 0.999;
  double epsilon = 0.00000001;
  std::vector<double> expected_gradient;
  std::vector<double> expected_squared_gradient;
  void step(const std::vector<double> &features, std::vector<double> target) override;
  void set_num_features(size_t num_features) override {
    this->regressor.weights = std::vector<double>(num_features, 0);
    expected_gradient = std::vector<double>(num_features, 0);
    expected_squared_gradient = std::vector<double>(num_features, 0);
  };
};

}

#endif /* SRC_LEARNING_MACHINE_LEARNING_H_ */
