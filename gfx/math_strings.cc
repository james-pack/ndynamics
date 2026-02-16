#include "gfx/math_strings.h"

#include <cmath>
#include <string>

#include "gfx/math.h"

namespace ndyn::gfx {

std::string to_string(const Vec2& v) {
  using std::to_string;
  return "x: " + to_string(v.x) + ", y: " + to_string(v.y);
}

std::ostream& operator<<(std::ostream& os, const Vec2& v) { return os << to_string(v); }

std::string to_string(const Vec3& v) {
  using std::to_string;
  return "x: " + to_string(v.x) + ", y: " + to_string(v.y) + ", z: " + to_string(v.z);
}

std::ostream& operator<<(std::ostream& os, const Vec3& v) { return os << to_string(v); }

std::string to_string(const Vec4& v) {
  using std::to_string;
  return "x: " + to_string(v.x) + ", y: " + to_string(v.y) + ", z: " + to_string(v.z) +
         ", w: " + to_string(v.w);
}

std::ostream& operator<<(std::ostream& os, const Vec4& v) { return os << to_string(v); }

std::string to_string(const Quat& q) {
  using std::to_string;
  return "w: " + to_string(q.w) + ", x: " + to_string(q.x) + ", y: " + to_string(q.y) +
         ", z: " + to_string(q.z);
}

std::ostream& operator<<(std::ostream& os, const Quat& q) { return os << to_string(q); }

std::string to_string(const Vertex& v) {
  using std::to_string;
  return "px: " + to_string(v.px) + ", py: " + to_string(v.py) + ", pz: " + to_string(v.pz) +
         ", nx: " + to_string(v.nx) + ", ny: " + to_string(v.ny) + ", nz: " + to_string(v.nz);
}

std::ostream& operator<<(std::ostream& os, const Vertex& v) { return os << to_string(v); }

std::string to_string(const Position& p) {
  using std::to_string;
  return "position: " + to_string(p.position) + ", orientation: " + to_string(p.orientation);
}

std::ostream& operator<<(std::ostream& os, const Position& p) { return os << to_string(p); }

std::string to_string(const Mat4& m) {
  using std::to_string;
  std::string s = "m: ";
  s += "[\n";
  for (int i = 0; i < 4; ++i) {
    s += "\t";
    for (int j = 0; j < 4; ++j) {
      s += to_string(m.m[i][j]);
      if (j < 3) s += ", ";
    }
    if (i < 3) {
      s += ",";
    } else {
      s += " ";
    }
    s += " //\n";
  }
  s += "]\n";
  return s;
}

std::ostream& operator<<(std::ostream& os, const Mat4& m) { return os << to_string(m); }

}  // namespace ndyn::gfx
