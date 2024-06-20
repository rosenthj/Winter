/*
 * wdl_score.h
 *
 *  Created on: May 28, 2020
 *      Author: jonathan
 */

#ifndef SRC_GENERAL_WDL_SCORE_H_
#define SRC_GENERAL_WDL_SCORE_H_

#include <algorithm>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

struct WDLScore {
  static constexpr int32_t scale = 4000;
  static constexpr int32_t kNumMateInScores = 2000;

  int32_t win;
  int32_t win_draw;

  constexpr float to_wpct() const { return static_cast<float>(win + win_draw) / (2* scale); }
  constexpr int32_t to_nscore() const { return value() - scale; }
  int32_t to_cp() const {
    constexpr float kEpsilon = 0.000001;
    float_t wpct = std::clamp(to_wpct(), kEpsilon,  1-kEpsilon);
    return std::round(std::log(wpct / (1-wpct)) * 1024);
  }

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

  constexpr bool is_static_eval() const {
    return win >= 0 && win <= scale && win_draw >= 0 && win_draw <= scale;
  }

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
  
  static constexpr WDLScore get_no_score() { return WDLScore{-1, scale + 1}; }
  static constexpr WDLScore get_draw_score() { return from_pct_truncated(0.0, 1.0); }
  
  static constexpr WDLScore get_max_static() { return from_pct_truncated(1.0, 1.0); }
  static constexpr WDLScore get_min_static() { return -get_max_static(); }
  static constexpr WDLScore get_min_mating() { return WDLScore{get_max_static().win + 100, get_max_static().win_draw + 100}; }
  static constexpr WDLScore get_max_mated() { return -get_min_mating(); }
  
  static constexpr WDLScore get_max_score() {
    return WDLScore{get_min_mating().win + kNumMateInScores,
                    get_min_mating().win_draw + kNumMateInScores}; }
  static constexpr WDLScore get_min_score() { return -get_max_score(); }

  constexpr WDLScore get_next_score() const {
    if (win == scale && win_draw == scale) {
      return get_min_mating();
    }
    if (win == get_max_mated().win) {
      assert(win == win_draw);
      return get_min_static();
    }
    if (is_mate_score()) {
      return WDLScore{win + 1, win_draw + 1};
    }
    if (win_draw == scale) {
      return WDLScore{win + 1, win_draw};
    }
    return WDLScore{win, win_draw + 1};
  }
  
  constexpr WDLScore get_previous_score() const {
    if (win == get_min_mating().win) {
      assert(win == win_draw);
      return get_max_static();
    }
    if (win == 0 && win_draw == 0) {
      return get_max_mated();
    }
    if (is_mate_score()) {
      return WDLScore{win - 1, win_draw - 1};
    }
    if (win == 0) {
      return WDLScore{win, win_draw - 1};
    }
    return WDLScore{win - 1, win_draw};
  }

  constexpr int32_t value() const { return win + win_draw; }
  constexpr int32_t value(int32_t contempt_factor) const {
//    assert(contempt_factor >= 0);
//    assert(contempt_factor <= 100);
    return contempt_factor * win + (100-contempt_factor) * win_draw;
  }
  constexpr bool is_draw() const { return win == 0 && win_draw == scale; }
  constexpr bool is_win() const { return win >= scale; }
  constexpr bool is_loss() const { return win_draw == 0; }
  
  constexpr bool is_mate_score() const {
    return (win == win_draw) && ((win <= get_max_mated().win && win >= get_min_score().win)
        || (win >= get_min_mating().win && win <= get_max_score().win));
  }
  
  constexpr bool is_valid() const {
    return is_static_eval() || is_mate_score() ||
        (win == get_no_score().win && win_draw == get_no_score().win_draw);
  }

//  static constexpr WDLScore from_score(int32_t score);
  static constexpr WDLScore from_pct_truncated(const float win, const float win_draw) {
    return WDLScore{static_cast<int32_t>(win * scale),
                    static_cast<int32_t>(win_draw * scale)};
  }
  
  static constexpr WDLScore from_pct_valid(const float win, const float win_draw) {
    if (win <= win_draw) {
      return from_pct(win, win_draw);
    }
    float win_ = win / (win + (1-win_draw));
    return from_pct(win_, win_);
  }

  static constexpr WDLScore from_pct(const float win, const float win_draw) {
    return WDLScore{static_cast<int32_t>(std::round(win * scale)),
                    static_cast<int32_t>(std::round(win_draw * scale))};
  }
  constexpr WDLScore get_valid_score() const {
    int32_t w = win, wd = win_draw;

    // Enforce w <= wd constraint
    if (w > wd) {
      wd = (w + wd) / 2;
      w = wd;
    }

    if (wd <= get_min_score().win_draw) {
      return get_min_score();
    }
    if (wd <= get_max_mated().win_draw) {
      return WDLScore{wd, wd}; // Valid mate score
    }

    // Enforce w >= min static eval
    if (w < get_min_static().win) {
      wd -= get_min_static().win - w;
      w = get_min_static().win;
    }
    if (wd < get_min_static().win_draw) {
      return get_min_static();
    }
    if (wd < get_max_static().win_draw) {
      return WDLScore{w, wd};
    }
    if (wd < get_min_mating().win_draw) {
      w += wd - get_max_static().win_draw;
      wd = get_max_static().win_draw;
      if (w <= get_max_static().win) {
        return WDLScore{w, wd};
      }
      return get_max_static();
    }
    if (wd != w) {
      wd = (w + wd) / 2;
      w = wd;
    }
    if (wd < get_min_mating().win_draw) {
      return get_max_static();
    }
    if (wd <= get_max_score().win_draw) {
      return WDLScore{w, wd};
    }
    return get_max_score();
  }
  
  static constexpr WDLScore mated_on_ply(int32_t ply) {
    return WDLScore{get_min_score().win + ply, get_min_score().win_draw + ply};
  }
  void print() const { std::cout << "(" << win << ", " << win_draw << ")"; }
};


inline bool contempt_greater(const WDLScore lhs, const WDLScore rhs,
                                       int32_t contempt_factor) {
  assert(contempt_factor >= 0);
  assert(contempt_factor <= 100);
  return (100 - contempt_factor) * static_cast<int32_t>(lhs.win_draw - rhs.win_draw)
          + contempt_factor * static_cast<int32_t>(lhs.win - rhs.win) > 0;
}


#endif /* SRC_GENERAL_WDL_SCORE_H_ */
