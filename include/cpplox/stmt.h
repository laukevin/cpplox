
#pragma once
#include <memory>
#include <utility>
#include <any>

#include "token.h"
#include "expr.h"

using namespace std;

namespace CppLox
{

  class Block;
  class Class;
  class Expression;
  class Print;
  class Return;
  class Var;
  class Function;
  class If;
  class While;

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
    virtual R visitClassStmt(const Class *stmt) = 0;
    virtual R visitExpressionStmt(const Expression *stmt) = 0;
    virtual R visitPrintStmt(const Print *stmt) = 0;
    virtual R visitReturnStmt(const Return *stmt) = 0;
    virtual R visitVarStmt(const Var *stmt) = 0;
    virtual R visitFunctionStmt(const Function *stmt) = 0;
    virtual R visitIfStmt(const If *stmt) = 0;
    virtual R visitWhileStmt(const While *stmt) = 0;
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

  struct Class : public Stmt
  {

    Class(Token name, ExprPtr superclass, vector<StmtPtr> methods) : name(std::move(name)), superclass(std::move(superclass)), methods(std::move(methods)) {}

    std::any accept(BaseStmtVisitor &visitor) const override
    {
      auto visitor_ptr = dynamic_cast<StmtVisitor<std::any> *>(&visitor);
      if (visitor_ptr)
        return std::any(visitor_ptr->visitClassStmt(this));
      return std::any();
    }

    Token name;
    ExprPtr superclass;
    vector<StmtPtr> methods;
  };

  using ClassPtr = std::unique_ptr<Class>;

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

  struct Return : public Stmt
  {

    Return(Token keyword, ExprPtr value) : keyword(std::move(keyword)), value(std::move(value)) {}

    std::any accept(BaseStmtVisitor &visitor) const override
    {
      auto visitor_ptr = dynamic_cast<StmtVisitor<std::any> *>(&visitor);
      if (visitor_ptr)
        return std::any(visitor_ptr->visitReturnStmt(this));
      return std::any();
    }

    Token keyword;
    ExprPtr value;
  };

  using ReturnPtr = std::unique_ptr<Return>;

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

  struct Function : public Stmt
  {

    Function(Token name, vector<Token> params, vector<StmtPtr> body) : name(std::move(name)), params(std::move(params)), body(std::move(body)) {}

    std::any accept(BaseStmtVisitor &visitor) const override
    {
      auto visitor_ptr = dynamic_cast<StmtVisitor<std::any> *>(&visitor);
      if (visitor_ptr)
        return std::any(visitor_ptr->visitFunctionStmt(this));
      return std::any();
    }

    Token name;
    vector<Token> params;
    vector<StmtPtr> body;
  };

  using FunctionPtr = std::unique_ptr<Function>;

  struct If : public Stmt
  {

    If(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch) : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    std::any accept(BaseStmtVisitor &visitor) const override
    {
      auto visitor_ptr = dynamic_cast<StmtVisitor<std::any> *>(&visitor);
      if (visitor_ptr)
        return std::any(visitor_ptr->visitIfStmt(this));
      return std::any();
    }

    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;
  };

  using IfPtr = std::unique_ptr<If>;

  struct While : public Stmt
  {

    While(ExprPtr condition, StmtPtr body) : condition(std::move(condition)), body(std::move(body)) {}

    std::any accept(BaseStmtVisitor &visitor) const override
    {
      auto visitor_ptr = dynamic_cast<StmtVisitor<std::any> *>(&visitor);
      if (visitor_ptr)
        return std::any(visitor_ptr->visitWhileStmt(this));
      return std::any();
    }

    ExprPtr condition;
    StmtPtr body;
  };

  using WhilePtr = std::unique_ptr<While>;

}
