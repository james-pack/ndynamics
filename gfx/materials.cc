#include "gfx/materials.h"

#include "gfx/material.h"
#include "gfx/math.h"

namespace ndyn::gfx {

const Material MATTE_RED = make_matte(Vec3{1.f, 0.f, 0.f});
const Material GLOSSY_RED = make_glossy(Vec3{1.f, 0.f, 0.f});

const Material MATTE_GREEN = make_matte(Vec3{0.f, 1.f, 0.f});
const Material GLOSSY_GREEN = make_glossy(Vec3{0.f, 1.f, 0.f});

const Material MATTE_BLUE = make_matte(Vec3{0.f, 0.f, 1.f});
const Material GLOSSY_BLUE = make_glossy(Vec3{0.f, 0.f, 1.f});

const Material MATTE_CYAN = make_matte(Vec3{0.f, 1.f, 1.f});
const Material GLOSSY_CYAN = make_glossy(Vec3{0.f, 1.f, 1.f});

const Material MATTE_YELLOW = make_matte(Vec3{1.f, 1.f, 0.f});
const Material GLOSSY_YELLOW = make_glossy(Vec3{1.f, 1.f, 0.f});

const Material MATTE_MAGENTA = make_matte(Vec3{1.f, 0.f, 1.f});
const Material GLOSSY_MAGENTA = make_glossy(Vec3{1.f, 0.f, 1.f});

const Material MATTE_WHITE = make_matte(Vec3{1.f, 1.f, 1.f});
const Material GLOSSY_WHITE = make_glossy(Vec3{1.f, 1.f, 1.f});

const Material MATTE_BLACK = make_matte(Vec3{0.f, 0.f, 0.f});
const Material GLOSSY_BLACK = make_glossy(Vec3{0.f, 0.f, 0.f});

}  // namespace ndyn::gfx
