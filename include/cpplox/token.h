#pragma once

#include <any>
#include <string>
#include <variant>
#include <utility>

#include "tokentype.h"

namespace CppLox
{
  using LiteralType = std::variant<std::nullptr_t, int, double, std::string>;

  struct ToStringVisitor
  {
    std::string operator()(std::nullptr_t) const
    {
      return "nullptr";
    }
    std::string operator()(int value) const
    {
      return std::to_string(value);
    }
    std::string operator()(double value) const
    {
      return std::to_string(value);
    }
    std::string operator()(const std::string &value) const
    {
      return value;
    }
  };

  inline std::string literal_to_string(const LiteralType &var)
  {
    return std::visit(ToStringVisitor(), var);
  }

  class Token
  {
  public:
    Token(TokenType type, std::string &&lexeme, LiteralType &&literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

    std::string toString() const;
    const TokenType type;
    const LiteralType literal;
    const std::string lexeme;
    const int line;

  private:
    std::string literalString() const;
  };
} // namespace CppLox