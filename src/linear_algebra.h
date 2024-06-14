/*
 *  Winter is a UCI chess engine.
 *
 *  Copyright (C) 2016 Jonas Kuratli, Jonathan Maurer, Jonathan Rosenthal
 *  Copyright (C) 2017-2019 Jonathan Rosenthal
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
 * linear_algebra.h
 *
 *  Created on: Mar 17, 2017
 *      Author: Jonathan Rosenthal
 */

#ifndef LEARNING_LINEAR_ALGEBRA_H_
#define LEARNING_LINEAR_ALGEBRA_H_

#include "../general/types.h"
#include <algorithm>
#include <cmath>
#include <iostream>

template <typename type, size_t length>
struct Vec;

#ifdef __AVX__
#include <immintrin.h>
using SIMDFloat = __m256;
constexpr size_t kSIMDWidth = 8;

namespace simd {

// Sept. 10, 2020. Taken from: https://stackoverflow.com/questions/13219146/how-to-sum-m256-horizontally
inline float sum(__m256 x) {
    const __m128 hiQuad = _mm256_extractf128_ps(x, 1);
    const __m128 loQuad = _mm256_castps256_ps128(x);
    const __m128 sumQuad = _mm_add_ps(loQuad, hiQuad);
    const __m128 loDual = sumQuad;
    const __m128 hiDual = _mm_movehl_ps(sumQuad, sumQuad);
    const __m128 sumDual = _mm_add_ps(loDual, hiDual);
    const __m128 lo = sumDual;
    const __m128 hi = _mm_shuffle_ps(sumDual, sumDual, 0x1);
    const __m128 sum = _mm_add_ss(lo, hi);
    return _mm_cvtss_f32(sum);
}

inline SIMDFloat add(const SIMDFloat a, const SIMDFloat b) { return _mm256_add_ps(a, b); }
inline SIMDFloat multiply(const SIMDFloat a, const SIMDFloat b) { return _mm256_mul_ps(a, b); }
inline void store(float* mem_addr, SIMDFloat a) { _mm256_storeu_ps(mem_addr, a); }
inline SIMDFloat load(float const* mem_addr) { return _mm256_loadu_ps(mem_addr); }
inline SIMDFloat max(SIMDFloat a, SIMDFloat b) { return _mm256_max_ps(a,  b); }
inline SIMDFloat min(SIMDFloat a, SIMDFloat b) { return _mm256_min_ps(a,  b); }
inline SIMDFloat set(float a) { return _mm256_set1_ps(a); }

inline SIMDFloat fmadd(SIMDFloat a, SIMDFloat b, SIMDFloat c) {
  //return _mm256_fmadd_ps(a, b, c);
  return add(multiply(a, b), c);
}

}

#else
#if !defined(_ARM)
#include <xmmintrin.h>
#else
#include "sse2neon.h"
#endif

using SIMDFloat = __m128;
constexpr size_t kSIMDWidth = 4;

namespace simd {
// Sept. 10, 2020. Taken from: https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction
inline float sum(SIMDFloat v) {                                 
  SIMDFloat shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 0, 1));
  SIMDFloat sums = _mm_add_ps(v, shuf);
  shuf = _mm_movehl_ps(shuf, sums);
  sums = _mm_add_ss(sums, shuf);
  return _mm_cvtss_f32(sums);
}

inline SIMDFloat add(const SIMDFloat a, const SIMDFloat b) { return _mm_add_ps(a, b); }
inline SIMDFloat multiply(const SIMDFloat a, const SIMDFloat b) { return _mm_mul_ps(a, b); }
inline void store(float* mem_addr, SIMDFloat a) { _mm_storeu_ps(mem_addr, a); }
inline SIMDFloat load(float const* mem_addr) { return _mm_loadu_ps(mem_addr); }
inline SIMDFloat max(SIMDFloat a, SIMDFloat b) { return _mm_max_ps(a,  b); }
inline SIMDFloat min(SIMDFloat a, SIMDFloat b) { return _mm_min_ps(a,  b); }
inline SIMDFloat set(float a) { return _mm_set1_ps(a); }

inline SIMDFloat fmadd(SIMDFloat a, SIMDFloat b, SIMDFloat c) {
  //return _mm256_fmadd_ps(a, b, c);
  return add(multiply(a, b), c);
}

}
#endif

template<size_t length>
struct Vec<float, length> {
  Vec() {}
  
  Vec(float val) {
    for (size_t i = 0; i < length; ++i) {
      values[i] = val;
    }
  }
  
  inline size_t size() const {
    return length;
  }
  
  float sum() const {
    float t = 0;
    for (size_t i = 0; i < length; ++i) {
      t += values[i];
    }
    return t;
  }
  
  inline Vec<float, length>& operator+=(const Vec<float, length> &rhs) {
    #pragma GCC unroll 32
    for (size_t i = 0; i <= length-kSIMDWidth; i+=kSIMDWidth) {
      SIMDFloat v1 = simd::load(&values[i]);
      SIMDFloat v2 = simd::load(&rhs.values[i]);
      simd::store(&values[i], simd::add(v1, v2));
    }
    return *this;
  }
  
  inline Vec<float, length>& operator*=(const Vec<float, length> &rhs) {
    for (size_t i = 0; i <= length-kSIMDWidth; i+=kSIMDWidth) {
      SIMDFloat v1 = simd::load(&values[i]);
      SIMDFloat v2 = simd::load(&rhs.values[i]);
      simd::store(&values[i], simd::multiply(v1, v2));
    }
    return *this;
  }
  
  inline Vec<float, length>& relu() {
    const SIMDFloat zero = simd::set(0);
    for (size_t i = 0; i <= length-kSIMDWidth; i += kSIMDWidth) {
      SIMDFloat v1 = simd::load(&values[i]);
      simd::store(&values[i], simd::max(v1, zero));
    }
    return *this;
  }
  
  [[nodiscard]] inline Vec<float, length> clipped_relu(const float max_val) const {
    const SIMDFloat zero = simd::set(0);
    const SIMDFloat max_simd = simd::set(max_val);
    Vec<float, length> result;
    #pragma GCC unroll 32
    for (size_t i = 0; i <= length-kSIMDWidth; i += kSIMDWidth) {
      SIMDFloat v1 = simd::load(&values[i]);
      simd::store(&result.values[i], simd::min(simd::max(v1, zero), max_simd));
    }
    return result;
  }
  
  inline Vec<float, length>& FMA(const Vec<float, length> &a, const float &b) {
    static_assert(length == 8 * (length / 8), "Input length is not multiple of 16");
    SIMDFloat vb = simd::set(b);
    for (size_t i = 0; i <= length-kSIMDWidth; i+=kSIMDWidth) {
      SIMDFloat c = simd::load(&values[i]);
      SIMDFloat va = simd::load(&a.values[i]);
     simd::store(&values[i], simd::fmadd(va, vb, c));
    }
    return *this;
  }

  inline Vec<float, length>& FMA(const Vec<float, length> &a, const Vec<float, length> &b) {
    static_assert(length == 8 * (length / 8), "Input length is not multiple of 16");
    for (size_t i = 0; i <= length-kSIMDWidth; i+=kSIMDWidth) {
      SIMDFloat c = simd::load(&values[i]);
      SIMDFloat va = simd::load(&a.values[i]);
      SIMDFloat vb = simd::load(&b.values[i]);
      simd::store(&values[i], simd::fmadd(va, vb, c));
    }
    return *this;
  }
  
  template<typename t>
  float dot(const Vec<t, length> &other) const {
    static_assert(length == 8 * (length / 8), "Input length is not multiple of 16");
    SIMDFloat c = simd::set(0);
    for (size_t i = 0; i <= length-kSIMDWidth; i+=kSIMDWidth) {
      SIMDFloat va = simd::load(&values[i]);
      SIMDFloat vb = simd::load(&other.values[i]);
      c = simd::fmadd(va, vb, c);
    }
    return simd::sum(c);
  }
  
  template<size_t new_length>
  Vec<float, new_length> reduce_sum() const {
      static_assert(new_length % kSIMDWidth == 0, "Reducing to non-SIMDWidth multiple");
      static_assert(new_length < length, "Reduction not reducing");
      static_assert(length % new_length == 0, "Invalid reduction size");
      Vec<float, new_length> result;
      for (size_t i = 0; i <= new_length-kSIMDWidth; i+=kSIMDWidth) {
          SIMDFloat c = simd::set(0);
          for (size_t j = 0; j <= length - new_length; j+=new_length) {
              SIMDFloat v = simd::load(&values[i+j]);
              c = simd::add(c, v);
          }
          simd::store(&result.values[i], c);
      }
      return result;
  }
  
  
  template<size_t new_length>
  Vec<float, new_length> reduce_weighted(Vec<float, length> weights) const {
      static_assert(new_length % kSIMDWidth == 0, "Reducing to non-SIMDWidth multiple");
      static_assert(new_length < length, "Reduction not reducing");
      static_assert(length % new_length == 0, "Invalid reduction size");
      Vec<float, new_length> result;
      for (size_t i = 0; i <= new_length-kSIMDWidth; i+=kSIMDWidth) {
          SIMDFloat c = simd::set(0);
          for (size_t j = 0; j <= length - new_length; j+=new_length) {
              SIMDFloat v1 = simd::load(&values[i+j]);
              SIMDFloat v2 = simd::load(&weights.values[i+j]);
              c = simd::fmadd(v1, v2, c);
          }
          simd::store(&result.values[i], c);
      }
      return result;
  }
  
  inline float& operator[](std::size_t idx) { return values[idx]; }
  inline const float operator[](std::size_t idx) const { return values[idx]; }
  
  alignas(sizeof(SIMDFloat)) float values[length];
};

template<typename t, size_t l>
Vec<t,l> operator*(Vec<t,l> lhs, const int rhs) {
  lhs *= rhs;
  return lhs;
}
template<typename t, size_t l>
Vec<t,l> operator/(Vec<t,l> lhs, const int rhs) {
  lhs /= rhs;
  return lhs;
}
template<typename t, size_t l>
Vec<t,l> operator*(const double &rhs, Vec<t,l> lhs) {
  lhs *= rhs;
  return lhs;
}
template<typename t, size_t l>
Vec<t,l> operator*(const int &rhs, Vec<t,l> lhs) { return lhs * rhs; }
template<typename t, size_t l>
Vec<t,l> operator-(Vec<t,l> score) {
  for (size_t i = 0; i < l; ++i) {
    score[i] = -score[i];
  }
  return score;
}
template<typename t, size_t l>
inline bool operator==(const Vec<t,l>& lhs, const Vec<t,l>& rhs) {
  bool res = true;
  for (size_t i = 0; i < l; ++i) {
    res = res && lhs[i] == rhs[i];
  }
  return res;
}
template<typename t, size_t l>
inline bool operator!=(const Vec<t,l>& lhs, const Vec<t,l>& rhs) { return !(lhs == rhs); }

template<typename t, size_t l>
Vec<t,l> squared_differences(const Vec<t,l> &v1, const Vec<t,l> &v2) {
  Vec<t,l> v;
  for (size_t i = 0; i < l; ++i) {
    v[i] = std::pow(v1[i] - v2[i], 2);
  }
  return v;
}

#endif /* LEARNING_LINEAR_ALGEBRA_H_ */
