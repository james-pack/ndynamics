"""
This file defines custom rules to compile GLSL
shader files into SPIR-V bytecode using glslangValidator.
"""

def _glsl_library_impl(ctx, stage):
    """Implementation function for shader library rules.
    `stage` should be one of: 'comp', 'vert', 'frag', 'geom', 'tesc', 'tese'
    """
    outputs = []
    for src in ctx.files.srcs:
        out_name = src.basename.replace(".glsl", ".spv")
        out_file = ctx.actions.declare_file(out_name)
        outputs.append(out_file)

        ctx.actions.run(
            inputs = [src],
            outputs = [out_file],
            executable = "glslangValidator",
            arguments = [
                "-V",
                "-S",
                stage,
                src.path,
                "--target-env",
                "vulkan1.2",
                "-o",
                out_file.path,
            ],
            use_default_shell_env = True,
        )

    return DefaultInfo(files = depset(outputs))

# Stage-specific wrapper rules
compute_shader_library = rule(
    implementation = lambda ctx: _glsl_library_impl(ctx, "comp"),
    attrs = {
        "srcs": attr.label_list(allow_files = [".glsl", ".comp"]),
        "deps": attr.label_list(),
    },
    executable = False,
)

vertex_shader_library = rule(
    implementation = lambda ctx: _glsl_library_impl(ctx, "vert"),
    attrs = {
        "srcs": attr.label_list(allow_files = [".glsl", ".vert"]),
        "deps": attr.label_list(),
    },
    executable = False,
)

fragment_shader_library = rule(
    implementation = lambda ctx: _glsl_library_impl(ctx, "frag"),
    attrs = {
        "srcs": attr.label_list(allow_files = [".glsl", ".frag"]),
        "deps": attr.label_list(),
    },
    executable = False,
)
