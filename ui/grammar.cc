#include "ui/grammar.h"

#include "peglib.h"

namespace ndyn::ui {

peg::parser create_parser() {
  peg::parser parser{R"GRAMMAR(
# REPL INPUT
REPLLine <- _ (Statement _)?

# STATEMENTS
Statement <- AlgebraDeclaration
           / Assignment
           / Expression
           / Command

#  ALGEBRA CONFIGURATION 
AlgebraDeclaration <- "algebra" _ "=" _ AlgebraSpec

AlgebraSpec <- BasisList (_ ";" _ MetricSpec)?  # optional metric

#  BASIS LIST 
BasisList <- BasisMetric ( _ "," _ BasisMetric )*

BasisMetric <- BasisName (_ ":" _ SimpleMetric)?   # optional simple metric

BasisName <- [a-zA-Z]+[a-zA-Z0-9]*

SimpleMetric <- ("1" / "-1" / "0")

#  METRIC SPECIFICATION 
MetricSpec <- "metric" _ "=" _ MetricMatrix

MetricMatrix <- "[" _ MatrixRows _ "]"

MatrixRows <- "[" _ NumberList _ "]" ( _ "," _ "[" _ NumberList _ "]" )*

NumberList <- Number ( _ "," _ Number )*

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
         / MultivectorLiteral
         / Identifier
         / "(" _ Expression _ ")"

#  NUMBER (supports scientific notation) 
Number <- [+-]? [0-9]+ ("." [0-9]+)? ([eE] [+-]? [0-9]+)?

#  MULTIVECTOR LITERAL 
MultivectorLiteral <- "<" _ BladeList _ ">"

BladeList <- Blade ( _ "," _ Blade )*

Blade <- Number? _ BasisName+

#  COMMANDS 
Command <- "exit" / "help"

#  WHITESPACE 
_ <- [ \t\n\r]*
)GRAMMAR"};

  return parser;
}

}  // namespace ndyn::ui
