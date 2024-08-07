#pragma once
#include <any>
#include <unordered_map>
#include <iostream>

#include "token.h"
#include "runtime_error.h"

namespace CppLox
{
  class Environment : public std::enable_shared_from_this<Environment>
  {
  public:
    Environment() : enclosing(nullptr) {}
    Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}
    std::shared_ptr<Environment> enclosing;

    std::shared_ptr<Environment> ancestor(int distance)
    {
      std::shared_ptr<Environment> environment = shared_from_this();
      for (int i = 0; i < distance; i++)
      {
        environment = environment->enclosing;
      }
      return environment;
    }

    std::any getAt(int distance, const std::string &lexeme)
    {
      return ancestor(distance)->values[lexeme];
    }

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
      throw RuntimeError(name, "get - Undefined variable '" + name.lexeme + "'.");
    }
    void define(std::string name, std::any value)
    {
      values[name] = value;
    }

    void assignAt(int distance, const Token &name, std::any value)
    {
      ancestor(distance)->values[name.lexeme] = value;
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

      throw RuntimeError(name, "assign - Undefined variable '" + name.lexeme + "'.");
    }

  private:
    std::unordered_map<std::string, std::any> values;
  };
}