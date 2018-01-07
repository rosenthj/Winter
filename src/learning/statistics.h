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
 * statistics.h
 *
 *  Created on: Mar 17, 2017
 *      Author: Jonathan
 */

#ifndef LEARNING_STATISTICS_H_
#define LEARNING_STATISTICS_H_

#include "linear_algebra.h"
#include <cmath>
#include <iostream>

double get_tail(double dif, double sig) {
  const double sqrt_2 = std::sqrt(2);
  return (1 - std::erf(std::abs(dif / (sig * sqrt_2))));
}

template<int length>
struct Gaussian {
  double cdf_tails(const Vec<double, length> &sample) {
    double prob = 1.0;
    for (size_t i = 0; i < length-1; i++) {
      prob *= (1 - std::erf(std::abs((sample[i]-mu[i]) / std::sqrt(sigma[i][i] * 2))));
    }
    return prob;
  }
  double pdf(const Vec<double, length> &sample) {
    //double prob = 1;
    float exponent = 0;
    Vec<double, length> dif = sample - mu;
    exponent = -((sigma_inv.dot(dif)).dot(dif)) / 2;
    return std::exp(exponent) / sqrt_det_sigma_times_divisor;
  }
  double pdf_explicit(const Vec<double, length> &sample) {
    //double prob = 1;
    float exponent = 0;
    Vec<double, length> dif = sample - mu;
    std::cout << "Dif: ";
    dif.print();
    std::cout << std::endl;
    exponent = -((sigma_inv.dot(dif)).dot(dif)) / 2;
    std::cout << "Exponent: " << exponent << std::endl;
    return std::exp(exponent) / sqrt_det_sigma_times_divisor;
  }
  //Run this between changing sigma and calling pdf.
  void set_sigma_inv() {
    sigma_inv = SymmetricSquareMatrixInv(sigma);
    sqrt_det_sigma_times_divisor = std::sqrt(divisor() * Determinant(sigma));
  }
  Vec<double, length> mu;
  SqMatrix<double, length> sigma;
//private:
  static double divisor() {
    return std::pow(std::sqrt(2 * std::acos(-1)), length);
  }
  SqMatrix<double, length> sigma_inv;
  double sqrt_det_sigma_times_divisor;

};

template<int k, int length>
struct GMM{
  GMM() {
    for (size_t mix = 0; mix < k; mix++) {
      weights[mix] = 1;
      for (size_t i = 0; i < length; i++) {
        for (size_t j = 0; j < length; j++) {
          mixtures[mix].sigma[i][j] = 1;
        }
      }
    }
  }
  Vec<double, k> GetWeightedProbabilities(const Vec<double, length> &sample) {
    Vec<double, k> weighted_probabilities(0.00000001);
    double sum = 0;
    for (size_t i = 0; i < k; i++) {
      weighted_probabilities[i] += weights[i] * mixtures[i].pdf(sample);
      sum += weighted_probabilities[i];
    }
    for (size_t i = 0; i < k; i++) {
      weighted_probabilities[i] /= sum;
    }
    return weighted_probabilities;
  }
  double GetSampleProbability(const Vec<double, length> &sample) {
    double weighted_probability = 0.00000001;
    double sum = 0;
    for (size_t i = 0; i < k; i++) {
      weighted_probability += weights[i] * mixtures[i].pdf(sample);
      sum += weights[i];
    }
    weighted_probability /= sum;
    return weighted_probability;
  }
  Vec<double, k> GetD2MixtureAssignment(const Vec<double, length> &sample) {
    Vec<double, k> distances;
    double sum = 0;
    for (size_t i = 0; i < k; i++) {
      distances[i] = mixtures[i].mu.squared_distance(sample);
      if (distances[i] == 0) {
        distances[i] = 0.01;
      }
      distances[i] = 1 / distances[i];
      sum += distances[i];
    }
    for (size_t i = 0; i < k; i++) {
      distances[i] /= sum;
    }
    return distances;
  }
  Gaussian<length> mixtures[k];
  double weights[k];
};


#endif /* LEARNING_STATISTICS_H_ */
