#include "base/initializer.h"
#include "glog/logging.h"
#include "math/algebra.h"
#include "math/basis_representation.h"
#include "ui/algebra_options.h"
#include "ui/repl.h"

using namespace ndyn::ui;

int main(int argc, char* argv[]) {
  using namespace ndyn::math;

  ndyn::initialize(&argc, &argv);

  const auto algebra{get_algebra_flag()};
  if (algebra == Algebras::Scalar) {
    using Algebra = Scalar<>;
    std::cout << "Scalar algebra (real numbers)\n";
    Repl<Algebra, BasisRepresentation<Algebra>> repl{};
    repl.loop();
  } else if (algebra == Algebras::Complex) {
    using Algebra = Complex<>;
    std::cout << "Complex algebra\n";
    Repl<Algebra, BasisRepresentation<Algebra>> repl{};
    repl.loop();
  } else if (algebra == Algebras::Euclid2d) {
    using Algebra = Vga2d<>;
    std::cout << "2D Euclidean algebra\n";
    Repl<Algebra, BasisRepresentation<Algebra>> repl{};
    repl.loop();
  } else if (algebra == Algebras::Euclid3d) {
    using Algebra = Vga<>;
    std::cout << "3D Euclidean algebra\n";
    Repl<Algebra, BasisRepresentation<Algebra>> repl{};
    repl.loop();
  }
  return 0;
}
