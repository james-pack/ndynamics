#include "calculator/parser.h"

#include <iomanip>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include "base/except.h"
#include "glog/logging.h"
#include "peglib.h"
#include "utf8proc.h"

namespace ndyn::ui {

const char grammar_definition[] = R"GRAMMAR(
Line <- _ Command _ / _ Statement _ / _

Statement <- Assignment / Expression

Assignment <- Identifier _ "=" _ Expression

Identifier <- !Keyword IdentifierStart IdentifierContinue*
IdentifierStart    <- UppercaseLetter
                    / LowercaseLetter
                    / TitlecaseLetter
                    / ModifierLetter
                    / OtherLetter
                    / LetterNumber
                    / CurrencySymbol
                    / OtherSymbol
                    / [_]

IdentifierContinue <- IdentifierStart
                    / DecimalDigit
                    / OtherNumber
                    / NonSpacingMark
                    / SpacingCombiningMark
                    / EnclosingMark
                    / ModifierSymbol
                    / MathSymbol
                    / ConnectorPunctuation

Keyword <- "dict" / "help" / "quit" / "exit" / "algebra" / "metric"

Expression <- Term ( _ ( AddOp / SubOp ) _ Expression )?
AddOp <- "+"
SubOp <- "-"

Term <- Factor ( _ ( MultOp / DivOp / OuterOp / LeftContractOp / RightContractOp ) _ Term )?
MultOp <- "*"
DivOp <- "/"
OuterOp <- "^"
LeftContractOp <- "<<"
RightContractOp <- ">>"

Factor <- (( UnaryMinus / UnaryPlus / DualOp / ReverseOp ) _ )? Primary
UnaryMinus <- "-"
UnaryPlus <- "+"
DualOp <- "!"
ReverseOp <- "~"

Primary <- Scalar / RValue / Parenthetical

RValue <- Identifier

Scalar <- [+-]? [0-9]+ ("." [0-9]+)?

Parenthetical <- "(" _ Expression _ ")"

Command <- DictCommand / ExitCommand / HelpCommand
DictCommand <- "dict"
ExitCommand <- "exit" / "quit"
HelpCommand <- "help" / "?"

~_ <- [ \t]*


# ── Identifier-start categories ─────────────────────────────────────────────

# Lu — Uppercase Letter
UppercaseLetter            <- [\u0041-\u005A] / [\u00C0-\u00D6] / [\u00D8-\u00DE]
                            / [\u0100-\u024E] / [\u0370-\u0556]
                            / [\u10A0-\u10CD] / [\u1E00-\u1FFC]
                            / [\u2102-\u2183] / [\u2C00-\u2CF2]
                            / [\uFF21-\uFF3A]

# Ll — Lowercase Letter
LowercaseLetter            <- [\u0061-\u007A] / [\u00DF-\u00F6] / [\u00F8-\u00FF]
                            / [\u0101-\u024F] / [\u0371-\u052F]
                            / [\u10D0-\u10FF] / [\u1E01-\u1EFF]
                            / [\u2C30-\u2C7B] / [\uAB30-\uABBF]
                            / [\uFF41-\uFF5A]

# Lt — Titlecase Letter
TitlecaseLetter            <- [\u01C5\u01C8\u01CB\u01F2]
                            / [\u1F88-\u1F8F] / [\u1F98-\u1F9F]
                            / [\u1FA8-\u1FAF]
                            / [\u1FBC\u1FCC\u1FFC]

# Lm — Modifier Letter
ModifierLetter             <- [\u02B0-\u02D1] / [\u02E0-\u02EE]
                            / [\u0640\u0E46\u0EC6]
                            / [\u1D2C-\u1DBF]
                            / [\u2090-\u209C]
                            / [\u3005\u3031-\u3035\u303B\u309D-\u309E\u30FC-\u30FE]
                            / [\uA4F8-\uA4FD]
                            / [\uA717-\uA71F]
                            / [\uFF70\uFF9E-\uFF9F]

# Lo — Other Letter  (CJK block alone is ~28,000 codepoints)
OtherLetter                <- [\u00AA\u00BA]
                            / [\u0600-\u06FF]
                            / [\u0900-\u0D7F]
                            / [\u1000-\u109F]
                            / [\u1100-\u11FF]
                            / [\u3041-\u30FF]
                            / [\u3400-\u4DBF]
                            / [\u4E00-\u9FFF]
                            / [\uAC00-\uD7A3]
                            / [\uF900-\uFAFF]

# Nl — Letter Number
LetterNumber               <- [\u16EE-\u16F0]
                            / [\u2160-\u2188]
                            / [\u3007\u3021-\u3029\u3038-\u303C]
                            / [\uA6E6-\uA6EF]

# Sc — Currency Symbol
CurrencySymbol             <- [\u0024\u00A2-\u00A5]
                            / [\u058F\u060B\u07FE-\u07FF]
                            / [\u09F2-\u09F3\u09FB]
                            / [\u0AF1\u0BF9\u0E3F]
                            / [\u17DB\u20A0-\u20CF]
                            / [\uA838\uFDFC\uFE69\uFF04]
                            / [\uFFE0-\uFFE1\uFFE5-\uFFE6]

# So — Other Symbol  (extremely scattered)
OtherSymbol                <- [\u00A6-\u00A7\u00A9\u00AE\u00B0]
                            / [\u0482\u058D-\u058E\u060E-\u060F]
                            / [\u2100-\u214F]
                            / [\u2300-\u23FF]
                            / [\u2600-\u26FF]
                            / [\u2700-\u27BF]
                            / [\u1F300-\u1F9FF]

# Pc — Connector Punctuation
ConnectorPunctuation       <- [\u203F-\u2040]
                            / [\u2054]
                            / [\uFE33-\uFE34]
                            / [\uFE4D-\uFE4F]
                            / [\uFF3F]

# Sm — Math Symbol (coarse approximation)
MathSymbol                 <- [\u03F6]
                            / [\u0606-\u0608]
                            / [\u207A-\u207E]
                            / [\u208A-\u208E]
                            / [\u2044\u2052]
                            / [\u2200-\u22FF]
                            / [\u2320-\u2321]
                            / [\u2A00-\u2AFF]

# ── Identifier-continue-only categories ─────────────────────────────────────

# Nd — Decimal Digit Number
DecimalDigit               <- [\u0030-\u0039] / [\u0660-\u0669] / [\u06F0-\u06F9]
                            / [\u07C0-\u07C9] / [\u0966-\u096F] / [\u09E6-\u09EF]
                            / [\u0A66-\u0A6F] / [\u0AE6-\u0AEF] / [\u0B66-\u0B6F]
                            / [\u0BE6-\u0BEF]

# No — Other Number
OtherNumber                <- [\u00B2-\u00B3\u00B9]
                            / [\u00BC-\u00BE]
                            / [\u09F4-\u09F9]
                            / [\u0B72-\u0B77]
                            / [\u2070\u2074-\u2079]
                            / [\u2080-\u2089]
                            / [\u2150-\u215F]
                            / [\u2460-\u249B]
                            / [\u24EA-\u24FF]
                            / [\u2776-\u2793]
                            / [\u3220-\u3247]

# Mn — Non-Spacing Mark
NonSpacingMark             <- [\u0300-\u036F] / [\u0610-\u061A]
                            / [\u064B-\u065F] / [\u0670\u06D6-\u06DC]
                            / [\u0730-\u074A] / [\u0900-\u0902]
                            / [\u0941-\u0948\u094D]
                            / [\u20D0-\u20FF]
                            / [\uFE20-\uFE2F]

# Mc — Spacing Combining Mark
SpacingCombiningMark       <- [\u0903\u093B\u093E-\u0940]
                            / [\u0949-\u094C\u094E-\u094F]
                            / [\u0982-\u0983\u09BE-\u09C0]
                            / [\u09C7-\u09C8\u09CB-\u09CC]
                            / [\u0A03\u0A3E-\u0A40]
                            / [\u0A83\u0ABE-\u0AC0\u0AC9]
                            / [\u0ACB-\u0ACC\u0B02-\u0B03]
                            / [\u0B3E\u0B40\u0B47-\u0B48]
                            / [\u0B4B-\u0B4C\u0BBE-\u0BBF]
                            / [\u0BC1-\u0BC2\u0BC6-\u0BC8]

# Me — Enclosing Mark  (small and stable — good approximation)
EnclosingMark              <- [\u0488-\u0489]
                            / [\u1ABE]
                            / [\u20DD-\u20E0]
                            / [\u20E2-\u20E4]
                            / [\uA670-\uA672]

# Sk — Modifier Symbol
ModifierSymbol             <- [\u00A8\u00AF\u00B4\u00B8]
                            / [\u02C2-\u02C5\u02D2-\u02DF\u02E5-\u02EB\u02ED]
                            / [\u02EF-\u02FF]
                            / [\u0375\u0384-\u0385\u1FBD\u1FBF-\u1FC1]
                            / [\u1FCD-\u1FCF\u1FDD-\u1FDF\u1FED-\u1FEF\u1FFD-\u1FFE]
                            / [\u309B-\u309C]
                            / [\uA700-\uA716\uA720-\uA721\uA789-\uA78A]
                            / [\uAB5B\uAB6A-\uAB6B]
                            / [\uFBB2-\uFBC2]

)GRAMMAR";

std::string to_string(UnaryOp op) {
  switch (op) {
    case UnaryOp::Plus:
      return "+";
    case UnaryOp::Minus:
      return "-";
    case UnaryOp::Dual:
      return "!";
    case UnaryOp::Reverse:
      return "~";
  }
  return "<unknown-unary-op>";
}

std::string to_string(BinaryOp op) {
  switch (op) {
    case BinaryOp::Add:
      return "+";
    case BinaryOp::Sub:
      return "-";
    case BinaryOp::Mult:
      return "*";
    case BinaryOp::Div:
      return "/";
    case BinaryOp::Outer:
      return "^";
    case BinaryOp::LeftContract:
      return "<<";
    case BinaryOp::RightContract:
      return ">>";
  }
  return "<unknown-binary-op>";
}

namespace {

// The correction table applied before NFC normalization. Each entry maps a source codepoint that is
// either visually indistinguishable from, or trivially confused with, a canonical mathematical
// codepoint.  Rejections are entries whose target is 0 — these codepoints are illegal in
// identifiers regardless of position because they are confusable with grammar operators.
struct CodepointCorrection final {
  utf8proc_int32_t source;
  utf8proc_int32_t target;  // 0 == reject unconditionally
};

constexpr CodepointCorrection CORRECTION_TABLE[] = {
    {0x00B5, 0x03BC},  // µ MICRO SIGN         → μ GREEK SMALL LETTER MU
    {0x025B, 0x03B5},  // ɛ LATIN SMALL LETTER OPEN E → ε GREEK SMALL LETTER EPSILON
    {0x00B7, 0x22C5},  // · MIDDLE DOT         → ⋅ DOT OPERATOR
    {0x0387, 0x22C5},  // · GREEK ANO TELEIA   → ⋅ DOT OPERATOR
    {0x2212, 0x0000},  // − MINUS SIGN         → reject (confusable with SubOp '-')
    {0x2010, 0x0000},  // ‐ HYPHEN             → reject (confusable with SubOp '-')
    {0x2011, 0x0000},  // ‑ NON-BREAKING HYPHEN → reject
    {0x2012, 0x0000},  // ‒ FIGURE DASH        → reject
    {0x2013, 0x0000},  // – EN DASH            → reject
    {0x2014, 0x0000},  // — EM DASH            → reject
    {0x2015, 0x0000},  // ― HORIZONTAL BAR     → reject
};

// Returns the corrected codepoint, or 0 if the codepoint is unconditionally
// rejected.  Returns the input unchanged if it has no correction entry.
utf8proc_int32_t ApplyCorrection(const utf8proc_int32_t cp) {
  for (const auto& entry : CORRECTION_TABLE) {
    if (entry.source == cp) {
      return entry.target;
    }
  }
  return cp;
}

// Returns true if the codepoint is in the extended math symbol set that we
// permit in identifier-continue position beyond what TR31 XID_Continue allows.
// These ranges are chosen to support subscript/superscript notation, primes,
// combining marks, and mathematical alphanumeric variants.
/*constexpr*/ bool IsExtendedMathContinue(const utf8proc_int32_t cp) {
  DLOG(INFO) << "IsExtendedMathContinue() -- cp: " << std::hex << cp;
  return (cp >= 0x0300 && cp <= 0x036F)       // Combining diacritical marks
         || (cp >= 0x2032 && cp <= 0x2037)    // Prime family ′ ″ ‴ ⁗
         || (cp >= 0x2070 && cp <= 0x2079)    // Superscript digits ⁰¹²…⁹
         || (cp >= 0x207A && cp <= 0x207E)    // Superscript operators ⁺⁻⁼⁽⁾
         || (cp >= 0x2080 && cp <= 0x2089)    // Subscript digits ₀₁₂…₉
         || (cp >= 0x208A && cp <= 0x208E)    // Subscript operators ₊₋₌₍₎
         || (cp >= 0x1D400 && cp <= 0x1D7FF)  // Mathematical alphanumeric symbols
         || cp == 0x0305;                     // Combining overline
}

/**
 * Decodes raw UTF-8 bytes into a codepoint sequence, applies the correction
 * table to each codepoint, then re-encodes as UTF-8 and applies NFC
 * normalization.  The result is the canonical form used for all subsequent
 * keyword comparison and identifier interning.
 *
 * Throws std::runtime_error if the input is not valid UTF-8 or contains a rejected
 * codepoint (i.e. one that maps to 0 in the correction table).
 */
std::string NormalizeIdentifier(const std::string_view raw) {
  // Decode → correct → re-encode into a temporary buffer, then NFC-normalize.
  std::string corrected{};
  corrected.reserve(raw.size());

  const auto* src = reinterpret_cast<const utf8proc_uint8_t*>(raw.data());
  utf8proc_ssize_t remaining = static_cast<utf8proc_ssize_t>(raw.size());

  while (remaining > 0) {
    utf8proc_int32_t cp{};
    const utf8proc_ssize_t consumed = utf8proc_iterate(src, remaining, &cp);
    if (consumed <= 0 || cp < 0) {
      except<std::runtime_error>("Identifier contains invalid UTF-8");
    }

    const utf8proc_int32_t corrected_cp = ApplyCorrection(cp);
    if (corrected_cp == 0) {
      except<std::runtime_error>("Identifier contains a character that is not permitted");
    }

    utf8proc_uint8_t encoded[4]{};
    const utf8proc_ssize_t len = utf8proc_encode_char(corrected_cp, encoded);
    corrected.append(reinterpret_cast<const char*>(encoded), static_cast<size_t>(len));

    src += consumed;
    remaining -= consumed;
  }

  // NFC normalization runs on the corrected codepoints.  utf8proc_NFC
  // allocates; we take ownership immediately into a unique_ptr.
  utf8proc_uint8_t* nfc_raw =
      utf8proc_NFC(reinterpret_cast<const utf8proc_uint8_t*>(corrected.data()));
  if (nfc_raw == nullptr) {
    except<std::runtime_error>("NFC normalization failed for identifier");
  }
  std::unique_ptr<utf8proc_uint8_t, decltype(&free)> nfc_owned{nfc_raw, &free};
  return std::string{reinterpret_cast<const char*>(nfc_owned.get())};
}

}  // namespace

void attach_actions(peg::parser& p, std::shared_ptr<LineAst>& result) {
  p["Scalar"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Scalar] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[Scalar] -- sv.token_to_number<float>(): " << sv.token_to_number<float>();
    return std::make_shared<ScalarAst>(sv.token_to_number<float>());
  };

  p["Identifier"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<IdentifierAst> {
    DLOG(INFO) << "[Identifier] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[Identifier] -- sv.token(): " << sv.token();
    return std::make_shared<IdentifierAst>(NormalizeIdentifier(sv.token()));
  };

  p["UnicodeStart"] = [](const peg::SemanticValues& sv) {
    DLOG(INFO) << "[UnicodeStart] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[UnicodeStart] -- sv.token(): " << sv.token();

    const auto* src = reinterpret_cast<const utf8proc_uint8_t*>(sv.token().data());
    utf8proc_ssize_t remaining = static_cast<utf8proc_ssize_t>(sv.token().size());

    while (remaining > 0) {
      utf8proc_int32_t cp{};
      const utf8proc_ssize_t consumed = utf8proc_iterate(src, remaining, &cp);
      if (consumed <= 0 || cp < 0) {
        except<std::runtime_error>("Identifier start contains invalid UTF-8");
      }

      // utf8proc_get_property returns a pointer to a static property struct;
      // the category field encodes the Unicode general category.
      const utf8proc_property_t* prop = utf8proc_get_property(cp);
      const bool is_xid_start =
          (prop->category == UTF8PROC_CATEGORY_LU)      // Uppercase letter
          || (prop->category == UTF8PROC_CATEGORY_LL)   // Lowercase letter
          || (prop->category == UTF8PROC_CATEGORY_LT)   // Titlecase letter
          || (prop->category == UTF8PROC_CATEGORY_LM)   // Modifier letter
          || (prop->category == UTF8PROC_CATEGORY_LO)   // Other letter
          || (prop->category == UTF8PROC_CATEGORY_NL)   // Letter number (e.g. ℂ, ℤ)
          || (prop->category == UTF8PROC_CATEGORY_PC);  // Connector punctuation (e.g. ‿)

      if (!is_xid_start) {
        except<std::runtime_error>("Identifier start character does not have XID_Start property");
      }

      src += consumed;
      remaining -= consumed;
    }
  };

  p["UnicodeContinue"] = [](const peg::SemanticValues& sv) {
    DLOG(INFO) << "[UnicodeContinue] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[UnicodeContinue] -- sv.token(): " << sv.token();

    const auto* src = reinterpret_cast<const utf8proc_uint8_t*>(sv.token().data());
    utf8proc_ssize_t remaining = static_cast<utf8proc_ssize_t>(sv.token().size());

    while (remaining > 0) {
      utf8proc_int32_t cp{};
      const utf8proc_ssize_t consumed = utf8proc_iterate(src, remaining, &cp);
      if (consumed <= 0 || cp < 0) {
        DLOG(INFO) << "Identifier continue has invalid UTF-8.";
        except<std::runtime_error>("Identifier continue contains invalid UTF-8");
      }

      const utf8proc_property_t* prop = utf8proc_get_property(cp);

      // XID_Continue is a superset of XID_Start, adding decimal digits,
      // non-spacing marks, spacing combining marks, and format characters.
      const bool is_xid_continue =
          (prop->category == UTF8PROC_CATEGORY_LU) || (prop->category == UTF8PROC_CATEGORY_LL) ||
          (prop->category == UTF8PROC_CATEGORY_LT) || (prop->category == UTF8PROC_CATEGORY_LM) ||
          (prop->category == UTF8PROC_CATEGORY_LO) || (prop->category == UTF8PROC_CATEGORY_NL) ||
          (prop->category == UTF8PROC_CATEGORY_PC) ||
          (prop->category == UTF8PROC_CATEGORY_ND)      // Decimal digit
          || (prop->category == UTF8PROC_CATEGORY_MN)   // Non-spacing mark
          || (prop->category == UTF8PROC_CATEGORY_MC)   // Spacing combining mark
          || (prop->category == UTF8PROC_CATEGORY_CF);  // Format character

      if (!is_xid_continue && !IsExtendedMathContinue(cp)) {
        DLOG(INFO) << "Identifier continue character rejected.";
        except<std::runtime_error>(
            "Identifier continue character does not have XID_Continue property");
      }

      src += consumed;
      remaining -= consumed;
    }
  };

  p["RValue"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[RValue] -- sv.size(): " << sv.size();
    auto identifier = std::any_cast<std::shared_ptr<IdentifierAst>>(sv[0]);
    return std::make_shared<RvalueAst>(identifier);
  };

  p["Parenthetical"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Parenthetical] -- sv.size(): " << sv.size();
    return std::make_shared<ParentheticalAst>(std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]));
  };

  p["UnaryMinus"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<UnaryOpAst> {
    DLOG(INFO) << "[UnaryMinus] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[UnaryMinus] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<UnaryOpAst>(UnaryOp::Minus);
  };

  p["UnaryPlus"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<UnaryOpAst> {
    DLOG(INFO) << "[UnaryPlus] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[UnaryPlus] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<UnaryOpAst>(UnaryOp::Plus);
  };

  p["DualOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<UnaryOpAst> {
    DLOG(INFO) << "[DualOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[DualOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<UnaryOpAst>(UnaryOp::Dual);
  };

  p["ReverseOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<UnaryOpAst> {
    DLOG(INFO) << "[ReverseOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[ReverseOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<UnaryOpAst>(UnaryOp::Reverse);
  };

  p["Factor"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Factor] -- sv.size(): " << sv.size();
    if (sv.size() == 1) {
      return std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]);
    }
    auto op = std::any_cast<std::shared_ptr<UnaryOpAst>>(sv[0]);
    auto operand = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[1]);
    return std::make_shared<UnaryAst>(op, operand);
  };

  p["MultOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[MultOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[MultOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Mult);
  };

  p["DivOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[DivOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[DivOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Div);
  };

  p["AddOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[AddOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[AddOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Add);
  };

  p["SubOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[SubOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[SubOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Sub);
  };

  p["LeftContractOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[LeftContractOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[LeftContractOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::LeftContract);
  };

  p["RightContractOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[RightContractOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[RightContractOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::RightContract);
  };

  p["OuterOp"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<BinaryOpAst> {
    DLOG(INFO) << "[OuterOp] -- sv.size(): " << sv.size();
    DLOG(INFO) << "[OuterOp] -- sv.token: '" << sv.token() << "'";
    return std::make_shared<BinaryOpAst>(BinaryOp::Outer);
  };

  p["Term"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Term] -- sv.size(): " << sv.size();
    auto lhs = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]);
    if (sv.size() == 1) {
      return lhs;
    }
    auto op = std::any_cast<std::shared_ptr<BinaryOpAst>>(sv[1]);
    auto rhs = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[2]);
    rhs->ingest_terms(lhs, op, rhs);
    return rhs;
  };

  p["Expression"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<ExpressionAst> {
    DLOG(INFO) << "[Expression] -- sv.size(): " << sv.size();
    auto lhs = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]);
    if (sv.size() == 1) {
      return lhs;
    }
    auto op = std::any_cast<std::shared_ptr<BinaryOpAst>>(sv[1]);
    auto rhs = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[2]);
    rhs->ingest_terms(lhs, op, rhs);
    return rhs;
  };

  p["Assignment"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<AssignmentAst> {
    DLOG(INFO) << "[Assignment] -- sv.size(): " << sv.size();
    auto name = std::any_cast<std::shared_ptr<IdentifierAst>>(sv[0]);
    auto value = std::any_cast<std::shared_ptr<ExpressionAst>>(sv[1]);
    return std::make_shared<AssignmentAst>(name, value);
  };

  p["DictCommand"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<CommandAst> {
    DLOG(INFO) << "[DictCommand] -- sv.size(): " << sv.size() << ", sv.choice(): " << sv.choice();
    return std::make_shared<DictCommandAst>();
  };

  p["ExitCommand"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<CommandAst> {
    DLOG(INFO) << "[ExitCommand] -- sv.size(): " << sv.size();
    return std::make_shared<ExitCommandAst>();
  };

  p["HelpCommand"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<CommandAst> {
    DLOG(INFO) << "[HelpCommand] -- sv.size(): " << sv.size();
    return std::make_shared<HelpCommandAst>();
  };

  p["Command"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<CommandAst> {
    DLOG(INFO) << "[Command] -- sv.size(): " << sv.size();
    return std::any_cast<std::shared_ptr<CommandAst>>(sv[0]);
  };

  p["Statement"] = [](const peg::SemanticValues& sv) -> std::shared_ptr<StatementAst> {
    DLOG(INFO) << "[Statement] -- sv.size(): " << sv.size() << ", sv.choice: " << sv.choice();
    switch (sv.choice()) {
      case 0:
        return std::make_shared<StatementAst>(std::any_cast<std::shared_ptr<AssignmentAst>>(sv[0]));
      case 1:
        return std::make_shared<StatementAst>(std::any_cast<std::shared_ptr<ExpressionAst>>(sv[0]));
      default:
        except<std::logic_error>("Unknown statement type");
    }
    return nullptr;
  };

  p["Line"] = [&result](const peg::SemanticValues& sv) -> std::shared_ptr<LineAst> {
    DLOG(INFO) << "[Line] -- sv.size(): " << sv.size() << ", sv.choice: " << sv.choice();
    switch (sv.choice()) {
      case 0: {
        std::shared_ptr<CommandAst> cmd = std::any_cast<std::shared_ptr<CommandAst>>(sv[0]);
        result = std::make_shared<LineAst>(cmd);
        break;
      }
      case 1: {
        std::shared_ptr<StatementAst> stmt = std::any_cast<std::shared_ptr<StatementAst>>(sv[0]);
        result = std::make_shared<LineAst>(stmt);
        break;
      }
      default:
        result = std::make_shared<LineAst>();
    }
    return result;
  };
}

peg::parser create_raw_parser(std::shared_ptr<LineAst>& result) {
  peg::parser parser{grammar_definition};
  if (parser) {
    attach_actions(parser, result);

    return parser;
  } else {
    except<std::logic_error>("Could not create parser. Error in grammar definition.");
  }
}

void Visitor::visit(ExpressionAst&) { /* Do nothing by default but can be overridden. */ }
void Visitor::visit(CommandAst&) { /* Do nothing by default but can be overridden. */ }
void Visitor::visit(StatementAst&) { /* Do nothing by default but can be overridden. */ }
void Visitor::visit(UnaryOpAst&) { /* Do nothing by default but can be overridden. */ }
void Visitor::visit(BinaryOpAst&) { /* Do nothing by default but can be overridden. */ }

void ScalarAst::visit(Visitor& v) { v.visit(*this); }
void IdentifierAst::visit(Visitor& v) { v.visit(*this); }
void ParentheticalAst::visit(Visitor& v) { v.visit(*this); }
void RvalueAst::visit(Visitor& v) { v.visit(*this); }
void UnaryAst::visit(Visitor& v) { v.visit(*this); }
void BinaryAst::visit(Visitor& v) { v.visit(*this); }

void UnaryOpAst::visit(Visitor& v) { v.visit(*this); }
void BinaryOpAst::visit(Visitor& v) { v.visit(*this); }

void DictCommandAst::visit(Visitor& v) { v.visit(*this); }
void ExitCommandAst::visit(Visitor& v) { v.visit(*this); }
void HelpCommandAst::visit(Visitor& v) { v.visit(*this); }

void StatementAst::visit(Visitor& v) { v.visit(*this); }
void AssignmentAst::visit(Visitor& v) { v.visit(*this); }

void LineAst::visit(Visitor& v) { v.visit(*this); }

void ExpressionAst::ingest_terms(std::shared_ptr<ExpressionAst> lhs,
                                 std::shared_ptr<BinaryOpAst> op,
                                 std::shared_ptr<ExpressionAst>& holder) {
  assert(holder.get() == this);
  holder = std::make_shared<BinaryAst>(op, lhs, std::shared_ptr<ExpressionAst>(holder));
}

void BinaryAst::ingest_terms(std::shared_ptr<ExpressionAst> lhs, std::shared_ptr<BinaryOpAst> op,
                             std::shared_ptr<ExpressionAst>& holder) {
  assert(holder.get() == this);
  if (op->op == BinaryOp::Div) {
    this->lhs = std::make_shared<BinaryAst>(op, lhs, this->lhs);
  } else if (op->op == BinaryOp::Sub &&
             (this->op->op == BinaryOp::Add || this->op->op == BinaryOp::Sub)) {
    this->lhs = std::make_shared<BinaryAst>(op, lhs, this->lhs);
  } else {
    holder = std::make_shared<BinaryAst>(op, lhs, std::shared_ptr<ExpressionAst>(holder));
  }
}

}  // namespace ndyn::ui
