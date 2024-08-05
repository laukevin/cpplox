#pragma once
#include <any>
#include <unordered_map>
#include <iostream>

#include "token.h"
#include "runtime_error.h"

namespace CppLox
{
  class Environment
  {
  public:
    Environment() : enclosing(nullptr) {}
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

    std::any get(const Token &name)
    {
      if (values.find(name.lexeme) != values.end())
      {
        return values[name.lexeme];
      }

      if (enclosing != nullptr)
      {
        return enclosing->get(name);
      }
      throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }
    void define(std::string name, std::any value)
    {
      values[name] = value;
    }

    void assign(const Token &name, std::any value)
    {
      if (values.find(name.lexeme) != values.end())
      {
        values[name.lexeme] = value;
        return;
      }

      if (enclosing != nullptr)
      {
        enclosing->assign(name, value);
        return;
      }

      throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

  private:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, std::any> values;
  };
}