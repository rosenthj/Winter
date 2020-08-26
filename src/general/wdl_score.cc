/*
 * wdl_score.cc
 *
 *  Created on: May 29, 2020
 *      Author: jonathan
 */

#include <iostream>
#include "wdl_score.h"
#include "types.h"

WDLScore WDLScore::get_next_score() const {
  if (win == scale && win_draw == scale) {
    return kMinMatingScore;
  }
  if (win == kMaxMatedScore.win) {
    assert(win == win_draw);
    return kMinStaticEval;
  }
  if (is_mate_score()) {
    return WDLScore{win + 1, win_draw + 1};
  }
  if (win_draw == scale) {
    return WDLScore{win + 1, win_draw};
  }
  return WDLScore{win, win_draw + 1};
}

WDLScore WDLScore::get_previous_score() const {
  if (win == kMinMatingScore.win) {
    assert(win == win_draw);
    return kMaxStaticEval;
  }
  if (win == 0 && win_draw == 0) {
    return kMaxMatedScore;
  }
  if (is_mate_score()) {
    return WDLScore{win - 1, win_draw - 1};
  }
  if (win == 0) {
    return WDLScore{win, win_draw - 1};
  }
  return WDLScore{win - 1, win_draw};
}

// TODO improve this. Specifically it is not currently symmetric.
WDLScore WDLScore::get_valid_score() const {
  int32_t w = win, wd = win_draw;

  // Enforce w <= wd constraint
  if (w > wd) {
    wd = (w + wd) / 2;
    w = wd;
  }

  if (wd <= kMinScore.win_draw) {
    return kMinScore;
  }
  if (wd <= kMaxMatedScore.win_draw) {
    return WDLScore{wd, wd}; // Valid mate score
  }

  // Enforce w >= min static eval
  if (w < kMinStaticEval.win) {
    wd -= kMinStaticEval.win - w;
    w = kMinStaticEval.win;
  }
  if (wd < kMinStaticEval.win_draw) {
    return kMinStaticEval;
  }
  if (wd < kMaxStaticEval.win_draw) {
    return WDLScore{w, wd};
  }
  if (wd < kMinMatingScore.win_draw) {
    w += wd - kMaxStaticEval.win_draw;
    wd = kMaxStaticEval.win_draw;
    if (w <= kMaxStaticEval.win) {
      return WDLScore{w, wd};
    }
    return kMaxStaticEval;
  }
  if (wd != w) {
    wd = (w + wd) / 2;
    w = wd;
  }
  if (wd < kMinMatingScore.win_draw) {
    return kMaxStaticEval;
  }
  if (wd <= kMaxScore.win_draw) {
    return WDLScore{w, wd};
  }
  return kMaxScore;
}

bool WDLScore::is_valid() const {
  return is_static_eval() || is_mate_score() ||
      (win == kNoScore.win && win_draw == kNoScore.win_draw);
}

bool WDLScore::is_mate_score() const {
  return (win == win_draw) && ((win <= kMaxMatedScore.win && win >= kMinScore.win)
      || (win >= kMinMatingScore.win && win <= kMaxScore.win));
}

void WDLScore::print() const {
  std::cout << "(" << win << ", " << win_draw << ")";
}

NScore WDLScore::to_cp() const {
  constexpr float kEpsilon = 0.000001;
  float wpct = to_wpct();
  wpct = std::max(std::min(wpct, 1-kEpsilon), kEpsilon);
  return std::round(std::log(wpct / (1-wpct)) * 1024);
}
