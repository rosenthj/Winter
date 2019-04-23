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
 * machine_learning.cc
 *
 *  Created on: Mar 8, 2018
 *      Author: Jonathan Rosenthal
 */

#include <cmath>
#include <vector>
#include <iostream>
#include "machine_learning.h"

namespace {

double Sigmoid(double x) {
  return 1 / (1 + std::exp(-x));
}

}

namespace ml {

std::vector<double> Wrap(const double value) {
  std::vector<double> container;
  container.emplace_back(value);
  return container;
}

template<ActivatedLoss loss>
std::vector<double> Regressor<loss>::gradient(
    const std::vector<double> &features, const std::vector<double> &targets) {
  std::vector<double> gradients;
  gradients.reserve(weights.size());
  double sum = 0;
  for (size_t i = 0; i < weights.size(); i++) {
    sum += weights[i] * features[i];
  }
  double grad = 0;
  if (loss == kSigmoidCrossEntropy) {
    grad = Sigmoid(sum) - targets[0];
  }
  for (size_t i = 0; i < weights.size(); i++) {
    gradients.emplace_back(grad * features[i]);
  }
  return gradients;
}

template struct Regressor<kSigmoidCrossEntropy>;

template<ActivatedLoss loss>
void SGD<loss>::step(const std::vector<double> &features, std::vector<double> target) {
  std::vector<double> gradients = this->regressor.gradient(features, target);
  for (size_t i = 0; i < gradients.size(); i++) {
    this->regressor.weights[i] -= this->nu * gradients[i];
  }
  this->counter++;
}

template<ActivatedLoss loss>
void Adam<loss>::step(const std::vector<double> &features, std::vector<double> target) {
  std::vector<double> gradients = this->regressor.gradient(features, target);
  for (size_t i = 0; i < gradients.size(); i++) {
    expected_gradient[i] = beta1 * expected_gradient[i] + (1 - beta1) * gradients[i];
    expected_squared_gradient[i] = beta2 * expected_squared_gradient[i]
                                 + (1 - beta2) * gradients[i] * gradients[i];
    double m_hat = expected_gradient[i] / (1 - beta1);
    double v_hat = expected_squared_gradient[i] / (1 - beta2);
    this->regressor.weights[i] -= (this->nu / (std::sqrt(v_hat) + epsilon)) * m_hat;
  }
  this->counter++;
}

template struct SGD<kSigmoidCrossEntropy>;
template struct Adam<kSigmoidCrossEntropy>;

}
