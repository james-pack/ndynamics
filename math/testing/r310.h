#pragma once

#include <algorithm>
#include <array>
#include <cmath>

#include "math/testing/types.h"

class R310 {
 public:
  static constexpr const char *basis[] = {"1",   "e1",  "e2",  "e3",   "e4",   "e12",  "e13",  "e14",
                                "e23", "e24", "e34", "e123", "e124", "e134", "e234", "e1234"};
  static constexpr std::array<size_t, 16> bit_basis_indices{0, 1,  2,  4, 8,  3,  5,  9,
                                                            6, 10, 12, 7, 11, 13, 14, 15};

  static constexpr std::array<bool, 16> reversed_bases{0, 0, 0, 0, 0, 0, 0, 0,
                                                       0, 0, 0, 0, 0, 0, 0, 0};

  R310() { std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f); }
  R310(float f, int idx = 0) {
    std::fill(mvec, mvec + sizeof(mvec) / 4, 0.0f);
    mvec[idx] = f;
  }
  float &operator[](size_t idx) { return mvec[idx]; }
  const float &operator[](size_t idx) const { return mvec[idx]; }

  R310 Conjugate();
  R310 Involute();
  float norm();
  float inorm();
  R310 normalized();

 private:
  float mvec[16];
};

template <>
class BivectorNetTypes<3, 1, 0> final {
 public:
  using type = R310;
};

//***********************
// R310.Reverse : res = ~a
// Reverse the order of the basis blades.
//***********************
inline R310 operator~(const R310 &a) {
  R310 res;
  res[0] = a[0];
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  res[4] = a[4];
  res[5] = -a[5];
  res[6] = -a[6];
  res[7] = -a[7];
  res[8] = -a[8];
  res[9] = -a[9];
  res[10] = -a[10];
  res[11] = -a[11];
  res[12] = -a[12];
  res[13] = -a[13];
  res[14] = -a[14];
  res[15] = a[15];
  return res;
};

//***********************
// R310.Dual : res = !a
// Poincare duality operator.
//***********************
inline R310 operator!(const R310 &a) {
  R310 res;
  res[0] = -a[15];
  res[1] = -a[14];
  res[2] = a[13];
  res[3] = -a[12];
  res[4] = -a[11];
  res[5] = a[10];
  res[6] = -a[9];
  res[7] = -a[8];
  res[8] = a[7];
  res[9] = a[6];
  res[10] = -a[5];
  res[11] = a[4];
  res[12] = a[3];
  res[13] = -a[2];
  res[14] = a[1];
  res[15] = a[0];
  return res;
};

//***********************
// R310.Conjugate : res = a.Conjugate()
// Clifford Conjugation
//***********************
inline R310 R310::Conjugate() {
  R310 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  res[2] = -this->mvec[2];
  res[3] = -this->mvec[3];
  res[4] = -this->mvec[4];
  res[5] = -this->mvec[5];
  res[6] = -this->mvec[6];
  res[7] = -this->mvec[7];
  res[8] = -this->mvec[8];
  res[9] = -this->mvec[9];
  res[10] = -this->mvec[10];
  res[11] = this->mvec[11];
  res[12] = this->mvec[12];
  res[13] = this->mvec[13];
  res[14] = this->mvec[14];
  res[15] = this->mvec[15];
  return res;
};

//***********************
// R310.Involute : res = a.Involute()
// Main involution
//***********************
inline R310 R310::Involute() {
  R310 res;
  res[0] = this->mvec[0];
  res[1] = -this->mvec[1];
  res[2] = -this->mvec[2];
  res[3] = -this->mvec[3];
  res[4] = -this->mvec[4];
  res[5] = this->mvec[5];
  res[6] = this->mvec[6];
  res[7] = this->mvec[7];
  res[8] = this->mvec[8];
  res[9] = this->mvec[9];
  res[10] = this->mvec[10];
  res[11] = -this->mvec[11];
  res[12] = -this->mvec[12];
  res[13] = -this->mvec[13];
  res[14] = -this->mvec[14];
  res[15] = this->mvec[15];
  return res;
};

//***********************
// R310.Mul : res = a * b
// The geometric product.
//***********************
inline R310 operator*(const R310 &a, const R310 &b) {
  R310 res;
  res[0] = b[0] * a[0] + b[1] * a[1] + b[2] * a[2] + b[3] * a[3] - b[4] * a[4] - b[5] * a[5] -
           b[6] * a[6] + b[7] * a[7] - b[8] * a[8] + b[9] * a[9] + b[10] * a[10] - b[11] * a[11] +
           b[12] * a[12] + b[13] * a[13] + b[14] * a[14] - b[15] * a[15];
  res[1] = b[1] * a[0] + b[0] * a[1] - b[5] * a[2] - b[6] * a[3] + b[7] * a[4] + b[2] * a[5] +
           b[3] * a[6] - b[4] * a[7] - b[11] * a[8] + b[12] * a[9] + b[13] * a[10] - b[8] * a[11] +
           b[9] * a[12] + b[10] * a[13] - b[15] * a[14] + b[14] * a[15];
  res[2] = b[2] * a[0] + b[5] * a[1] + b[0] * a[2] - b[8] * a[3] + b[9] * a[4] - b[1] * a[5] +
           b[11] * a[6] - b[12] * a[7] + b[3] * a[8] - b[4] * a[9] + b[14] * a[10] + b[6] * a[11] -
           b[7] * a[12] + b[15] * a[13] + b[10] * a[14] - b[13] * a[15];
  res[3] = b[3] * a[0] + b[6] * a[1] + b[8] * a[2] + b[0] * a[3] + b[10] * a[4] - b[11] * a[5] -
           b[1] * a[6] - b[13] * a[7] - b[2] * a[8] - b[14] * a[9] - b[4] * a[10] - b[5] * a[11] -
           b[15] * a[12] - b[7] * a[13] - b[9] * a[14] + b[12] * a[15];
  res[4] = b[4] * a[0] + b[7] * a[1] + b[9] * a[2] + b[10] * a[3] + b[0] * a[4] - b[12] * a[5] -
           b[13] * a[6] - b[1] * a[7] - b[14] * a[8] - b[2] * a[9] - b[3] * a[10] - b[15] * a[11] -
           b[5] * a[12] - b[6] * a[13] - b[8] * a[14] + b[11] * a[15];
  res[5] = b[5] * a[0] + b[2] * a[1] - b[1] * a[2] + b[11] * a[3] - b[12] * a[4] + b[0] * a[5] -
           b[8] * a[6] + b[9] * a[7] + b[6] * a[8] - b[7] * a[9] + b[15] * a[10] + b[3] * a[11] -
           b[4] * a[12] + b[14] * a[13] - b[13] * a[14] + b[10] * a[15];
  res[6] = b[6] * a[0] + b[3] * a[1] - b[11] * a[2] - b[1] * a[3] - b[13] * a[4] + b[8] * a[5] +
           b[0] * a[6] + b[10] * a[7] - b[5] * a[8] - b[15] * a[9] - b[7] * a[10] - b[2] * a[11] -
           b[14] * a[12] - b[4] * a[13] + b[12] * a[14] - b[9] * a[15];
  res[7] = b[7] * a[0] + b[4] * a[1] - b[12] * a[2] - b[13] * a[3] - b[1] * a[4] + b[9] * a[5] +
           b[10] * a[6] + b[0] * a[7] - b[15] * a[8] - b[5] * a[9] - b[6] * a[10] - b[14] * a[11] -
           b[2] * a[12] - b[3] * a[13] + b[11] * a[14] - b[8] * a[15];
  res[8] = b[8] * a[0] + b[11] * a[1] + b[3] * a[2] - b[2] * a[3] - b[14] * a[4] - b[6] * a[5] +
           b[5] * a[6] + b[15] * a[7] + b[0] * a[8] + b[10] * a[9] - b[9] * a[10] + b[1] * a[11] +
           b[13] * a[12] - b[12] * a[13] - b[4] * a[14] + b[7] * a[15];
  res[9] = b[9] * a[0] + b[12] * a[1] + b[4] * a[2] - b[14] * a[3] - b[2] * a[4] - b[7] * a[5] +
           b[15] * a[6] + b[5] * a[7] + b[10] * a[8] + b[0] * a[9] - b[8] * a[10] + b[13] * a[11] +
           b[1] * a[12] - b[11] * a[13] - b[3] * a[14] + b[6] * a[15];
  res[10] = b[10] * a[0] + b[13] * a[1] + b[14] * a[2] + b[4] * a[3] - b[3] * a[4] - b[15] * a[5] -
            b[7] * a[6] + b[6] * a[7] - b[9] * a[8] + b[8] * a[9] + b[0] * a[10] - b[12] * a[11] +
            b[11] * a[12] + b[1] * a[13] + b[2] * a[14] - b[5] * a[15];
  res[11] = b[11] * a[0] + b[8] * a[1] - b[6] * a[2] + b[5] * a[3] + b[15] * a[4] + b[3] * a[5] -
            b[2] * a[6] - b[14] * a[7] + b[1] * a[8] + b[13] * a[9] - b[12] * a[10] + b[0] * a[11] +
            b[10] * a[12] - b[9] * a[13] + b[7] * a[14] - b[4] * a[15];
  res[12] = b[12] * a[0] + b[9] * a[1] - b[7] * a[2] + b[15] * a[3] + b[5] * a[4] + b[4] * a[5] -
            b[14] * a[6] - b[2] * a[7] + b[13] * a[8] + b[1] * a[9] - b[11] * a[10] +
            b[10] * a[11] + b[0] * a[12] - b[8] * a[13] + b[6] * a[14] - b[3] * a[15];
  res[13] = b[13] * a[0] + b[10] * a[1] - b[15] * a[2] - b[7] * a[3] + b[6] * a[4] + b[14] * a[5] +
            b[4] * a[6] - b[3] * a[7] - b[12] * a[8] + b[11] * a[9] + b[1] * a[10] - b[9] * a[11] +
            b[8] * a[12] + b[0] * a[13] - b[5] * a[14] + b[2] * a[15];
  res[14] = b[14] * a[0] + b[15] * a[1] + b[10] * a[2] - b[9] * a[3] + b[8] * a[4] - b[13] * a[5] +
            b[12] * a[6] - b[11] * a[7] + b[4] * a[8] - b[3] * a[9] + b[2] * a[10] + b[7] * a[11] -
            b[6] * a[12] + b[5] * a[13] + b[0] * a[14] - b[1] * a[15];
  res[15] = b[15] * a[0] + b[14] * a[1] - b[13] * a[2] + b[12] * a[3] - b[11] * a[4] +
            b[10] * a[5] - b[9] * a[6] + b[8] * a[7] + b[7] * a[8] - b[6] * a[9] + b[5] * a[10] +
            b[4] * a[11] - b[3] * a[12] + b[2] * a[13] - b[1] * a[14] + b[0] * a[15];
  return res;
};

//***********************
// R310.Wedge : res = a ^ b
// The outer product. (MEET)
//***********************
inline R310 operator^(const R310 &a, const R310 &b) {
  R310 res;
  res[0] = b[0] * a[0];
  res[1] = b[1] * a[0] + b[0] * a[1];
  res[2] = b[2] * a[0] + b[0] * a[2];
  res[3] = b[3] * a[0] + b[0] * a[3];
  res[4] = b[4] * a[0] + b[0] * a[4];
  res[5] = b[5] * a[0] + b[2] * a[1] - b[1] * a[2] + b[0] * a[5];
  res[6] = b[6] * a[0] + b[3] * a[1] - b[1] * a[3] + b[0] * a[6];
  res[7] = b[7] * a[0] + b[4] * a[1] - b[1] * a[4] + b[0] * a[7];
  res[8] = b[8] * a[0] + b[3] * a[2] - b[2] * a[3] + b[0] * a[8];
  res[9] = b[9] * a[0] + b[4] * a[2] - b[2] * a[4] + b[0] * a[9];
  res[10] = b[10] * a[0] + b[4] * a[3] - b[3] * a[4] + b[0] * a[10];
  res[11] = b[11] * a[0] + b[8] * a[1] - b[6] * a[2] + b[5] * a[3] + b[3] * a[5] - b[2] * a[6] +
            b[1] * a[8] + b[0] * a[11];
  res[12] = b[12] * a[0] + b[9] * a[1] - b[7] * a[2] + b[5] * a[4] + b[4] * a[5] - b[2] * a[7] +
            b[1] * a[9] + b[0] * a[12];
  res[13] = b[13] * a[0] + b[10] * a[1] - b[7] * a[3] + b[6] * a[4] + b[4] * a[6] - b[3] * a[7] +
            b[1] * a[10] + b[0] * a[13];
  res[14] = b[14] * a[0] + b[10] * a[2] - b[9] * a[3] + b[8] * a[4] + b[4] * a[8] - b[3] * a[9] +
            b[2] * a[10] + b[0] * a[14];
  res[15] = b[15] * a[0] + b[14] * a[1] - b[13] * a[2] + b[12] * a[3] - b[11] * a[4] +
            b[10] * a[5] - b[9] * a[6] + b[8] * a[7] + b[7] * a[8] - b[6] * a[9] + b[5] * a[10] +
            b[4] * a[11] - b[3] * a[12] + b[2] * a[13] - b[1] * a[14] + b[0] * a[15];
  return res;
};

//***********************
// R310.Vee : res = a & b
// The regressive product. (JOIN)
//***********************
inline R310 operator&(const R310 &a, const R310 &b) {
  R310 res;
  res[15] = 1 * (a[15] * b[15]);
  res[14] = -1 * (a[14] * -1 * b[15] + a[15] * b[14] * -1);
  res[13] = 1 * (a[13] * b[15] + a[15] * b[13]);
  res[12] = -1 * (a[12] * -1 * b[15] + a[15] * b[12] * -1);
  res[11] = 1 * (a[11] * b[15] + a[15] * b[11]);
  res[10] = 1 * (a[10] * b[15] + a[13] * b[14] * -1 - a[14] * -1 * b[13] + a[15] * b[10]);
  res[9] = -1 * (a[9] * -1 * b[15] + a[12] * -1 * b[14] * -1 - a[14] * -1 * b[12] * -1 +
                 a[15] * b[9] * -1);
  res[8] = 1 * (a[8] * b[15] + a[11] * b[14] * -1 - a[14] * -1 * b[11] + a[15] * b[8]);
  res[7] = 1 * (a[7] * b[15] + a[12] * -1 * b[13] - a[13] * b[12] * -1 + a[15] * b[7]);
  res[6] = -1 * (a[6] * -1 * b[15] + a[11] * b[13] - a[13] * b[11] + a[15] * b[6] * -1);
  res[5] = 1 * (a[5] * b[15] + a[11] * b[12] * -1 - a[12] * -1 * b[11] + a[15] * b[5]);
  res[4] = -1 * (a[4] * -1 * b[15] + a[7] * b[14] * -1 - a[9] * -1 * b[13] + a[10] * b[12] * -1 +
                 a[12] * -1 * b[10] - a[13] * b[9] * -1 + a[14] * -1 * b[7] + a[15] * b[4] * -1);
  res[3] = 1 * (a[3] * b[15] + a[6] * -1 * b[14] * -1 - a[8] * b[13] + a[10] * b[11] +
                a[11] * b[10] - a[13] * b[8] + a[14] * -1 * b[6] * -1 + a[15] * b[3]);
  res[2] = -1 * (a[2] * -1 * b[15] + a[5] * b[14] * -1 - a[8] * b[12] * -1 + a[9] * -1 * b[11] +
                 a[11] * b[9] * -1 - a[12] * -1 * b[8] + a[14] * -1 * b[5] + a[15] * b[2] * -1);
  res[1] = 1 * (a[1] * b[15] + a[5] * b[13] - a[6] * -1 * b[12] * -1 + a[7] * b[11] + a[11] * b[7] -
                a[12] * -1 * b[6] * -1 + a[13] * b[5] + a[15] * b[1]);
  res[0] = 1 * (a[0] * b[15] + a[1] * b[14] * -1 - a[2] * -1 * b[13] + a[3] * b[12] * -1 -
                a[4] * -1 * b[11] + a[5] * b[10] - a[6] * -1 * b[9] * -1 + a[7] * b[8] +
                a[8] * b[7] - a[9] * -1 * b[6] * -1 + a[10] * b[5] + a[11] * b[4] * -1 -
                a[12] * -1 * b[3] + a[13] * b[2] * -1 - a[14] * -1 * b[1] + a[15] * b[0]);
  return res;
};

//***********************
// R310.Dot : res = a | b
// The inner product.
//***********************
inline R310 operator|(const R310 &a, const R310 &b) {
  R310 res;
  res[0] = b[0] * a[0] + b[1] * a[1] + b[2] * a[2] + b[3] * a[3] - b[4] * a[4] - b[5] * a[5] -
           b[6] * a[6] + b[7] * a[7] - b[8] * a[8] + b[9] * a[9] + b[10] * a[10] - b[11] * a[11] +
           b[12] * a[12] + b[13] * a[13] + b[14] * a[14] - b[15] * a[15];
  res[1] = b[1] * a[0] + b[0] * a[1] - b[5] * a[2] - b[6] * a[3] + b[7] * a[4] + b[2] * a[5] +
           b[3] * a[6] - b[4] * a[7] - b[11] * a[8] + b[12] * a[9] + b[13] * a[10] - b[8] * a[11] +
           b[9] * a[12] + b[10] * a[13] - b[15] * a[14] + b[14] * a[15];
  res[2] = b[2] * a[0] + b[5] * a[1] + b[0] * a[2] - b[8] * a[3] + b[9] * a[4] - b[1] * a[5] +
           b[11] * a[6] - b[12] * a[7] + b[3] * a[8] - b[4] * a[9] + b[14] * a[10] + b[6] * a[11] -
           b[7] * a[12] + b[15] * a[13] + b[10] * a[14] - b[13] * a[15];
  res[3] = b[3] * a[0] + b[6] * a[1] + b[8] * a[2] + b[0] * a[3] + b[10] * a[4] - b[11] * a[5] -
           b[1] * a[6] - b[13] * a[7] - b[2] * a[8] - b[14] * a[9] - b[4] * a[10] - b[5] * a[11] -
           b[15] * a[12] - b[7] * a[13] - b[9] * a[14] + b[12] * a[15];
  res[4] = b[4] * a[0] + b[7] * a[1] + b[9] * a[2] + b[10] * a[3] + b[0] * a[4] - b[12] * a[5] -
           b[13] * a[6] - b[1] * a[7] - b[14] * a[8] - b[2] * a[9] - b[3] * a[10] - b[15] * a[11] -
           b[5] * a[12] - b[6] * a[13] - b[8] * a[14] + b[11] * a[15];
  res[5] = b[5] * a[0] + b[11] * a[3] - b[12] * a[4] + b[0] * a[5] + b[15] * a[10] + b[3] * a[11] -
           b[4] * a[12] + b[10] * a[15];
  res[6] = b[6] * a[0] - b[11] * a[2] - b[13] * a[4] + b[0] * a[6] - b[15] * a[9] - b[2] * a[11] -
           b[4] * a[13] - b[9] * a[15];
  res[7] = b[7] * a[0] - b[12] * a[2] - b[13] * a[3] + b[0] * a[7] - b[15] * a[8] - b[2] * a[12] -
           b[3] * a[13] - b[8] * a[15];
  res[8] = b[8] * a[0] + b[11] * a[1] - b[14] * a[4] + b[15] * a[7] + b[0] * a[8] + b[1] * a[11] -
           b[4] * a[14] + b[7] * a[15];
  res[9] = b[9] * a[0] + b[12] * a[1] - b[14] * a[3] + b[15] * a[6] + b[0] * a[9] + b[1] * a[12] -
           b[3] * a[14] + b[6] * a[15];
  res[10] = b[10] * a[0] + b[13] * a[1] + b[14] * a[2] - b[15] * a[5] + b[0] * a[10] +
            b[1] * a[13] + b[2] * a[14] - b[5] * a[15];
  res[11] = b[11] * a[0] + b[15] * a[4] + b[0] * a[11] - b[4] * a[15];
  res[12] = b[12] * a[0] + b[15] * a[3] + b[0] * a[12] - b[3] * a[15];
  res[13] = b[13] * a[0] - b[15] * a[2] + b[0] * a[13] + b[2] * a[15];
  res[14] = b[14] * a[0] + b[15] * a[1] + b[0] * a[14] - b[1] * a[15];
  res[15] = b[15] * a[0] + b[0] * a[15];
  return res;
};

//***********************
// R310.Add : res = a + b
// Multivector addition
//***********************
inline R310 operator+(const R310 &a, const R310 &b) {
  R310 res;
  res[0] = a[0] + b[0];
  res[1] = a[1] + b[1];
  res[2] = a[2] + b[2];
  res[3] = a[3] + b[3];
  res[4] = a[4] + b[4];
  res[5] = a[5] + b[5];
  res[6] = a[6] + b[6];
  res[7] = a[7] + b[7];
  res[8] = a[8] + b[8];
  res[9] = a[9] + b[9];
  res[10] = a[10] + b[10];
  res[11] = a[11] + b[11];
  res[12] = a[12] + b[12];
  res[13] = a[13] + b[13];
  res[14] = a[14] + b[14];
  res[15] = a[15] + b[15];
  return res;
};

//***********************
// R310.Sub : res = a - b
// Multivector subtraction
//***********************
inline R310 operator-(const R310 &a, const R310 &b) {
  R310 res;
  res[0] = a[0] - b[0];
  res[1] = a[1] - b[1];
  res[2] = a[2] - b[2];
  res[3] = a[3] - b[3];
  res[4] = a[4] - b[4];
  res[5] = a[5] - b[5];
  res[6] = a[6] - b[6];
  res[7] = a[7] - b[7];
  res[8] = a[8] - b[8];
  res[9] = a[9] - b[9];
  res[10] = a[10] - b[10];
  res[11] = a[11] - b[11];
  res[12] = a[12] - b[12];
  res[13] = a[13] - b[13];
  res[14] = a[14] - b[14];
  res[15] = a[15] - b[15];
  return res;
};

//***********************
// R310.smul : res = a * b
// scalar/multivector multiplication
//***********************
inline R310 operator*(const float &a, const R310 &b) {
  R310 res;
  res[0] = a * b[0];
  res[1] = a * b[1];
  res[2] = a * b[2];
  res[3] = a * b[3];
  res[4] = a * b[4];
  res[5] = a * b[5];
  res[6] = a * b[6];
  res[7] = a * b[7];
  res[8] = a * b[8];
  res[9] = a * b[9];
  res[10] = a * b[10];
  res[11] = a * b[11];
  res[12] = a * b[12];
  res[13] = a * b[13];
  res[14] = a * b[14];
  res[15] = a * b[15];
  return res;
};

//***********************
// R310.muls : res = a * b
// multivector/scalar multiplication
//***********************
inline R310 operator*(const R310 &a, const float &b) {
  R310 res;
  res[0] = a[0] * b;
  res[1] = a[1] * b;
  res[2] = a[2] * b;
  res[3] = a[3] * b;
  res[4] = a[4] * b;
  res[5] = a[5] * b;
  res[6] = a[6] * b;
  res[7] = a[7] * b;
  res[8] = a[8] * b;
  res[9] = a[9] * b;
  res[10] = a[10] * b;
  res[11] = a[11] * b;
  res[12] = a[12] * b;
  res[13] = a[13] * b;
  res[14] = a[14] * b;
  res[15] = a[15] * b;
  return res;
};

//***********************
// R310.sadd : res = a + b
// scalar/multivector addition
//***********************
inline R310 operator+(const float &a, const R310 &b) {
  R310 res;
  res[0] = a + b[0];
  res[1] = b[1];
  res[2] = b[2];
  res[3] = b[3];
  res[4] = b[4];
  res[5] = b[5];
  res[6] = b[6];
  res[7] = b[7];
  res[8] = b[8];
  res[9] = b[9];
  res[10] = b[10];
  res[11] = b[11];
  res[12] = b[12];
  res[13] = b[13];
  res[14] = b[14];
  res[15] = b[15];
  return res;
};

//***********************
// R310.adds : res = a + b
// multivector/scalar addition
//***********************
inline R310 operator+(const R310 &a, const float &b) {
  R310 res;
  res[0] = a[0] + b;
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  res[4] = a[4];
  res[5] = a[5];
  res[6] = a[6];
  res[7] = a[7];
  res[8] = a[8];
  res[9] = a[9];
  res[10] = a[10];
  res[11] = a[11];
  res[12] = a[12];
  res[13] = a[13];
  res[14] = a[14];
  res[15] = a[15];
  return res;
};

//***********************
// R310.ssub : res = a - b
// scalar/multivector subtraction
//***********************
inline R310 operator-(const float &a, const R310 &b) {
  R310 res;
  res[0] = a - b[0];
  res[1] = -b[1];
  res[2] = -b[2];
  res[3] = -b[3];
  res[4] = -b[4];
  res[5] = -b[5];
  res[6] = -b[6];
  res[7] = -b[7];
  res[8] = -b[8];
  res[9] = -b[9];
  res[10] = -b[10];
  res[11] = -b[11];
  res[12] = -b[12];
  res[13] = -b[13];
  res[14] = -b[14];
  res[15] = -b[15];
  return res;
};

//***********************
// R310.subs : res = a - b
// multivector/scalar subtraction
//***********************
inline R310 operator-(const R310 &a, const float &b) {
  R310 res;
  res[0] = a[0] - b;
  res[1] = a[1];
  res[2] = a[2];
  res[3] = a[3];
  res[4] = a[4];
  res[5] = a[5];
  res[6] = a[6];
  res[7] = a[7];
  res[8] = a[8];
  res[9] = a[9];
  res[10] = a[10];
  res[11] = a[11];
  res[12] = a[12];
  res[13] = a[13];
  res[14] = a[14];
  res[15] = a[15];
  return res;
};

inline float R310::norm() { return sqrt(std::abs(((*this) * Conjugate()).mvec[0])); }
inline float R310::inorm() { return (!(*this)).norm(); }
inline R310 R310::normalized() { return (*this) * (1 / norm()); }
