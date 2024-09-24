// clang-format off
#include "glad/gl.h"
// clang-format on

#include "base/initializer.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "simulation/cube_ui.h"
#include "ui/app.h"

int main(int argc, char* argv[]) {
  using namespace ndyn::simulation;
  using namespace ndyn::ui;

  FLAGS_logtostderr = true;
  ndyn::initialize(&argc, &argv);

  App app{"Cube", 1920, 1080};
  Cube cube{app.gl_window()};
  app.add_direct_render_element(cube);
  app.run();
  return 0;
}
