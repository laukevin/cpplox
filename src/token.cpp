#include <any>
#include <string>
#include <vector>
#include <variant>

#include <iostream>
#include "cpplox/token.h"
#include "cpplox/tokentype.h"

namespace CppLox
{
  std::string Token::literalString() const
  {
    return std::visit([](const auto &arg) -> std::string
                      {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>)
            return "null";
        else if constexpr (std::is_same_v<T, int>)
            return std::to_string(arg);
        else if constexpr (std::is_same_v<T, double>)
            return std::to_string(arg);
        else if constexpr (std::is_same_v<T, std::string>)
            return arg;
        else
            return "[unknown type]"; }, literal);
  }

  std::string Token::toString() const
  {
    return tokenTypeToString(type) + " " + lexeme + " " + literalString();
  }
}