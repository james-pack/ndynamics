#pragma once

#include <array>

#include "GLFW/glfw3.h"

namespace ndyn::graphics {

using ColorType = std::array<GLfloat, 3>;

template <size_t SIZE>
constexpr std::array<ColorType, SIZE> rainbow_colors() {
  constexpr size_t NUM_RANDOM_VALUES{3 * 36};
  // Preselected set of random colors.
  constexpr std::array<GLfloat, NUM_RANDOM_VALUES> random_values{
      0.583f, 0.771f, 0.014f,  //
      0.609f, 0.115f, 0.436f,  //
      0.327f, 0.483f, 0.844f,  //
      0.822f, 0.569f, 0.201f,  //
      0.435f, 0.602f, 0.223f,  //
      0.310f, 0.747f, 0.185f,  //
      0.597f, 0.770f, 0.761f,  //
      0.559f, 0.436f, 0.730f,  //
      0.359f, 0.583f, 0.152f,  //
      0.483f, 0.596f, 0.789f,  //
      0.559f, 0.861f, 0.639f,  //
      0.195f, 0.548f, 0.859f,  //
      0.014f, 0.184f, 0.576f,  //
      0.771f, 0.328f, 0.970f,  //
      0.406f, 0.615f, 0.116f,  //
      0.676f, 0.977f, 0.133f,  //
      0.971f, 0.572f, 0.833f,  //
      0.140f, 0.616f, 0.489f,  //
      0.997f, 0.513f, 0.064f,  //
      0.945f, 0.719f, 0.592f,  //
      0.543f, 0.021f, 0.978f,  //
      0.279f, 0.317f, 0.505f,  //
      0.167f, 0.620f, 0.077f,  //
      0.347f, 0.857f, 0.137f,  //
      0.055f, 0.953f, 0.042f,  //
      0.714f, 0.505f, 0.345f,  //
      0.783f, 0.290f, 0.734f,  //
      0.722f, 0.645f, 0.174f,  //
      0.302f, 0.455f, 0.848f,  //
      0.225f, 0.587f, 0.040f,  //
      0.517f, 0.713f, 0.338f,  //
      0.053f, 0.959f, 0.120f,  //
      0.393f, 0.621f, 0.362f,  //
      0.673f, 0.211f, 0.457f,  //
      0.820f, 0.883f, 0.371f,  //
      0.982f, 0.099f, 0.879f   //
  };
  std::array<ColorType, SIZE> result{};
  for (size_t i = 0; i < SIZE; ++i) {
    result[i] = {random_values[(3 * i + 0) % NUM_RANDOM_VALUES],
                 random_values[(3 * i + 1) % NUM_RANDOM_VALUES],
                 random_values[(3 * i + 2) % NUM_RANDOM_VALUES]};
  }
  return result;
}

template <size_t SIZE>
constexpr std::array<ColorType, SIZE> smooth_colors(  //
    const ColorType& start = ColorType{1.0f, .1f, .1f},
    const ColorType& end = ColorType{.1f, .5f, 1.f}) {
  std::array<ColorType, SIZE> result{};
  for (size_t i = 0; i < SIZE; ++i) {
    ColorType color{};
    for (size_t j = 0; j < 3; ++j) {
      color[j] = start[j] + (end[j] - start[j]) * i / (SIZE - 1);
    }
    result[i] = color;
  }
  return result;
}

}  // namespace ndyn::graphics