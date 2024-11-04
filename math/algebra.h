#pragma once

#include <cstddef>
#include <cstdint>

namespace ndyn::math {

enum class InnerProduct : uint8_t {
  LEFT_CONTRACTION,
  RIGHT_CONTRACTION,
  BIDIRECTIONAL,  // Left contraction when the grade of the lhs is lower, right contraction
                  // otherwise. Used in texts by Hestenes and others.
  NO_IMPLICIT_DEFINITION,
};

template <typename AlgebraT>
class Multivector;

template <typename ScalarT, size_t NUM_POSITIVE_BASES, size_t NUM_NEGATIVE_BASES,
          size_t NUM_ZERO_BASES, InnerProduct INNER_PRODUCT_STYLE = InnerProduct::LEFT_CONTRACTION>
class Algebra final {
 private:
  // The Algebra class is meant to be a collection of types and properties describing the algebra.
  // It is not instantiable.
  Algebra() = delete;

 public:
  using ScalarType = ScalarT;
  using VectorType = Multivector<Algebra>;

  static constexpr size_t POSITIVE_BASES{NUM_POSITIVE_BASES};
  static constexpr size_t NEGATIVE_BASES{NUM_NEGATIVE_BASES};
  static constexpr size_t ZERO_BASES{NUM_ZERO_BASES};

  static constexpr InnerProduct INNER_PRODUCT{INNER_PRODUCT_STYLE};

  // Number of grade-1 basis elements (vectors) in this multivector.
  static constexpr size_t vector_count() { return POSITIVE_BASES + NEGATIVE_BASES + ZERO_BASES; }
  // Number of grades in this multivector, counting the scalar (grade-0) as its own grade.
  static constexpr size_t grade_count() { return vector_count() + 1; }

  // Number of bases in this multivector, including the number of vectors, bivectors, trivectors,
  // etc., and scalars.
  static constexpr size_t bases_count() { return 1UL << vector_count(); }
};

}  // namespace ndyn::math
