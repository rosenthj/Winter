/*
 * wdl_score.h
 *
 *  Created on: May 28, 2020
 *      Author: jonathan
 */

#ifndef SRC_GENERAL_WDL_SCORE_H_
#define SRC_GENERAL_WDL_SCORE_H_

#include <cstdint>
#include <cassert>
#include <cmath>
#include <string>

struct WDLScore {
  static constexpr int32_t scale = 4000;

  int32_t win;
  int32_t win_draw;

  constexpr float to_wpct() const { return static_cast<float>(win + win_draw) / (2* scale); }
  constexpr int32_t to_nscore() const { return value() - scale; }
  int32_t to_cp() const;

  constexpr float get_win_probability() const { return win / static_cast<float>(scale); }
  constexpr float get_draw_probability() const { return (win_draw - win) / static_cast<float>(scale); }
  constexpr float get_win_draw_probability() const { return win_draw / static_cast<float>(scale); }
  constexpr float get_loss_probability() const { return 1.0 - get_win_draw_probability(); }

  std::string get_uci_string() const {
    std::string output = "wdl";
    output += " " + std::to_string(win / 4);
    output += " " + std::to_string((win_draw - win) / 4);
    output += " " + std::to_string((scale - win_draw) / 4);
    return output;
  }

  bool is_mate_score() const;
  constexpr bool is_static_eval() const {
    return win >= 0 && win <= scale && win_draw >= 0 && win_draw <= scale;
  }
  bool is_valid() const;

  // Returns whether score is expected worse than equal.
  constexpr bool is_disadvantage() const {
    return (win + win_draw) < scale;
  }

  constexpr bool operator==(const WDLScore other) const { return win == other.win && win_draw == other.win_draw; }
  constexpr bool operator!=(const WDLScore other) const { return win != other.win || win_draw != other.win_draw; }
  constexpr bool operator>(const WDLScore other) const { return value() > other.value(); }
  constexpr bool operator<(const WDLScore other) const { return value() < other.value(); }
  constexpr bool operator>=(const WDLScore other) const { return value() >= other.value(); }
  constexpr bool operator<=(const WDLScore other) const { return value() <= other.value(); }
  WDLScore& operator*=(int32_t value) {
    win *= value;
    win_draw *= value;
    return *this;
  }
  constexpr WDLScore operator*(int32_t value) const { return WDLScore{win * value, win_draw * value}; }
  constexpr WDLScore operator/(int32_t value) const { return WDLScore{win / value, win_draw / value}; }
  constexpr WDLScore operator-() const {
    return WDLScore{scale - win_draw, scale - win};
  }
  constexpr WDLScore operator+(const WDLScore other) const { return WDLScore{win + other.win, win_draw + other.win_draw}; }
  constexpr WDLScore operator-(const WDLScore other) const { return WDLScore{win - other.win, win_draw - other.win_draw}; }

  WDLScore get_next_score() const;
  WDLScore get_previous_score() const;
  WDLScore get_valid_score() const;

  constexpr int32_t value() const { return win + win_draw; }
  constexpr int32_t value(int32_t contempt_factor) const {
//    assert(contempt_factor >= 0);
//    assert(contempt_factor <= 100);
    return contempt_factor * win + (100-contempt_factor) * win_draw;
  }
  constexpr bool is_draw() const { return win == 0 && win_draw == scale; }
  constexpr bool is_win() const { return win >= scale; }
  constexpr bool is_loss() const { return win_draw == 0; }

//  static constexpr WDLScore from_score(int32_t score);
  static constexpr WDLScore from_pct_truncated(const float win, const float win_draw) {
    return WDLScore{static_cast<int32_t>(win * scale),
                    static_cast<int32_t>(win_draw * scale)};
  }

  static WDLScore from_pct(const float win, const float win_draw) {
    return WDLScore{static_cast<int32_t>(std::round(win * scale)),
                    static_cast<int32_t>(std::round(win_draw * scale))};
  }
  static WDLScore mated_on_ply(int32_t ply);
  void print() const;
};

inline bool contempt_greater(const WDLScore lhs, const WDLScore rhs,
                                       int32_t contempt_factor) {
  assert(contempt_factor >= 0);
  assert(contempt_factor <= 100);
  return (100 - contempt_factor) * static_cast<int32_t>(lhs.win_draw - rhs.win_draw)
          + contempt_factor * static_cast<int32_t>(lhs.win - rhs.win) > 0;
}

//constexpr Score kRescale = 4000; // Used to transform wpct into Score.
//constexpr WDLScore kMaxStaticEval = WDLScore(1.0, 1.0);
//constexpr Score kMinStaticEval = WDLScore(0.0, 0.0);
//
//constexpr Score kMinMatingScore = kMaxStaticEval + 100;
//constexpr Score kMaxMatedScore = -kMinMatingScore;
//
//constexpr Score kNumMateInScores = 2000;
//constexpr Score kMaxScore = kMinMatingScore + kNumMateInScores;
//constexpr Score kMinScore = -kMaxScore;


#endif /* SRC_GENERAL_WDL_SCORE_H_ */
