#include "base/initializer.h"
#include "glog/logging.h"
#include "math/algebra.h"
#include "ui/algebra_options.h"
#include "ui/repl.h"

using namespace ndyn::ui;

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  const auto algebra{get_algebra_flag()};
  if (algebra == Algebras::Scalar) {
    std::cout << "Scalar algebra (real numbers)\n";
    Repl<ndyn::math::Scalar<>> repl{};
    repl.loop();
  } else if (algebra == Algebras::Complex) {
    std::cout << "Complex algebra\n";
    Repl<ndyn::math::Complex<>> repl{};
    repl.loop();
  } else if (algebra == Algebras::Euclid2d) {
    std::cout << "2D Euclidean algebra\n";
    Repl<ndyn::math::Vga2d<>> repl{};
    repl.loop();
  } else if (algebra == Algebras::Euclid3d) {
    std::cout << "3D Euclidean algebra\n";
    Repl<ndyn::math::Vga<>> repl{};
    repl.loop();
  }
  return 0;
}
