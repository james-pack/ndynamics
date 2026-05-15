#pragma once

#include "math/conformal_geometry.h"
#include "math/geometry_model.h"
#include "math/homogeneous_geometry.h"
#include "math/vector_space_geometry.h"

namespace ndyn::math {

template <typename Scalar = DefaultScalarType>
using Vga2dGeometry = VectorSpaceGeometryType<2, Scalar>;

template <typename Scalar = DefaultScalarType>
using Vga3dGeometry = VectorSpaceGeometryType<3, Scalar>;

template <typename Scalar = DefaultScalarType>
using Vga4dGeometry = VectorSpaceGeometryType<4, Scalar>;

template <typename Scalar = DefaultScalarType,
          typename AtlasType = HomogeneousAtlas<Vga2dGeometry<Scalar>>>
using Hga2dGeometry = HomogeneousGeometryType<Vga2dGeometry<Scalar>, AtlasType, Scalar>;
static_assert(std::is_same_v<Hga2dGeometry<>::Scalar, Vga2dGeometry<>::Scalar>);

template <typename Scalar = DefaultScalarType,
          typename AtlasType = HomogeneousAtlas<Vga3dGeometry<Scalar>>>
using Hga3dGeometry = HomogeneousGeometryType<Vga3dGeometry<Scalar>, AtlasType, Scalar>;

template <typename Scalar = DefaultScalarType,
          typename AtlasType = HomogeneousAtlas<Vga4dGeometry<Scalar>>>
using Hga4dGeometry = HomogeneousGeometryType<Vga4dGeometry<Scalar>, AtlasType, Scalar>;

template <typename Scalar = DefaultScalarType,
          typename AtlasType = ConformalAtlas<Vga2dGeometry<Scalar>>>
using Cga2dGeometry = ConformalGeometryType<Vga2dGeometry<Scalar>, AtlasType, Scalar>;
static_assert(std::is_same_v<Cga2dGeometry<>::Scalar, Vga2dGeometry<>::Scalar>);

template <typename Scalar = DefaultScalarType,
          typename AtlasType = ConformalAtlas<Vga3dGeometry<Scalar>>>
using Cga3dGeometry = ConformalGeometryType<Vga3dGeometry<Scalar>, AtlasType, Scalar>;

template <typename Scalar = DefaultScalarType,
          typename AtlasType = ConformalAtlas<Vga4dGeometry<Scalar>>>
using Cga4dGeometry = ConformalGeometryType<Vga4dGeometry<Scalar>, AtlasType, Scalar>;

static_assert(IsAtlas<ConformalAtlas<Vga2dGeometry<>>>);
static_assert(IsAtlas<ConformalAtlas<Vga3dGeometry<>>>);
static_assert(IsAtlas<ConformalAtlas<Vga4dGeometry<>>>);

static_assert(IsAtlas<HomogeneousAtlas<Vga2dGeometry<>>>);
static_assert(IsAtlas<HomogeneousAtlas<Vga3dGeometry<>>>);
static_assert(IsAtlas<HomogeneousAtlas<Vga4dGeometry<>>>);

static_assert(HasGenericBases<Vga2dGeometry<>>);
static_assert(HasGenericBases<Vga3dGeometry<>>);
static_assert(HasGenericBases<Vga4dGeometry<>>);

static_assert(VectorSpaceGeometryModel<Vga2dGeometry<>>);
static_assert(VectorSpaceGeometryModel<Vga3dGeometry<>>);
static_assert(VectorSpaceGeometryModel<Vga4dGeometry<>>);

static_assert(HasGenericBases<Hga2dGeometry<>>);
static_assert(HasGenericBases<Hga3dGeometry<>>);
static_assert(HasGenericBases<Hga4dGeometry<>>);

static_assert(HomogeneousGeometryModel<Hga2dGeometry<>>);
static_assert(HomogeneousGeometryModel<Hga3dGeometry<>>);
static_assert(HomogeneousGeometryModel<Hga4dGeometry<>>);

static_assert(HasGenericBases<Cga2dGeometry<>>);
static_assert(HasGenericBases<Cga3dGeometry<>>);
static_assert(HasGenericBases<Cga4dGeometry<>>);

static_assert(ConformalGeometryModel<Cga2dGeometry<>>);
static_assert(ConformalGeometryModel<Cga3dGeometry<>>);
static_assert(ConformalGeometryModel<Cga4dGeometry<>>);

}  // namespace ndyn::math
