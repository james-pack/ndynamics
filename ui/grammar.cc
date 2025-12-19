#include "ui/grammar.h"

#include "peglib.h"

namespace ndyn::ui {

peg::parser create_parser() {
  peg::parser parser{R"GRAMMAR(
Line <- _ (Statement _)?

Statement <- Assignment
           / Expression
           / Command

Assignment <- Identifier _ "=" _ Expression

Identifier <- !Keyword [a-zA-Z_][a-zA-Z0-9_]*

Keyword <- Command / "algebra" / "metric"

Expression <- Additive

Additive <- Multiplicative ( _ ( AddOp / SubOp ) _ Additive )?
AddOp <- "+"
SubOp <- "-"

Multiplicative <- Unary ( _ ( MultOp / DivOp / OuterOp / InnerOp ) _ Multiplicative )?
MultOp <- "*"
DivOp <- "/"
OuterOp <- "^"
InnerOp <- "|"

Unary <- [+-]? Primary

Primary <- Scalar
         / RValue
         / "(" _ Expression _ ")"

RValue <- Identifier

Scalar <- [+-]? [0-9]+ ("." [0-9]+)? ([eE] [+-]? [0-9]+)?

Command <- DictCommand / ExitCommand / HelpCommand
DictCommand <- "dict" ( _ ("-l" / "--long") )?
ExitCommand <- "exit"
HelpCommand <- "help"

_ <- [ \t]*
)GRAMMAR"};

  return parser;
}

}  // namespace ndyn::ui
