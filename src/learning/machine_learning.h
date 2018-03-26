/*
 * machine_learning.h
 *
 *  Created on: Mar 8, 2018
 *      Author: jonathan
 */

#ifndef SRC_LEARNING_MACHINE_LEARNING_H_
#define SRC_LEARNING_MACHINE_LEARNING_H_

#include <vector>

namespace ml {

std::vector<double> Wrap(const double value);

enum ActivatedLoss {
  kSigmoidCrossEntropy
};

template<ActivatedLoss loss>
struct Regressor {
  std::vector<double> weights;
  std::vector<double> gradient(const std::vector<double> &features, const std::vector<double> &targets);
};

template<ActivatedLoss loss>
struct Optimizer {
  double nu;
  long counter = 0;
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
