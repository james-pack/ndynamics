#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "math/testing/types.h"

class R100 {
 public:
  static constexpr const char *basis[] = {"1", "e1"};
  static constexpr std::array<size_t, 2> bit_basis_indices{0, 1};

  static constexpr std::array<bool, 2> reversed_bases{0, 0};

  R100() { std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f); }
  R100(float f, int idx = 0) {
    std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f);
    mvec[idx] = f;
  }
  float &operator[](size_t idx) { return mvec[idx]; }
  const float &operator[](size_t idx) const { return mvec[idx]; }

  R100 Conjugate();
  R100 Involute();
  float norm();
  float inorm();
  R100 normalized();

 private:
  float mvec[2];
};

template <>
class BivectorNetTypes<1, 0, 0> final {
 public:
  using type = R100;
};

//***********************
// R100.Reverse : res = ~a
// Reverse the order of the basis blades.
//***********************
inline R100 operator~(const R100 &a) {
  R100 res;
  res[0] = a[0];
  res[1] = a[1];
  return res;
};

//***********************
// R100.Dual : res = !a
// Poincare duality operator.
//***********************
inline R100 operator!(const R100 &a) {
  R100 res;
  res[0] = a[1];
  res[1] = a[0];
  return res;
};

//***********************
// R100.Conjugate : res = a.Conjugate()
// Clifford Conjugation
//***********************
inline R100 R100::Conjugate() {
  R100 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  return res;
};

//***********************
// R100.Involute : res = a.Involute()
// Main involution
//***********************
inline R100 R100::Involute() {
  R100 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  return res;
};

//***********************
// R100.Mul : res = a * b
// The geometric product.
//***********************
inline R100 operator*(const R100 &a, const R100 &b) {
  R100 res;
  res[0] = b[0] * a[0] + b[1] * a[1];
  res[1] = b[1] * a[0] + b[0] * a[1];
  return res;
};

//***********************
// R100.Wedge : res = a ^ b
// The outer product. (MEET)
//***********************
inline R100 operator^(const R100 &a, const R100 &b) {
  R100 res;
  res[0] = b[0] * a[0];
  res[1] = b[1] * a[0] + b[0] * a[1];
  return res;
};

//***********************
// R100.Vee : res = a & b
// The regressive product. (JOIN)
//***********************
inline R100 operator&(const R100 &a, const R100 &b) {
  R100 res;
  res[1] = 1 * (a[1] * b[1]);
  res[0] = 1 * (a[0] * b[1] + a[1] * b[0]);
  return res;
};

//***********************
// R100.Dot : res = a | b
// The inner product.
//***********************
inline R100 operator|(const R100 &a, const R100 &b) {
  R100 res;
  res[0] = b[0] * a[0] + b[1] * a[1];
  res[1] = b[1] * a[0] + b[0] * a[1];
  return res;
};

//***********************
// R100.Add : res = a + b
// Multivector addition
//***********************
inline R100 operator+(const R100 &a, const R100 &b) {
  R100 res;
  res[0] = a[0] + b[0];
  res[1] = a[1] + b[1];
  return res;
};

//***********************
// R100.Sub : res = a - b
// Multivector subtraction
//***********************
inline R100 operator-(const R100 &a, const R100 &b) {
  R100 res;
  res[0] = a[0] - b[0];
  res[1] = a[1] - b[1];
  return res;
};

//***********************
// R100.smul : res = a * b
// scalar/multivector multiplication
//***********************
inline R100 operator*(const float &a, const R100 &b) {
  R100 res;
  res[0] = a * b[0];
  res[1] = a * b[1];
  return res;
};

//***********************
// R100.muls : res = a * b
// multivector/scalar multiplication
//***********************
inline R100 operator*(const R100 &a, const float &b) {
  R100 res;
  res[0] = a[0] * b;
  res[1] = a[1] * b;
  return res;
};

//***********************
// R100.sadd : res = a + b
// scalar/multivector addition
//***********************
inline R100 operator+(const float &a, const R100 &b) {
  R100 res;
  res[0] = a + b[0];
  res[1] = b[1];
  return res;
};

//***********************
// R100.adds : res = a + b
// multivector/scalar addition
//***********************
inline R100 operator+(const R100 &a, const float &b) {
  R100 res;
  res[0] = a[0] + b;
  res[1] = a[1];
  return res;
};

//***********************
// R100.ssub : res = a - b
// scalar/multivector subtraction
//***********************
inline R100 operator-(const float &a, const R100 &b) {
  R100 res;
  res[0] = a - b[0];
  res[1] = -b[1];
  return res;
};

//***********************
// R100.subs : res = a - b
// multivector/scalar subtraction
//***********************
inline R100 operator-(const R100 &a, const float &b) {
  R100 res;
  res[0] = a[0] - b;
  res[1] = a[1];
  return res;
};

inline float R100::norm() { return sqrt(std::abs(((*this) * Conjugate()).mvec[0])); }
inline float R100::inorm() { return (!(*this)).norm(); }
inline R100 R100::normalized() { return (*this) * (1 / norm()); }
