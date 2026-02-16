#pragma once

#include <ostream>
#include <string>

#include "gfx/math.h"

namespace ndyn::gfx {

std::string to_string(const Vec2& v);
std::ostream& operator<<(std::ostream& os, const Vec2& v);

std::string to_string(const Vec3& v);
std::ostream& operator<<(std::ostream& os, const Vec3& v);

std::string to_string(const Vec4& v);
std::ostream& operator<<(std::ostream& os, const Vec4& v);

std::string to_string(const Quat& q);
std::ostream& operator<<(std::ostream& os, const Quat& q);

std::string to_string(const Vertex& v);
std::ostream& operator<<(std::ostream& os, const Vertex& v);

std::string to_string(const Position& p);
std::ostream& operator<<(std::ostream& os, const Position& p);

std::string to_string(const Mat4& m);
std::ostream& operator<<(std::ostream& os, const Mat4& m);

}  // namespace ndyn::gfx
