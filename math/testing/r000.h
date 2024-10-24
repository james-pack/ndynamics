#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "math/testing/types.h"

class R000 {
 public:
  static constexpr const char *basis[] = {"1"};
  static constexpr std::array<size_t, 1> bit_basis_indices{0};

  static constexpr std::array<bool, 1> reversed_bases{0};

  R000() { std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f); }
  R000(float f, int idx = 0) {
    std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f);
    mvec[idx] = f;
  }
  float &operator[](size_t idx) { return mvec[idx]; }
  const float &operator[](size_t idx) const { return mvec[idx]; }

  R000 Conjugate();
  R000 Involute();
  float norm();
  float inorm();
  R000 normalized();

 private:
  float mvec[1];
};

template <>
class BivectorNetTypes<0, 0, 0> final {
 public:
  using type = R000;
};

//***********************
// R000.Reverse : res = ~a
// Reverse the order of the basis blades.
//***********************
inline R000 operator~(const R000 &a) {
  R000 res;
  res[0] = a[0];
  return res;
};

//***********************
// R000.Dual : res = !a
// Poincare duality operator.
//***********************
inline R000 operator!(const R000 &a) {
  R000 res;
  res[0] = a[0];
  return res;
};

//***********************
// R000.Conjugate : res = a.Conjugate()
// Clifford Conjugation
//***********************
inline R000 R000::Conjugate() {
  R000 res;
  res[0] = this->mvec[0];
  return res;
};

//***********************
// R000.Involute : res = a.Involute()
// Main involution
//***********************
inline R000 R000::Involute() {
  R000 res;
  res[0] = this->mvec[0];
  return res;
};

//***********************
// R000.Mul : res = a * b
// The geometric product.
//***********************
inline R000 operator*(const R000 &a, const R000 &b) {
  R000 res;
  res[0] = b[0] * a[0];
  return res;
};

//***********************
// R000.Wedge : res = a ^ b
// The outer product. (MEET)
//***********************
inline R000 operator^(const R000 &a, const R000 &b) {
  R000 res;
  res[0] = b[0] * a[0];
  return res;
};

//***********************
// R000.Vee : res = a & b
// The regressive product. (JOIN)
//***********************
inline R000 operator&(const R000 &a, const R000 &b) {
  R000 res;
  res[0] = 1 * (a[0] * b[0]);
  return res;
};

//***********************
// R000.Dot : res = a | b
// The inner product.
//***********************
inline R000 operator|(const R000 &a, const R000 &b) {
  R000 res;
  res[0] = b[0] * a[0];
  return res;
};

//***********************
// R000.Add : res = a + b
// Multivector addition
//***********************
inline R000 operator+(const R000 &a, const R000 &b) {
  R000 res;
  res[0] = a[0] + b[0];
  return res;
};

//***********************
// R000.Sub : res = a - b
// Multivector subtraction
//***********************
inline R000 operator-(const R000 &a, const R000 &b) {
  R000 res;
  res[0] = a[0] - b[0];
  return res;
};

//***********************
// R000.smul : res = a * b
// scalar/multivector multiplication
//***********************
inline R000 operator*(const float &a, const R000 &b) {
  R000 res;
  res[0] = a * b[0];
  return res;
};

//***********************
// R000.muls : res = a * b
// multivector/scalar multiplication
//***********************
inline R000 operator*(const R000 &a, const float &b) {
  R000 res;
  res[0] = a[0] * b;
  return res;
};

//***********************
// R000.sadd : res = a + b
// scalar/multivector addition
//***********************
inline R000 operator+(const float &a, const R000 &b) {
  R000 res;
  res[0] = a + b[0];
  return res;
};

//***********************
// R000.adds : res = a + b
// multivector/scalar addition
//***********************
inline R000 operator+(const R000 &a, const float &b) {
  R000 res;
  res[0] = a[0] + b;
  return res;
};

//***********************
// R000.ssub : res = a - b
// scalar/multivector subtraction
//***********************
inline R000 operator-(const float &a, const R000 &b) {
  R000 res;
  res[0] = a - b[0];
  return res;
};

//***********************
// R000.subs : res = a - b
// multivector/scalar subtraction
//***********************
inline R000 operator-(const R000 &a, const float &b) {
  R000 res;
  res[0] = a[0] - b;
  return res;
};

inline float R000::norm() { return sqrt(std::abs(((*this) * Conjugate()).mvec[0])); }
inline float R000::inorm() { return (!(*this)).norm(); }
inline R000 R000::normalized() { return (*this) * (1 / norm()); }
