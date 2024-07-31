#include "math/caley.h"

#include "gtest/gtest.h"

namespace ndyn::math {

TEST(CaleyTableTest, CanGenerateScalarEntries) {
  static constexpr size_t SCALAR_GRADE{
      ScalarCaleyTable<Operations::GEOMETRIC_PRODUCT>::SCALAR_GRADE};
  static constexpr ScalarCaleyTable<Operations::GEOMETRIC_PRODUCT> table{};
  const auto entry{table.entry(0, 0)};
  EXPECT_EQ(SCALAR_GRADE, entry.grade);
  EXPECT_EQ(1, entry.quadratic_multiplier);
}

static constexpr TableEntry
    complex_caley_table[ComplexCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT]
                       [ComplexCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT] =  //
    {
        {
            {0, 1}, {1, 1},  //
        },
        {
            {1, 1}, {0, -1},  //
        },
};

TEST(CaleyTableTest, CanGenerateComplexEntries) {
  static constexpr auto expected_table = complex_caley_table;
  static constexpr ComplexCaleyTable<Operations::GEOMETRIC_PRODUCT> table{};
  for (size_t lhs_grade = 0;
       lhs_grade < ComplexCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT; ++lhs_grade) {
    for (size_t rhs_grade = 0;
         rhs_grade < ComplexCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT; ++rhs_grade) {
      auto entry{table.entry(lhs_grade, rhs_grade)};
      EXPECT_EQ(expected_table[lhs_grade][rhs_grade], entry)
          << "lhs_grade: " << lhs_grade << ", rhs_grade: " << rhs_grade;
    }
  }
}

static constexpr TableEntry
    dual_caley_table[DualCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT]
                    [DualCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT] =  //
    {
        {
            {0, 1}, {1, 1},  //
        },
        {
            {1, 1}, {0, 0},  //
        },
};

TEST(CaleyTableTest, CanGenerateDualEntries) {
  static constexpr auto expected_table = dual_caley_table;
  static constexpr DualCaleyTable<Operations::GEOMETRIC_PRODUCT> table{};
  for (size_t lhs_grade = 0; lhs_grade < DualCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT;
       ++lhs_grade) {
    for (size_t rhs_grade = 0;
         rhs_grade < DualCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT; ++rhs_grade) {
      auto entry{table.entry(lhs_grade, rhs_grade)};
      EXPECT_EQ(expected_table[lhs_grade][rhs_grade], entry)
          << "lhs_grade: " << lhs_grade << ", rhs_grade: " << rhs_grade;
    }
  }
}

static constexpr TableEntry
    nontrivial_caley_table[CaleyTable<Operations::GEOMETRIC_PRODUCT, 0, 3, 0>::GRADE_COUNT]
                          [CaleyTable<Operations::GEOMETRIC_PRODUCT, 0, 3, 0>::GRADE_COUNT] =  //
    {
        // lhs_grade: 0 (scalar)
        {
            {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1}, {7, 1},  //
        },
        // lhs_grade: 1 (e1)
        {
            {1, 1},
            {0, -1},
            {3, 1},
            {2, -1},  // e1 * e12
            {5, 1},
            {4, -1},
            {7, 1},   // e1 * e23
            {6, -1},  // e1 * e123
        },
        // lhs_grade: 2 (e2)
        {
            {2, 1},
            {3, -1},  // e2 * e1
            {0, -1},
            {1, 1},
            {6, 1},
            {7, -1},  // e2 * e13
            {4, -1},  // e2 * e23
            {5, 1},   //
        },
        // lhs_grade: 3 (e12)
        {
            {3, 1}, {2, 1}, {1, -1}, {0, -1}, {7, 1}, {6, 1}, {5, -1}, {4, -1},  //
        },
        // lhs_grade: 4 (e3)
        {
            {4, 1},
            {5, -1},  // e3 * e1
            {6, -1},  // e3 * e2
            {7, 1},   // e3 * e12
            {0, -1},  // e3 * e3
            {1, 1},   // e3 * e13
            {2, 1},   // e3 * e23
            {3, -1},  // e3 * e123
        },
        // lhs_grade: 5 (e13)
        {
            {5, 1},
            {4, 1},   // e13 * e1
            {7, -1},  // e13 * e2
            {6, -1},  // e13 * e12
            {1, -1},  // e13 * e3
            {0, -1},  // e13 * e13
            {3, 1},   // e13 * e23
            {2, 1},   // e13 * e123
        },
        // lhs_grade: 6 (e23)
        {
            {6, 1},
            {7, 1},   // e23 * e1
            {4, 1},   // e23 * e2
            {5, 1},   // e23 * e12
            {2, -1},  // e23 * e3
            {3, -1},  // e23 * e13
            {0, -1},  // e23 * e23
            {1, -1},  // e23 * e123
        },
        // lhs_grade: 7 (e123)
        {
            {7, 1},
            {6, -1},  // e123 * e1
            {5, 1},   // e123 * e2
            {4, -1},  // e123 * e12
            {3, -1},  // e123 * e3
            {2, 1},   // e123 * e13
            {1, -1},  // e123 * e23
            {0, 1},   // e123 * e123
        },
};

TEST(CaleyTableTest, CanGenerateNontrivialEntries) {
  static constexpr auto expected_table = nontrivial_caley_table;
  static constexpr CaleyTable<Operations::GEOMETRIC_PRODUCT, 0, 3, 0> table{};
  for (size_t lhs_grade = 0;
       lhs_grade < CaleyTable<Operations::GEOMETRIC_PRODUCT, 0, 3, 0>::GRADE_COUNT; ++lhs_grade) {
    for (size_t rhs_grade = 0;
         rhs_grade < CaleyTable<Operations::GEOMETRIC_PRODUCT, 0, 3, 0>::GRADE_COUNT; ++rhs_grade) {
      auto entry{table.entry(lhs_grade, rhs_grade)};
      EXPECT_EQ(expected_table[lhs_grade][rhs_grade], entry)
          << "lhs_grade: " << lhs_grade << ", rhs_grade: " << rhs_grade;
    }
  }
}

static constexpr TableEntry
    spacetime_caley_table[SpacetimeCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT]
                         [SpacetimeCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT] =  //
    {
        // lhs_grade: 0 (scalar)
        {
            {0, 1},
            {1, 1},
            {2, 1},
            {3, 1},
            {4, 1},
            {5, 1},
            {6, 1},
            {7, 1},
            {8, 1},
            {9, 1},
            {10, 1},
            {11, 1},
            {12, 1},
            {13, 1},
            {14, 1},
            {15, 1},
        },
        // lhs_grade: 1 (e1)
        {
            {1, 1},
            {0, 1},   // e1 * e1 = 1
            {3, 1},   // e1 * e2 = e12
            {2, 1},   // e1 * e12 = e2
            {5, 1},   // e1 * e3 = e13
            {4, 1},   // e1 * e13 = e3
            {7, 1},   // e1 * e23 = e123
            {6, 1},   // e1 * e123 = e23
            {9, 1},   // e1 * e4 = e14
            {8, 1},   // e1 * e14 = e4
            {11, 1},  // e1 * e24 = e124
            {10, 1},  // e1 * e124 = e24
            {13, 1},  // e1 * e34 = e134
            {12, 1},  // e1 * e134 = e34
            {15, 1},  // e1 * e234 = e1234
            {14, 1},  // e1 * e1234 = e234
        },
        // lhs_grade: 2 (e2)
        {
            {2, 1},
            {3, -1},   // e2 * e1 = -e12
            {0, -1},   // e2 * e2 = -1
            {1, 1},    // e2 * e12 = e1
            {6, 1},    // e2 * e3 = e23
            {7, -1},   // e2 * e13 = -e123
            {4, -1},   // e2 * e23 = -e3
            {5, 1},    // e2 * e123 = e13
            {10, 1},   // e2 * e4 = e24 ; rhs_grade: 8
            {11, -1},  // e2 * e14 = -e124
            {8, -1},   // e2 * e24 = -e4
            {9, 1},    // e2 * e124 = e14
            {14, 1},   // e2 * e34 = e234
            {15, -1},  // e2 * e134 = -e1234
            {12, -1},  // e2 * e234 = -e34
            {13, 1},   // e2 * e1234 = e134
        },
        // lhs_grade: 3 (e12)
        {
            {3, 1},
            {2, -1},   // e12 * e1 = -e2
            {1, -1},   // e12 * e2 = -e1
            {0, 1},    // e12 * e12 = 1
            {7, 1},    // e12 * e3 = e123
            {6, -1},   // e12 * e13 = -e23
            {5, -1},   // e12 * e23 = -e13
            {4, 1},    // e12 * e123 = e3
            {11, 1},   // e12 * e4 = e124 ; rhs_grade: 8
            {10, -1},  // e12 * e14 = -e24
            {9, -1},   // e12 * e24 = -e14
            {8, 1},    // e12 * e124 = e4
            {15, 1},   // e12 * e34 = e1234
            {14, -1},  // e12 * e134 = -e234
            {13, -1},  // e12 * e234 = -e134
            {12, 1},   // e12 * e1234 = e34
        },
        // lhs_grade: 4 (e3)
        {
            {4, 1},
            {5, -1},   // e3 * e1 = -e13
            {6, -1},   // e3 * e2 = -e23
            {7, 1},    // e3 * e12 = e123
            {0, -1},   // e3 * e3 = -1
            {1, 1},    // e3 * e13 = e1
            {2, 1},    // e3 * e23 = e2
            {3, -1},   // e3 * e123 = -e12
            {12, 1},   // e3 * e4 = e34 ; rhs_grade: 8
            {13, -1},  // e3 * e14 = -e134
            {14, -1},  // e3 * e24 = -e234
            {15, 1},   // e3 * e124 = e1234
            {8, -1},   // e3 * e34 = -e4
            {9, 1},    // e3 * e134 = e14
            {10, 1},   // e3 * e234 = e24
            {11, -1},  // e3 * e1234 = -e124
        },
        // lhs_grade: 5 (e13)
        {
            {5, 1},
            {4, -1},   // e13 * e1 = -e3
            {7, -1},   // e13 * e2 = -e123
            {6, 1},    // e13 * e12 = e23
            {1, -1},   // e13 * e3 = -e1
            {0, 1},    // e13 * e13 = 1
            {3, 1},    // e13 * e23 = e12
            {2, -1},   // e13 * e123 = -e2
            {13, 1},   // e13 * e4 = e134 ; rhs_grade: 8
            {12, -1},  // e13 * e14 = -e34
            {15, -1},  // e13 * e24 = -e1234
            {14, 1},   // e13 * e124 = e234
            {9, -1},   // e13 * e34 = -e14
            {8, 1},    // e13 * e134 = e4
            {11, 1},   // e13 * e234 = e124
            {10, -1},  // e13 * e1234 = -e24
        },
        // lhs_grade: 6 (e23)
        {
            {6, 1},
            {7, 1},    // e23 * e1 = e123
            {4, 1},    // e23 * e2 = e3
            {5, 1},    // e23 * e12 = e13
            {2, -1},   // e23 * e3 = -e2
            {3, -1},   // e23 * e13 = -e12
            {0, -1},   // e23 * e23 = -1
            {1, -1},   // e23 * e123 = -e1
            {14, 1},   // e23 * e4 = e234 ; rhs_grade: 8
            {15, 1},   // e23 * e14 = e1234
            {12, 1},   // e23 * e24 = e34
            {13, 1},   // e23 * e124 = e134
            {10, -1},  // e23 * e34 = -e24
            {11, -1},  // e23 * e134 = -e124
            {8, -1},   // e23 * e234 = -e4
            {9, -1},   // e23 * e1234 = -e14
        },
        // lhs_grade: 7 (e123)
        {
            {7, 1},
            {6, 1},    // e123 * e1 = e23
            {5, 1},    // e123 * e2 = e13
            {4, 1},    // e123 * e12 = e3
            {3, -1},   // e123 * e3 = -e12
            {2, -1},   // e123 * e13 = -e2
            {1, -1},   // e123 * e23 = -e1
            {0, -1},   // e123 * e123 = -1
            {15, 1},   // e123 * e4 = e1234
            {14, 1},   // e123 * e14 = e234
            {13, 1},   // e123 * e24 = e134
            {12, 1},   // e123 * e124 = e34
            {11, -1},  // e123 * e34 = -e124
            {10, -1},  // e123 * e134 = -e24
            {9, -1},   // e123 * e234 = -e14
            {8, -1},   // e123 * e1234 = -e4
        },
        // lhs_grade: 8 (e4)
        {
            {8, 1},
            {9, -1},   // e4 * e1 = -e14
            {10, -1},  // e4 * e2 = -e24
            {11, 1},   // e4 * e12 = e124
            {12, -1},  // e4 * e3 = -e34
            {13, 1},   // e4 * e13 = e134
            {14, 1},   // e4 * e23 = e234
            {15, -1},  // e4 * e123 = -e1234
            {0, -1},   // e4 * e4 = -1 ; rhs_grade: 8
            {1, 1},    // e4 * e14 = e1
            {2, 1},    // e4 * e24 = e2
            {3, -1},   // e4 * e124 = -e12
            {4, 1},    // e4 * e34 = e3
            {5, -1},   // e4 * e134 = -e13
            {6, -1},   // e4 * e234 = -e23
            {7, 1},    // e4 * e1234 = e123
        },
        // lhs_grade: 9 (e14)
        {
            {9, 1},
            {8, -1},   // e14 * e1 = -e4
            {11, -1},  // e14 * e2 = -e124
            {10, 1},   // e14 * e12 = e24
            {13, -1},  // e14 * e3 = -e134
            {12, 1},   // e14 * e13 = e34
            {15, 1},   // e14 * e23 = e1234
            {14, -1},  // e14 * e123 = -e234
            {1, -1},   // e14 * e4 = -e1 ; rhs_grade: 8
            {0, 1},    // e14 * e14 = 1
            {3, 1},    // e14 * e24 = e12
            {2, -1},   // e14 * e124 = -e2
            {5, 1},    // e14 * e34 = e13
            {4, -1},   // e14 * e134 = -e3
            {7, -1},   // e14 * e234 = -e123
            {6, 1},    // e14 * e1234 = e23
        },
        // lhs_grade: 10 (e24)
        {
            {10, 1},
            {11, 1},   // e24 * e1 = e124
            {8, 1},    // e24 * e2 = e4
            {9, 1},    // e24 * e12 = e14
            {14, -1},  // e24 * e3 = -e234
            {15, -1},  // e24 * e13 = -e1234
            {12, -1},  // e24 * e23 = -e34
            {13, -1},  // e24 * e123 = -e134
            {2, -1},   // e24 * e4 = -e2
            {3, -1},   // e24 * e14 = -e12
            {0, -1},   // e24 * e24 = -1
            {1, -1},   // e24 * e124 = -e1
            {6, 1},    // e24 * e34 = e23
            {7, 1},    // e24 * e134 = e123
            {4, 1},    // e24 * e234 = e3
            {5, 1},    // e24 * e1234 = e13
        },
        // lhs_grade: 11 (e124)
        {
            {11, 1},
            {10, 1},   // e124 * e1 = e24
            {9, 1},    // e124 * e2 = e14
            {8, 1},    // e124 * e12 = e4
            {15, -1},  // e124 * e3 = -e1234
            {14, -1},  // e124 * e13 = -e234
            {13, -1},  // e124 * e23 = -e134
            {12, -1},  // e124 * e123 = -e34
            {3, -1},   // e124 * e4 = -e12
            {2, -1},   // e124 * e14 = -e2
            {1, -1},   // e124 * e24 = -e1
            {0, -1},   // e124 * e124 = -1
            {7, 1},    // e124 * e34 = e123
            {6, 1},    // e124 * e134 = e23
            {5, 1},    // e124 * e234 = e13
            {4, 1},    // e124 * e1234 = e3
        },
        // lhs_grade: 12 (e34)
        {
            {12, 1},
            {13, 1},  // e34 * e1 = e134
            {14, 1},  // e34 * e2 = e234
            {15, 1},  // e34 * e12 = e1234
            {8, 1},   // e34 * e3 = e4
            {9, 1},   // e34 * e13 = e14
            {10, 1},  // e34 * e23 = e24
            {11, 1},  // e34 * e123 = e124
            {4, -1},  // e34 * e4 = -e3
            {5, -1},  // e34 * e14 = -e13
            {6, -1},  // e34 * e24 = -e23
            {7, -1},  // e34 * e124 = -e123
            {0, -1},  // e34 * e34 = -1
            {1, -1},  // e34 * e134 = -e1
            {2, -1},  // e34 * e234 = -e2
            {3, -1},  // e34 * e1234 = -e12
        },
        // lhs_grade: 13 (e134)
        {
            {13, 1},
            {12, 1},  // e134 * e1 = e34
            {15, 1},  // e134 * e2 = e1234
            {14, 1},  // e134 * e12 = e234
            {9, 1},   // e134 * e3 = e14
            {8, 1},   // e134 * e13 = e4
            {11, 1},  // e134 * e23 = e124
            {10, 1},  // e134 * e123 = e24
            {5, -1},  // e134 * e4 = -e13
            {4, -1},  // e134 * e14 = -e3
            {7, -1},  // e134 * e24 = -e123
            {6, -1},  // e134 * e124 = -e23
            {1, -1},  // e134 * e34 = -e1
            {0, -1},  // e134 * e134 = -1
            {3, -1},  // e134 * e234 = -e12
            {2, -1},  // e134 * e1234 = -e2
        },
        // lhs_grade: 14 (e234)
        {
            {14, 1},
            {15, -1},  // e234 * e1 = -e1234
            {12, -1},  // e234 * e2 = -e34
            {13, 1},   // e234 * e12 = e134
            {10, 1},   // e234 * e3 = e24
            {11, -1},  // e234 * e13 = -e124
            {8, -1},   // e234 * e23 = -e4
            {9, 1},    // e234 * e123 = e14
            {6, -1},   // e234 * e4 = -e23
            {7, 1},    // e234 * e14 = e123
            {4, 1},    // e234 * e24 = e3
            {5, -1},   // e234 * e124 = -e13
            {2, -1},   // e234 * e34 = -e2
            {3, 1},    // e234 * e134 = e12
            {0, 1},    // e234 * e234 = 1
            {1, -1},   // e234 * e1234 = -e1
        },
        // lhs_grade: 15 (e1234)
        {
            {15, 1},
            {14, -1},  // e1234 * e1 = -e234
            {13, -1},  // e1234 * e2 = -e134
            {12, 1},   // e1234 * e12 = e34
            {11, 1},   // e1234 * e3 = e124
            {10, -1},  // e1234 * e13 = -e24
            {9, -1},   // e1234 * e23 = -e14
            {8, 1},    // e1234 * e123 = e4
            {7, -1},   // e1234 * e4 = -e123 ; rhs_grade: 8
            {6, 1},    // e1234 * e14 = e23
            {5, 1},    // e1234 * e24 = e13
            {4, -1},   // e1234 * e124 = -e3
            {3, -1},   // e1234 * e34 = -e12
            {2, 1},    // e1234 * e134 = e2
            {1, 1},    // e1234 * e234 = e1
            {0, -1},   // e1234 * e1234 = -1
        },
};

TEST(CaleyTableTest, CanGenerateSpacetimeEntries) {
  static constexpr auto expected_table = spacetime_caley_table;
  static constexpr SpacetimeCaleyTable<Operations::GEOMETRIC_PRODUCT> table{};
  for (size_t lhs_grade = 0;
       lhs_grade < SpacetimeCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT; ++lhs_grade) {
    for (size_t rhs_grade = 0;
         rhs_grade < SpacetimeCaleyTable<Operations::GEOMETRIC_PRODUCT>::GRADE_COUNT; ++rhs_grade) {
      auto entry{table.entry(lhs_grade, rhs_grade)};
      EXPECT_EQ(expected_table[lhs_grade][rhs_grade], entry)
          << "lhs_grade: " << lhs_grade << ", rhs_grade: " << rhs_grade;
    }
  }
}

}  // namespace ndyn::math
