#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "math/testing/types.h"

class R110 {
 public:
  static constexpr const char *basis[] = {"1", "e1", "e2", "e12"};
  static constexpr std::array<size_t, 4> bit_basis_indices{0, 1, 2, 3};

  static constexpr std::array<bool, 4> reversed_bases{0, 0, 0, 0};

  R110() { std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f); }
  R110(float f, int idx = 0) {
    std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f);
    mvec[idx] = f;
  }
  float &operator[](size_t idx) { return mvec[idx]; }
  const float &operator[](size_t idx) const { return mvec[idx]; }

  R110 Conjugate();
  R110 Involute();
  float norm();
  float inorm();
  R110 normalized();

 private:
  float mvec[4];
};

template <>
class BivectorNetTypes<1, 1, 0> final {
 public:
  using type = R110;
};

//***********************
// R110.Reverse : res = ~a
// Reverse the order of the basis blades.
//***********************
inline R110 operator~(const R110 &a) {
  R110 res;
  res[0] = a[0];
  res[1] = a[1];
  res[2] = a[2];
  res[3] = -a[3];
  return res;
};

//***********************
// R110.Dual : res = !a
// Poincare duality operator.
//***********************
inline R110 operator!(const R110 &a) {
  R110 res;
  res[0] = a[3];
  res[1] = a[2];
  res[2] = a[1];
  res[3] = a[0];
  return res;
};

//***********************
// R110.Conjugate : res = a.Conjugate()
// Clifford Conjugation
//***********************
inline R110 R110::Conjugate() {
  R110 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  res[2] = -this->mvec[2];
  res[3] = -this->mvec[3];
  return res;
};

//***********************
// R110.Involute : res = a.Involute()
// Main involution
//***********************
inline R110 R110::Involute() {
  R110 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  res[2] = -this->mvec[2];
  res[3] = this->mvec[3];
  return res;
};

//***********************
// R110.Mul : res = a * b
// The geometric product.
//***********************
inline R110 operator*(const R110 &a, const R110 &b) {
  R110 res;
  res[0] = b[0] * a[0] + b[1] * a[1] - b[2] * a[2] + b[3] * a[3];
  res[1] = b[1] * a[0] + b[0] * a[1] + b[3] * a[2] - b[2] * a[3];
  res[2] = b[2] * a[0] + b[3] * a[1] + b[0] * a[2] - b[1] * a[3];
  res[3] = b[3] * a[0] + b[2] * a[1] - b[1] * a[2] + b[0] * a[3];
  return res;
};

//***********************
// R110.Wedge : res = a ^ b
// The outer product. (MEET)
//***********************
inline R110 operator^(const R110 &a, const R110 &b) {
  R110 res;
  res[0] = b[0] * a[0];
  res[1] = b[1] * a[0] + b[0] * a[1];
  res[2] = b[2] * a[0] + b[0] * a[2];
  res[3] = b[3] * a[0] + b[2] * a[1] - b[1] * a[2] + b[0] * a[3];
  return res;
};

//***********************
// R110.Vee : res = a & b
// The regressive product. (JOIN)
//***********************
inline R110 operator&(const R110 &a, const R110 &b) {
  R110 res;
  res[3] = 1 * (a[3] * b[3]);
  res[2] = -1 * (a[2] * -1 * b[3] + a[3] * b[2] * -1);
  res[1] = 1 * (a[1] * b[3] + a[3] * b[1]);
  res[0] = 1 * (a[0] * b[3] + a[1] * b[2] * -1 - a[2] * -1 * b[1] + a[3] * b[0]);
  return res;
};

//***********************
// R110.Dot : res = a | b
// The inner product.
//***********************
inline R110 operator|(const R110 &a, const R110 &b) {
  R110 res;
  res[0] = b[0] * a[0] + b[1] * a[1] - b[2] * a[2] + b[3] * a[3];
  res[1] = b[1] * a[0] + b[0] * a[1] + b[3] * a[2] - b[2] * a[3];
  res[2] = b[2] * a[0] + b[3] * a[1] + b[0] * a[2] - b[1] * a[3];
  res[3] = b[3] * a[0] + b[0] * a[3];
  return res;
};

//***********************
// R110.Add : res = a + b
// Multivector addition
//***********************
inline R110 operator+(const R110 &a, const R110 &b) {
  R110 res;
  res[0] = a[0] + b[0];
  res[1] = a[1] + b[1];
  res[2] = a[2] + b[2];
  res[3] = a[3] + b[3];
  return res;
};

//***********************
// R110.Sub : res = a - b
// Multivector subtraction
//***********************
inline R110 operator-(const R110 &a, const R110 &b) {
  R110 res;
  res[0] = a[0] - b[0];
  res[1] = a[1] - b[1];
  res[2] = a[2] - b[2];
  res[3] = a[3] - b[3];
  return res;
};

//***********************
// R110.smul : res = a * b
// scalar/multivector multiplication
//***********************
inline R110 operator*(const float &a, const R110 &b) {
  R110 res;
  res[0] = a * b[0];
  res[1] = a * b[1];
  res[2] = a * b[2];
  res[3] = a * b[3];
  return res;
};

//***********************
// R110.muls : res = a * b
// multivector/scalar multiplication
//***********************
inline R110 operator*(const R110 &a, const float &b) {
  R110 res;
  res[0] = a[0] * b;
  res[1] = a[1] * b;
  res[2] = a[2] * b;
  res[3] = a[3] * b;
  return res;
};

//***********************
// R110.sadd : res = a + b
// scalar/multivector addition
//***********************
inline R110 operator+(const float &a, const R110 &b) {
  R110 res;
  res[0] = a + b[0];
  res[1] = b[1];
  res[2] = b[2];
  res[3] = b[3];
  return res;
};

//***********************
// R110.adds : res = a + b
// multivector/scalar addition
//***********************
inline R110 operator+(const R110 &a, const float &b) {
  R110 res;
  res[0] = a[0] + b;
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  return res;
};

//***********************
// R110.ssub : res = a - b
// scalar/multivector subtraction
//***********************
inline R110 operator-(const float &a, const R110 &b) {
  R110 res;
  res[0] = a - b[0];
  res[1] = -b[1];
  res[2] = -b[2];
  res[3] = -b[3];
  return res;
};

//***********************
// R110.subs : res = a - b
// multivector/scalar subtraction
//***********************
inline R110 operator-(const R110 &a, const float &b) {
  R110 res;
  res[0] = a[0] - b;
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  return res;
};

inline float R110::norm() { return sqrt(std::abs(((*this) * Conjugate()).mvec[0])); }
inline float R110::inorm() { return (!(*this)).norm(); }
inline R110 R110::normalized() { return (*this) * (1 / norm()); }
