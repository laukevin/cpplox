#pragma once
#include <any>
#include <vector>
#include <chrono>

namespace CppLox
{
  class Interpreter;

  class LoxCallable
  {
  public:
    virtual int arity() const = 0;
    virtual std::any call(std::shared_ptr<Interpreter> interpreter, const std::vector<std::any> &arguments) = 0;
  };

  class ClockCallable : public LoxCallable
  {
  public:
    int arity() const override
    {
      return 0;
    }

    std::any call(std::shared_ptr<Interpreter> interpreter, const std::vector<std::any> &arguments) override
    {
      auto now = std::chrono::system_clock::now();
      auto now_time_t = std::chrono::system_clock::to_time_t(now);
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
      auto milliseconds = duration.count();
      return milliseconds / 1000.0;
    }

    std::string toString() const
    {
      return "<native fn>";
    }
  };
} // namespace CppLox