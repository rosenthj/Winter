#include "data.h"
#include "net_evaluation.h"
#include "general/magic.h"
#include "general/types.h"
#include "incbin/incbin.h"

#include <random>
#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

INCBIN(float_t, NetWeights, "f160rS02_ep8.bin");


// NN types
constexpr size_t block_size = 160;
using NetLayerType = Vec<float, block_size>;
using FNetLayerType = Vec<SIMDFloat, block_size>;

constexpr bool use_pk_net = false;
constexpr size_t pk_l2_layer_size = use_pk_net ? block_size : 0;

std::array<int32_t, 2> contempt = { 0, 0 };

namespace pawn_hash {

// long hits = 0;
// long misses = 0;

struct PawnEntry {
  NetLayerType output;
  HashType hash;
};

size_t size = 10000;

std::vector<PawnEntry> table(size);

PawnEntry GetEntry(const HashType hash_p) {
  return table[hash_p % table.size()];
}
void SaveEntry(const NetLayerType &output, const HashType hash_p) {
  PawnEntry entry;
  entry.output = output;
  entry.hash = hash_p ^ (HashType)std::round(output[0] * 1024);
  table[hash_p % table.size()] = entry;
}

bool ValidateHash(const PawnEntry &entry, const HashType hash_p) {
  return hash_p == (entry.hash ^ (HashType)std::round(entry.output[0] * 1024));
}

}

namespace {

//inline float sigmoid(float x) {
//  return 1 / (1 + std::exp(-x));
//}

// PK weights

NetLayerType pk_layer_one_bias(0);
std::vector<NetLayerType> pk_layer_two_weights(pk_l2_layer_size, 0);

// NN weights

std::vector<NetLayerType> net_input_weights(772, 0);
NetLayerType bias_layer_one(0);

//std::vector<NetLayerType> second_layer_weights(16 * 16, 0);
//NetLayerType bias_layer_two(0);

std::vector<NetLayerType> output_weights;
std::array<float_t, 3> output_bias;

template<typename T>
T init() {
  return T(0);
}

template<typename T> inline
void AddFeature(T &s, const int index, const int value) {
  s[index] += value;
}

template<> inline void AddFeature<NetLayerType>(NetLayerType &s, const int index, const int value) {
  s.FMA(net_input_weights[index], value);
}

template<typename T> inline
void AddFeature(T &s, const int index) {
  assert(index >= 0);
  assert(index < 772);
  s[index]++;
}

template<> inline void AddFeature<NetLayerType>(NetLayerType &s, const int index) {
  assert(index >= 0);
  assert(index < 772);
  s += net_input_weights[index];
}

template<typename T, Color color, Color our_color>
inline void AddPieceType(T &score, const Board &board, const PieceType pt) {
  constexpr size_t c_offset = color == our_color ? 0 : 64 * 6;
  const size_t offset = 64 * pt + c_offset;
  
  for (BitBoard pieces = board.get_piece_bitboard(color, pt); pieces; bitops::PopLSB(pieces)) {
    Square piece_square = bitops::NumberOfTrailingZeros(pieces);
    if (our_color == kBlack) {
        piece_square = GetMirroredSquare(piece_square);
    }
    //score += net_input_weights[piece_square + offset];
    AddFeature<T>(score, piece_square + offset);
  }
}

template<typename T, Color color, Color our_color>
inline void ScorePieces(T &score, const Board &board) {
  for (PieceType piece_type = kKnight; piece_type <= kQueen; ++piece_type) {
      AddPieceType<T, color, our_color>(score, board, piece_type);
      //std::cout << score[0] << std::endl;
  }
}

template<typename T, Color color>
inline void ScoreCastlingRights(T &score, const Board &board) {
  constexpr int offset_us = color == kWhite ? 0 : 2;
  constexpr int offset_them = color == kWhite? 2 : 0;
  constexpr size_t base_castling_right_idx = 12 * 64;
  
  const CastlingRights castling_rights = board.get_castling_rights();
  if (castling_rights & (kWLCastle << offset_us)) {
      AddFeature<T>(score, base_castling_right_idx + 0);
  }
  if (castling_rights & (kWSCastle << offset_us)) {
      AddFeature<T>(score, base_castling_right_idx + 1);
  }
    if (castling_rights & (kWLCastle << offset_them)) {
      AddFeature<T>(score, base_castling_right_idx + 2);
  }
  if (castling_rights & (kWSCastle << offset_them)) {
      AddFeature<T>(score, base_castling_right_idx + 3);
  }
}

template<typename T, Color our_color>
inline T _ScorePawnsAndKings(const Board &board) {
  T score = init<T>();
  AddPieceType<T, kWhite, our_color>(score, board, kPawn);
  AddPieceType<T, kWhite, our_color>(score, board, kKing);
  AddPieceType<T, kBlack, our_color>(score, board, kPawn);
  AddPieceType<T, kBlack, our_color>(score, board, kKing);
  return score;
}

template<typename T>
inline T ScorePawnsAndKings(const Board &board) {
  if (board.get_turn() == kWhite) {
    return _ScorePawnsAndKings<T,kWhite>(board);
  }
  return _ScorePawnsAndKings<T,kBlack>(board);
}

}

namespace net_evaluation {

void SetPHashSize(const size_t bytes) {
  pawn_hash::size = bytes / 72;
  pawn_hash::table.resize(pawn_hash::size);
}

template<typename T, Color our_color>
T ScoreBoard(const Board &board) {
  T score = init<T>();

  // Piece evaluations
  ScorePieces<T, kWhite, our_color>(score, board);
  ScorePieces<T, kBlack, our_color>(score, board);
  
  ScoreCastlingRights<T, our_color>(score, board);

  return score;
}

//Score NetForward(NetLayerType &layer_one) {
  //layer_one += bias_layer_one;
  //layer_one.relu();

  //NetLayerType layer_two = bias_layer_two;
  //for (size_t i = 0; i < layer_one.size(); ++i) {
    //layer_two.FMA(second_layer_weights[i], layer_one[i]);
  //}
  //layer_two.relu();

  //float win = layer_two.dot(win_weights) + win_bias;
  //float win_draw = layer_two.dot(win_draw_weights) + win_draw_bias;

  ////float wpct = sigmoid(win) * c + sigmoid(win_draw) * (1 - c);

  ////return wpct_to_score(wpct);
  //return WDLScore::from_pct_valid(sigmoid(win), sigmoid(win_draw));
//}

NetLayerType PKNetForward(NetLayerType &pk_layer_one) {
  if (!use_pk_net) {
    return pk_layer_one;
  }
  pk_layer_one += pk_layer_one_bias;
  pk_layer_one.relu();
  
  NetLayerType layer_two = NetLayerType(0);
  for (size_t i = 0; i < pk_layer_one.size(); ++i) {
    layer_two.FMA(pk_layer_two_weights[i], pk_layer_one[i]);
    // layer_two[i] = pk_layer_one.dot(pk_layer_two_weights[i]);
  }
  
  return layer_two;
}

Score NetForward(NetLayerType &layer_one) {
  layer_one += bias_layer_one;
  layer_one.relu();
  
  float_t sum = 0;
  std::array<float_t, 3> outcomes;
  for (size_t i = 0; i < 3; ++i) {
      outcomes[i] = layer_one.dot(output_weights[i]) + output_bias[i];
      outcomes[i] = std::exp(outcomes[i]);
      sum += outcomes[i];
  }
  // std::cout << sum << std::endl;
  
  for (size_t i = 0; i < 3; ++i) {
      outcomes[i] /= sum;
  }

  //NetLayerType layer_two = bias_layer_two;
  //for (size_t i = 0; i < layer_one.size(); ++i) {
  //  layer_two.FMA(second_layer_weights[i], layer_one[i]);
  //}
  //layer_two.relu();
  
  // std::cout << outcomes[0] << " " << outcomes[1] << " " << outcomes[2] << std::endl;

  float win = outcomes[0];
  float win_draw = outcomes[0] + outcomes[1];
  return WDLScore::from_pct_valid(win, win_draw);

  //float wpct = sigmoid(win) * c + sigmoid(win_draw) * (1 - c);

  //return wpct_to_score(wpct);
  //return WDLScore::from_pct_valid(sigmoid(win), sigmoid(win_draw));
}

Score ScoreBoard(const Board &board) {
  //const EvalConstants ec(board);
  HashType p_hash = board.get_pawn_hash();
  pawn_hash::PawnEntry entry = pawn_hash::GetEntry(p_hash);
  NetLayerType cnn_out;
  if (pawn_hash::ValidateHash(entry, p_hash)) {
    cnn_out = entry.output;
  }
  else {
    NetLayerType pk_in = ScorePawnsAndKings<NetLayerType>(board);
    cnn_out = PKNetForward(pk_in);
    pawn_hash::SaveEntry(cnn_out, p_hash);
  }

  NetLayerType layer_one = init<NetLayerType>();
  if (board.get_turn() == kWhite) {
    layer_one = ScoreBoard<NetLayerType, kWhite>(board);
  }
  else {
    layer_one = ScoreBoard<NetLayerType, kBlack>(board);
  }
  layer_one += cnn_out;
  if (contempt[board.get_turn()] != 0) {
    return AddContempt(NetForward(layer_one), board.get_turn());
  }
  return NetForward(layer_one);
}

std::vector<NetLayerType> load_weights(size_t in_size, size_t &offset) {
  std::vector<NetLayerType> weights = std::vector<NetLayerType>(in_size);
  for (size_t i = 0; i < in_size; ++i) {
    for (size_t k = 0; k < block_size; ++k) {
      weights[i][k] = gNetWeightsData[offset + i + in_size * k];
    }
  }
  offset += in_size * block_size;
  return weights;
}

void init_weights() {
  // Init regular net weights
  
  size_t offset = 0;
  
  if (use_pk_net) {
    std::vector<NetLayerType> l1_weights = load_weights(8*64 + 4, offset);
    
    for (size_t k = 0; k < block_size; ++k) {
      bias_layer_one[k] = gNetWeightsData[offset+k];
    }
    offset += block_size;
    
    // PK l1 weights
    std::vector<NetLayerType> pk_l1_weights = load_weights(4*64, offset);
    net_input_weights = std::vector<NetLayerType>(772);
    for (PieceType pt = kPawn; pt <= kKing; ++pt) {
      for (Color color = kWhite; color <= kBlack; ++color) {
        for (Square square = 0; square < 64; ++square) {
          size_t idx = (pt + 6 * color) * 64 + square;
          if (pt == kKing || pt == kPawn) {
            net_input_weights.at(idx) = pk_l1_weights.at(((pt/kKing)  + 2 * color) * 64 + square);
          }
          else {
            net_input_weights.at(idx) = l1_weights.at(((pt-1) + 4 * color) * 64 + square);
          }
        }
      }
    }
    for (size_t i=0; i < 4; ++i) {
      net_input_weights.at(12*64+i) = l1_weights.at(8*64+i);
    }
    
    // PK l1 bias
    for (size_t k = 0; k < block_size; ++k) {
      pk_layer_one_bias[k] = gNetWeightsData[offset+k];
    }
    offset += block_size;
    
    // PK l2 weights
    pk_layer_two_weights = std::vector<NetLayerType>(block_size);
    for (size_t i = 0; i < block_size; ++i) {
      for (size_t k = 0; k < block_size; ++k) {
        pk_layer_two_weights[i][k] = gNetWeightsData[offset + i + block_size * k];
      }
    }
    //std::cout << pk_layer_two_weights[0][0] << " " << pk_layer_two_weights[0][1] << std::endl;
    offset += block_size * block_size;
  }
  else {
    // L1 Weights
    net_input_weights = load_weights(772, offset);
  
    // L1 Bias
    for (size_t k = 0; k < block_size; ++k) {
      bias_layer_one[k] = gNetWeightsData[offset+k];
    }
    offset += block_size;
  }
  
  
  // Output Weights
  output_weights = std::vector<NetLayerType>(3);
  for (size_t i = 0; i < block_size; ++i) {
    //size_t j = i * block_size;
    for (size_t k = 0; k < 3; ++k) {
      //output_weights[i][k] = gNetWeightsData[offset+j+k];
      output_weights[k][i] = gNetWeightsData[offset+i+block_size*k];
    }
  }
  offset += 3 * block_size;
  
  // Output Bias
  for (size_t k = 0; k < 3; ++k) {
    output_bias[k] = gNetWeightsData[offset+k];
  }
  
  //std::cout << "l1 w0:" << net_input_weights[0][0] << " l1 w1:" << net_input_weights[0][1]
  //          << " l1 b0:" << bias_layer_one[0] << " l1 b1:" << bias_layer_one[1] << std::endl;
  //std::cout << "out w0:" << output_weights[0][0] << " out w1:" << output_weights[0][1]
  //          << " out b0:" << output_bias[0] << " out b1:" << output_bias[1] << std::endl;
}

std::vector<int32_t> GetNetInputs(const Board &board) {
  if (board.get_turn() == kWhite) {
    return ScoreBoard<std::vector<int32_t>, kWhite>(board);
  }
  return ScoreBoard<std::vector<int32_t>, kBlack>(board);
}

#ifdef EVAL_TRAINING
void AddHeader(std::ofstream &file, std::string feature_name_prefix, int feature_count) {
  file << feature_name_prefix << "0";
  for (int i = 1; i < feature_count; ++i) {
    file << "," << feature_name_prefix << i;
  }
  file << std::endl;
}

void AddGame(const Game &game, std::ofstream &res_file, std::ofstream &dynamic_features_file,
             std::ofstream &static_features_file) {
  WDLScore result = game.result;
  if (game.board.get_turn() == kBlack) {
    result = -result;
  }
  res_file << result.get_win_probability() << ","
      << result.get_win_draw_probability() << std::endl;

  std::vector<int> dynamic_features = GetNetInputs(game.board);
  dynamic_features_file << dynamic_features[0];
  for (int i = 1; i < dynamic_features.size(); ++i) {
    dynamic_features_file << "," << dynamic_features[i];
  }
  dynamic_features_file << std::endl;

  std::vector<int> static_features = GetCNNInputs(game.board);
  static_features_file << static_features[0];
  for (int i = 1; i < static_features.size(); ++i) {
    static_features_file << "," << static_features[i];
  }
  static_features_file << std::endl;
}

void StoreEvalDataset(const std::vector<Game> &games, std::string out_file_name) {
  std::ofstream res_file(out_file_name + ".res.csv");
  AddHeader(res_file, "result_feature_", 2);
  std::ofstream dynamic_features_file(out_file_name + ".dynamic.csv");
  AddHeader(dynamic_features_file, "dy_fe", kTotalNumFeatures);
  std::ofstream static_features_file(out_file_name + ".static.csv");
  AddHeader(static_features_file, "st_fe", kBoardSize * kNumChannels);

  Game game_start;
  game_start.board.SetStartBoard();
  game_start.result = WDLScore::from_pct(0.0, 1.0);
  AddGame(game_start, res_file, dynamic_features_file, static_features_file);

  size_t samples = 1;
  for (Game game : games) {
    AddGame(game, res_file, dynamic_features_file, static_features_file);
    samples++;
    if (samples % 10000 == 0) {
      std::cout << "Processed " << samples << " samples!" << std::endl;
    }
  }

  res_file.flush();
  res_file.close();
  dynamic_features_file.flush();
  dynamic_features_file.close();
  static_features_file.flush();
  static_features_file.close();
}


void StoreEvalDatasetOld(const std::vector<Game> &games, std::string out_file_name) {
  std::ofstream dfile(out_file_name);
  size_t samples = 0;
  if (true) {
    Board board;
    std::vector<int> features = GetNetInputs(board);
    if (samples == 0) {
      dfile << "win,windraw";
      for (size_t i = 0; i < features.size(); i++) {
        dfile << ",fe" << i;
      }
      dfile << std::endl;
    }
    dfile << "0, 1";
    for (int feature : features) {
      dfile << ", " << feature;
    }
    dfile << std::endl;
    samples++;
  }

  for (Game game : games) {
    WDLScore result = game.result;
    if (game.board.get_turn() == kBlack) {
      result = -result;
    }
    std::vector<int> features = GetNetInputs(game.board);
    if (samples == 0) {
      dfile << "win,windraw";
      for (size_t i = 0; i < features.size(); i++) {
        dfile << ",fe" << i;
      }
      dfile << std::endl;
    }
    dfile << result.get_win_probability() << ", " << result.get_win_draw_probability();
    for (int feature : features) {
      dfile << ", " << feature;
    }
    dfile << std::endl;
    samples++;
    if (samples % 10000 == 0) {
      std::cout << "Processed " << samples << " samples!" << std::endl;
    }
  }
  dfile.flush();
  dfile.close();
}

void RerollCommonFeatureGames(std::vector<Game> &games, size_t reroll_pct) {
  reroll_pct = reroll_pct % 100;
  if (reroll_pct > 0) {
    std::vector<double> feature_counts(kTotalNumFeatures, 1);
    for (const Game &game : games) {
      const auto features = GetNetInputs(game.board);
      assert(features.size() == feature_counts.size());
      for (size_t idx = 0; idx < features.size(); ++idx) {
        feature_counts[idx] += (features[idx] != 0);
      }
    }
    std::cout << "Got feature counts" << std::endl;
    std::vector<double> feature_values(kTotalNumFeatures, 0);
    for (size_t idx = 0; idx < feature_counts.size(); ++idx) {
      feature_values[idx] = 1.0 / (feature_counts[idx] / games.size());
    }
    std::cout << "Estimated feature values" << std::endl;
    std::vector<double> game_value_estimate(games.size(), 0);
    for (size_t game_idx = 0; game_idx < games.size(); ++game_idx) {
      const auto features = GetNetInputs(games[game_idx].board);
      for (size_t idx = 0; idx < features.size(); ++idx) {
        game_value_estimate[game_idx] += (features[idx] != 0) * feature_values[idx];
      }
    }
    std::cout << "Estimated game values" << std::endl;
    std::vector<double> values(game_value_estimate.size(), 0);
    for (size_t idx = 0; idx < values.size(); ++idx) {
      values[idx] = game_value_estimate[idx];
    }
    std::cout << "Copied games" << std::endl;
    std::cout << "Values size: " << values.size() << std::endl;
    std::sort(values.begin(), values.end(), std::less<double>());
    std::cout << "Sorted game values" << std::endl;
    double threshold = values[(reroll_pct * games.size()) / 100];
    std::cout << "Threshold: " << threshold << std::endl;
    size_t reroll_count = 0;
    for (size_t game_idx = 0; game_idx < games.size(); ++game_idx) {
      if (game_value_estimate[game_idx] < threshold) {
        data::SetGameToRandomQuiescent(games[game_idx]);
        reroll_count++;
        if (reroll_count % 10000 == 0) {
          std::cout << "rerolled " << reroll_count << " positions" << std::endl;
        }
      }
    }
  }
}


std::vector<Game> LoadTrainingGamesForTraining(std::string filename, size_t reroll_pct) {
  std::vector<Game> games = data::LoadGames(30000000, filename);
  data::SetGamesToRandomQuiescent(games);
  for (int i = 0; i < 5; i++) {
//    RerollCommonFeatureGames(games, reroll_pct);
  }
  return games;
}

void AddArasanEPDs(std::vector<Game> &games) {
  std::string line;
  std::ifstream file("lichess-new-labeled.epd");

  while(std::getline(file, line)) {
    std::vector<std::string> tokens = parse::split(line, ' ');
    std::string fen = tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + tokens[3];
    Board board(fen);
    if (board.InCheck()) {
      continue;
    }
    Game game;
    game.board = board;
    if (tokens[8].compare("\"1.000\";") == 0) {
      game.result = WDLScore::from_pct(1.0, 1.0);
    }
    else if (tokens[8].compare("\"0.000\";") == 0) {
      game.result = WDLScore::from_pct(0.0, 0.0);
    }
    else if (tokens[8].compare("\"0.500\";") == 0) {
      game.result = WDLScore::from_pct(0.0, 1.0);
    }
    else {
      std::cout << "Error detected! Line:" << std::endl;
      std::cout << line << std::endl;
      file.close();
      return;
    }
    games.emplace_back(game);
    if (games.size() % 10000 == 0) {
      std::cout << "Loaded " << games.size() << " samples!" << std::endl;
    }
  }
  file.close();
}

void AddZCEPDs(std::vector<Game> &games) {
  std::string line;
  std::ifstream file("quiet-labeled.epd");

  while(std::getline(file, line)) {
    std::vector<std::string> tokens = parse::split(line, ' ');
    std::string fen = tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + tokens[3];
    Board board(fen);
    if (board.InCheck()) {
      continue;
    }
    Game game;
    game.board = board;
    if (tokens[5].compare("\"1-0\";") == 0) {
      game.result = WDLScore::from_pct(1.0, 1.0);
    }
    else if (tokens[5].compare("\"0-1\";") == 0) {
      game.result = WDLScore::from_pct(0.0, 0.0);
    }
    else if (tokens[5].compare("\"1/2-1/2\";") == 0) {
      game.result = WDLScore::from_pct(0.0, 1.0);
    }
    else {
      std::cout << "Error detected! Line:" << std::endl;
      std::cout << line << std::endl;
      file.close();
      return;
    }
    games.emplace_back(game);
    if (games.size() % 10000 == 0) {
      std::cout << "Loaded " << games.size() << " samples!" << std::endl;
    }
  }
  file.close();
}

void GenerateDatasetFromEPD() {

  std::vector<Game> games = LoadTrainingGamesForTraining("Games.ucig", 3000000);
  std::cout << "Added positions from Games.ucig: " << games.size() << std::endl;
  AddZCEPDs(games);
  std::cout << "Added Zurichess EPDs. Position count: " << games.size() << std::endl;
  AddArasanEPDs(games);
  std::cout << "Added Lichess EPDs from Arasan. Position count: " << games.size() << std::endl;

  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(games.begin(), games.end(), g);

  StoreEvalDataset(games, "eval_dataset");
}

void GenerateDatasetFromUCIGames(std::string filename, std::string out_name, size_t reroll_pct) {
  std::vector<Game> games = LoadTrainingGamesForTraining(filename, reroll_pct);
  StoreEvalDataset(games, out_name);
}

void EstimateFeatureImpact() {
  std::cout << "Currently EstimateFeatureImpact() is disabled!" << std::endl;
//  std::string line;
//  std::ifstream file("quiet-labeled.epd");
//  std::vector<int64_t> feature_sums(kTotalNumFeatures, 0);
//  std::vector<int64_t> score_difference_sums(kTotalNumFeatures, 0);
//  size_t count = 0;
//  std::vector<int64_t> f_counts(kTotalNumFeatures, 0);
//  while(std::getline(file, line)) {
//    std::vector<std::string> tokens = parse::split(line, ' ');
//    std::string fen = tokens[0] + " " + tokens[1] + " " + tokens[2] + " " + tokens[3];
//    Board board(fen);
//    const EvalConstants ec(board);
//    NetLayerType layer_one;
//    if (board.get_turn() == kWhite) {
//      layer_one = ScoreBoard<NetLayerType, kWhite>(board, ec);
//    }
//    else {
//      layer_one = ScoreBoard<NetLayerType, kBlack>(board, ec);
//    }
//    const Score eval = NetForward(layer_one);
//    std::vector<int32_t> features = GetNetInputs(board);
//    for (size_t i = 0; i < kTotalNumFeatures; ++i) {
//      if (!features[i]) {
//        continue;
//      }
//      feature_sums[i] += features[i];
//      f_counts[i] += features[i] > 0;
//      if (i >= kSideDependentFeatureCount) {
//        continue;
//      }
//      NetLayerType modified_input(0);
//      for (size_t j = 0; j < modified_input.size(); ++j) {
//        modified_input[j] = layer_one[j] - features[i] * net_input_weights[i][j];
//        modified_input[j] -= features[i+kSideDependentFeatureCount] * net_input_weights[i+kSideDependentFeatureCount][j];
//      }
//      score_difference_sums[i] += std::abs(eval - NetForward(modified_input));
//    }
//    count++;
//    if (count % 10000 == 0) {
//      std::cout << "Loaded " << count << " samples!" << std::endl;
//    }
//  }
//  file.close();
//
//  for (size_t i = 0; i < kTotalNumFeatures; ++i) {
//    float feature_weight_sums = 0;
//    for (size_t j = 0; j < net_input_weights[i].size(); ++j) {
//      feature_weight_sums += std::abs(net_input_weights[i][j]);
//    }
//    std::cout << "Feature: " << i << " weight: " << feature_weight_sums << " count: " << f_counts[i]
//              << " weighted count: " << (feature_sums[i]*feature_weight_sums / f_counts[i]);
//    if (i < kSideDependentFeatureCount) {
//      std::cout << " weighted score diff: " << ((1.0 * score_difference_sums[i]) / f_counts[i]);
//    }
//    std::cout << std::endl;
//  }
}
#endif

void SetContempt(Color color, int32_t value) {
  contempt[color] = value;
  contempt[other_color(color)] = -value;
  if (value == 0) {
    contempt[other_color(color)] = 0;
  }
}

std::array<Score, 2> GetDrawArray() {
  if (contempt[kWhite] == 0) {
    return std::array<Score, 2> { kDrawScore, kDrawScore };
  }
  return std::array<Score, 2> { AddContempt(kDrawScore, kWhite), AddContempt(kDrawScore, kBlack) };
}

Score AddContempt(Score score, Color color) {
  assert(score.is_static_eval());
  int32_t diff = score.win_draw - score.win;
  if (contempt[color] > 0) { // Contempt is positive, draws are counted as losses
    diff = (diff * contempt[color]) / 100;
    return WDLScore { score.win, score.win_draw - diff };
  }
  // contempt is negative, draws are counted as wins
  diff = -(diff * contempt[color]) / 100;
  return WDLScore { score.win + diff, score.win_draw};
}

Score RemoveContempt(Score score, Color color) {
  if (!score.is_static_eval() || contempt[color] == 0
      || contempt[color] >= 100 || contempt[color] <= -100) {
    return score;
  }
  int32_t diff = score.win_draw - score.win;
  if (contempt[color] >= 0) {
    int32_t orig_diff = (diff * 100) / (100 - contempt[color]);
    diff = orig_diff - diff;
    return WDLScore { score.win, score.win_draw + diff };
  }
  int32_t orig_diff = (diff * 100) / (100 + contempt[color]);
  diff = orig_diff - diff;
  return WDLScore { score.win - diff, score.win_draw };
}

}
