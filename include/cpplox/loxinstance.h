#pragma once

#include <string>
#include <memory>
#include <any>
#include <unordered_map>

namespace CppLox
{
  class LoxClass;
  class Token;

  class LoxInstance : public std::enable_shared_from_this<LoxInstance>
  {
  public:
    LoxInstance(std::shared_ptr<LoxClass> klass) : klass(klass) {}
    std::string toString();
    std::any get(const Token &name);
    std::any set(const Token &name, std::any value);

  private:
    std::shared_ptr<LoxClass> klass;
    std::unordered_map<std::string, std::any> fields;
  };
}