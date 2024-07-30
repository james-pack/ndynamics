#include "math/caley.h"

#include "gtest/gtest.h"

namespace ndyn::math {

TEST(CaleyTableTest, CanGenerateScalarEntries) {
  static constexpr size_t SCALAR_GRADE{ScalarCaleyTable<float>::SCALAR_GRADE};
  static constexpr ScalarCaleyTable<float> table{};
  const auto entry{table.entry(0, 0)};
  EXPECT_EQ(SCALAR_GRADE, entry.grade);
  EXPECT_EQ(1.f, entry.quadratic_multiplier);
}

static constexpr TableEntry<float> complex_caley_table[ComplexCaleyTable<float>::GRADE_COUNT]
                                                      [ComplexCaleyTable<float>::GRADE_COUNT] =  //
    {
        {
            {0, 1.f}, {1, 1.f},  //
        },
        {
            {1, 1.f}, {0, -1.f},  //
        },
};

TEST(CaleyTableTest, CanGenerateComplexEntries) {
  static constexpr auto expected_table = complex_caley_table;
  static constexpr ComplexCaleyTable<float> table{};
  for (size_t lhs_grade = 0; lhs_grade < ComplexCaleyTable<float>::GRADE_COUNT; ++lhs_grade) {
    for (size_t rhs_grade = 0; rhs_grade < ComplexCaleyTable<float>::GRADE_COUNT; ++rhs_grade) {
      auto entry{table.entry(lhs_grade, rhs_grade)};
      EXPECT_EQ(expected_table[lhs_grade][rhs_grade], entry)
          << "lhs_grade: " << lhs_grade << ", rhs_grade: " << rhs_grade;
    }
  }
}

static constexpr TableEntry<float> dual_caley_table[DualCaleyTable<float>::GRADE_COUNT]
                                                   [DualCaleyTable<float>::GRADE_COUNT] =  //
    {
        {
            {0, 1.f}, {1, 1.f},  //
        },
        {
            {1, 1.f}, {0, 0.f},  //
        },
};

TEST(CaleyTableTest, CanGenerateDualEntries) {
  static constexpr auto expected_table = dual_caley_table;
  static constexpr DualCaleyTable<float> table{};
  for (size_t lhs_grade = 0; lhs_grade < DualCaleyTable<float>::GRADE_COUNT; ++lhs_grade) {
    for (size_t rhs_grade = 0; rhs_grade < DualCaleyTable<float>::GRADE_COUNT; ++rhs_grade) {
      auto entry{table.entry(lhs_grade, rhs_grade)};
      EXPECT_EQ(expected_table[lhs_grade][rhs_grade], entry)
          << "lhs_grade: " << lhs_grade << ", rhs_grade: " << rhs_grade;
    }
  }
}

static constexpr TableEntry<float>
    nontrivial_caley_table[CaleyTable<float, 0, 3, 0>::GRADE_COUNT]
                          [CaleyTable<float, 0, 3, 0>::GRADE_COUNT] =  //
    {
        // lhs_grade: 0 (scalar)
        {
            {0, 1.f}, {1, 1.f}, {2, 1.f}, {3, 1.f}, {4, 1.f}, {5, 1.f}, {6, 1.f}, {7, 1.f},  //
        },
        // lhs_grade: 1 (e1)
        {
            {1, 1.f},
            {0, -1.f},
            {3, 1.f},
            {2, -1.f},  // e1 * e12
            {5, 1.f},
            {4, -1.f},
            {7, 1.f},   // e1 * e23
            {6, -1.f},  // e1 * e123
        },
        // lhs_grade: 2 (e2)
        {
            {2, 1.f},
            {3, -1.f},  // e2 * e1
            {0, -1.f},
            {1, 1.f},
            {6, 1.f},
            {7, -1.f},  // e2 * e13
            {4, -1.f},  // e2 * e23
            {5, 1.f},   //
        },
        // lhs_grade: 3 (e12)
        {
            {3, 1.f}, {2, 1.f}, {1, -1.f}, {0, -1.f}, {7, 1.f}, {6, 1.f}, {5, -1.f}, {4, -1.f},  //
        },
        // lhs_grade: 4 (e3)
        {
            {4, 1.f},
            {5, -1.f},  // e3 * e1
            {6, -1.f},  // e3 * e2
            {7, 1.f},   // e3 * e12
            {0, -1.f},  // e3 * e3
            {1, 1.f},   // e3 * e13
            {2, 1.f},   // e3 * e23
            {3, -1.f},  // e3 * e123
        },
        // lhs_grade: 5 (e13)
        {
            {5, 1.f},
            {4, 1.f},   // e13 * e1
            {7, -1.f},  // e13 * e2
            {6, -1.f},  // e13 * e12
            {1, -1.f},  // e13 * e3
            {0, -1.f},  // e13 * e13
            {3, 1.f},   // e13 * e23
            {2, 1.f},   // e13 * e123
        },
        // lhs_grade: 6 (e23)
        {
            {6, 1.f},
            {7, 1.f},   // e23 * e1
            {4, 1.f},   // e23 * e2
            {5, 1.f},   // e23 * e12
            {2, -1.f},  // e23 * e3
            {3, -1.f},  // e23 * e13
            {0, -1.f},  // e23 * e23
            {1, -1.f},  // e23 * e123
        },
        // lhs_grade: 7 (e123)
        {
            {7, 1.f},
            {6, -1.f},  // e123 * e1
            {5, 1.f},   // e123 * e2
            {4, -1.f},  // e123 * e12
            {3, -1.f},  // e123 * e3
            {2, 1.f},   // e123 * e13
            {1, -1.f},  // e123 * e23
            {0, 1.f},   // e123 * e123
        },
};

TEST(CaleyTableTest, CanGenerateNontrivialEntries) {
  static constexpr auto expected_table = nontrivial_caley_table;
  static constexpr CaleyTable<float, 0, 3, 0> table{};
  for (size_t lhs_grade = 0; lhs_grade < CaleyTable<float, 0, 3, 0>::GRADE_COUNT; ++lhs_grade) {
    for (size_t rhs_grade = 0; rhs_grade < CaleyTable<float, 0, 3, 0>::GRADE_COUNT; ++rhs_grade) {
      auto entry{table.entry(lhs_grade, rhs_grade)};
      EXPECT_EQ(expected_table[lhs_grade][rhs_grade], entry)
          << "lhs_grade: " << lhs_grade << ", rhs_grade: " << rhs_grade;
    }
  }
}

}  // namespace ndyn::math
