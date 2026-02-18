#include "gfx/light.h"

namespace ndyn::gfx {

Light Light::no_light() {
  Light result{};

  result.position = {0.f, 0.f, 0.f, 1.f};
  result.direction = {0.f, 0.f, 0.f, 1.f};
  result.color = {0.f, 0.f, 0.f, 0.f};
  result.type = 0;

  return result;
}

Light Light::default_ambient_light() {
  Light result{};

  result.position = {0.f, 0.f, 0.f, 1.f};
  result.color = {1.f, 0.98f, 0.95f, 0.3f};
  result.type = 0;

  return result;
}

Light Light::default_directional_light() {
  Light result{};

  result.position = {10.f, 10.f, 10.f, 1.f};
  result.direction = {-1.f, -1.f, -1.f, 1.f};
  result.color = {1.f, 0.98f, 0.95f, 0.3f};
  result.type = 1;

  return result;
}

}  // namespace ndyn::gfx
