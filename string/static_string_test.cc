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

TEST(ConcatTest, CanConcatenateTwoCharArrays) {
  static constexpr char MESSAGE1[] = "Hello, ";
  static constexpr char MESSAGE2[] = "world!";
  static constexpr std::string_view EXPECTED{"Hello, world!"};
  static constexpr std::string_view str{concat<MESSAGE1, MESSAGE2>()};

  static_assert(str == EXPECTED);

  EXPECT_EQ(EXPECTED, str);
}

TEST(ConcatTest, CanConcatenateCharArrayAndNumber) {
  static constexpr char MESSAGE[] = "Hello, ";
  static constexpr int VALUE{128};
  static constexpr std::string_view EXPECTED{"Hello, 128"};
  static constexpr std::string_view str{concat<MESSAGE, VALUE>()};

  static_assert(str == EXPECTED);

  EXPECT_EQ(EXPECTED, str);
}

TEST(ConcatTest, CanConcatenateTwoInts) {
  static constexpr int VALUE1 = 10;
  static constexpr int VALUE2 = 24;
  static constexpr std::string_view EXPECTED{"1024"};
  static constexpr std::string_view str{concat<VALUE1, VALUE2>()};

  static_assert(str == EXPECTED);

  EXPECT_EQ(EXPECTED, str);
}

[[nodiscard]] constexpr auto square(std::integral auto v) noexcept { return v * v; }

TEST(ConcatTest, CanConcatenateGeneratedInts) {
  static constexpr int VALUE1 = square(10);
  static constexpr int VALUE2 = square(24);
  static constexpr std::string_view EXPECTED{"100576"};
  static constexpr std::string_view str{concat<VALUE1, VALUE2>()};

  static_assert(str == EXPECTED);

  EXPECT_EQ(EXPECTED, str);
}

}  // namespace ndyn::string
