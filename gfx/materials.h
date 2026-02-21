#pragma once

#include "gfx/material.h"
#include "gfx/math.h"

namespace ndyn::gfx {

extern const Material MATTE_RED;
extern const Material GLOSSY_RED;
extern const Material MATTE_GREEN;
extern const Material GLOSSY_GREEN;
extern const Material MATTE_BLUE;
extern const Material GLOSSY_BLUE;
extern const Material MATTE_CYAN;
extern const Material GLOSSY_CYAN;
extern const Material MATTE_YELLOW;
extern const Material GLOSSY_YELLOW;
extern const Material MATTE_MAGENTA;
extern const Material GLOSSY_MAGENTA;
extern const Material MATTE_WHITE;
extern const Material GLOSSY_WHITE;
extern const Material MATTE_BLACK;
extern const Material GLOSSY_BLACK;

Material make_matte(const Vec3& color);
Material make_glossy(const Vec3& color);
Material make_metallic(const Vec3& color, float roughness);
Material make_plastic(const Vec3& color, float roughness);
Material make_diffuse(const Vec3& color);
Material make_glass(const Vec3& tint, float ior, float roughness);
Material make_emissive(const Vec3& color, float intensity);
Material make_clearcoat_paint(const Vec3& color, float roughness);

}  // namespace ndyn::gfx
