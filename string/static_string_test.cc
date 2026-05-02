#include "string/static_string.h"

#include <string>
#include <string_view>

#include "glog/logging.h"
#include "gtest/gtest.h"

namespace ndyn::string {

constexpr std::string make_repeated_string(std::string_view base, size_t repeat) {
  std::string result{};
  for (size_t i = 0; i < repeat; ++i) {
    result += base;
  }
  return result;
}

TEST(StaticStringTest, CanBuildStaticString) {
  static constexpr std::string_view MESSAGE{"Hello, world! "};
  static constexpr auto build_string = []() { return std::string(MESSAGE); };
  static constexpr std::string_view str{to_static_string(build_string)};

  static_assert(str.starts_with(MESSAGE));

  EXPECT_TRUE(str.starts_with(MESSAGE));
}

TEST(StaticStringTest, CanUseFunctionInGenerator) {
  static constexpr std::string_view MESSAGE{"Hello, world! "};
  static constexpr auto build_string = []() { return make_repeated_string(MESSAGE, 3); };
  static constexpr std::string_view str{to_static_string(build_string)};

  static_assert(str.starts_with(MESSAGE));

  EXPECT_TRUE(str.starts_with(MESSAGE));
}

}  // namespace ndyn::string
