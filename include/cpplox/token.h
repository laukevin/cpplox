#ifndef CPPLOX_TOKEN_H
#define CPPLOX_TOKEN_H

#include <any>
#include <string>
#include <variant>
#include <utility>

#include "tokentype.h"

namespace CppLox
{
  using LiteralType = std::variant<std::nullptr_t, int, double, std::string>;

  class Token
  {
  public:
    Token(TokenType type, std::string &&lexeme, LiteralType &&literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

    std::string toString() const;

  private:
    const TokenType type;
    const std::string lexeme;
    const LiteralType literal;
    const int line;

    std::string literalString() const;
  };
} // namespace CppLox

#endif // CPPLOX_TOKEN_H