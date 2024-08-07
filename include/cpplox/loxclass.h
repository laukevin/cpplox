#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "loxcallable.h"

namespace CppLox
{
  class Interpreter;
  class LoxFunction;

  class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass>
  {
  public:
    explicit LoxClass(const std::string &name, std::shared_ptr<LoxClass> superclass, std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods) : name(name), superclass(superclass), methods(std::move(methods)) {}
    std::string toString();
    int arity() const override;
    std::any call(std::shared_ptr<Interpreter> interpreter, const std::vector<std::any> &arguments) override;
    std::shared_ptr<LoxFunction> findMethod(const std::string &name) const;
    std::shared_ptr<LoxFunction> findMethod(const std::string &name);

  private:
    const std::string name;
    std::shared_ptr<LoxClass> superclass;
    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;
  };
}