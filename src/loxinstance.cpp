#pragma once

#include "cpplox/loxinstance.h"
#include "cpplox/loxfunction.h"
#include "cpplox/token.h"
#include "cpplox/loxclass.h"
#include "cpplox/runtime_error.h"

namespace CppLox
{
  std::string LoxInstance::toString()
  {
    return klass->toString() + " instance";
  }

  std::any LoxInstance::get(const Token &name)
  {
    if (fields.find(name.lexeme) != fields.end())
    {
      return fields[name.lexeme];
    }

    auto method = klass->findMethod(name.lexeme);
    if (method != nullptr)
    {
      return method->bind(shared_from_this());
    }

    throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
  }

  std::any LoxInstance::set(const Token &name, std::any value)
  {
    fields[name.lexeme] = value;
    return value;
  }
}