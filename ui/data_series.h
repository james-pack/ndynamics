#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>

namespace ndyn::ui {

template <typename T, size_t NUM_POINTS, size_t NUM_FUNCTIONS = 1>
class DataSeries final {
 private:
  std::array<std::array<T, NUM_POINTS>, NUM_FUNCTIONS + 1> data_{};
  std::array<std::string, NUM_FUNCTIONS + 1> labels_{};

 public:
  constexpr DataSeries() = default;
  constexpr DataSeries(const DataSeries&) = default;
  constexpr DataSeries(DataSeries&&) = default;

  constexpr DataSeries(std::string_view x_label, std::initializer_list<std::string_view> y_labels) {
    labels_.at(0) = x_label;
    size_t i = 1;
    for (auto label : y_labels) {
      labels_.at(i) = label;
      ++i;
    }
  }

  constexpr DataSeries& operator=(const DataSeries&) = default;
  constexpr DataSeries& operator=(DataSeries&&) = default;

  void update(T x, const std::array<T, NUM_FUNCTIONS>& y) {
    using std::copy_n;
    for (size_t i = 0; i < NUM_FUNCTIONS + 1; ++i) {
      copy_n(data_[i].begin() + 1, data_[i].size() - 1, data_[i].begin());
    }
    data_[0][NUM_POINTS - 1] = x;
    for (size_t i = 0; i < NUM_FUNCTIONS; ++i) {
      data_.at(i + 1).at(NUM_POINTS - 1) = y.at(i);
    }
  }

  const std::string& x_label() const { return labels_[0]; }

  template <size_t DIMENSION = 0>
  const std::string& y_label() const {
    return labels_.at(DIMENSION + 1);
  }

  // The x label expressed as a C-style string.
  const char* x_clabel() const { return labels_[0].c_str(); }

  // The y labels expressed as a C-style string.
  template <size_t DIMENSION = 0>
  const char* y_clabel() const {
    return labels_.at(DIMENSION + 1).c_str();
  }

  // The y labels expressed as a C-style string.
  const char* y_clabel(size_t i) const { return labels_.at(i + 1).c_str(); }

  const T* x_data() const { return data_[0].data(); }

  template <size_t DIMENSION = 0>
  const T* y_data() const {
    return data_.at(DIMENSION + 1).data();
  }

  const T* y_data(size_t i) const { return data_.at(i + 1).data(); }

  static constexpr size_t size() { return NUM_POINTS; }

  static constexpr size_t num_functions() { return NUM_FUNCTIONS; }
};

}  // namespace ndyn::ui
