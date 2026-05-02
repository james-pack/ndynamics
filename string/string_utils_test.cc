#include "string/string_utils.h"

#include <string>
#include <string_view>

#include "glog/logging.h"
#include "gtest/gtest.h"
#include "string/static_string.h"

namespace ndyn::string {

TEST(AsStringTest, CanMakeStringFromDigit) {
  static constexpr int VALUE{7};
  static constexpr std::string_view EXPECTED{"7"};
  static constexpr std::string_view str{to_static_string([]() { return as_string(VALUE); })};

  static_assert(str == EXPECTED);

  EXPECT_EQ(EXPECTED, str);
}

TEST(AsStringTest, CanMakeStringFromInteger) {
  static constexpr int VALUE{128};
  static constexpr std::string_view EXPECTED{"128"};
  static constexpr std::string_view str{to_static_string([]() { return as_string(VALUE); })};

  static_assert(str == EXPECTED);

  EXPECT_EQ(EXPECTED, str);
}

TEST(AsStringTest, CanMakeStringFromBigInteger) {
  static constexpr size_t VALUE{(1UL << 12) - 1};
  static constexpr std::string_view EXPECTED{"4095"};
  static constexpr std::string_view str{to_static_string([]() { return as_string(VALUE); })};

  static_assert(str == EXPECTED);

  EXPECT_EQ(EXPECTED, str);
}

TEST(AsStringTest, IdentityForStrings) {
  static constexpr std::string VALUE{"The Dude"};
  static constexpr std::string_view EXPECTED{VALUE};
  static constexpr std::string_view str{to_static_string([]() { return as_string(VALUE); })};

  static_assert(str == EXPECTED);

  EXPECT_EQ(EXPECTED, str);
}

TEST(AsStringTest, IdentityForStringViews) {
  static constexpr std::string_view VALUE{"The Dude"};
  static constexpr std::string_view EXPECTED{VALUE};
  static constexpr std::string_view str{to_static_string([]() { return as_string(VALUE); })};

  static_assert(str == EXPECTED);

  EXPECT_EQ(EXPECTED, str);
}

}  // namespace ndyn::string
