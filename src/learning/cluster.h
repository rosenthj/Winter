/*
 * clustering.h
 *
 *  Created on: Nov 13, 2018
 *      Author: jonathan
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
    std::cout << "Fatal error, illegal function call!" << std::endl;
  }
  virtual void LoadFromParams(const std::vector<double> &params) {
    std::cout << "Fatal error, illegal function call!" << std::endl;
  }
  virtual void SaveHardCode(std::string file_name) const {
    std::cout << "Fatal error, illegal function call!" << std::endl;
  }
  virtual void SetModel(ClusterModel<k>* other) {
    std::cout << "Fatal error, illegal function call!" << std::endl;
  }
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

// Currently there is no abstract mixture model.
// In case of implementation of a new mixture model, it may make sense to add
// such a class.
template <size_t k, size_t length>
struct GaussianMixtureModel : ClusterModel<k> {

  Vec<double, k> GetWeightedProbabilities(const Board &board) const override;
  void LoadFromParams(const std::vector<double> &params) override;
  void SaveHardCode(std::string file_name) const override;
  void SetModel(ClusterModel<k>* other) override;

  double GetSampleProbability(const Board &board) const;

  std::array<Gaussian<length>, k> components;
  Vec<double, k> weights;
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

};

}

#endif /* SRC_LEARNING_CLUSTER_H_ */
