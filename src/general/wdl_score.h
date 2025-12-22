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
#include <tuple>

struct WDLScore {
  static constexpr int32_t scale = 4000;
  static constexpr int32_t kNumMateInScores = 2000;

  int32_t win;
  int32_t loss;

  constexpr float to_wpct() const { return static_cast<float>(win + scale - loss) / (2* scale); }
  constexpr int32_t to_nscore() const { return value() - scale; }
  int32_t to_cp() const {
    constexpr float kEpsilon = 0.000001;
    float_t wpct = std::clamp(to_wpct(), kEpsilon,  1-kEpsilon);
    return std::round(std::log(wpct / (1-wpct)) * 1024);
  }

  constexpr float get_win_probability() const { return std::min(std::max(win / static_cast<float>(scale), 0.0f), 1.0f); }
  constexpr float get_draw_probability() const { return std::min(std::max((scale - win - loss) / static_cast<float>(scale), 0.0f), 1.0f); }
  constexpr float get_loss_probability() const { return std::min(std::max(loss / static_cast<float>(scale), 0.0f), 1.0f); }
  inline std::tuple<float, float, float> get_wdl_probabilities() const {
    return {get_win_probability(), get_draw_probability(), get_loss_probability()};
  }

  std::string get_uci_string() const {
    std::string output = "wdl";
    output += " " + std::to_string(win / 4);
    output += " " + std::to_string((scale - win - loss) / 4);
    output += " " + std::to_string(loss / 4);
    return output;
  }

  constexpr bool is_static_eval() const {
    return win >= 0 && win <= scale && loss <= scale;
  }

  // Returns whether score is expected worse than equal.
  constexpr bool is_disadvantage() const {
    return win < loss;
  }

  constexpr bool operator==(const WDLScore other) const { return win == other.win && loss == other.loss; }
  constexpr bool operator!=(const WDLScore other) const { return win != other.win || loss != other.loss; }
  constexpr bool operator>(const WDLScore other) const { return value() > other.value(); }
  constexpr bool operator<(const WDLScore other) const { return value() < other.value(); }
  constexpr bool operator>=(const WDLScore other) const { return value() >= other.value(); }
  constexpr bool operator<=(const WDLScore other) const { return value() <= other.value(); }
  
  WDLScore& operator*=(int32_t value) {
    win *= value;
    loss *= value;
    return *this;
  }
  constexpr WDLScore operator*(int32_t value) const { return WDLScore{win * value, loss * value}; }
  constexpr WDLScore operator/(int32_t value) const { return WDLScore{win / value, loss / value}; }
  constexpr WDLScore operator-() const {
    return WDLScore{loss, win};
  }
  constexpr WDLScore operator+(const WDLScore other) const { return WDLScore{win + other.win, loss + other.loss}; }
  // constexpr WDLScore operator-(const WDLScore other) const { return WDLScore{win - other.win, loss - other.loss}; }
  
  static constexpr WDLScore get_no_score() { return WDLScore{-1, -1}; }
  static constexpr WDLScore get_draw_score() { return from_pct_truncated_new(0.0, 0.0); }
  
  static constexpr WDLScore get_max_static() { return from_pct_truncated_new(1.0, 0.0); }
  static constexpr WDLScore get_min_static() { return -get_max_static(); }
  static constexpr WDLScore get_min_mating() { return WDLScore{get_max_static().win + 100, 0}; }
  static constexpr WDLScore get_max_mated() { return -get_min_mating(); }
  
  static constexpr WDLScore get_max_score() {
    return WDLScore{get_min_mating().win + kNumMateInScores, 0}; }
  static constexpr WDLScore get_min_score() { return -get_max_score(); }

  constexpr WDLScore get_next_score() const {
    if (win >= scale) {
      assert(loss == 0);
      if (win == scale) {
        return get_min_mating();
      }
      return WDLScore{win + 1, 0};
    }
    
    if (win + loss >= scale) {
      if (loss == get_min_mating().win) {
        return get_min_static();
      }
      return WDLScore{win, loss - 1};
    }
    return WDLScore{win + 1, loss};
  }
  
  constexpr WDLScore get_previous_score() const {
    if (loss >= scale) {
      assert(win == 0);
      if (loss == scale) {
        return -get_min_mating();
      }
      return WDLScore{0, loss+1};
    }
    
    if (win + loss >= scale) {
      if (win == get_min_mating().win) {
        return get_max_static();
      }
      return WDLScore{win - 1, loss};
    }
    return WDLScore{win, loss + 1};
  }

  constexpr int32_t value() const { return win - loss; }
  //constexpr int32_t value(int32_t contempt_factor) const {
//    assert(contempt_factor >= 0);
//    assert(contempt_factor <= 100);
  //  return contempt_factor * win + (100-contempt_factor) * win_draw; // - loss;
  //}
  constexpr bool is_draw() const { return win == 0 && loss == 0; }
  constexpr bool is_win() const { return win >= scale; }
  constexpr bool is_loss() const { return loss >= scale; }
  
  constexpr bool is_mate_score() const {
    return win >= get_min_mating().win || loss >= get_min_mating().win;
  }

  constexpr bool is_valid() const {
    return is_static_eval() || (is_mate_score() && (win==0 || loss==0) ) ||
        (win == get_no_score().win && loss == get_no_score().loss);
  }

//  static constexpr WDLScore from_score(int32_t score);
  static constexpr WDLScore from_pct_truncated_new(const float win, const float loss) {
    return WDLScore{static_cast<int32_t>(win * scale),
                    static_cast<int32_t>(loss * scale)};
  }
  
  static constexpr WDLScore from_pct_valid_new(const float win, const float loss) {
    if (win + loss <= 1.0) {
      return from_pct_new(win, loss);
    }
    float sum = win + loss;
    return from_pct_new(win / sum, loss / sum);
  }

  static constexpr WDLScore from_pct_new(const float win, const float loss) {
    return WDLScore{static_cast<int32_t>(std::round(win * scale)),
                    static_cast<int32_t>(std::round(loss * scale))};
  }
  
  constexpr WDLScore get_valid_score() const {
    int32_t w = win, l = loss;

    // Enforce w <= wd constraint
    // if (w > wd) {
    //  wd = (w + wd) / 2;
    //  w = wd;
    // }
    
    if (w < 0 || l < 0) {
      int32_t min_val = std::min(w,l);
      w += min_val;
      l += min_val;
    }
    
    if (w > WDLScore::scale || l > WDLScore::scale) {
      if (l > w) {
        if (l >= get_min_score().loss) {
          return get_min_score();
        }

        if (l >= get_max_mated().loss) {
          assert(w == 0);
          return WDLScore{0, l}; // Valid mate score
        }
        return get_min_static();
      }
      else if(w > l) {
        if (w >= get_max_score().win) {
          return get_max_score();
        }

        if (w >= get_min_mating().win) {
          assert(l == 0);
          return WDLScore{w, 0}; // Valid mate score
        }
      }
      return WDLScore { WDLScore::scale / 2, WDLScore::scale / 2 };
    }
    
    if (w+l > WDLScore::scale) {
      int32_t sum = w+l;
      w = w * WDLScore::scale / sum;
      l = l * WDLScore::scale / sum;
    } 
    return WDLScore { w, l };
  }
  
  static constexpr WDLScore mated_on_ply(int32_t ply) {
    return WDLScore{0, get_min_score().loss - ply};
  }
  void print() const { std::cout << "(" << win << ", " << loss << ")"; }
};

/*
inline bool contempt_greater(const WDLScore lhs, const WDLScore rhs,
                                       int32_t contempt_factor) {
  assert(contempt_factor >= 0);
  assert(contempt_factor <= 100);
  return (100 - contempt_factor) * static_cast<int32_t>(lhs.win_draw - rhs.win_draw)
          + contempt_factor * static_cast<int32_t>(lhs.win - rhs.win) > 0;
}
*/


#endif /* SRC_GENERAL_WDL_SCORE_H_ */
