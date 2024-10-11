#include "math/cartesian_vector.h"

#include <cmath>
#include <vector>

#include "gtest/gtest.h"
#include "math/coordinates.h"
#include "math/unit_set.h"
#include "math/vector_tests.h"
#include "units.h"

namespace ndyn::math {

class Cartesian2DVectorTest : public VectorTest<Coordinates::CARTESIAN, 2> {
 public:
  void SetUp() override {
    additive_sets.emplace_back(AdditiveSet{{}, {}, {}});
    additive_sets.emplace_back(AdditiveSet{{1, 2}, {2, 3}, {3, 5}});
    additive_sets.emplace_back(AdditiveSet{{1, 2}, {0, 0}, {1, 2}});

    multiplicative_sets.emplace_back(MultiplicativeSet{2, {1, 2}, {2, 4}});
    multiplicative_sets.emplace_back(MultiplicativeSet{1, {1, 2}, {1, 2}});
    multiplicative_sets.emplace_back(MultiplicativeSet{0, {}, {}});
    multiplicative_sets.emplace_back(MultiplicativeSet{0, {1, 1}, {}});
    multiplicative_sets.emplace_back(MultiplicativeSet{-2, {1, 2}, {-2, -4}});

    magnitudes.emplace_back(Magnitude{{1, 2}, 5});
    magnitudes.emplace_back(Magnitude{{2, 2}, 8});
    magnitudes.emplace_back(Magnitude{{3, 4}, 25});

    inner_products.emplace_back(InnerProduct{{1, 1}, {0, 1}, 1});
    inner_products.emplace_back(InnerProduct{{1, 1}, {1, 0}, 1});
    inner_products.emplace_back(InnerProduct{{1, 0}, {1, 0}, 1});
    inner_products.emplace_back(InnerProduct{{1, 0}, {0, 1}, 0});
    inner_products.emplace_back(InnerProduct{{1, 2}, {3, 7}, 17});

    basis_decompositions.emplace_back(BasisDecomposition{{}, {1, 0}, {}, {}});
    basis_decompositions.emplace_back(BasisDecomposition{{}, {1, 1}, {}, {}});
    basis_decompositions.emplace_back(BasisDecomposition{{1, 2}, {1, 0}, {1, 0}, {0, 2}});
    basis_decompositions.emplace_back(BasisDecomposition{{1, 2}, {1, 1}, {1.5, 1.5}, {-0.5, 0.5}});
  }
};

class Cartesian3DVectorTest : public VectorTest<Coordinates::CARTESIAN, 3> {
 public:
  void SetUp() override {
    additive_sets.emplace_back(AdditiveSet{{}, {}, {}});
    additive_sets.emplace_back(AdditiveSet{{1}, {}, {1}});
    additive_sets.emplace_back(AdditiveSet{{0, 1}, {1}, {1, 1}});
    additive_sets.emplace_back(AdditiveSet{{1, 2, 3}, {2, 3, 4}, {3, 5, 7}});
    additive_sets.emplace_back(AdditiveSet{{1, 2, 3}, {0, 0, 0}, {1, 2, 3}});

    multiplicative_sets.emplace_back(MultiplicativeSet{2, {1, 2, 3}, {2, 4, 6}});
    multiplicative_sets.emplace_back(MultiplicativeSet{1, {1, 2, 3}, {1, 2, 3}});
    multiplicative_sets.emplace_back(MultiplicativeSet{0, {}, {}});
    multiplicative_sets.emplace_back(MultiplicativeSet{0, {1, 1}, {}});
    multiplicative_sets.emplace_back(MultiplicativeSet{-2, {1, 2, 7}, {-2, -4, -14}});

    magnitudes.emplace_back(Magnitude{{1, 2, 5}, 30});
    magnitudes.emplace_back(Magnitude{{2, 2, 2}, 12});
    magnitudes.emplace_back(Magnitude{{3, 4, 5}, 50});

    inner_products.emplace_back(InnerProduct{{1, 1, 1}, {0, 1, 0}, 1});
    inner_products.emplace_back(InnerProduct{{1, 1, 1}, {1, 0, 0}, 1});
    inner_products.emplace_back(InnerProduct{{1, 0, 1}, {1, 0, 0}, 1});
    inner_products.emplace_back(InnerProduct{{1, 0, 1}, {0, 0, 1}, 1});
    inner_products.emplace_back(InnerProduct{{1, 0, 1}, {0, 1, 1}, 1});
    inner_products.emplace_back(InnerProduct{{1, 0, 0}, {0, 1, 0}, 0});
    inner_products.emplace_back(InnerProduct{{1, 0, 0}, {0, 0, 1}, 0});
    inner_products.emplace_back(InnerProduct{{1, 2, 3}, {3, 7, 11}, 50});

    basis_decompositions.emplace_back(BasisDecomposition{{}, {1, 0, 0}, {}, {}});
    basis_decompositions.emplace_back(BasisDecomposition{{}, {1, 1, 1}, {}, {}});
    basis_decompositions.emplace_back(
        BasisDecomposition{{1, 2, 0}, {1, 0, 0}, {1, 0, 0}, {0, 2, 0}});
    basis_decompositions.emplace_back(
        BasisDecomposition{{1, 2, 0}, {1, 1, 0}, {1.5, 1.5, 0}, {-0.5, 0.5, 0}});
  }
};

TEST_F(Cartesian2DVectorTest, RunAllTests) { RunAllTests(); }

TEST_F(Cartesian3DVectorTest, RunAllTests) { RunAllTests(); }

}  // namespace ndyn::math
