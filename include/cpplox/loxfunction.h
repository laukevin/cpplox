#pragma once
#include <any>
#include <vector>
#include "interpreter.h"
#include "environment.h"
#include "loxcallable.h"
#include "loxreturn.h"
#include "stmt.h"

namespace CppLox
{
  class LoxFunction : public LoxCallable
  {
  public:
    LoxFunction(const Function *declaration, std::shared_ptr<Environment> enclosing) : declaration(declaration), enclosing(enclosing) {}
    std::any call(std::shared_ptr<Interpreter> interpreter, const std::vector<std::any> &arguments) override
    {
      std::shared_ptr<Environment> environment = std::make_shared<Environment>(enclosing);
      for (int i = 0; i < declaration->params.size(); i++)
      {
        environment->define(declaration->params[i].lexeme, arguments[i]);
      }

      try
      {
        interpreter->executeBlock(declaration->body, environment);
      }
      catch (const LoxReturn &returnValue)
      {
        return returnValue.value;
      }

      return std::any();
    }

    int arity() const override
    {
      return declaration->params.size();
    }

    std::string toString() const
    {
      return "<fn " + declaration->name.lexeme + ">";
    }

  private:
    const Function *declaration;
    std::shared_ptr<Environment> enclosing;
  };
} // namespace CppLox