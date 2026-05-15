#include "base/initializer.h"
#include "calculator/options.h"
#include "calculator/repl.h"
#include "glog/logging.h"
#include "math/geometries.h"
#include "math/representation.h"

namespace ndyn::calculator {

/**
 * Primary entry point for the specialized algebra logic.
 * This function is instantiated for every valid combination of geometric
 * parameters, allowing the compiler to optimize the physics kernels based on
 * the specific metric and dimensionality.
 */
template <Model M, Metric Met, MinkowskiConvention Conv, GeometricInterpretation Interp,
          typename Scalar, int Dims>
struct Calculator final {
  static void run() { LOG(FATAL) << "Unsupported or unimplemented combination of flags."; }
};

template <MinkowskiConvention Conv, typename Scalar, int Dims>
struct Calculator<Model::Cga, Metric::Euclidean, Conv, GeometricInterpretation::Opns, Scalar, Dims>
    final {
  static void run() {
    using EmbeddedGeometry = math::VectorSpaceGeometryType<Dims, Scalar>;
    using Atlas = math::ConformalAtlas<EmbeddedGeometry>;
    using Geometry = math::ConformalGeometryType<EmbeddedGeometry, Atlas>;
    using Algebra = Geometry::Algebra;
    Repl<Algebra, math::GenericRepresentation<Algebra>> repl{};
    repl.loop();
  }
};

template <MinkowskiConvention Conv, typename Scalar, int Dims>
struct Calculator<Model::Hga, Metric::Euclidean, Conv, GeometricInterpretation::Opns, Scalar, Dims>
    final {
  static void run() {
    using EmbeddedGeometry = math::VectorSpaceGeometryType<Dims, Scalar>;
    using Atlas = math::HomogeneousAtlas<EmbeddedGeometry>;
    using Geometry = math::HomogeneousGeometryType<EmbeddedGeometry, Atlas>;
    using Algebra = Geometry::Algebra;
    Repl<Algebra, math::GenericRepresentation<Algebra>> repl{};
    repl.loop();
  }
};

template <MinkowskiConvention Conv, typename Scalar, int Dims>
struct Calculator<Model::Vga, Metric::Euclidean, Conv, GeometricInterpretation::Opns, Scalar, Dims>
    final {
  static void run() {
    using Geometry = math::VectorSpaceGeometryType<Dims, Scalar>;
    using Algebra = Geometry::Algebra;
    Repl<Algebra, math::GenericRepresentation<Algebra>> repl{};
    repl.loop();
  }
};

/**
 * Orchestrates the conversion of runtime configuration flags into a
 * compile-time type environment. This dispatcher uses a layered
 * approach to resolve each algebraic dimension sequentially.
 */
class GeometryDispatcher {
 public:
  /**
   * Reads the current global flag states and begins the dispatch process.
   */
  static void dispatch() {
    const Model model = get_model_flag();
    const Metric metric = get_metric_flag();
    const MinkowskiConvention conv = get_convention_flag();
    const GeometricInterpretation interp = get_interpretation_flag();
    const Scalar scalar = get_scalar_flag();
    const int dims = get_dims_flag();

    dispatch_model(model, metric, conv, interp, scalar, dims);
  }

 private:
  /**
   * Resolves the high-level geometric model (VGA, PGA, or CGA).
   */
  static void dispatch_model(Model m, Metric met, MinkowskiConvention c, GeometricInterpretation i,
                             Scalar scalar, int d) {
    switch (m) {
      case Model::Vga:
        dispatch_metric<Model::Vga>(met, c, i, scalar, d);
        break;
      case Model::Hga:
        dispatch_metric<Model::Hga>(met, c, i, scalar, d);
        break;
      case Model::Cga:
        dispatch_metric<Model::Cga>(met, c, i, scalar, d);
        break;
    }
  }

  /**
   * Resolves the metric properties of the base manifold.
   */
  template <Model M>
  static void dispatch_metric(Metric met, MinkowskiConvention c, GeometricInterpretation i,
                              Scalar scalar, int d) {
    switch (met) {
      case Metric::Euclidean:
        dispatch_convention<M, Metric::Euclidean>(c, i, scalar, d);
        break;
      case Metric::Minkowski:
        dispatch_convention<M, Metric::Minkowski>(c, i, scalar, d);
        break;
    }
  }

  /**
   * Resolves the sign convention for pseudo-Riemannian metrics.
   */
  template <Model M, Metric Met>
  static void dispatch_convention(MinkowskiConvention c, GeometricInterpretation i, Scalar scalar,
                                  int d) {
    switch (c) {
      case MinkowskiConvention::EastCoast:
        dispatch_interp<M, Met, MinkowskiConvention::EastCoast>(i, scalar, d);
        break;
      case MinkowskiConvention::WestCoast:
        dispatch_interp<M, Met, MinkowskiConvention::WestCoast>(i, scalar, d);
        break;
    }
  }

  /**
   * Resolves the representation style (OPNS vs IPNS) for the geometric elements.
   */
  template <Model M, Metric Met, MinkowskiConvention C>
  static void dispatch_interp(GeometricInterpretation i, Scalar scalar, int d) {
    switch (i) {
      case GeometricInterpretation::Opns:
        dispatch_scalar<M, Met, C, GeometricInterpretation::Opns>(scalar, d);
        break;
      case GeometricInterpretation::Ipns:
        dispatch_scalar<M, Met, C, GeometricInterpretation::Ipns>(scalar, d);
        break;
    }
  }

  template <Model M, Metric Met, MinkowskiConvention C, GeometricInterpretation I>
  static void dispatch_scalar(Scalar scalar, int d) {
    switch (scalar) {
      case Scalar::Short:
        dispatch_dims<M, Met, C, I, short>(d);
        break;
      case Scalar::Int:
        dispatch_dims<M, Met, C, I, int>(d);
        break;
      case Scalar::Long:
        dispatch_dims<M, Met, C, I, long>(d);
        break;
      case Scalar::Float:
        dispatch_dims<M, Met, C, I, float>(d);
        break;
      case Scalar::Double:
        dispatch_dims<M, Met, C, I, double>(d);
        break;
    }
  }

  template <Model M, Metric Met, MinkowskiConvention C, GeometricInterpretation I, typename Scalar>
  static void dispatch_dims(int d) {
    switch (d) {
      case 1:
        Calculator<M, Met, C, I, Scalar, 1>::run();
        break;
      case 2:
        Calculator<M, Met, C, I, Scalar, 2>::run();
        break;
      case 3:
        Calculator<M, Met, C, I, Scalar, 3>::run();
        break;
      case 4:
        Calculator<M, Met, C, I, Scalar, 4>::run();
        break;
      default:
        LOG(FATAL) << d << " dimensions not supported/implemented.";
    }
  }
};

}  // namespace ndyn::calculator

int main(int argc, char* argv[]) {
  ndyn::initialize(&argc, &argv);

  ndyn::calculator::GeometryDispatcher::dispatch();

  return 0;
}
