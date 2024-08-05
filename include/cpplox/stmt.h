
#pragma once
#include <memory>
#include <utility>
#include <any>

#include "token.h"
#include "stmt.h"

using namespace std;

namespace CppLox
{

  class Block;
  class Expression;
  class Print;
  class Var;

  class BaseStmtVisitor
  {
  public:
    virtual ~BaseStmtVisitor() = default;
  };

  template <typename R>
  class StmtVisitor : public BaseStmtVisitor
  {
  public:
    virtual R visitBlockStmt(const Block *stmt) = 0;
    virtual R visitExpressionStmt(const Expression *stmt) = 0;
    virtual R visitPrintStmt(const Print *stmt) = 0;
    virtual R visitVarStmt(const Var *stmt) = 0;
  };

  class Stmt
  {
  public:
    virtual ~Stmt() = default;
    virtual std::any accept(BaseStmtVisitor &visitor) const = 0;
  };

  using StmtPtr = std::unique_ptr<Stmt>;

  struct Block : public Stmt
  {

    Block(vector<StmtPtr> statements) : statements(std::move(statements)) {}

    std::any accept(BaseStmtVisitor &visitor) const override
    {
      auto visitor_ptr = dynamic_cast<StmtVisitor<std::any> *>(&visitor);
      if (visitor_ptr)
        return std::any(visitor_ptr->visitBlockStmt(this));
      return std::any();
    }

    vector<StmtPtr> statements;
  };

  using BlockPtr = std::unique_ptr<Block>;

  struct Expression : public Stmt
  {

    Expression(ExprPtr expression) : expression(std::move(expression)) {}

    std::any accept(BaseStmtVisitor &visitor) const override
    {
      auto visitor_ptr = dynamic_cast<StmtVisitor<std::any> *>(&visitor);
      if (visitor_ptr)
        return std::any(visitor_ptr->visitExpressionStmt(this));
      return std::any();
    }

    ExprPtr expression;
  };

  using ExpressionPtr = std::unique_ptr<Expression>;

  struct Print : public Stmt
  {

    Print(ExprPtr expression) : expression(std::move(expression)) {}

    std::any accept(BaseStmtVisitor &visitor) const override
    {
      auto visitor_ptr = dynamic_cast<StmtVisitor<std::any> *>(&visitor);
      if (visitor_ptr)
        return std::any(visitor_ptr->visitPrintStmt(this));
      return std::any();
    }

    ExprPtr expression;
  };

  using PrintPtr = std::unique_ptr<Print>;

  struct Var : public Stmt
  {

    Var(Token name, ExprPtr initializer) : name(std::move(name)), initializer(std::move(initializer)) {}

    std::any accept(BaseStmtVisitor &visitor) const override
    {
      auto visitor_ptr = dynamic_cast<StmtVisitor<std::any> *>(&visitor);
      if (visitor_ptr)
        return std::any(visitor_ptr->visitVarStmt(this));
      return std::any();
    }

    Token name;
    ExprPtr initializer;
  };

  using VarPtr = std::unique_ptr<Var>;

}
