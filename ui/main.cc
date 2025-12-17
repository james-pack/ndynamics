#include "base/initializer.h"
#include "glog/logging.h"
#include "ui/algebra_options.h"

using namespace ndyn::ui;

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  const auto algebra{get_algebra_flag()};
  if (algebra == Algebras::Euclid3d) {
    LOG(INFO) << "Euclid 3D";
  } else if (algebra == Algebras::Euclid2d) {
    LOG(INFO) << "Euclid 2D";
  }
  return 0;
}
