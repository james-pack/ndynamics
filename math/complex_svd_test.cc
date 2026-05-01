#include <algorithm>
#include <array>
#include <cmath>
#include <complex>

#include "gtest/gtest.h"
#include "math/complex.h"
#include "math/svd_test.h"

namespace ndyn::math {

using ComplexTypes = ::testing::Types<Complex<float>>;
INSTANTIATE_TYPED_TEST_SUITE_P(ComplexTests, SvdTest, ComplexTypes);

}  // namespace ndyn::math
