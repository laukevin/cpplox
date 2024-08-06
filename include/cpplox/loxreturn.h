#pragma once

#include <stdexcept>
#include "token.h"

namespace CppLox
{
  class LoxReturn : public std::runtime_error
  {
  public:
    LoxReturn(std::any value) : std::runtime_error("Return Exception"), value(value) {}
    std::any value;
  };
}