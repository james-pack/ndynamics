#include "gfx/math.h"

#include <cmath>

#include "gfx/math_strings.h"
#include "glog/logging.h"
#include "gtest/gtest.h"

constexpr float EPS = 1e-6f;

namespace ndyn::gfx {

::testing::AssertionResult approx_equal(const Vec4& v1, const Vec4& v2, float eps = 1e-6f) {
  if (std::fabs(v1.x - v2.x) >= eps) {
    return ::testing::AssertionFailure()
           << "v1: " << v1 << ", v2: " << v2 << " (x: " << v1.x << " != " << v2.x << ")";
  }
  if (std::fabs(v1.y - v2.y) >= eps) {
    return ::testing::AssertionFailure()
           << "v1: " << v1 << ", v2: " << v2 << " (y: " << v1.y << " != " << v2.y << ")";
  }
  if (std::fabs(v1.z - v2.z) >= eps) {
    return ::testing::AssertionFailure()
           << "v1: " << v1 << ", v2: " << v2 << " (z: " << v1.z << " != " << v2.z << ")";
  }
  if (std::fabs(v1.w - v2.w) >= eps) {
    return ::testing::AssertionFailure()
           << "v1: " << v1 << ", v2: " << v2 << " (w: " << v1.w << " != " << v2.w << ")";
  }
  return ::testing::AssertionSuccess();
}

::testing::AssertionResult approx_equal(const Mat4& expected, const Mat4& actual,
                                        float eps = 1e-6f) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (std::fabs(expected.m[i][j] - actual.m[i][j]) > eps) {
        return ::testing::AssertionFailure()  //
               << "expected:\n"
               << expected  //
               << "actual:\n"
               << actual  //
               << "\t(i, j): (" << i << ", " << j << ")";
      }
    }
  }
  return ::testing::AssertionSuccess();
}

TEST(Mat4Test, TestDefaultConstructionIsZeroMatrix) {
  Mat4 m{};
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_FLOAT_EQ(m.m[i][j], 0.f);
    }
  }
}

TEST(Mat4Test, TestIdentityMatrix) {
  const Mat4 m{Mat4::identity()};
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      EXPECT_FLOAT_EQ(m.m[i][j], i == j ? 1.f : 0.f);
    }
  }
}

TEST(Mat4Test, TestScalingTransform) {
  const Mat4 scale{Mat4::scaling(Vec3{2.f, 3.f, 4.f})};
  const Vec4 v{1.f, 1.f, 1.f, 1.f};
  const Vec4 expected{2.f, 3.f, 4.f, 1.f};
  EXPECT_TRUE(approx_equal(expected, scale * v));
}

TEST(Mat4Test, TestTrivialRotationTransform) {
  // 90-degree rotation around Z
  const Mat4 rot{Mat4::rotation(Vec3{0.f, 0.f, 1.f}, M_PI_2)};
  const Vec4 v{1.f, 0.f, 2.f, 1.f};
  const Vec4 expected{0.f, 1.f, 2.f, 1.f};
  const Vec4 result{rot * v};
  EXPECT_TRUE(approx_equal(expected, result));
}

TEST(Mat4Test, TestRotationTransform) {
  // 180-degree rotation around x=y @ z=0
  const Mat4 rot{Mat4::rotation(Vec3{1.f, 1.f, 0.f}, M_PI)};
  const Vec4 v{1.f, 0.f, 1.f, 1.f};
  const Vec4 expected{0.f, 1.f, -1.f, 1.f};
  const Vec4 result{rot * v};
  EXPECT_TRUE(approx_equal(expected, result));
}

TEST(Mat4Test, TestTranslationTransform) {
  const Mat4 trans{Mat4::translation(Vec3{1.f, 2.f, 3.f})};
  const Vec4 v{1.f, 1.f, 1.f, 1.f};
  const Vec4 expected{2.f, 3.f, 4.f, 1.f};
  const Vec4 result{trans * v};
  EXPECT_TRUE(approx_equal(expected, result));
}

TEST(Mat4Test, TestMultiplicationAssociativity) {
  // 90-degree rotation around Z
  const Mat4 rot{Mat4::rotation(Vec3{0.f, 0.f, 1.f}, M_PI_2)};
  const Mat4 trans{Mat4::translation(Vec3{1.f, 2.f, 4.f})};
  const Vec4 v{1.f, 0.f, 0.f, 1.f};

  const Vec4 left{(trans * rot) * v};
  const Vec4 right{trans * (rot * v)};

  EXPECT_TRUE(approx_equal(left, right));
}

TEST(Mat4Test, TestMultiplicationViaScaledRowSwap) {
  const Mat4 swap{
      0, 2, 0, 0,  //
      1, 0, 0, 0,  //
      0, 0, 1, 0,  //
      0, 0, 0, 1,  //
  };

  const Mat4 a{
      5, 1,  0,  0,   //
      1, 10, 0,  0,   //
      2, 0,  15, 0,   //
      3, 0,  0,  20,  //
  };

  const Mat4 expected{
      2, 20, 0,  0,   //
      5, 1,  0,  0,   //
      2, 0,  15, 0,   //
      3, 0,  0,  20,  //
  };

  const Mat4 result{swap * a};
  EXPECT_TRUE(approx_equal(expected, result));
}

TEST(Mat4Test, TestMultiplicationViaColumnSwap) {
  const Mat4 swap{
      0, 1, 0, 0,  //
      1, 0, 0, 0,  //
      0, 0, 1, 0,  //
      0, 0, 0, 1,  //
  };

  const Mat4 a{
      5, 1,  0,  0,   //
      1, 10, 0,  0,   //
      2, 0,  15, 0,   //
      3, 0,  0,  20,  //
  };

  const Mat4 expected{
      1,  5, 0,  0,   //
      10, 1, 0,  0,   //
      0,  2, 15, 0,   //
      0,  3, 0,  20,  //
  };

  const Mat4 result{a * swap};
  EXPECT_TRUE(approx_equal(expected, result));
}

TEST(PositionTest, TestInvertOnRotation) {
  const Position rot{Position::rotation(Vec3{0.f, 0.f, 1.f}, M_PI_2)};
  const Position inverse{rot.invert()};
  EXPECT_TRUE(
      approx_equal(Mat4::identity(), rot.as_matrix_transform() * inverse.as_matrix_transform()));
}

TEST(PositionTest, TestInvertOnTranslation) {
  const Position trans{Position::translation(Vec3{1.f, 2.f, 4.f})};
  const Position inverse{trans.invert()};
  EXPECT_TRUE(
      approx_equal(Mat4::identity(), trans.as_matrix_transform() * inverse.as_matrix_transform()));
}

TEST(PositionTest, TestInvert) {
  const Position rot{Position::rotation(Vec3{0.f, 0.f, 1.f}, M_PI_2)};
  const Position trans{Position::translation(Vec3{1.f, 2.f, 4.f})};
  const Position inverse{(trans * rot).invert()};
  EXPECT_TRUE(approx_equal(
      Mat4::identity(),
      trans.as_matrix_transform() * rot.as_matrix_transform() * inverse.as_matrix_transform()));
}

}  // namespace ndyn::gfx
