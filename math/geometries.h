#pragma once

#include "math/conformal_geometry.h"
#include "math/geometry_model.h"
#include "math/projective_geometry.h"
#include "math/vector_space_geometry.h"

namespace ndyn::math {

template <typename Scalar = DefaultScalarType>
using Vga2dGeometry = VectorSpaceGeometryType<2, Scalar>;

template <typename Scalar = DefaultScalarType>
using Vga3dGeometry = VectorSpaceGeometryType<3, Scalar>;

template <typename Scalar = DefaultScalarType>
using Vga4dGeometry = VectorSpaceGeometryType<4, Scalar>;

template <typename Scalar = DefaultScalarType,
          typename AtlasType = ProjectiveAtlas<Vga2dGeometry<Scalar>>>
using Pga2dGeometry = ProjectiveGeometryType<Vga2dGeometry<Scalar>, AtlasType, Scalar>;
static_assert(std::is_same_v<Pga2dGeometry<>::Scalar, Vga2dGeometry<>::Scalar>);

template <typename Scalar = DefaultScalarType,
          typename AtlasType = ProjectiveAtlas<Vga3dGeometry<Scalar>>>
using Pga3dGeometry = ProjectiveGeometryType<Vga3dGeometry<Scalar>, AtlasType, Scalar>;

template <typename Scalar = DefaultScalarType,
          typename AtlasType = ProjectiveAtlas<Vga4dGeometry<Scalar>>>
using Pga4dGeometry = ProjectiveGeometryType<Vga4dGeometry<Scalar>, AtlasType, Scalar>;

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

static_assert(IsAtlas<ProjectiveAtlas<Vga2dGeometry<>>>);
static_assert(IsAtlas<ProjectiveAtlas<Vga3dGeometry<>>>);
static_assert(IsAtlas<ProjectiveAtlas<Vga4dGeometry<>>>);

static_assert(HasGenericBases<Vga2dGeometry<>>);
static_assert(HasGenericBases<Vga3dGeometry<>>);
static_assert(HasGenericBases<Vga4dGeometry<>>);

static_assert(VectorSpaceGeometryModel<Vga2dGeometry<>>);
static_assert(VectorSpaceGeometryModel<Vga3dGeometry<>>);
static_assert(VectorSpaceGeometryModel<Vga4dGeometry<>>);

static_assert(HasGenericBases<Pga2dGeometry<>>);
static_assert(HasGenericBases<Pga3dGeometry<>>);
static_assert(HasGenericBases<Pga4dGeometry<>>);

static_assert(ProjectiveGeometryModel<Pga2dGeometry<>>);
static_assert(ProjectiveGeometryModel<Pga3dGeometry<>>);
static_assert(ProjectiveGeometryModel<Pga4dGeometry<>>);

static_assert(HasGenericBases<Cga2dGeometry<>>);
static_assert(HasGenericBases<Cga3dGeometry<>>);
static_assert(HasGenericBases<Cga4dGeometry<>>);

static_assert(ConformalGeometryModel<Cga2dGeometry<>>);
static_assert(ConformalGeometryModel<Cga3dGeometry<>>);
static_assert(ConformalGeometryModel<Cga4dGeometry<>>);

}  // namespace ndyn::math
