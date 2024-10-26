#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "math/testing/types.h"

class R010 {
 public:
  static constexpr const char *basis[] = {"1", "e1"};
  static constexpr size_t NUM_BASES{2};
  static constexpr std::array<size_t, NUM_BASES> bit_basis_indices{0, 1};

  static constexpr std::array<bool, NUM_BASES> reversed_bases{0, 0};

  R010() { std::fill(mvec, mvec + NUM_BASES, 0.0f); }
  R010(float f, int idx = 0) {
    std::fill(mvec, mvec + NUM_BASES, 0.0f);
    mvec[idx] = f;
  }

  R010(const R010 &rhs) = default;
  R010(R010 &&rhs) = default;

  bool operator==(const R010 &rhs) const {
    for (size_t i = 0; i < NUM_BASES; ++i) {
      if (mvec[i] != rhs.mvec[i]) {
        return false;
      }
    }
    return true;
  }

  float &operator[](size_t idx) { return mvec[idx]; }
  const float &operator[](size_t idx) const { return mvec[idx]; }

  R010 Conjugate();
  R010 Involute();
  float norm();
  float inorm();
  R010 normalized();

 private:
  float mvec[NUM_BASES];
};

template <>
class BivectorNetTypes<0, 1, 0> final {
 public:
  using type = R010;
};

//***********************
// R010.Reverse : res = ~a
// Reverse the order of the basis blades.
//***********************
inline R010 operator~(const R010 &a) {
  R010 res;
  res[0] = a[0];
  res[1] = a[1];
  return res;
};

//***********************
// R010.Dual : res = !a
// Poincare duality operator.
//***********************
inline R010 operator!(const R010 &a) {
  R010 res;
  res[0] = -a[1];
  res[1] = a[0];
  return res;
};

//***********************
// R010.Conjugate : res = a.Conjugate()
// Clifford Conjugation
//***********************
inline R010 R010::Conjugate() {
  R010 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  return res;
};

//***********************
// R010.Involute : res = a.Involute()
// Main involution
//***********************
inline R010 R010::Involute() {
  R010 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  return res;
};

//***********************
// R010.Mul : res = a * b
// The geometric product.
//***********************
inline R010 operator*(const R010 &a, const R010 &b) {
  R010 res;
  res[0] = b[0] * a[0] - b[1] * a[1];
  res[1] = b[1] * a[0] + b[0] * a[1];
  return res;
};

//***********************
// R010.Wedge : res = a ^ b
// The outer product. (MEET)
//***********************
inline R010 operator^(const R010 &a, const R010 &b) {
  R010 res;
  res[0] = b[0] * a[0];
  res[1] = b[1] * a[0] + b[0] * a[1];
  return res;
};

//***********************
// R010.Vee : res = a & b
// The regressive product. (JOIN)
//***********************
inline R010 operator&(const R010 &a, const R010 &b) {
  R010 res;
  res[1] = 1 * (a[1] * b[1]);
  res[0] = 1 * (a[0] * b[1] + a[1] * b[0]);
  return res;
};

//***********************
// R010.Dot : res = a | b
// The inner product.
//***********************
inline R010 operator|(const R010 &a, const R010 &b) {
  R010 res;
  res[0] = b[0] * a[0] - b[1] * a[1];
  res[1] = b[1] * a[0] + b[0] * a[1];
  return res;
};

//***********************
// R010.Add : res = a + b
// Multivector addition
//***********************
inline R010 operator+(const R010 &a, const R010 &b) {
  R010 res;
  res[0] = a[0] + b[0];
  res[1] = a[1] + b[1];
  return res;
};

//***********************
// R010.Sub : res = a - b
// Multivector subtraction
//***********************
inline R010 operator-(const R010 &a, const R010 &b) {
  R010 res;
  res[0] = a[0] - b[0];
  res[1] = a[1] - b[1];
  return res;
};

//***********************
// R010.smul : res = a * b
// scalar/multivector multiplication
//***********************
inline R010 operator*(const float &a, const R010 &b) {
  R010 res;
  res[0] = a * b[0];
  res[1] = a * b[1];
  return res;
};

//***********************
// R010.muls : res = a * b
// multivector/scalar multiplication
//***********************
inline R010 operator*(const R010 &a, const float &b) {
  R010 res;
  res[0] = a[0] * b;
  res[1] = a[1] * b;
  return res;
};

//***********************
// R010.sadd : res = a + b
// scalar/multivector addition
//***********************
inline R010 operator+(const float &a, const R010 &b) {
  R010 res;
  res[0] = a + b[0];
  res[1] = b[1];
  return res;
};

//***********************
// R010.adds : res = a + b
// multivector/scalar addition
//***********************
inline R010 operator+(const R010 &a, const float &b) {
  R010 res;
  res[0] = a[0] + b;
  res[1] = a[1];
  return res;
};

//***********************
// R010.ssub : res = a - b
// scalar/multivector subtraction
//***********************
inline R010 operator-(const float &a, const R010 &b) {
  R010 res;
  res[0] = a - b[0];
  res[1] = -b[1];
  return res;
};

//***********************
// R010.subs : res = a - b
// multivector/scalar subtraction
//***********************
inline R010 operator-(const R010 &a, const float &b) {
  R010 res;
  res[0] = a[0] - b;
  res[1] = a[1];
  return res;
};

inline float R010::norm() { return sqrt(std::abs(((*this) * Conjugate()).mvec[0])); }
inline float R010::inorm() { return (!(*this)).norm(); }
inline R010 R010::normalized() { return (*this) * (1 / norm()); }
