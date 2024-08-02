#ifndef CPPLOX_CPPLOX_H
#define CPPLOX_CPPLOX_H

#include <any>
#include <string>
#include <vector>
#include "token.h"
#include "runtime_error.h"

namespace CppLox
{
  class lox
  {
  public:
    static void error(int line, const std::string &message);
    static void error(Token token, const std::string &message);
    static void report(int line, const std::string &where, const std::string &message);
    static void runtimeError(const RuntimeError &error);
  };
}

#endif