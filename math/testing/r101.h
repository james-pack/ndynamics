#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "math/testing/types.h"

class R101 {
 public:
  static constexpr const char *basis[] = {"1", "e0", "e1", "e01"};
  static constexpr size_t NUM_BASES{4};
  static constexpr std::array<size_t, 4> bit_basis_indices{0, 1, 2, 3};

  static constexpr std::array<bool, 4> reversed_bases{0, 0, 0, 0};

  R101() { std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f); }
  R101(float f, int idx = 0) {
    std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f);
    mvec[idx] = f;
  }

  R101(const R101 &rhs) = default;
  R101(R101 &&rhs) = default;

  bool operator==(const R101 &rhs) const {
    for (size_t i = 0; i < NUM_BASES; ++i) {
      if (mvec[i] != rhs.mvec[i]) {
        return false;
      }
    }
    return true;
  }

  float &operator[](size_t idx) { return mvec[idx]; }
  const float &operator[](size_t idx) const { return mvec[idx]; }

  R101 Conjugate();
  R101 Involute();
  float norm();
  float inorm();
  R101 normalized();

 private:
  float mvec[4];
};

template <>
class BivectorNetTypes<1, 0, 1> final {
 public:
  using type = R101;
};

//***********************
// R101.Reverse : res = ~a
// Reverse the order of the basis blades.
//***********************
inline R101 operator~(const R101 &a) {
  R101 res;
  res[0] = a[0];
  res[1] = a[1];
  res[2] = a[2];
  res[3] = -a[3];
  return res;
};

//***********************
// R101.Dual : res = !a
// Poincare duality operator.
//***********************
inline R101 operator!(const R101 &a) {
  R101 res;
  res[0] = a[3];
  res[1] = a[2];
  res[2] = a[1];
  res[3] = a[0];
  return res;
};

//***********************
// R101.Conjugate : res = a.Conjugate()
// Clifford Conjugation
//***********************
inline R101 R101::Conjugate() {
  R101 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  res[2] = -this->mvec[2];
  res[3] = -this->mvec[3];
  return res;
};

//***********************
// R101.Involute : res = a.Involute()
// Main involution
//***********************
inline R101 R101::Involute() {
  R101 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  res[2] = -this->mvec[2];
  res[3] = this->mvec[3];
  return res;
};

//***********************
// R101.Mul : res = a * b
// The geometric product.
//***********************
inline R101 operator*(const R101 &a, const R101 &b) {
  R101 res;
  res[0] = b[0] * a[0] + b[2] * a[2];
  res[1] = b[1] * a[0] + b[0] * a[1] - b[3] * a[2] + b[2] * a[3];
  res[2] = b[2] * a[0] + b[0] * a[2];
  res[3] = b[3] * a[0] + b[2] * a[1] - b[1] * a[2] + b[0] * a[3];
  return res;
};

//***********************
// R101.Wedge : res = a ^ b
// The outer product. (MEET)
//***********************
inline R101 operator^(const R101 &a, const R101 &b) {
  R101 res;
  res[0] = b[0] * a[0];
  res[1] = b[1] * a[0] + b[0] * a[1];
  res[2] = b[2] * a[0] + b[0] * a[2];
  res[3] = b[3] * a[0] + b[2] * a[1] - b[1] * a[2] + b[0] * a[3];
  return res;
};

//***********************
// R101.Vee : res = a & b
// The regressive product. (JOIN)
//***********************
inline R101 operator&(const R101 &a, const R101 &b) {
  R101 res;
  res[3] = 1 * (a[3] * b[3]);
  res[2] = -1 * (a[2] * -1 * b[3] + a[3] * b[2] * -1);
  res[1] = 1 * (a[1] * b[3] + a[3] * b[1]);
  res[0] = 1 * (a[0] * b[3] + a[1] * b[2] * -1 - a[2] * -1 * b[1] + a[3] * b[0]);
  return res;
};

//***********************
// R101.Dot : res = a | b
// The inner product.
//***********************
inline R101 operator|(const R101 &a, const R101 &b) {
  R101 res;
  res[0] = b[0] * a[0] + b[2] * a[2];
  res[1] = b[1] * a[0] + b[0] * a[1] - b[3] * a[2] + b[2] * a[3];
  res[2] = b[2] * a[0] + b[0] * a[2];
  res[3] = b[3] * a[0] + b[0] * a[3];
  return res;
};

//***********************
// R101.Add : res = a + b
// Multivector addition
//***********************
inline R101 operator+(const R101 &a, const R101 &b) {
  R101 res;
  res[0] = a[0] + b[0];
  res[1] = a[1] + b[1];
  res[2] = a[2] + b[2];
  res[3] = a[3] + b[3];
  return res;
};

//***********************
// R101.Sub : res = a - b
// Multivector subtraction
//***********************
inline R101 operator-(const R101 &a, const R101 &b) {
  R101 res;
  res[0] = a[0] - b[0];
  res[1] = a[1] - b[1];
  res[2] = a[2] - b[2];
  res[3] = a[3] - b[3];
  return res;
};

//***********************
// R101.smul : res = a * b
// scalar/multivector multiplication
//***********************
inline R101 operator*(const float &a, const R101 &b) {
  R101 res;
  res[0] = a * b[0];
  res[1] = a * b[1];
  res[2] = a * b[2];
  res[3] = a * b[3];
  return res;
};

//***********************
// R101.muls : res = a * b
// multivector/scalar multiplication
//***********************
inline R101 operator*(const R101 &a, const float &b) {
  R101 res;
  res[0] = a[0] * b;
  res[1] = a[1] * b;
  res[2] = a[2] * b;
  res[3] = a[3] * b;
  return res;
};

//***********************
// R101.sadd : res = a + b
// scalar/multivector addition
//***********************
inline R101 operator+(const float &a, const R101 &b) {
  R101 res;
  res[0] = a + b[0];
  res[1] = b[1];
  res[2] = b[2];
  res[3] = b[3];
  return res;
};

//***********************
// R101.adds : res = a + b
// multivector/scalar addition
//***********************
inline R101 operator+(const R101 &a, const float &b) {
  R101 res;
  res[0] = a[0] + b;
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  return res;
};

//***********************
// R101.ssub : res = a - b
// scalar/multivector subtraction
//***********************
inline R101 operator-(const float &a, const R101 &b) {
  R101 res;
  res[0] = a - b[0];
  res[1] = -b[1];
  res[2] = -b[2];
  res[3] = -b[3];
  return res;
};

//***********************
// R101.subs : res = a - b
// multivector/scalar subtraction
//***********************
inline R101 operator-(const R101 &a, const float &b) {
  R101 res;
  res[0] = a[0] - b;
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  return res;
};

inline float R101::norm() { return sqrt(std::abs(((*this) * Conjugate()).mvec[0])); }
inline float R101::inorm() { return (!(*this)).norm(); }
inline R101 R101::normalized() { return (*this) * (1 / norm()); }
