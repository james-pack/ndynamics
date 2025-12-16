#include "ui/grammar.h"

#include "peglib.h"

namespace ndyn::ui {

peg::parser create_parser() {
  peg::parser parser{R"GRAMMAR(
# REPL INPUT
REPLLine <- _ (Statement _)?

# STATEMENTS
Statement <- Assignment
           / Expression
           / Command

#  VARIABLE ASSIGNMENTS 
Assignment <- Identifier _ "=" _ Expression

Identifier <- !Keyword [a-zA-Z_][a-zA-Z0-9_]*

Keyword <- "algebra" / "metric" / "exit" / "help"

#  EXPRESSIONS 
Expression <- Additive

Additive <- Multiplicative ( _ ("+" / "-") _ Additive )?

Multiplicative <- Unary ( _ ( "*" / "^" / "|" ) _ Multiplicative )?

Unary <- [+-]? Primary

Primary <- Number
         / Identifier
         / "(" _ Expression _ ")"

#  NUMBER (supports scientific notation) 
Number <- [+-]? [0-9]+ ("." [0-9]+)? ([eE] [+-]? [0-9]+)?

#  COMMANDS 
Command <- "exit" / "help"

#  WHITESPACE 
_ <- [ \t]*
)GRAMMAR"};

  return parser;
}

}  // namespace ndyn::ui
