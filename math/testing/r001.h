#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "math/testing/types.h"

class R001 {
 public:
  static constexpr const char *basis[] = {"1", "e0"};
  static constexpr size_t NUM_BASES{2};
  static constexpr std::array<size_t, NUM_BASES> bit_basis_indices{0, 1};

  static constexpr std::array<bool, NUM_BASES> reversed_bases{0, 0};

  R001() { std::fill(mvec, mvec + NUM_BASES, 0.0f); }
  R001(float f, int idx = 0) {
    std::fill(mvec, mvec + NUM_BASES, 0.0f);
    mvec[idx] = f;
  }

  R001(const R001 &rhs) = default;
  R001(R001 &&rhs) = default;

  bool operator==(const R001 &rhs) const {
    for (size_t i = 0; i < NUM_BASES; ++i) {
      if (mvec[i] != rhs.mvec[i]) {
        return false;
      }
    }
    return true;
  }

  float &operator[](size_t idx) { return mvec[idx]; }
  const float &operator[](size_t idx) const { return mvec[idx]; }

  R001 Conjugate();
  R001 Involute();
  float norm();
  float inorm();
  R001 normalized();

 private:
  float mvec[NUM_BASES];
};

template <>
class BivectorNetTypes<0, 0, 1> final {
 public:
  using type = R001;
};

//***********************
// R001.Reverse : res = ~a
// Reverse the order of the basis blades.
//***********************
inline R001 operator~(const R001 &a) {
  R001 res;
  res[0] = a[0];
  res[1] = a[1];
  return res;
};

//***********************
// R001.Dual : res = !a
// Poincare duality operator.
//***********************
inline R001 operator!(const R001 &a) {
  R001 res;
  res[0] = a[1];
  res[1] = a[0];
  return res;
};

//***********************
// R001.Conjugate : res = a.Conjugate()
// Clifford Conjugation
//***********************
inline R001 R001::Conjugate() {
  R001 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  return res;
};

//***********************
// R001.Involute : res = a.Involute()
// Main involution
//***********************
inline R001 R001::Involute() {
  R001 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  return res;
};

//***********************
// R001.Mul : res = a * b
// The geometric product.
//***********************
inline R001 operator*(const R001 &a, const R001 &b) {
  R001 res;
  res[0] = b[0] * a[0];
  res[1] = b[1] * a[0] + b[0] * a[1];
  return res;
};

//***********************
// R001.Wedge : res = a ^ b
// The outer product. (MEET)
//***********************
inline R001 operator^(const R001 &a, const R001 &b) {
  R001 res;
  res[0] = b[0] * a[0];
  res[1] = b[1] * a[0] + b[0] * a[1];
  return res;
};

//***********************
// R001.Vee : res = a & b
// The regressive product. (JOIN)
//***********************
inline R001 operator&(const R001 &a, const R001 &b) {
  R001 res;
  res[1] = 1 * (a[1] * b[1]);
  res[0] = 1 * (a[0] * b[1] + a[1] * b[0]);
  return res;
};

//***********************
// R001.Dot : res = a | b
// The inner product.
//***********************
inline R001 operator|(const R001 &a, const R001 &b) {
  R001 res;
  res[0] = b[0] * a[0];
  res[1] = b[1] * a[0] + b[0] * a[1];
  return res;
};

//***********************
// R001.Add : res = a + b
// Multivector addition
//***********************
inline R001 operator+(const R001 &a, const R001 &b) {
  R001 res;
  res[0] = a[0] + b[0];
  res[1] = a[1] + b[1];
  return res;
};

//***********************
// R001.Sub : res = a - b
// Multivector subtraction
//***********************
inline R001 operator-(const R001 &a, const R001 &b) {
  R001 res;
  res[0] = a[0] - b[0];
  res[1] = a[1] - b[1];
  return res;
};

//***********************
// R001.smul : res = a * b
// scalar/multivector multiplication
//***********************
inline R001 operator*(const float &a, const R001 &b) {
  R001 res;
  res[0] = a * b[0];
  res[1] = a * b[1];
  return res;
};

//***********************
// R001.muls : res = a * b
// multivector/scalar multiplication
//***********************
inline R001 operator*(const R001 &a, const float &b) {
  R001 res;
  res[0] = a[0] * b;
  res[1] = a[1] * b;
  return res;
};

//***********************
// R001.sadd : res = a + b
// scalar/multivector addition
//***********************
inline R001 operator+(const float &a, const R001 &b) {
  R001 res;
  res[0] = a + b[0];
  res[1] = b[1];
  return res;
};

//***********************
// R001.adds : res = a + b
// multivector/scalar addition
//***********************
inline R001 operator+(const R001 &a, const float &b) {
  R001 res;
  res[0] = a[0] + b;
  res[1] = a[1];
  return res;
};

//***********************
// R001.ssub : res = a - b
// scalar/multivector subtraction
//***********************
inline R001 operator-(const float &a, const R001 &b) {
  R001 res;
  res[0] = a - b[0];
  res[1] = -b[1];
  return res;
};

//***********************
// R001.subs : res = a - b
// multivector/scalar subtraction
//***********************
inline R001 operator-(const R001 &a, const float &b) {
  R001 res;
  res[0] = a[0] - b;
  res[1] = a[1];
  return res;
};

inline float R001::norm() { return sqrt(std::abs(((*this) * Conjugate()).mvec[0])); }
inline float R001::inorm() { return (!(*this)).norm(); }
inline R001 R001::normalized() { return (*this) * (1 / norm()); }
