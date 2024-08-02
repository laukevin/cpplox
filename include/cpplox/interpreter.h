#pragma once
#include <any>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

#include "expr.h"

namespace CppLox
{
  class Interpreter : public ExprVisitor<std::any>
  {
  public:
    std::any visitBinaryExpr(const Binary *expr) override;
    std::any visitGroupingExpr(const Grouping *expr) override;
    std::any visitLiteralExpr(const Literal *expr) override;
    std::any visitUnaryExpr(const Unary *expr) override;
    void interpret(Expr &expr);

  private:
    std::any evaluate(Expr &expr);
    bool isTruthy(const std::any &value);
    bool isEqual(const std::any &left, const std::any &right);
    void checkNumberOperand(const Token &op, const std::any &operand);
    void checkNumberOperands(const Token &op, const std::any &left, const std::any &right);
    std::string stringify(std::any &obj);
  };
}