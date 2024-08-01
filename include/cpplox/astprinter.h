#pragma once
#include <any>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

#include "expr.h"

namespace CppLox
{
  class AstPrinter : public ExprVisitor<std::any>
  {
  public:
    std::any print(Expr &expr)
    {
      return expr.accept(*this);
    }

    std::string parenthesize(const std::string &name, const std::vector<std::reference_wrapper<Expr>> exprs)
    {
      std::ostringstream oss;

      oss << "(" << name;
      for (auto &expr : exprs)
      {
        // Implementation of parenthesize
        oss << " ";

        try
        {
          std::string exprStr = std::any_cast<std::string>(expr.get().accept(*this));
          oss << exprStr;
        }
        catch (const std::bad_any_cast &e)
        {
          std::cout << "Bad any cast: " << e.what() << std::endl;
        }
      }
      oss << ")";
      return oss.str();
    }

    std::any visitBinaryExpr(const Binary *expr) override
    {
      // Implementation of visitBinaryExpr
      return std::any(
          parenthesize(expr->op.lexeme, {*expr->left, *expr->right}));
    }

    std::any visitGroupingExpr(const Grouping *expr) override
    {
      // Implementation of visitGroupingExpr
      return std::any(
          parenthesize("group", {*expr->expression}));
    }

    std::any visitLiteralExpr(const Literal *expr) override
    {
      // Implementation of visitLiteralExpr
      return std::any(literal_to_string(expr->value));
    }

    std::any visitUnaryExpr(const Unary *expr) override
    {
      // Implementation of visitUnaryExpr
      return std::any(parenthesize(expr->op.lexeme, {*expr->right}));
    }
  };
}