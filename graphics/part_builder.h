#pragma once

#include <memory>
#include <vector>

#include "graphics/box.h"
#include "graphics/cylinder.h"
#include "graphics/gpu_element.h"
#include "graphics/offset.h"
#include "graphics/revolute_joint.h"
#include "graphics/sphere.h"

namespace ndyn::graphics {

template <typename GeometryT>
class PartBuilder final {
 public:
  using GeometryType = GeometryT;
  using AlgebraType = typename GeometryType::AlgebraType;
  using ScalarType = typename AlgebraType::ScalarType;
  using VectorType = typename AlgebraType::VectorType;

  using ElementType = GpuElement<GeometryType>;

 private:
  // The top_ element owns all of the elements that get added.
  std::unique_ptr<ElementType> top_{nullptr};

  // parents_ and current_ are references that aid in navigating the tree of elements as they are
  // being created.
  std::vector<ElementType*> parents_{};
  ElementType* current_{nullptr};

  void add_element(std::unique_ptr<ElementType> element) {
    current_ = element.get();
    if (!top_) {
      top_ = std::move(element);
    } else {
      parents_.back()->add_element(std::move(element));
    }
  }

 public:
  PartBuilder& up() {
    current_ = parents_.back();
    parents_.pop_back();
    return *this;
  }

  PartBuilder& down() {
    if (current_ != nullptr) {
      parents_.push_back(current_);
      current_ = nullptr;
    }
    return *this;
  }

  PartBuilder& top() {
    parents_.clear();
    current_ = top_.get();
    return *this;
  }

  template <typename SubelementType>
  PartBuilder& bind(SubelementType** reference) {
    *reference = reinterpret_cast<SubelementType*>(current_);
    return *this;
  }

  std::unique_ptr<ElementType> build() {
    current_ = nullptr;
    parents_.clear();
    return std::move(top_);
  }

  // Joints.
  PartBuilder& add_revolute_joint(const VectorType& rotation_axis, const ScalarType& angle = 0) {
    add_element(std::make_unique<RevoluteJoint<GeometryType>>(rotation_axis, angle));
    return *this;
  }

  // An offset in a direction, usually used to change the binding location between the parent and
  // the next child element.
  PartBuilder& add_offset(const ScalarType& distance, const VectorType& direction) {
    add_element(std::make_unique<Offset<GeometryType>>(distance, direction));
    return *this;
  }

  // Geometric shapes.
  PartBuilder& add_rod(const ScalarType& radius, const ScalarType& length,
                       const VectorType& length_axis) {
    add_element(std::make_unique<Cylinder<GeometryType>>(radius, length));
    return *this;
  }

  PartBuilder& add_sphere(const ScalarType& radius) {
    add_element(std::make_unique<Sphere<GeometryType>>(radius));
    return *this;
  }

  PartBuilder& add_cube(const ScalarType& length) {
    add_element(std::make_unique<Box<GeometryType>>(length));
    return *this;
  }
};

}  // namespace ndyn::graphics
