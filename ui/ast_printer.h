#pragma once

#include "ui/grammar.h"

namespace ndyn::ui {

// Function to print the AST starting with a LineAst node.
void print_ast(LineAst& line, int starting_indent = 0);

}  // namespace ndyn::ui
