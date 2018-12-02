/*
 * cluster.cc
 *
 *  Created on: Nov 14, 2018
 *      Author: jonathan
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

    // OCB feature
//    if (board.get_piece_count(kWhite, kBishop) == 1 && board.get_piece_count(kBlack, kBishop) == 1 &&
//        bitops::PopCount(board.get_piecetype_bitboard(kBishop) & bitops::light_squares) == 1) {
//      vec[4] = 1;
//    }
//    else {
//      vec[4] = 0;
//    }

  return vec;
}

template<size_t k, size_t length>
Vec<double, k> GaussianMixtureModel<k, length>::GetWeightedProbabilities(const Board &board) const {
  const Vec<double,kPhaseVecLength> sample = GetBoardPhaseVec(board);

  Vec<double, k> weighted_probabilities(0.00000001);
  double sum = 0;
  for (size_t i = 0; i < k; i++) {
    weighted_probabilities[i] += weights[i] * components[i].pdf(sample);
    sum += weighted_probabilities[i];
  }
  for (size_t i = 0; i < k; i++) {
    weighted_probabilities[i] /= sum;
  }
  return weighted_probabilities;
}

template<size_t k, size_t length>
void GaussianMixtureModel<k, length>::LoadFromParams(const std::vector<double> &params) {
  int c = 0;
  for (size_t m = 0; m < k; m++) {
    weights[m] = params[c++];
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      components[m].mu[i] = params[c++];
    }
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      for (size_t j = 0; j < kPhaseVecLength; j++) {
        components[m].sigma[i][j] = params[c++];
      }
    }
    components[m].set_sigma_inv();
  }
}

template<size_t k, size_t length>
void GaussianMixtureModel<k, length>::SaveHardCode(std::string file_name) const {
  std::ofstream file(file_name);
  file << "const std::array<double, (" << k << "+" << k << "*("
      << kPhaseVecLength << "+1)*" << kPhaseVecLength << ")> gmm_components = {" << std::endl;

  for (size_t m = 0; m < k; m++) {
    file << "    // Component " << m << " weights" << std::endl
        << "    " << weights[m] << "," << std::endl;

    file << "    // Component " << m << " means" << std::endl << "    ";
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      file << components[m].mu[i] << ", ";
    }
    file << std::endl;
    file << "    // Component " << m << " covariances" << std::endl;
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      for (size_t j = 0; j < kPhaseVecLength; j++) {
        if (j > 0) {
          file << ", ";
        }
        else {
          file << "    ";
        }
        file << components[m].sigma[i][j];
      }
      if (i+1 < kPhaseVecLength || m+1 < k) {
        file << ",";
      }
      file << std::endl;
    }
    if (m+1 == k) {
      file << "};";
    }
    file << std::endl;
  }
  file.flush();
  file.close();
}

template<size_t k, size_t length>
void GaussianMixtureModel<k, length>::SetModel(ClusterModel<k>* other) {
  cluster::GaussianMixtureModel <k, kPhaseVecLength>* gmm_p =
      static_cast<cluster::GaussianMixtureModel <k, kPhaseVecLength>* >(other);
  components = gmm_p->components;
  weights = gmm_p->weights;
}

template<size_t k, size_t length>
double GaussianMixtureModel<k, length>::GetSampleProbability(const Board &board) const {
  Vec<double, length> sample = GetBoardPhaseVec(board);
  double weighted_probability = 0.00000001;
  double sum = 0;
  for (size_t i = 0; i < k; i++) {
    weighted_probability += weights[i] * components[i].pdf(sample);
    sum += weights[i];
  }
  weighted_probability /= sum;
  return weighted_probability;
}

template<size_t k, size_t length>
Vec<double, k> NormFuzzyCMeans<k, length>::GetWeightedProbabilities(const Board &board) const {
  const Vec<double,kPhaseVecLength> sample = normalizer.normalize(GetBoardPhaseVec(board));
  const double kEpsilon = 0.00001;
  Vec<double, k> distances;
  for (int i = 0; i < k; i++) {
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

template struct GaussianMixtureModel<settings::kNumClusters, kPhaseVecLength>;
template struct NormFuzzyCMeans<settings::kNumClusters, kPhaseVecLength>;

}
