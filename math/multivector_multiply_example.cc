#include <iostream>

#include "math/multivector.h"

using namespace ndyn::math;

int main() {
  auto i{ComplexMultivector<float>::e<0>()};
  auto a{1.f + i};
  auto result{a * a};

  std::cout << "(1 + i)^2 = " << result << "\n";

  return result.scalar();
}
