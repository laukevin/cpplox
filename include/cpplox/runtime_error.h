#pragma once

#include <stdexcept>
#include "token.h"

namespace CppLox
{
  class RuntimeError : public std::runtime_error
  {
  public:
    RuntimeError(const Token &op, const std::string &message) : std::runtime_error(message), op(op) {}
    const Token &op;
  };
}