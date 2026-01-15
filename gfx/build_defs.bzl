"""
This file defines custom Bazel build rules for the gfx package.
It includes stage-specific glsl_library rules that compile GLSL
shader files into SPIR-V bytecode using glslangValidator.
"""

def _glsl_library_impl(ctx, stage):
    """Implementation function for glsl_library rule.
    `stage` should be one of: 'compute', 'vertex', 'fragment', 'geometry', 'tesscontrol', 'tesseval'
    """
    outputs = []
    for src in ctx.files.srcs:
        # Compute output filename: replace .glsl with .spv
        out_name = src.basename.replace(".glsl", ".spv")
        out_file = ctx.actions.declare_file(out_name)
        outputs.append(out_file)

        # Generate SPIR-V using glslangValidator with explicit stage
        ctx.actions.run(
            inputs = [src],
            outputs = [out_file],
            executable = "glslangValidator",
            arguments = [
                "-V",  # Vulkan target
                "-S",
                stage,  # Shader stage (compute, vertex, fragment, etc.)
                src.path,
                "-o",
                out_file.path,
            ],
            use_default_shell_env = True,
        )

    return DefaultInfo(files = depset(outputs))

# Stage-specific wrapper rules
def compute_shader_library(name, srcs, deps = []):
    return rule(
        implementation = lambda ctx: _glsl_library_impl(ctx, "compute"),
        attrs = {
            "srcs": attr.label_list(allow_files = [".glsl"]),
            "deps": attr.label_list(),
        },
        name = name,
        executable = False,
    )(srcs = srcs, deps = deps)

def vertex_shader_library(name, srcs, deps = []):
    return rule(
        implementation = lambda ctx: _glsl_library_impl(ctx, "vertex"),
        attrs = {
            "srcs": attr.label_list(allow_files = [".glsl"]),
            "deps": attr.label_list(),
        },
        name = name,
        executable = False,
    )(srcs = srcs, deps = deps)

def fragment_shader_library(name, srcs, deps = []):
    return rule(
        implementation = lambda ctx: _glsl_library_impl(ctx, "fragment"),
        attrs = {
            "srcs": attr.label_list(allow_files = [".glsl"]),
            "deps": attr.label_list(),
        },
        name = name,
        executable = False,
    )(srcs = srcs, deps = deps)
