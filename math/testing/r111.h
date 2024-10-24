#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "math/testing/types.h"

class R111 {
 public:
  static constexpr const char *basis[] = {"1", "e0", "e1", "e2", "e01", "e02", "e12", "e012"};
  static constexpr std::array<size_t, 8> bit_basis_indices{0, 1, 2, 4, 3, 5, 6, 7};

  static constexpr std::array<bool, 8> reversed_bases{0, 0, 0, 0, 0, 0, 0, 0};

  R111() { std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f); }
  R111(float f, int idx = 0) {
    std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f);
    mvec[idx] = f;
  }
  float &operator[](size_t idx) { return mvec[idx]; }
  const float &operator[](size_t idx) const { return mvec[idx]; }

  R111 Conjugate();
  R111 Involute();
  float norm();
  float inorm();
  R111 normalized();

 private:
  float mvec[8];
};

template <>
class BivectorNetTypes<1, 1, 1> final {
 public:
  using type = R111;
};

//***********************
// R111.Reverse : res = ~a
// Reverse the order of the basis blades.
//***********************
inline R111 operator~(const R111 &a) {
  R111 res;
  res[0] = a[0];
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  res[4] = -a[4];
  res[5] = -a[5];
  res[6] = -a[6];
  res[7] = -a[7];
  return res;
};

//***********************
// R111.Dual : res = !a
// Poincare duality operator.
//***********************
inline R111 operator!(const R111 &a) {
  R111 res;
  res[0] = a[7];
  res[1] = a[6];
  res[2] = a[5];
  res[3] = a[4];
  res[4] = a[3];
  res[5] = a[2];
  res[6] = a[1];
  res[7] = a[0];
  return res;
};

//***********************
// R111.Conjugate : res = a.Conjugate()
// Clifford Conjugation
//***********************
inline R111 R111::Conjugate() {
  R111 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  res[2] = -this->mvec[2];
  res[3] = -this->mvec[3];
  res[4] = -this->mvec[4];
  res[5] = -this->mvec[5];
  res[6] = -this->mvec[6];
  res[7] = this->mvec[7];
  return res;
};

//***********************
// R111.Involute : res = a.Involute()
// Main involution
//***********************
inline R111 R111::Involute() {
  R111 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  res[2] = -this->mvec[2];
  res[3] = -this->mvec[3];
  res[4] = this->mvec[4];
  res[5] = this->mvec[5];
  res[6] = this->mvec[6];
  res[7] = -this->mvec[7];
  return res;
};

//***********************
// R111.Mul : res = a * b
// The geometric product.
//***********************
inline R111 operator*(const R111 &a, const R111 &b) {
  R111 res;
  res[0] = b[0] * a[0] + b[2] * a[2] - b[3] * a[3] + b[6] * a[6];
  res[1] = b[1] * a[0] + b[0] * a[1] - b[4] * a[2] + b[5] * a[3] + b[2] * a[4] - b[3] * a[5] +
           b[7] * a[6] + b[6] * a[7];
  res[2] = b[2] * a[0] + b[0] * a[2] + b[6] * a[3] - b[3] * a[6];
  res[3] = b[3] * a[0] + b[6] * a[2] + b[0] * a[3] - b[2] * a[6];
  res[4] = b[4] * a[0] + b[2] * a[1] - b[1] * a[2] - b[7] * a[3] + b[0] * a[4] + b[6] * a[5] -
           b[5] * a[6] - b[3] * a[7];
  res[5] = b[5] * a[0] + b[3] * a[1] - b[7] * a[2] - b[1] * a[3] + b[6] * a[4] + b[0] * a[5] -
           b[4] * a[6] - b[2] * a[7];
  res[6] = b[6] * a[0] + b[3] * a[2] - b[2] * a[3] + b[0] * a[6];
  res[7] = b[7] * a[0] + b[6] * a[1] - b[5] * a[2] + b[4] * a[3] + b[3] * a[4] - b[2] * a[5] +
           b[1] * a[6] + b[0] * a[7];
  return res;
};

//***********************
// R111.Wedge : res = a ^ b
// The outer product. (MEET)
//***********************
inline R111 operator^(const R111 &a, const R111 &b) {
  R111 res;
  res[0] = b[0] * a[0];
  res[1] = b[1] * a[0] + b[0] * a[1];
  res[2] = b[2] * a[0] + b[0] * a[2];
  res[3] = b[3] * a[0] + b[0] * a[3];
  res[4] = b[4] * a[0] + b[2] * a[1] - b[1] * a[2] + b[0] * a[4];
  res[5] = b[5] * a[0] + b[3] * a[1] - b[1] * a[3] + b[0] * a[5];
  res[6] = b[6] * a[0] + b[3] * a[2] - b[2] * a[3] + b[0] * a[6];
  res[7] = b[7] * a[0] + b[6] * a[1] - b[5] * a[2] + b[4] * a[3] + b[3] * a[4] - b[2] * a[5] +
           b[1] * a[6] + b[0] * a[7];
  return res;
};

//***********************
// R111.Vee : res = a & b
// The regressive product. (JOIN)
//***********************
inline R111 operator&(const R111 &a, const R111 &b) {
  R111 res;
  res[7] = 1 * (a[7] * b[7]);
  res[6] = 1 * (a[6] * b[7] + a[7] * b[6]);
  res[5] = -1 * (a[5] * -1 * b[7] + a[7] * b[5] * -1);
  res[4] = 1 * (a[4] * b[7] + a[7] * b[4]);
  res[3] = 1 * (a[3] * b[7] + a[5] * -1 * b[6] - a[6] * b[5] * -1 + a[7] * b[3]);
  res[2] = -1 * (a[2] * -1 * b[7] + a[4] * b[6] - a[6] * b[4] + a[7] * b[2] * -1);
  res[1] = 1 * (a[1] * b[7] + a[4] * b[5] * -1 - a[5] * -1 * b[4] + a[7] * b[1]);
  res[0] = 1 * (a[0] * b[7] + a[1] * b[6] - a[2] * -1 * b[5] * -1 + a[3] * b[4] + a[4] * b[3] -
                a[5] * -1 * b[2] * -1 + a[6] * b[1] + a[7] * b[0]);
  return res;
};

//***********************
// R111.Dot : res = a | b
// The inner product.
//***********************
inline R111 operator|(const R111 &a, const R111 &b) {
  R111 res;
  res[0] = b[0] * a[0] + b[2] * a[2] - b[3] * a[3] + b[6] * a[6];
  res[1] = b[1] * a[0] + b[0] * a[1] - b[4] * a[2] + b[5] * a[3] + b[2] * a[4] - b[3] * a[5] +
           b[7] * a[6] + b[6] * a[7];
  res[2] = b[2] * a[0] + b[0] * a[2] + b[6] * a[3] - b[3] * a[6];
  res[3] = b[3] * a[0] + b[6] * a[2] + b[0] * a[3] - b[2] * a[6];
  res[4] = b[4] * a[0] - b[7] * a[3] + b[0] * a[4] - b[3] * a[7];
  res[5] = b[5] * a[0] - b[7] * a[2] + b[0] * a[5] - b[2] * a[7];
  res[6] = b[6] * a[0] + b[0] * a[6];
  res[7] = b[7] * a[0] + b[0] * a[7];
  return res;
};

//***********************
// R111.Add : res = a + b
// Multivector addition
//***********************
inline R111 operator+(const R111 &a, const R111 &b) {
  R111 res;
  res[0] = a[0] + b[0];
  res[1] = a[1] + b[1];
  res[2] = a[2] + b[2];
  res[3] = a[3] + b[3];
  res[4] = a[4] + b[4];
  res[5] = a[5] + b[5];
  res[6] = a[6] + b[6];
  res[7] = a[7] + b[7];
  return res;
};

//***********************
// R111.Sub : res = a - b
// Multivector subtraction
//***********************
inline R111 operator-(const R111 &a, const R111 &b) {
  R111 res;
  res[0] = a[0] - b[0];
  res[1] = a[1] - b[1];
  res[2] = a[2] - b[2];
  res[3] = a[3] - b[3];
  res[4] = a[4] - b[4];
  res[5] = a[5] - b[5];
  res[6] = a[6] - b[6];
  res[7] = a[7] - b[7];
  return res;
};

//***********************
// R111.smul : res = a * b
// scalar/multivector multiplication
//***********************
inline R111 operator*(const float &a, const R111 &b) {
  R111 res;
  res[0] = a * b[0];
  res[1] = a * b[1];
  res[2] = a * b[2];
  res[3] = a * b[3];
  res[4] = a * b[4];
  res[5] = a * b[5];
  res[6] = a * b[6];
  res[7] = a * b[7];
  return res;
};

//***********************
// R111.muls : res = a * b
// multivector/scalar multiplication
//***********************
inline R111 operator*(const R111 &a, const float &b) {
  R111 res;
  res[0] = a[0] * b;
  res[1] = a[1] * b;
  res[2] = a[2] * b;
  res[3] = a[3] * b;
  res[4] = a[4] * b;
  res[5] = a[5] * b;
  res[6] = a[6] * b;
  res[7] = a[7] * b;
  return res;
};

//***********************
// R111.sadd : res = a + b
// scalar/multivector addition
//***********************
inline R111 operator+(const float &a, const R111 &b) {
  R111 res;
  res[0] = a + b[0];
  res[1] = b[1];
  res[2] = b[2];
  res[3] = b[3];
  res[4] = b[4];
  res[5] = b[5];
  res[6] = b[6];
  res[7] = b[7];
  return res;
};

//***********************
// R111.adds : res = a + b
// multivector/scalar addition
//***********************
inline R111 operator+(const R111 &a, const float &b) {
  R111 res;
  res[0] = a[0] + b;
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  res[4] = a[4];
  res[5] = a[5];
  res[6] = a[6];
  res[7] = a[7];
  return res;
};

//***********************
// R111.ssub : res = a - b
// scalar/multivector subtraction
//***********************
inline R111 operator-(const float &a, const R111 &b) {
  R111 res;
  res[0] = a - b[0];
  res[1] = -b[1];
  res[2] = -b[2];
  res[3] = -b[3];
  res[4] = -b[4];
  res[5] = -b[5];
  res[6] = -b[6];
  res[7] = -b[7];
  return res;
};

//***********************
// R111.subs : res = a - b
// multivector/scalar subtraction
//***********************
inline R111 operator-(const R111 &a, const float &b) {
  R111 res;
  res[0] = a[0] - b;
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  res[4] = a[4];
  res[5] = a[5];
  res[6] = a[6];
  res[7] = a[7];
  return res;
};

inline float R111::norm() { return sqrt(std::abs(((*this) * Conjugate()).mvec[0])); }
inline float R111::inorm() { return (!(*this)).norm(); }
inline R111 R111::normalized() { return (*this) * (1 / norm()); }
