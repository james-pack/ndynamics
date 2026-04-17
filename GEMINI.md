# Project Status: ndynamics (ndyn)
**Current Reference Date:** April 2026  
**Primary Architect:** User  
**Collaborator:** Gemini (Adaptive Technical AI)

---

## 1. System Architecture Overview
The **ndynamics** project is a high-fidelity physics simulation engine built on a unified Geometric Algebra (GA) framework. The system is designed to handle Newtonian, relativistic, and conformal kinematics within a hierarchical, CAD-like component tree. Unlike traditional engines that use matrices and vectors, `ndyn` utilizes **Motors** (multivectors) as the primary operators for poses, frame transitions, and parallel transport.

### Core Design Principles
* **Coordinate-Free Physics:** Laws are expressed using GA operators, ensuring they remain invariant across different geometric models.
* **Causal Connectivity:** The engine explicitly resolves retardation (light-cone intersection) via a General Covariant Manifold.
* **Hierarchical Composition:** Components are organized in a parent-child tree.

---

## 2. Formal Naming Conventions
To maintain clarity within the fiber-bundle architecture, the following naming standard is strictly enforced:
* **Leaf:** The particle or component currently experiencing an influence/force. It is the "receiver" in a connection.
* **Parent:** Any node above the leaf in the assembly tree. The **immediate parent** (index 0) is typically the source of the physical field being evaluated.
* **Connection:** The algebraic bridge between a Parent's fiber and a Leaf's fiber, resolving both hierarchical offsets and manifold transport.

---

## 3. Algebraic Frameworks
The engine supports multiple Clifford Algebras, allowing the simulation to scale from simple kinematics to complex relativistic manifolds.

| Algebra | Notation | Domain / Use Case |
| :--- | :--- | :--- |
| **VGA** | $Cl(3,0,0)$ | Vectorspace Geometric Algebra; used for simple GA demonstrations (Euclidean only). |
| **PGA** | $Cl(3,0,1)$ | Projective Geometric Algebra; used for Euclidean rigid body mechanics. |
| **CGA** | $Cl(4,1)$ | Conformal Geometric Algebra; represents points, lines, and circles as multivectors. |
| **TCGA** | $Cl(5,1)$ | **Time-Extended Conformal GA**; supports non-relativistic kinematics via conformal algebra. |
| **CSTA** | $Cl(2,4)$ | **Conformal Space-Time Algebra**; supports relativistic kinematics via conformal algebra. |
| **STA** | $Cl(1,3)$ | Standard Space-Time Algebra; for relativistic field evaluation. |

---

## 4. Library Status: `ndyn::math`
The math library provides the fundamental types and operators for Clifford Algebra.
* **State Containers:** Implemented as `math::State<Geometry, N>`, where `element<0>` is the Pose (Motor) and `element<1>` is the Kinematics (Bivector velocity).
* **Operator Integrity:** Supports the anti-automorphism property $\widetilde{AB} = \tilde{B}\tilde{A}$. Reversion is the primary tool for inverting frame transitions.
* **Stable Integration:** Includes 4th-order Runge-Kutta and **Munthe-Kaas** integrators specifically designed for Lie Groups (ensuring states stay on the manifold).
* **Zero-Basis Support:** Templates now explicitly support degenerate basis vectors (e.g., $ZERO\_BASES$), enabling PGA functionality.

---

## 5. Library Status: `ndyn::assembly`
The assembly library manages the organization and interaction of physical entities.
* **Worldline:** Features a circular buffer of states with **Cubic Hermite Spline** interpolation. It supports **Causal Decimation**: if the coordinate speed of light drops, the history is decimated (removing every other item) to increase temporal reach without increasing memory footprint.
* **Manifold:** Acts as the "Global Geometer." It provides `solve_retardation()` using **Brent’s Method** to find the light-cone intersection between worldlines.
* **Connection:** A sophisticated "Translator" class. It resolves the total motor $M$ mapping a parent's local frame to a leaf's local frame using the composition:
    $$M = (\tilde{M}_{leaf\_world}) \times M_{transport} \times M_{parent\_world}$$
* **Field Evaluation:** Connections map the leaf's pose and coupling into the parent's local frame, evaluate the field, and transport the resulting bivector acceleration back to the leaf.

---

## 6. Implementation Notes for Engineers
1.  **Memory Management:** `Worldline` provides a view of a particle's history. This structure has a fixed memory usage to better enable scaling.
2.  **Performance:** Hierarchical world-poses are resolved via left-multiplication starting from the leaf-parent up to the root to minimize redundant computations.
3.  **Numerical Stability:** Brent's method is chosen for retardation due to its robustness against the "twists" in worldlines caused by high acceleration or variable light speeds.
