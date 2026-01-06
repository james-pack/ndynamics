#include "base/initializer.h"
#include "glog/logging.h"
#include "math/algebra.h"
#include "ui/algebra_options.h"
#include "ui/interpreter.h"

using namespace ndyn::ui;

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  const auto algebra{get_algebra_flag()};
  if (algebra == Algebras::Euclid3d) {
    LOG(INFO) << "Euclid 3D";
    Interpreter<ndyn::math::Vga<>> interpreter{};
    auto result{interpreter.eval("e1 * e2 + 1 + e1")};
    LOG(INFO) << result;
  } else if (algebra == Algebras::Euclid2d) {
    LOG(INFO) << "Euclid 2D";
    Interpreter<ndyn::math::Vga2d<>> interpreter{};
    auto result{interpreter.eval("e1 * e2 + 1 + e1")};
    LOG(INFO) << result;
  }
  return 0;
}
