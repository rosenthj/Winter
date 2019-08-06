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
#include <cmath>
#include <iostream>

template <typename type, size_t length>
struct Vec {
  Vec() {
//    std::cout << "Basic initialization detected" << std::endl;
//    for (size_t i = 0; i < length; ++i) {
//      values[i] = 0;
//    }
  }

  Vec(type val) {
    for (size_t i = 0; i < length; ++i) {
      values[i] = val;
    }
  }

  Vec(const Vec<type, length/2> &vec_to_crelu) {
    const size_t size = vec_to_crelu.size();
    for (size_t i = 0; i < size; ++i) {
      values[i] = std::max(vec_to_crelu[i], static_cast<type>(0));
      values[i+size] = std::max(-vec_to_crelu[i], static_cast<type>(0));
    }
  }

  type sum() const {
    type t = 0;
    for (size_t i = 0; i < length; ++i) {
      t += values[i];
    }
    return t;
  }

  type normL1() const {
    type res = 0;
    for (size_t i = 0; i < length; ++i) {
      res += std::abs(values[i]);
    }
    return res;
  }

  type normL2() const {
    type res = 0;
    for (size_t i = 0; i < length; ++i) {
      res += values[i] * values[i];
    }
    return std::sqrt(res);
  }

  inline size_t size() const {
    return length;
  }

  void print() const {
    std::cout << "(" << values[0];
    for (size_t i = 1; i < length; ++i) {
      std::cout << "," << values[i];
    }
    std::cout << ")";
  }
  Vec<type, length>& operator=(const Vec<type, length> &rhs) {
    for (size_t i = 0; i < length; ++i) {
      this->values[i] = rhs.values[i];
    }
    return *this;
  }

  inline Vec<type, length>& operator+=(const Vec<type, length> &rhs) {
    for (size_t i = 0; i < length; ++i) {
      values[i] += rhs[i];
    }
    return *this;
  }
  
  inline Vec<type, length>& relu() {
    for (size_t i = 0; i < length; ++i) {
      values[i] = std::max(values[i], static_cast<type>(0));
    }
    return *this;
  }

  /// Non-standard PReLU activation assumes alpha <= 1
  inline Vec<type, length>& ns_prelu(const Vec<type, length> &alpha) {
    for (size_t i = 0; i < length; ++i) {
      values[i] = std::max(values[i], values[i] * alpha[i]);
    }
    return *this;
  }

  /// Non-standard PReLU activation assumes alpha <= 1
  inline Vec<type, length>& ns_prelu(const std::array<type, length> &alpha) {
    for (size_t i = 0; i < length; ++i) {
      values[i] = std::max(values[i], values[i] * alpha[i]);
    }
    return *this;
  }

  inline Vec<type, length>& sigmoid() {
    for (size_t i = 0; i < length; ++i) {
      values[i] = static_cast<type>(1) / (1 + std::exp(-values[i]));
    }
    return *this;
  }

  friend Vec<type,length> operator+(Vec<type,length> lhs, const Vec<type,length> &rhs) {
    lhs += rhs;
    return lhs;
  }
  
  inline Vec<type, length>& operator-=(const Vec<type, length> &rhs) {
    for (size_t i = 0; i < length; ++i) {
      values[i] -= rhs[i];
    }
    return *this;
  }
  
  friend Vec<type,length> operator-(Vec<type,length> lhs, const Vec<type,length> &rhs) {
    lhs -= rhs;
    return lhs;
  }
  
  inline Vec<type, length>& operator/=(const Vec<type, length> &rhs) {
    for (size_t i = 0; i < length; ++i) {
      values[i] /= rhs[i];
    }
    return *this;
  }
  
  friend Vec<type,length> operator/(Vec<type,length> lhs, const Vec<type,length> &rhs) {
    lhs /= rhs;
    return lhs;
  }
  
  inline Vec<type, length>& operator*=(const Vec<type, length> &rhs) {
    for (size_t i = 0; i < length; ++i) {
      values[i] *= rhs[i];
    }
    return *this;
  }
  
  friend Vec<type,length> operator*(Vec<type,length> lhs, const Vec<type,length> &rhs) {
    lhs *= rhs;
    return lhs;
  }
  
  inline Vec<type, length>& operator*=(const int rhs) {
    for (size_t i = 0; i < length; ++i) {
      this->values[i] *= rhs;
    }
    return *this;
  }
  
  inline Vec<type, length>& operator*=(const double rhs) {
    for (size_t i = 0; i < length; ++i) {
      this->values[i] *= rhs;
    }
    return *this;
  }
  
  inline Vec<type, length>& operator/=(const int rhs) {
    for (size_t i = 0; i < length; ++i) {
      this->values[i] /= rhs;
    }
    return *this;
  }
  
  inline Vec<type, length>& operator/=(const double rhs) {
    for (size_t i = 0; i < length; ++i) {
      this->values[i] /= rhs;
    }
    return *this;
  }

  inline Vec<type, length>& FMA(const Vec<type, length> &a, const type &b) {
    for (size_t i = 0; i < length; ++i) {
      this->values[i] += a[i] * b;
    }
    return *this;
  }

  inline Vec<type, length>& FMA(const Vec<type, length> &a, const Vec<type, length> &b) {
    for (size_t i = 0; i < length; ++i) {
      this->values[i] += a[i] * b[i];
    }
    return *this;
  }

  inline type squared_distance(const Vec<type,length> &other) {
    type s = 0;
    for (size_t i = 0; i < length; ++i) {
      s += std::pow(values[i] - other[i], 2);
    }
    return s;
  }

  inline Vec<type,length> sqrt() {
    Vec<type, length> res;
    for (size_t i = 0; i < length; ++i) {
      res[i] = std::sqrt(values[i]);
    }
    return res;
  }

  template<typename t>
  type dot(const Vec<t,length> &other) const {
    type s = 0;
    for (size_t i = 0; i < length; ++i) {
      s += values[i] * other[i];
    }
    return s;
  }

  inline type& operator[](std::size_t idx) { return values[idx]; }
  inline const type operator[](std::size_t idx) const { return values[idx]; }

  type values[length];
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

template <typename type,size_t length>
struct SqMatrix {
  SqMatrix() {
    for (size_t i = 0; i < length; ++i) {
      values[i] = Vec<type, length>(0);
    }
  }

  SqMatrix(type val) {
    for (size_t i = 0; i < length; ++i) {
      values[i] = Vec<type, length>(val);
    }
  }

  SqMatrix<type, length>& operator/=(const double rhs) {
    for (size_t i = 0; i < length; ++i) {
      this->values[i] /= rhs;
    }
    return *this;
  }

  SqMatrix<type, length>& operator+=(const SqMatrix<type, length> rhs) {
    for (size_t i = 0; i < length; ++i) {
      this->values[i] += rhs[i];
    }
    return *this;
  }

  SqMatrix<type, length> transpose() const {
    SqMatrix<type, length> mat;
    for (size_t i = 0; i < length; ++i) {
      for (size_t j = 0; j < length; ++j) {
        mat[i][j] = values[j][i];
      }
    }
    return *this;
  }

  template<typename t>
  Vec<type,length> dot(const Vec<t, length> &other) const {
    Vec<type, length> s(0);
    for (size_t i = 0; i < length; ++i) {
      s[i] = values[i].dot(other);
    }
    return s;
  }

  SqMatrix<type,length> dot(const SqMatrix<type, length> &other) {
    SqMatrix<type, length> mat = other.transpose();
    SqMatrix<type, length> result;
    for (size_t i = 0; i < length; ++i) {
      result[i] = this->dot(mat[i]);
    }
    return result.transpose();
  }

  void print() const {
    for (size_t i = 0; i < length; ++i) {
      values[i].print();
      std::cout << std::endl;
    }
  }

  Vec<type, length>& operator[](std::size_t idx) { return values[idx]; }
  const Vec<type, length> operator[](std::size_t idx) const { return values[idx]; }

  Vec<type, length> values[length];
};

template Vec<double, kPhaseVecLength>
    SqMatrix<double, kPhaseVecLength>::dot<double>(
        const Vec<double, kPhaseVecLength> &other) const;

template Vec<double, kPhaseVecLength>
    SqMatrix<double, kPhaseVecLength>::dot<Score>(
        const Vec<Score, kPhaseVecLength> &other) const;

template<typename type, size_t length>
struct DeterminantHelper {
  static type calculate(const SqMatrix<type, length> &mat) {
    SqMatrix<type, length - 1> h_mat;
    double det = 0;
    for (size_t i = 0; i < length; ++i) {
      int sign = 1;
      if (i % 2) {
        sign = -1;
      }
      int inc = 0;
      for (size_t u = 0; u < length; ++u) {
        if (u == i) {
          inc++;
          continue;
        }
        for (size_t v = 1; v < length; ++v) {
          h_mat[u - inc][v-1] = mat[u][v];
        }
      }
      det += sign * mat[i][0] * calculate_rec(h_mat);
    }
    return det;
  }
  static type calculate_rec(const SqMatrix<type, length-1> &mat) {
    return DeterminantHelper<type, length-1>::calculate(mat);
  }
};

template<typename type>
struct DeterminantHelper<type, 1> {
  static type calculate(const SqMatrix<type, 1> &mat) {
    return mat[0][0];
  }
};

template struct DeterminantHelper<double, 2>;

template<typename type, size_t length> type Determinant(const SqMatrix<type, length> &mat) {
  return DeterminantHelper<type, length>::calculate(mat);
}


template<typename type, size_t length> SqMatrix<type, length> SymmetricSquareMatrixInv(
      const SqMatrix<type, length> &mat) {
  double det = Determinant<type, length>(mat);
  SqMatrix<type, length> adj_mat;
  SqMatrix<type, length - 1> h_mat;
  for (size_t i = 0; i < length; ++i) {
    for (size_t j = 0; j < length; ++j) {
      int sign = 1;
      if ((i+j) % 2) {
        sign = -1;
      }
      int inc_u = 0;
      for (size_t u = 0; u < length; ++u) {
        if (u == i) {
          inc_u++;
          continue;
        }
        int inc_v = 0;
        for (size_t v = 0; v < length; ++v) {
          if (v == j) {
            inc_v++;
            continue;
          }
          h_mat[u - inc_u][v - inc_v] = mat[u][v];
        }
      }
      adj_mat[i][j] = sign * Determinant<type, length - 1>(h_mat);
    }
  }
  for (size_t i = 0; i < length; ++i) {
    for (size_t j = i+1; j < length; ++j) {
      double tmp = adj_mat[i][j];
      adj_mat[i][j] = adj_mat[j][i];
      adj_mat[j][i] = tmp;
    }
  }
  adj_mat /= det;
  return adj_mat;
}

#endif /* LEARNING_LINEAR_ALGEBRA_H_ */
