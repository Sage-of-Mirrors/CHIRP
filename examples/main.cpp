#include "libchirp-text/ChirpTextLexer.hpp"
#include "libchirp-text/ChirpTextParser.hpp"
#include "libchirp-text/ChirpTextSchema.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <variant>

static void printValue(const ChirpTextValue& value);

static void printValue(const ChirpTextValue& value) {
  std::visit(
      [](const auto& v) {
        using T = std::decay_t< decltype(v) >;
        if constexpr (std::is_same_v< T, std::nullptr_t >)
          std::cout << "null";
        else if constexpr (std::is_same_v< T, bool >)
          std::cout << (v ? "true" : "false");
        else if constexpr (std::is_same_v< T, std::string >)
          std::cout << '"' << v << '"';
        else if constexpr (std::is_same_v< T, ChirpTextValue::ChirpTextTuple >) {
          std::cout << '(';
          for (std::size_t i = 0; i < v.size(); ++i) {
            if (i)
              std::cout << ", ";
            printValue(v[i]);
          }
          std::cout << ')';
        } else {
          std::cout << v;
        }
      },
      value.mData);
}

int main() {
  const char* text = R"(
chirp_model

@game
{
    collision_layer = 4
    dynamic = true
    mass = 12.5
    tags = ("enemy", "flying")
}

%vertices%
count 1
%{ (0, 0, 0, 0), (0, 0, 0, 0), (0, 0), (255, 255, 255, 255) }%
)";

  ChirpTextDiagnosticBag diagnostics;
  ChirpTextLexer lexer(text, "example.chirm");
  auto tokens = lexer.tokenize(diagnostics);
  if (diagnostics.hasErrors()) {
    for (const auto& d : diagnostics.all()) {
      std::cerr << d.mSpan.mBegin.mFile << ':' << d.mSpan.mBegin.mLine << ':'
                << d.mSpan.mBegin.mColumn << ": " << d.message << '\n';
    }
    return 1;
  }

  ChirpTextParser parser(std::move(tokens));
  ChirpTextDocument document = parser.parse(diagnostics);

  ChirpModelSchema schema;
  schema.validate(document, diagnostics);

  for (const auto& d : diagnostics.all()) {
    std::cerr << d.mSpan.mBegin.mFile << ':' << d.mSpan.mBegin.mLine << ':'
              << d.mSpan.mBegin.mColumn << ": " << d.message << '\n';
  }

  for (const auto& ns : document.mUserData) {
    std::cout << '@' << ns.mNamespaceName << '\n';
    for (const auto& field : ns.mFields) {
      std::cout << "  " << field.mName << " = ";
      printValue(field.mValue);
      std::cout << '\n';
    }
  }

  return diagnostics.hasErrors() ? 1 : 0;
}
