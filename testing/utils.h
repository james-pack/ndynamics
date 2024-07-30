#pragma once

#include "gtest/gtest.h"

namespace ndyn {

#define STATIC_EXPECT_EQ(val1, val2) \
  do {                               \
    static_assert(val1 == val2, ""); \
    EXPECT_EQ(val1, val2);           \
  } while (false)

}  // namespace ndyn
