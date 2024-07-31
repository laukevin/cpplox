
#pragma once
#include <memory>
#include <utility>

#include "token.h"

namespace CppLox
{

  class Binary;
  class Grouping;
  class Literal;
  class Unary;

  // Base Visitor interface
  class BaseExprVisitor
  {
  public:
    virtual void visitBinaryExpr(const Binary &expr) = 0;
    virtual void visitGroupingExpr(const Grouping &expr) = 0;
    virtual void visitLiteralExpr(const Literal &expr) = 0;
    virtual void visitUnaryExpr(const Unary &expr) = 0;
  };

  // Template Visitor interface
  template <typename R>
  class ExprVisitor : public BaseExprVisitor
  {
  public:
    virtual void visitBinaryExpr(const Binary &expr) = 0;
    virtual void visitGroupingExpr(const Grouping &expr) = 0;
    virtual void visitLiteralExpr(const Literal &expr) = 0;
    virtual void visitUnaryExpr(const Unary &expr) = 0;
  };

  // Base Expr class
  class Expr
  {
  public:
    virtual ~Expr() = default;

    virtual void accept(BaseExprVisitor &visitor) const = 0;
  };

  using ExprPtr = std::unique_ptr<Expr>;

  class Binary : public Expr
  {
  public:
    Binary(ExprPtr left, Token op, ExprPtr right) : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

    void accept(BaseExprVisitor &visitor) const override
    {
      visitor.visitBinaryExpr(*this);
    }

    ExprPtr left;
    Token op;
    ExprPtr right;
  };

  using BinaryPtr = std::unique_ptr<Binary>;

  class Grouping : public Expr
  {
  public:
    Grouping(ExprPtr expression) : expression(std::move(expression)) {}

    void accept(BaseExprVisitor &visitor) const override
    {
      visitor.visitGroupingExpr(*this);
    }

    ExprPtr expression;
  };

  using GroupingPtr = std::unique_ptr<Grouping>;

  class Literal : public Expr
  {
  public:
    Literal(LiteralType value) : value(std::move(value)) {}

    void accept(BaseExprVisitor &visitor) const override
    {
      visitor.visitLiteralExpr(*this);
    }

    LiteralType value;
  };

  using LiteralPtr = std::unique_ptr<Literal>;

  class Unary : public Expr
  {
  public:
    Unary(Token op, ExprPtr right) : op(std::move(op)), right(std::move(right)) {}

    void accept(BaseExprVisitor &visitor) const override
    {
      visitor.visitUnaryExpr(*this);
    }

    Token op;
    ExprPtr right;
  };

  using UnaryPtr = std::unique_ptr<Unary>;

}
