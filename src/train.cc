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
 * train.cc
 *
 *  Created on: Nov 16, 2018
 *      Author: Jonathan Rosenthal
 */

#include "train.h"
#include "learning/linear_algebra.h"
#include "learning/cluster.h"
#include "learning/machine_learning.h"
#include "general/feature_indexes.h"
#include "evaluation.h"
#include "search.h"
#include "general/types.h"
#include "data.h"
#include "benchmark.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>

namespace {

const double eval_scaling = 1024.0;
std::mt19937_64 rng;

}

namespace train {


enum SGDVariantType {
  SGDNormal, SGDAdaDelta, SGDAdaGrad, SGDAdam, SGDRotating
};

template<typename T>
double Sigmoid(T x) {
  return 1 / ( 1 + std::exp(-x) );
}

void EnforceConstraints(std::vector<double> &variables) {
  const int k = settings::kNumClusters;
  int idx = 0;
  for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
    if (i == positional_features::kFeatureInfos[idx + 1].idx) {
      idx++;
    }
    for (size_t j = 0; j < k; j++) {
      variables.at(i * k + j) = std::max((double) positional_features::kFeatureInfos[idx].min_value, variables[i * k + j]);
      variables.at(i * k + j) = std::min((double) positional_features::kFeatureInfos[idx].max_value, variables[i * k + j]);
      if (positional_features::kFeatureInfos[idx].encoded_info == 1 && i < (positional_features::kFeatureInfos[idx + 1].idx - 1)
          && variables.at(i * k + j) > variables.at((i + 1) * k + j)) {
        double dif = variables.at(i * k + j) - variables.at((i + 1) * k + j);
        variables.at(i * k + j) -= dif / 1000;
        variables.at((i + 1) * k + j) += dif / 1000;
      }
    }
  }

  for (size_t j = 0; j < k; j++) {
    //variables[(kKingPSTIndex + 0) * k + j] = 0;
    variables[(positional_features::kKnightSquaresIndex + 4) * k + j] = 0;
    for (int idx = positional_features::kKnightPSTIndex; idx < positional_features::kBaseValueIndex; idx++) {
      variables[(idx) * k + j] += variables[(positional_features::kBaseValueIndex + kKnight) * k + j];
    }
    variables[(positional_features::kBaseValueIndex + kKnight) * k + j] = 0;
    for (int idx = positional_features::kBishopMobility; idx < positional_features::kRookMobility; idx++) {
      variables[(idx) * k + j] += variables[(positional_features::kBaseValueIndex + kBishop) * k + j];
    }
    variables[(positional_features::kBaseValueIndex + kBishop) * k + j] = 0;
    for (int idx = positional_features::kRookMobility; idx < positional_features::kQueenMobility; idx++) {
      variables[(idx) * k + j] += variables[(positional_features::kBaseValueIndex + kRook) * k + j];
    }
    variables[(positional_features::kBaseValueIndex + kRook) * k + j] = 0;
    for (int idx = positional_features::kQueenMobility; idx < positional_features::kSafeChecks; idx++) {
      variables[(idx) * k + j] += variables[(positional_features::kBaseValueIndex + kQueen) * k + j];
    }
    variables[(positional_features::kBaseValueIndex + kQueen) * k + j] = 0;
  }
  for (size_t cat = 2; cat < 4; cat++) {
    for (size_t rank = 0; rank < 6; rank++) {
      for (size_t j = 0; j < k; j++) {
        variables[(positional_features::kPawnEvalIndex + cat * 24 + 18 + rank) * k + j] =
            std::max(variables[(positional_features::kPawnEvalIndex + cat * 24 + 6 + rank) * k + j],
                     variables[(positional_features::kPawnEvalIndex + cat * 24 + 12 + rank) * k + j]);
      }
    }
  }
}

enum LearningStyle {
  kSupervised, kTDL, kMixedLearning
};

template<LearningStyle learning_style>
bool SamplePositionAndTarget(Game &game, Board &position, double &target) {
  if (learning_style == kSupervised) {
    bool success = data::SetGameToRandomQuiescent(game);
    if (!success) {
      return false;
    }
    position.SetToSamePosition(game.board);
    target = game.result;
    if (position.get_turn() == kBlack) {
      target = 1 - target;
    }
  }
  else if (learning_style == kMixedLearning) {
    const double game_res_weight = 0.3;
    const double td_weight = 1 - game_res_weight;
    Score score = 0, qscore = 100;
    //std::cout << "-> " << index << std::flush;
    int qsearch_attempts = 0;
    while (score != qscore) {
      data::SetGameToRandom(game);
      if (game.board.InCheck() || game.board.IsDraw())
        continue;
      qsearch_attempts++;
      if (qsearch_attempts == 20)
        break;
      score = evaluation::ScoreBoard(game.board);
      qscore = search::QSearch(game.board);
    }
//    std::cout << "Qsearch attempts: " << qsearch_attempts << std::endl;
    if (qsearch_attempts == 20)
      return false;
    //std::cout << " -> " << games[index].board.get_num_made_moves() << std::flush;
    search::set_print_info(false);
    search::DepthSearch(game.board, 4);
//    search::NodeSearch(game.board, 8000);
    search::set_print_info(true);
    //std::cout << "<- " << std::endl;
    target = search::get_last_search_score() / eval_scaling;
    double game_res = game.result;
    if (game.board.get_turn() == kBlack) {
      game_res = 1 - game_res;
    }
    target = td_weight * Sigmoid<double>(target)
                            + game_res_weight * game_res;
    position.SetToSamePosition(game.board);
  }
  else if (learning_style == kTDL) {
    //Score score = 0, qscore = -1;
    //int qsearch_attempts = 0;
    data::SetGameToRandom(game);
    position.SetToSamePosition(game.board);
    search::set_print_info(false);
    Move move = search::DepthSearch(position, 6);
    search::set_print_info(true);
    position.Make(move);
    if (position.GetMoves<kNonQuiescent>().size() == 0) {
      return false;
    }
    if (evaluation::ScoreBoard(position) != search::SQSearch(position)) {
      return false;
    }
    search::set_print_info(false);
    search::DepthSearch(position, 6);
    search::set_print_info(true);
    target = Sigmoid<double>(search::get_last_search_score() / eval_scaling);
  }
  return true;
}

template<int k>
void SaveGMM(cluster::GaussianMixtureModel<k,kPhaseVecLength> &gmm, std::string file_name) {
//void SaveGMM(GMM<k,kPhaseVecLength> &gmm, std::string file_name) {
  std::ofstream file(file_name);
  for (size_t m = 0; m < k; m++) {
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      file << gmm.components[m].mu[i] << " ";
    }
    file << gmm.weights[m] << std::endl;
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      for (size_t j = 0; j < kPhaseVecLength; j++) {
        file << gmm.components[m].sigma[i][j] << " ";
      }
      file << std::endl;
    }
  }
  file.flush();
  file.close();
}

template<int k>
cluster::GaussianMixtureModel<k, kPhaseVecLength> EMForGMM(std::vector<Game> &games,
                                                           int iterations = 50) {
//GMM<k, kPhaseVecLength> EMForGMM(std::vector<Game> &games, int iterations = 50) {
  data::SetGamesToRandomQuiescent(games);
  std::shuffle(games.begin(), games.end(), rng);
  std::vector<Game> validation;
  for (int i = 0; i < 10000; i++) {
    validation.emplace_back(games.back());
    games.pop_back();
  }
  cluster::GaussianMixtureModel<k, kPhaseVecLength> best_gmm;
//  GMM<k, kPhaseVecLength> best_gmm;
  double log_prob = 0, best_log_prob = -100000000;
  int initializations = 100;
  while (initializations--) {
    int it = iterations;
    cluster::GaussianMixtureModel<k, kPhaseVecLength> gmm;
//    GMM<k, kPhaseVecLength> gmm;
    std::cout << "setting random means" << std::endl;
    for (size_t i = 0; i < k; i++) {
      bool unique = false;
      while (!unique) {
        gmm.components[i].mu = cluster::GetBoardPhaseVec(games[rng() % games.size()].board);
        unique = true;
        for (size_t j = 0; j < i; j++) {
          if (gmm.components[i].mu == gmm.components[j].mu) {
            unique = false;
          }
        }
      }
      gmm.components[i].mu.print();
      std::cout << std::endl;
    }
    std::cout << "initializing centroid assignments" << std::endl;
    std::vector<Vec<double, k> > assignments(games.size(), 0);
    for (size_t i = 0; i < games.size(); i++) {
      if (i < games.size() / 25) {
        assignments[i] = i % k;
      }
      else {
//        assignments[i] = gmm.GetD2MixtureAssignment(GetBoardPhaseVec(games[i].board));//TODO readd
      }
    }
    while (it--) {
      std::cout << "Remaining iterations: " << it << std::endl;
      //Calculate Mu
      std::cout << "Calculating Means" << std::endl;
      std::vector<Vec<double,kPhaseVecLength> > sums(k, 0);
      Vec<double,k> counts(0);
      std::cout << "Counts: ";
      counts.print(); std::cout << std::endl;
      for (size_t i = 0; i < games.size(); i++) {
        for (size_t mix = 0; mix < k; mix++) {
          sums.at(mix) += assignments[i][mix] * cluster::GetBoardPhaseVec(games[i].board);
        }
        counts += assignments[i];
      }
      for (size_t i = 0; i < k; i++) {
        gmm.components[i].mu = sums[i] / counts[i];
        gmm.weights[i] = counts[i];
        std::cout << "Mixture Count[" << i <<"]: "<< counts[i] << std::endl;
        std::cout << "Mixture Means: ";
        gmm.components[i].mu.print();
        std::cout << std::endl;
      }
      std::cout << "Calculating Standard Deviations" << std::endl;
      //Calculate Sigma
      for (size_t j = 0; j < k; j++) {
        for (size_t m = 0; m < kPhaseVecLength; m++) {
          for (size_t n = 0; n < kPhaseVecLength; n++) {
            gmm.components[j].sigma[m][n] = 0;
          }
        }
      }
      for (size_t i = 0; i < games.size(); i++) {
        Vec<double, kPhaseVecLength> game_vec = cluster::GetBoardPhaseVec(games[i].board);
        for (size_t j = 0; j < k; j++) {
          for (size_t m = 0; m < kPhaseVecLength; m++) {
            double d1 = gmm.components[j].mu[m] - game_vec[m];
            for (size_t n = 0; n < kPhaseVecLength; n++) {
              double d2 = gmm.components[j].mu[n] - game_vec[n];
              gmm.components[j].sigma[m][n] += assignments[i][j] * (d1 * d2);
            }
          }
        }
      }
      for (size_t j = 0; j < k; j++) {
        if (counts[j] <= 1) {
          std::cout << "Mixture " << j << " has no count!" << std::endl;
          continue;
        }
        std::cout << "Mixture " << j << " Covariance Matrix:" << std::endl;
        for (size_t m = 0; m < kPhaseVecLength; m++) {
          for (size_t n = 0; n < kPhaseVecLength; n++) {
            gmm.components[j].sigma[m][n] /= (counts[j]-1);
            if (m == n) {
              gmm.components[j].sigma[m][n] = std::max(gmm.components[j].sigma[m][n], std::pow(0.004,2));
            }
            std::cout << gmm.components[j].sigma[m][n] << " ";
          }
          std::cout << std::endl;
        }
        gmm.components[j].set_sigma_inv();
        std::cout << "Divisor " << j<< ":" << gmm.components[j].sqrt_det_sigma_times_divisor << std::endl;
      }
      //Calculate Indexes
      if (it > 0) {
        std::cout << "Calculating Sample assignments" << std::endl;
        for (size_t i = 0; i < games.size(); i++) {
          assignments[i] =
              gmm.GetWeightedProbabilities(games[i].board);
//              gmm.GetWeightedProbabilities(GetBoardPhaseVec(games[i].board));
        }
      }
      //Calculate mixture probability
      log_prob = 0;
      for (size_t i = 0; i < validation.size(); i++) {
        log_prob += std::log(gmm.GetSampleProbability(validation[i].board));
      }
      std::cout << "Log Mixture probability: " << log_prob << std::endl;
    }
    if (log_prob > best_log_prob) {
      best_log_prob = log_prob;
      best_gmm = gmm;
    }
  }
  while(validation.size() > 0) {
    games.emplace_back(validation.back());
    validation.pop_back();
  }
  std::cout << "Best log probability validation sum: " << best_log_prob << std::endl;
  return best_gmm;
}

void SampledEMForGMM(int iterations) {
  search::set_print_info(false);
  const int k = settings::kNumClusters;
  std::vector<Game> games = data::LoadGames();
  data::SetGamesToRandom(games);
  std::shuffle(games.begin(), games.end(), rng);
  std::vector<Board> positions;
  positions.reserve(games.size());
  for (int i = 0; i < games.size(); i++) {
    positions.emplace_back(search::SampleEval(games[i].board));
    if (positions.size() % 1000 == 0) {
      std::cout << "sampled " << positions.size() << " positions!" << std::endl;
    }
  }
  games.clear();
  std::vector<Board> validation;
  for (int i = 0; i < 10000; i++) {
    validation.emplace_back(positions.back());
    positions.pop_back();
  }
  cluster::GaussianMixtureModel<k, kPhaseVecLength> best_gmm;
//  GMM<k, kPhaseVecLength> best_gmm;
  double log_prob = 0, best_log_prob = -100000000;
  int initializations = 1;
  while (initializations--) {
    int it = iterations;
    cluster::GaussianMixtureModel<k, kPhaseVecLength> gmm;
//    GMM<k, kPhaseVecLength> gmm;
    std::cout << "setting random means" << std::endl;
    for (size_t i = 0; i < k; i++) {
      bool unique = false;
      while (!unique) {
        gmm.components[i].mu = cluster::GetBoardPhaseVec(positions[rng() % positions.size()]);
        unique = true;
        for (size_t j = 0; j < i; j++) {
          if (gmm.components[i].mu == gmm.components[j].mu) {
            unique = false;
          }
        }
      }
      gmm.components[i].mu.print();
      std::cout << std::endl;
    }
    std::cout << "initializing centroid assignments" << std::endl;
    std::vector<Vec<double, k> > assignments(positions.size(), 0);
    for (size_t i = 0; i < positions.size(); i++) {
//      assignments[i] = gmm.GetD2MixtureAssignment(GetBoardPhaseVec(positions[i]));
    }
    while (it--) {
      std::cout << "Remaining iterations: " << it << std::endl;
      //Calculate Mu
      std::cout << "Calculating Means" << std::endl;
      std::vector<Vec<double,kPhaseVecLength> > sums(k, 0);
      Vec<double,k> counts(0);
      std::cout << "Counts: ";
      counts.print(); std::cout << std::endl;
      for (size_t i = 0; i < positions.size(); i++) {
        for (size_t mix = 0; mix < k; mix++) {
          sums.at(mix) += assignments[i][mix] * cluster::GetBoardPhaseVec(positions[i]);
        }
        counts += assignments[i];
      }
      for (size_t i = 0; i < k; i++) {
        gmm.components[i].mu = sums[i] / counts[i];
        gmm.weights[i] = counts[i];
        std::cout << "Mixture Count[" << i <<"]: "<< counts[i] << std::endl;
      }
      std::cout << "Calculating Standard Deviations" << std::endl;
      //Calculate Sigma
      if (true) {
        for (size_t j = 0; j < k; j++) {
          for (size_t m = 0; m < kPhaseVecLength; m++) {
            for (size_t n = 0; n < kPhaseVecLength; n++) {
              gmm.components[j].sigma[m][n] = 0;
            }
          }
        }
        for (size_t i = 0; i < positions.size(); i++) {
          Vec<double, kPhaseVecLength> game_vec = cluster::GetBoardPhaseVec(positions[i]);
          for (size_t j = 0; j < k; j++) {
            for (size_t m = 0; m < kPhaseVecLength; m++) {
              double d1 = gmm.components[j].mu[m] - game_vec[m];
              for (size_t n = 0; n < kPhaseVecLength; n++) {
                double d2 = gmm.components[j].mu[n] - game_vec[n];
                gmm.components[j].sigma[m][n] += assignments[i][j] * (d1 * d2);
              }
            }
          }
        }
        for (size_t j = 0; j < k; j++) {
          if (counts[j] <= 1) {
            std::cout << "Mixture " << j << " has no count!" << std::endl;
            continue;
          }
          std::cout << "Mixture " << j << " Covariance Matrix:" << std::endl;
          for (size_t m = 0; m < kPhaseVecLength; m++) {
            for (size_t n = 0; n < kPhaseVecLength; n++) {
              gmm.components[j].sigma[m][n] /= (counts[j]-1);
              if (m == n) {
                gmm.components[j].sigma[m][n] = std::max(gmm.components[j].sigma[m][n], std::pow(0.004,2));
              }
              else {
                //gmm.mixtures[j].sigma[m][n] = 0;
              }
              std::cout << gmm.components[j].sigma[m][n] << " ";
            }
            std::cout << std::endl;
          }
          gmm.components[j].set_sigma_inv();
          std::cout << "Divisor " << j<< ":" << gmm.components[j].sqrt_det_sigma_times_divisor << std::endl;
        }
      }
      //Calculate Indexes
      if (it > 0) {
        std::cout << "Calculating Sample assignments" << std::endl;
        for (size_t i = 0; i < positions.size(); i++) {
          assignments[i] =
              gmm.GetWeightedProbabilities(positions[i]);
//              gmm.GetWeightedProbabilities(GetBoardPhaseVec(positions[i]));
        }
      }
      //Calculate mixture probability
      log_prob = 0;
      for (size_t i = 0; i < validation.size(); i++) {
//        log_prob += std::log(gmm.GetSampleProbability(validation[i]));
//        log_prob += std::log(gmm.GetSampleProbability(GetBoardPhaseVec(validation[i])));
      }
      std::cout << "Log Mixture probability: " << log_prob << std::endl;
    }
    if (log_prob > best_log_prob) {
      best_log_prob = log_prob;
      best_gmm = gmm;
    }
  }
  std::cout << "Best log probability validation sum: " << best_log_prob << std::endl;
  std::ofstream file(settings::kSampledMixtureFile);
  for (size_t m = 0; m < k; m++) {
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      file << best_gmm.components[m].mu[i] << " ";
    }
    file << best_gmm.weights[m] << std::endl;
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      for (size_t j = 0; j < kPhaseVecLength; j++) {
        file << best_gmm.components[m].sigma[i][j] << " ";
      }
      file << std::endl;
    }
  }
  file.flush();
  file.close();
  search::set_print_info(true);
}

template<int k>
cluster::GaussianMixtureModel<k, kPhaseVecLength> EMForGMM(std::vector<Board> &positions, int iterations = 50) {
//GMM<k, kPhaseVecLength> EMForGMM(std::vector<Board> &positions, int iterations = 50) {
  std::shuffle(positions.begin(), positions.end(), rng);
  std::vector<Board> validation;
  for (int i = 0; i < 10000; i++) {
    validation.emplace_back(positions.back());
    positions.pop_back();
  }
  cluster::GaussianMixtureModel<k, kPhaseVecLength> best_gmm;
//  GMM<k, kPhaseVecLength> best_gmm;
  double log_prob = 0, best_log_prob = -100000000;
  int initializations = 100;
  while (initializations--) {
    int it = iterations;
    cluster::GaussianMixtureModel<k, kPhaseVecLength> gmm;
//    GMM<k, kPhaseVecLength> gmm;
    std::cout << "setting random means" << std::endl;
    for (size_t i = 0; i < k; i++) {
      bool unique = false;
      while (!unique) {
        gmm.components[i].mu = cluster::GetBoardPhaseVec(positions[rng() % positions.size()]);
        unique = true;
        for (size_t j = 0; j < i; j++) {
          if (gmm.components[i].mu == gmm.components[j].mu) {
            unique = false;
          }
        }
      }
      gmm.components[i].mu.print();
      std::cout << std::endl;
    }
    std::cout << "initializing centroid assignments" << std::endl;
    std::vector<Vec<double, k> > assignments(positions.size(), 0);
    for (size_t i = 0; i < positions.size(); i++) {
      if (i < positions.size() / 25) {
        assignments[i] = i % k;
      }
      else {
//        assignments[i] = gmm.GetD2MixtureAssignment(GetBoardPhaseVec(positions[i]));//TODO readd
      }
    }
    while (it--) {
      std::cout << "Remaining iterations: " << it << std::endl;
      //Calculate Mu
      std::cout << "Calculating Means" << std::endl;
      std::vector<Vec<double,kPhaseVecLength> > sums(k, 0);
      Vec<double,k> counts(0);
      std::cout << "Counts: ";
      counts.print(); std::cout << std::endl;
      for (size_t i = 0; i < positions.size(); i++) {
        for (size_t mix = 0; mix < k; mix++) {
          sums.at(mix) += assignments[i][mix] * cluster::GetBoardPhaseVec(positions[i]);
        }
        counts += assignments[i];
      }
      for (size_t i = 0; i < k; i++) {
        gmm.components[i].mu = sums[i] / counts[i];
        gmm.weights[i] = counts[i];
        std::cout << "Component Count[" << i <<"]: "<< counts[i] << std::endl;
        std::cout << "Component Means: ";
        gmm.components[i].mu.print();
        std::cout << std::endl;
      }
      std::cout << "Calculating Standard Deviations" << std::endl;
      //Calculate Sigma
      for (size_t j = 0; j < k; j++) {
        for (size_t m = 0; m < kPhaseVecLength; m++) {
          for (size_t n = 0; n < kPhaseVecLength; n++) {
            gmm.components[j].sigma[m][n] = 0;
          }
        }
      }
      for (size_t i = 0; i < positions.size(); i++) {
        Vec<double, kPhaseVecLength> game_vec = cluster::GetBoardPhaseVec(positions[i]);
        for (size_t j = 0; j < k; j++) {
          for (size_t m = 0; m < kPhaseVecLength; m++) {
            double d1 = gmm.components[j].mu[m] - game_vec[m];
            for (size_t n = 0; n < kPhaseVecLength; n++) {
              double d2 = gmm.components[j].mu[n] - game_vec[n];
              gmm.components[j].sigma[m][n] += assignments[i][j] * (d1 * d2);
            }
          }
        }
      }
      for (size_t j = 0; j < k; j++) {
        if (counts[j] <= 1) {
          std::cout << "Mixture " << j << " has no count!" << std::endl;
          continue;
        }
        std::cout << "Mixture " << j << " Covariance Matrix:" << std::endl;
        for (size_t m = 0; m < kPhaseVecLength; m++) {
          for (size_t n = 0; n < kPhaseVecLength; n++) {
            gmm.components[j].sigma[m][n] /= (counts[j]-1);
            if (m == n) {
              gmm.components[j].sigma[m][n] = std::max(gmm.components[j].sigma[m][n], std::pow(0.004,2));
            }
            std::cout << gmm.components[j].sigma[m][n] << " ";
          }
          std::cout << std::endl;
        }
        gmm.components[j].set_sigma_inv();
        std::cout << "Divisor " << j<< ":" << gmm.components[j].sqrt_det_sigma_times_divisor << std::endl;
      }
      //Calculate Indexes
      if (it > 0) {
        std::cout << "Calculating Sample assignments" << std::endl;
        for (size_t i = 0; i < positions.size(); i++) {
          assignments[i] =
              gmm.GetWeightedProbabilities(positions[i]);
//              gmm.GetWeightedProbabilities(GetBoardPhaseVec(positions[i]));
        }
      }
      //Calculate mixture probability
      log_prob = 0;
      for (size_t i = 0; i < validation.size(); i++) {
        log_prob += std::log(gmm.GetSampleProbability(validation[i]));
      }
      std::cout << "Log Mixture probability: " << log_prob << std::endl;
    }
    if (log_prob > best_log_prob) {
      best_log_prob = log_prob;
      best_gmm = gmm;
    }
  }
  while(validation.size() > 0) {
    positions.emplace_back(validation.back());
    validation.pop_back();
  }
  std::cout << "Best log probability validation sum: " << best_log_prob << std::endl;
  return best_gmm;
}

template<int k>
cluster::NormFuzzyCMeans<k, kPhaseVecLength> EMForNFCM(std::vector<Board> &positions, int iterations = 80) {
  std::shuffle(positions.begin(), positions.end(), rng);
  cluster::NormFuzzyCMeans<k, kPhaseVecLength> nfcm_best;
  Vec<double, kPhaseVecLength> v_sum(0), v_s_sum(0.00000000001);
  for (Board position : positions) {
    v_sum += cluster::GetBoardPhaseVec(position);
    v_s_sum += cluster::GetBoardPhaseVec(position) * cluster::GetBoardPhaseVec(position);
  }
  ml::Normalizer<kPhaseVecLength> normalizer;
  normalizer.means = v_sum / positions.size();
  v_s_sum = v_s_sum / positions.size() - normalizer.means * normalizer.means;
  for (int i = 0; i < kPhaseVecLength; i++) {
    normalizer.std_dev[i] = std::sqrt(v_s_sum[i]);
  }
  std::cout << "Mean: " << std::endl;
  normalizer.means.print();
  std::cout << std::endl << "Std Dev: " << std::endl;
  normalizer.std_dev.print();
  std::cout << std::endl;
  nfcm_best.normalizer = normalizer;
  int initializations = 1;
  while (initializations--) {
    int it = iterations;
    cluster::NormFuzzyCMeans<k, kPhaseVecLength> nfcm;
    nfcm.normalizer = normalizer;
    std::cout << "setting random means" << std::endl;
    for (int i = 0; i < 4; i++) {
      nfcm.centroids[0][i] = 0;
    }
//    nfcm.centroids[0][5] = 6;
    nfcm.centroids[0].print();
    nfcm.centroids[0] = normalizer.normalize(nfcm.centroids[0]);
    nfcm.centroids[0].print();
    nfcm.centroids[k-1][0] = 4;
    nfcm.centroids[k-1][1] = 4;
    nfcm.centroids[k-1][2] = 4;
    nfcm.centroids[k-1][3] = 2;
    nfcm.centroids[k-1] = normalizer.normalize(nfcm.centroids[k-1]);
    for (size_t i = 1; i < k-1; i++) {
      bool unique = false;
      while (!unique) {
        nfcm.centroids[i] = normalizer.normalize(cluster::GetBoardPhaseVec(positions[rng() % positions.size()]));
        unique = true;
        for (size_t j = 0; j < i; j++) {
          if (nfcm.centroids[i] == nfcm.centroids[j]) {
            unique = false;
          }
        }
      }
      nfcm.centroids[i].print();
      std::cout << std::endl;
    }
    std::cout << "initializing centroid assignments" << std::endl;
    std::vector<Vec<double, k> > assignments(positions.size(), 0);
    for (size_t i = 0; i < positions.size(); i++) {
//        assignments[i] = nfcm.GetWeightedProbabilities(positions[i]);
      for (int j = 0; j < k; j++) {
        assignments[i][j] = 0;
      }
      assignments[i][i % k] = 1;
    }
    while (it--) {
      std::cout << "Remaining iterations: " << it << std::endl;
      //Calculate Mu
      std::cout << "Calculating Means" << std::endl;
      std::vector<Vec<double,kPhaseVecLength> > sums(k, 0);
      Vec<double,k> counts(0);
      std::cout << "Counts: ";
      counts.print(); std::cout << std::endl;
      for (size_t i = 0; i < positions.size(); i++) {
        // Calculate assignment power m
        // TODO Parametrize m
        Vec<double, k> assignment = assignments[i] * assignments[i] * assignments[i];
        for (size_t mix = 0; mix < k; mix++) {
          sums.at(mix) += assignment[mix] * nfcm.normalizer.normalize(cluster::GetBoardPhaseVec(positions[i]));
        }
        counts += assignment;
      }
      for (size_t i = 0; i < k; i++) {
        if (i == 0 || i == k-1) {
//          nfcm.centroids[i][0] = sums[i][0] / counts[i];
//          nfcm.centroids[i][0] = sums[i][5] / counts[i];
        }
        else {
          nfcm.centroids[i] = sums[i] / counts[i];
        }
        std::cout << "Component Count[" << i <<"]: "<< counts[i] << std::endl;
        std::cout << "Unnormalized Component Centroids: ";
        normalizer.unnormalize(nfcm.centroids[i]).print();
        std::cout << std::endl;
      }
      //Calculate Indexes
      if (it > 0) {
        std::cout << "Calculating Sample assignments" << std::endl;
        for (size_t i = 0; i < positions.size(); i++) {
          assignments[i] =
              nfcm.GetWeightedProbabilities(positions[i]);
//              gmm.GetWeightedProbabilities(GetBoardPhaseVec(positions[i]));
        }
      }
      //Calculate mixture probability
    }
    nfcm_best = nfcm;
  }
  return nfcm_best;
}

void RunEMForNFCM() {
  const int k = settings::kNumClusters;
  std::vector<Board> positions = data::LoadBoardFens("data/sample_evals.fen");
  cluster::NormFuzzyCMeans<k,kPhaseVecLength> nfcm = EMForNFCM<k>(positions);
  nfcm.SaveHardCode("nfcm.txt");
}

void RunEMForGMM() {
  const int k = settings::kNumClusters;
  //std::vector<Game> games = data::LoadGames(1200000);
  std::vector<Board> positions = data::LoadBoardFens("data/sample_evals.fen");
  cluster::GaussianMixtureModel<k,kPhaseVecLength> gmm = EMForGMM<k>(positions);
//  GMM<k,kPhaseVecLength> gmm = EMForGMMNoisy<k>(positions);
  std::ofstream file(settings::kMixtureFile);
  for (size_t m = 0; m < k; m++) {
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      file << gmm.components[m].mu[i] << " ";

    }
    file << gmm.weights[m] << std::endl;
    for (size_t i = 0; i < kPhaseVecLength; i++) {
      for (size_t j = 0; j < kPhaseVecLength; j++) {
        file << gmm.components[m].sigma[i][j] << " ";
      }
      file << std::endl;
    }
  }
  file.flush();
  file.close();
}

struct PositionAndTarget {
  Board board;
  double player_to_move_result;
};

std::vector<PositionAndTarget> get_MLDataset() {
  std::string line;
  std::ifstream file("dataset10kf.epd");
  size_t samples = 0;
  std::vector<PositionAndTarget> dataset;
  while(std::getline(file, line)) {
    std::vector<std::string> tokens = parse::split(line, ',');
    std::string fen = tokens[0];
    PositionAndTarget sample;
    sample.board.SetBoard(parse::split(fen, ' '));
    sample.player_to_move_result = 0.5 * std::stod(tokens[2].c_str()) + 0.5 * std::stod(tokens[1].c_str());
    dataset.emplace_back(sample);
    samples++;
    if (samples % 10000 == 0) {
      std::cout << "Processed " << samples << " samples!" << std::endl;
    }
  }
  file.close();
  return dataset;
}

template<SGDVariantType sgd_variant, LearningStyle learning_style>
void SGDTrainMLSet(bool from_scratch = false) {
  const int k = settings::kNumClusters;
  double kMinNu = (1.0 / kMillion);
  const int max_position_tries = learning_style == kSupervised ? 5 : 2;
  const long step_size = learning_style == kSupervised ? (200 * kMillion) : (max_position_tries * 1200 * kThousand);
  const long save_frequency = learning_style == kSupervised ? (1 * kMillion) : (100 * kThousand);
  const long benchmark_frequency = learning_style == kSupervised ? (500 * kMillion) : (5000 * kThousand);

  ml::Optimizer<ml::kSigmoidCrossEntropy>* optimizer;
  if (sgd_variant == SGDNormal) {
    optimizer = new ml::SGD<ml::kSigmoidCrossEntropy>();
    optimizer->nu = 1.0 / eval_scaling;
  }
  else if (sgd_variant == SGDAdam) {
    ml::Adam<ml::kSigmoidCrossEntropy>* opt = new ml::Adam<ml::kSigmoidCrossEntropy>();
    opt->nu = 2.0 * (1 - opt->beta1) / eval_scaling;
    kMinNu = (1.0 / kMillion) * (1 - opt->beta2);
    optimizer = opt;
  }
  optimizer->set_num_features(k * positional_features::kNumFeatures);

  if (!from_scratch) {
    for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        optimizer->regressor.weights[i * k + j] = evaluation::GetScore(i, j) / eval_scaling;
      }
    }
  }
  std::vector<PositionAndTarget> dataset = get_MLDataset();
  if (from_scratch && settings::kTrainGMMFromScratch) {
    std::vector<Game> games = data::LoadGames();
    cluster::GaussianMixtureModel<k, kPhaseVecLength> gmm = EMForGMM<k>(games);
    SaveGMM<k>(gmm, settings::kMixtureFile);
    evaluation::SetModel(&gmm);
  }
  else {
    if (from_scratch && !settings::kTrainGMMFromScratch) {
      evaluation::LoadMixturesHardCoded();
    }
  }

  std::vector<int8_t> position_tries(dataset.size(), max_position_tries);
  std::vector<double> position_targets(dataset.size(), 0);
  std::cout << "Starting training loop" << std::endl;
  size_t milestone = 1;
//  size_t successive_failures = 0;
  while (optimizer->nu > kMinNu) {
    size_t index = rng() % dataset.size();
    if (position_tries[index] >= max_position_tries) {
      if (learning_style == kSupervised) {
        position_targets[index] = dataset[index].player_to_move_result;
      }
      else if (learning_style == kMixedLearning) {
        const double game_res_weight = 1;//0.5;
        const double td_weight = 1 - game_res_weight;
//        search::set_print_info(false);
//        search::DepthSearch(dataset[index].board, 4);
//        search::set_print_info(true);
        double search_target = 0.0;//search::get_last_search_score() / eval_scaling;
        double game_res = dataset[index].player_to_move_result;
        position_targets[index] = td_weight * Sigmoid<double>(search_target)
                                + game_res_weight * game_res;
      }
      position_tries[index] = 0;
    }
    position_tries[index]++;
    Vec<double, k> probabilities = evaluation::BoardMixtureProbability(dataset[index].board);
//    Vec<double, k> probabilities = evaluation::gmm_main->GetWeightedProbabilities(positions[index]);
//    Vec<double, k> probabilities = gmm_main.GetWeightedProbabilities(
//        GetBoardPhaseVec(positions[index]));
    if (learning_style == kSupervised && optimizer->counter < 150 * kMillion) {
      if (optimizer->counter < 50 * kMillion) {
        for (int i = 0; i < k; i++) {
          probabilities[i] = 1.0 / k;
        }
      }
      else {
        double p = (optimizer->counter - 50 * kMillion) / (100.0 * kMillion);
        double sum = 0;
        for (int i = 0; i < k; i++) {
          probabilities[i] = std::pow(probabilities[i], p);
          sum += probabilities[i];
        }
        for (int i = 0; i < k; i++) {
          probabilities[i] /= sum;
        }
      }
    }

    std::vector<double> features =
        evaluation::ScoreBoard<std::vector<double> >(dataset[index].board);
    std::vector<double> split_features(positional_features::kNumFeatures * k);
    for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        split_features[i * k + j] = features[i] * probabilities[j];
      }
    }

//    double probability_of_position = 0;//gmm_main.GetSampleProbability(
//        //GetBoardPhaseVec(positions[index]));
//    if (probability_of_position == 0) {
//      std::cout << "Error! Found impossible position!" << std::endl;
//    }

    optimizer->step(split_features, ml::Wrap(position_targets[index]));
    EnforceConstraints(optimizer->regressor.weights);
    for (size_t widx = 0; widx < settings::kNumClusters; widx++) {
      optimizer->regressor.weights[positional_features::kTempoBonusIndex * settings::kNumClusters + widx] = 100 / eval_scaling;
    }

    if (optimizer->counter % save_frequency == 0) {
      std::cout << "completed " << optimizer->counter << " iterations!"
          << std::endl;
      for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
        for (size_t j = 0; j < k; j++) {
          evaluation::SetScore(i, j, std::round(optimizer->regressor.weights[i * k + j] * eval_scaling));
//          feature_GMM_score_values[i][j] = std::round(optimizer->regressor.weights[i * k + j] * eval_scaling);
        }
      }
      evaluation::SaveGMMVariables();
      evaluation::SaveGMMVariablesHardCode("eval_weights.txt");
      if (optimizer->counter % benchmark_frequency == 0) {
        benchmark::EntropyLossNodeSuite(5000);
//        benchmark::EntropyLossTimedSuite(Milliseconds(10));
      }
    }
    if (optimizer->counter % step_size == 0) {
      optimizer->nu /= 2;
      std::cout << "New Nu: " << optimizer->nu << std::endl;
    }
    if (optimizer->counter > milestone) {
      std::cout << "Milestone " << milestone << " reached" << std::endl;
      milestone *= 10;
    }
  }
  std::cout << "Training completed!" << std::endl;
  delete optimizer;
}

std::vector<PositionAndTarget> get_ZCDataset() {
  std::string line;
  std::ifstream file("quiet-labeled.epd");
  size_t samples = 0;
  std::vector<PositionAndTarget> dataset;
  while(std::getline(file, line)) {
    std::vector<std::string> tokens = parse::split(line, ' ');
    std::string fen = tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + tokens[3];
    PositionAndTarget sample;
    sample.board.SetBoard(parse::split(fen, ' '));
    double result = -1;
    if (tokens[5].compare("\"1-0\";") == 0) {
      result = 1;
    }
    else if (tokens[5].compare("\"0-1\";") == 0) {
      result = 0;
    }
    else if (tokens[5].compare("\"1/2-1/2\";") == 0) {
      result = 0.5;
    }
    else {
      std::cout << "Error detected! Line:" << std::endl;
      std::cout << line << std::endl;
      file.close();
      return dataset;
    }
    if (sample.board.get_turn() == kBlack) {
      result = 1 - result;
    }
    sample.player_to_move_result = result;
    dataset.emplace_back(sample);
    samples++;
    if (samples % 10000 == 0) {
      std::cout << "Processed " << samples << " samples!" << std::endl;
    }
  }
  file.close();
  return dataset;
}

template<SGDVariantType sgd_variant, LearningStyle learning_style>
void SGDTrainZCSet(bool from_scratch = false) {
  const int k = settings::kNumClusters;
  double kMinNu = (1.0 / kMillion);
  const int max_position_tries = learning_style == kSupervised ? 5 : 2;
  const long step_size = learning_style == kSupervised ? (200 * kMillion) : (max_position_tries * 1200 * kThousand);
  const long save_frequency = learning_style == kSupervised ? (1 * kMillion) : (10 * kThousand);
  const long benchmark_frequency = learning_style == kSupervised ? (500 * kMillion) : (500 * kThousand);

  ml::Optimizer<ml::kSigmoidCrossEntropy>* optimizer;
  if (sgd_variant == SGDNormal) {
    optimizer = new ml::SGD<ml::kSigmoidCrossEntropy>();
    optimizer->nu = 1.0 / eval_scaling;
  }
  else if (sgd_variant == SGDAdam) {
    ml::Adam<ml::kSigmoidCrossEntropy>* opt = new ml::Adam<ml::kSigmoidCrossEntropy>();
    opt->nu = 4.0 * (1 - opt->beta1) / eval_scaling;
    kMinNu = (1.0 / kMillion) * (1 - opt->beta2);
    optimizer = opt;
  }
  optimizer->set_num_features(k * positional_features::kNumFeatures);

  if (!from_scratch) {
    for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        optimizer->regressor.weights[i * k + j] = evaluation::GetScore(i, j) / eval_scaling;
      }
    }
  }
  std::vector<PositionAndTarget> dataset = get_ZCDataset();
  if (from_scratch && settings::kTrainGMMFromScratch) {
    std::vector<Game> games = data::LoadGames();
    cluster::GaussianMixtureModel<k, kPhaseVecLength> gmm = EMForGMM<k>(games);
    SaveGMM<k>(gmm, settings::kMixtureFile);
    evaluation::SetModel(&gmm);
  }
  else {
    if (from_scratch && !settings::kTrainGMMFromScratch) {
      evaluation::LoadMixturesHardCoded();
    }
  }

  std::vector<int8_t> position_tries(dataset.size(), max_position_tries);
  std::vector<double> position_targets(dataset.size(), 0);
  std::cout << "Starting training loop" << std::endl;
  size_t milestone = 1;
//  size_t successive_failures = 0;
  while (optimizer->nu > kMinNu) {
    size_t index = rng() % dataset.size();
    if (position_tries[index] >= max_position_tries) {
      if (learning_style == kSupervised) {
        position_targets[index] = dataset[index].player_to_move_result;
      }
      else if (learning_style == kMixedLearning) {
        const double game_res_weight = 0.5;
        const double td_weight = 1 - game_res_weight;
        search::set_print_info(false);
        search::DepthSearch(dataset[index].board, 4);
        search::set_print_info(true);
        double search_target = search::get_last_search_score() / eval_scaling;
        double game_res = dataset[index].player_to_move_result;
        position_targets[index] = td_weight * Sigmoid<double>(search_target)
                                + game_res_weight * game_res;
      }
      position_tries[index] = 0;
    }
    position_tries[index]++;
    Vec<double, k> probabilities = evaluation::BoardMixtureProbability(dataset[index].board);
//    Vec<double, k> probabilities = evaluation::gmm_main->GetWeightedProbabilities(positions[index]);
//    Vec<double, k> probabilities = gmm_main.GetWeightedProbabilities(
//        GetBoardPhaseVec(positions[index]));
    if (learning_style == kSupervised && optimizer->counter < 150 * kMillion) {
      if (optimizer->counter < 50 * kMillion) {
        for (int i = 0; i < k; i++) {
          probabilities[i] = 1.0 / k;
        }
      }
      else {
        double p = (optimizer->counter - 50 * kMillion) / (100.0 * kMillion);
        double sum = 0;
        for (int i = 0; i < k; i++) {
          probabilities[i] = std::pow(probabilities[i], p);
          sum += probabilities[i];
        }
        for (int i = 0; i < k; i++) {
          probabilities[i] /= sum;
        }
      }
    }

    std::vector<double> features =
        evaluation::ScoreBoard<std::vector<double> >(dataset[index].board);
    std::vector<double> split_features(positional_features::kNumFeatures * k);
    for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        split_features[i * k + j] = features[i] * probabilities[j];
      }
    }

//    double probability_of_position = 0;//gmm_main.GetSampleProbability(
//        //GetBoardPhaseVec(positions[index]));
//    if (probability_of_position == 0) {
//      std::cout << "Error! Found impossible position!" << std::endl;
//    }

    optimizer->step(split_features, ml::Wrap(position_targets[index]));
    EnforceConstraints(optimizer->regressor.weights);
    for (size_t widx = 0; widx < settings::kNumClusters; widx++) {
      optimizer->regressor.weights[positional_features::kTempoBonusIndex * settings::kNumClusters + widx] = 100 / eval_scaling;
    }

    if (optimizer->counter % save_frequency == 0) {
      std::cout << "completed " << optimizer->counter << " iterations!"
          << std::endl;
      for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
        for (size_t j = 0; j < k; j++) {
          evaluation::SetScore(i, j, std::round(optimizer->regressor.weights[i * k + j] * eval_scaling));
//          feature_GMM_score_values[i][j] = std::round(optimizer->regressor.weights[i * k + j] * eval_scaling);
        }
      }
      evaluation::SaveGMMVariables();
      evaluation::SaveGMMVariablesHardCode("eval_weights.txt");
      if (optimizer->counter % benchmark_frequency == 0) {
        benchmark::EntropyLossNodeSuite(5000);
//        benchmark::EntropyLossTimedSuite(Milliseconds(10));
      }
    }
    if (optimizer->counter % step_size == 0) {
      optimizer->nu /= 2;
      std::cout << "New Nu: " << optimizer->nu << std::endl;
    }
    if (optimizer->counter > milestone) {
      std::cout << "Milestone " << milestone << " reached" << std::endl;
      milestone *= 10;
    }
  }
  std::cout << "Training completed!" << std::endl;
  delete optimizer;
}

template<SGDVariantType sgd_variant, LearningStyle learning_style>
void SGDTrain(bool from_scratch = false) {
  const int k = settings::kNumClusters;
  double kMinNu = (1.0 / kMillion);
  const int max_position_tries = learning_style == kSupervised ? 5 : 2;
  const long step_size = learning_style == kSupervised ? (200 * kMillion) : (max_position_tries * 1200 * kThousand);
  const long save_frequency = learning_style == kSupervised ? (1 * kMillion) : (10 * kThousand);
  const long benchmark_frequency = learning_style == kSupervised ? (500 * kMillion) : (500 * kThousand);

  ml::Optimizer<ml::kSigmoidCrossEntropy>* optimizer;
  if (sgd_variant == SGDNormal) {
    optimizer = new ml::SGD<ml::kSigmoidCrossEntropy>();
    optimizer->nu = 1.0 / eval_scaling;
  }
  else if (sgd_variant == SGDAdam) {
    ml::Adam<ml::kSigmoidCrossEntropy>* opt = new ml::Adam<ml::kSigmoidCrossEntropy>();
    opt->nu = 4.0 * (1 - opt->beta1) / eval_scaling;
    kMinNu = (1.0 / kMillion) * (1 - opt->beta2);
    optimizer = opt;
  }
  optimizer->set_num_features(k * positional_features::kNumFeatures);

  if (!from_scratch) {
    for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        optimizer->regressor.weights[i * k + j] = evaluation::GetScore(i, j) / eval_scaling;
      }
    }
  }
  std::vector<Game> games = data::LoadGames();
  if (from_scratch && settings::kTrainGMMFromScratch) {
    cluster::GaussianMixtureModel<k, kPhaseVecLength> gmm = EMForGMM<k>(games);
    SaveGMM<k>(gmm, settings::kMixtureFile);
    evaluation::SetModel(&gmm);
  }
  else {
    if (from_scratch && !settings::kTrainGMMFromScratch) {
      evaluation::LoadMixturesHardCoded();
    }
  }

  std::vector<int8_t> position_tries(games.size(), max_position_tries);
  std::vector<double> position_targets(games.size(), 0);
  std::vector<Board> positions(games.size());
  std::cout << "Starting training loop" << std::endl;
  size_t milestone = 1;
//  size_t successive_failures = 0;
  while (optimizer->nu > kMinNu) {
    size_t index = rng() % games.size();
    if (position_tries[index] >= max_position_tries) {
      bool success = SamplePositionAndTarget<learning_style>(
          games[index], positions[index], position_targets[index]);
      if (!success) {
//        successive_failures++;
        continue;
      }
//      std::cout << "Successive failures: " << successive_failures << std::endl;
//      successive_failures = 0;
      position_tries[index] = 0;
    }
    position_tries[index]++;
    Vec<double, k> probabilities = evaluation::BoardMixtureProbability(positions[index]);
//    Vec<double, k> probabilities = evaluation::gmm_main->GetWeightedProbabilities(positions[index]);
//    Vec<double, k> probabilities = gmm_main.GetWeightedProbabilities(
//        GetBoardPhaseVec(positions[index]));
    if (learning_style == kSupervised && optimizer->counter < 150 * kMillion) {
      if (optimizer->counter < 50 * kMillion) {
        for (int i = 0; i < k; i++) {
          probabilities[i] = 1.0 / k;
        }
      }
      else {
        double p = (optimizer->counter - 50 * kMillion) / (100.0 * kMillion);
        double sum = 0;
        for (int i = 0; i < k; i++) {
          probabilities[i] = std::pow(probabilities[i], p);
          sum += probabilities[i];
        }
        for (int i = 0; i < k; i++) {
          probabilities[i] /= sum;
        }
      }
    }

    std::vector<double> features =
        evaluation::ScoreBoard<std::vector<double> >(positions[index]);
    std::vector<double> split_features(positional_features::kNumFeatures * k);
    for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
      for (size_t j = 0; j < k; j++) {
        split_features[i * k + j] = features[i] * probabilities[j];
      }
    }

//    double probability_of_position = 0;//gmm_main.GetSampleProbability(
//        //GetBoardPhaseVec(positions[index]));
//    if (probability_of_position == 0) {
//      std::cout << "Error! Found impossible position!" << std::endl;
//    }

    optimizer->step(split_features, ml::Wrap(position_targets[index]));
    EnforceConstraints(optimizer->regressor.weights);

    if (optimizer->counter % save_frequency == 0) {
      std::cout << "completed " << optimizer->counter << " iterations!"
          << std::endl;
      for (size_t i = 0; i < positional_features::kNumFeatures; i++) {
        for (size_t j = 0; j < k; j++) {
          evaluation::SetScore(i, j, std::round(optimizer->regressor.weights[i * k + j] * eval_scaling));
//          feature_GMM_score_values[i][j] = std::round(optimizer->regressor.weights[i * k + j] * eval_scaling);
        }
      }
      evaluation::SaveGMMVariables();
      evaluation::SaveGMMVariablesHardCode("eval_weights.txt");
      if (optimizer->counter % benchmark_frequency == 0) {
        benchmark::EntropyLossNodeSuite(5000);
//        benchmark::EntropyLossTimedSuite(Milliseconds(10));
      }
    }
    if (optimizer->counter % step_size == 0) {
      optimizer->nu /= 2;
      std::cout << "New Nu: " << optimizer->nu << std::endl;
    }
    if (optimizer->counter > milestone) {
      std::cout << "Milestone " << milestone << " reached" << std::endl;
      milestone *= 10;
    }
  }
  std::cout << "Training completed!" << std::endl;
  delete optimizer;
}

void Train(bool from_scratch) {
  if (settings::kTrainFromScratch) {
    SGDTrainMLSet<SGDAdam, kMixedLearning>(from_scratch);
//    SGDTrainZCSet<SGDAdam, kMixedLearning>(from_scratch);

//    SGDTrain<SGDAdam, kMixedLearning>(from_scratch);
//    SGDTrain<SGDNormal, kSupervised>(from_scratch);
  }
  else {
    SGDTrainMLSet<SGDAdam, kMixedLearning>();
//    SGDTrainZCSet<SGDAdam, kMixedLearning>();

    //SGDMarginVariantTrain<settings::kGMMk, SGDNormal>(false);
//    SGDTrain<SGDAdam, kMixedLearning>();
    //SGDVariantTDL<SGDAdam, false>();
  }
}

}
