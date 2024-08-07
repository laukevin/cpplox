#pragma once
#include <any>
#include <vector>
#include "interpreter.h"
#include "environment.h"
#include "loxcallable.h"
#include "loxinstance.h"
#include "loxreturn.h"
#include "stmt.h"

namespace CppLox
{
  class LoxFunction : public LoxCallable
  {
  public:
    LoxFunction(const Function *declaration, std::shared_ptr<Environment> enclosing, bool isInitializer) : declaration(declaration), enclosing(enclosing), isInitializer(isInitializer) {}
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
        if (isInitializer)
        {
          return enclosing->getAt(0, "this");
        }
        return returnValue.value;
      }

      if (isInitializer)
      {
        return enclosing->getAt(0, "this");
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

    std::any bind(std::shared_ptr<LoxInstance> instance)
    {
      std::shared_ptr<Environment> environment = std::make_shared<Environment>(enclosing);
      environment->define("this", instance);
      return std::any(std::make_shared<LoxFunction>(declaration, environment, isInitializer));
    }

  private:
    const Function *declaration;
    std::shared_ptr<Environment> enclosing;
    bool isInitializer;
  };
} // namespace CppLox