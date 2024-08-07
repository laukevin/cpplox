#pragma once

#include <string>
#include <vector>
#include <memory>

#include "cpplox/loxclass.h"
#include "cpplox/loxfunction.h"
#include "cpplox/loxinstance.h"
#include "cpplox/interpreter.h"

namespace CppLox
{
  std::any LoxClass::call(std::shared_ptr<Interpreter> interpreter, const std::vector<std::any> &arguments)
  {
    std::shared_ptr<LoxInstance> instance = std::make_shared<LoxInstance>(shared_from_this());
    std::shared_ptr<LoxFunction> initializer = findMethod("init");
    if (initializer != nullptr)
    {
      auto boundinitializer = std::any_cast<std::shared_ptr<LoxFunction>>(initializer->bind(instance));
      boundinitializer->call(interpreter, arguments);
    }

    return instance;
  }

  std::string LoxClass::toString()
  {
    return name;
  }

  std::shared_ptr<LoxFunction> LoxClass::findMethod(const std::string &name)
  {
    if (methods.find(name) != methods.end())
    {
      return methods[name];
    }

    if (superclass != nullptr)
    {
      return superclass->findMethod(name);
    }

    return nullptr;
  }

  std::shared_ptr<LoxFunction> LoxClass::findMethod(const std::string &name) const
  {
    auto it = methods.find(name);
    if (it != methods.end())
    {
      return it->second;
    }

    if (superclass != nullptr)
    {
      return superclass->findMethod(name);
    }

    return nullptr;
  }

  int LoxClass::arity() const
  {
    auto initializer = findMethod("init");
    if (initializer == nullptr)
    {
      return 0;
    }
    return initializer->arity();
  }
}