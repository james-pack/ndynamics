#pragma once

#include "math/algebra.h"
#include "math/multivector.h"

namespace ndyn::math {

template <typename Scalar = DefaultScalarType,
  InnerProduct INNER_PRODUCT = InnerProduct::LEFT_CONTRACTION>
class VgaToCga final {
 private:
 VgaGeometry<Scalar, INNER_PRODUCT> vga_{};
 CgaGeometry<Scalar, INNER_PRODUCT> cga_{};
    
  public:
[[nodiscard]] constexpr Multivector<Cga<Scalar>> transform(const Multivector<Vga<Scalar>>& vga) const noexcept {
    const float e_plus  =  0.5f * (vga.e1() + vga.e2() + vga.e3());
    const float e_minus = -0.5f * (vga.e1() + vga.e2() + vga.e3());

    const float norm_sq = vga.e1()*vga.e1()   + vga.e2()*vga.e2()   + vga.e3()*vga.e3()
                        + vga.e12()*vga.e12() + vga.e13()*vga.e13() + vga.e23()*vga.e23();

    Multivector<Cga> result{};
    cga.scalar(result)   = vga.scalar();
    cga.e1(result)       = vga.e1();
    cga.e2(result)       = vga.e2();
    cga.e3(result)       = vga.e3();
    cga.e_origin(result) = e_plus - e_minus;
    cga.e_inf(result)    = 0.5f * norm_sq * (e_plus + e_minus);
    cga.e12(result)      = vga.e12();
    cga.e13(result)      = vga.e13();
    cga.e23(result)      = vga.e23();
    cga.e1_inf(result)   = vga.e1()  * (e_plus + e_minus);
    cga.e2_inf(result)   = vga.e2()  * (e_plus + e_minus);
    cga.e3_inf(result)   = vga.e3()  * (e_plus + e_minus);
    cga.e123(result)     = vga.e123();
    cga.e12_inf(result)  = vga.e12()  * (e_plus + e_minus);
    cga.e13_inf(result)  = vga.e13()  * (e_plus + e_minus);
    cga.e23_inf(result)  = vga.e23()  * (e_plus + e_minus);
    cga.e123_inf(result) = vga.e123() * (e_plus + e_minus);
    return result;
}

  };

}
