#pragma once
#include <any>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

#include "expr.h"
#include "stmt.h"
#include "environment.h"

namespace CppLox
{
  class InterpreterBlockManager;

  class Interpreter : public ExprVisitor<std::any>, public StmtVisitor<std::any>
  {
  public:
    Interpreter() : environment(std::make_shared<Environment>()) {}
    std::any visitBinaryExpr(const Binary *expr) override;
    std::any visitGroupingExpr(const Grouping *expr) override;
    std::any visitLiteralExpr(const Literal *expr) override;
    std::any visitUnaryExpr(const Unary *expr) override;
    std::any visitLogicalExpr(const Logical *expr) override;
    std::any visitExpressionStmt(const Expression *stmt) override;
    std::any visitPrintStmt(const Print *stmt) override;
    std::any visitVariableExpr(const Variable *expr) override;
    std::any visitVarStmt(const Var *stmt) override;
    std::any visitAssignExpr(const Assign *expr) override;
    std::any visitBlockStmt(const Block *stmt) override;
    std::any visitIfStmt(const If *stmt) override;
    std::any visitWhileStmt(const While *stmt) override;
    void interpret(std::vector<StmtPtr> &stmts);

  protected:
    std::shared_ptr<Environment> environment;

  private:
    std::any evaluate(Expr &expr);
    bool isTruthy(const std::any &value);
    bool isEqual(const std::any &left, const std::any &right);
    void checkNumberOperand(const Token &op, const std::any &operand);
    void checkNumberOperands(const Token &op, const std::any &left, const std::any &right);
    std::string stringify(std::any &obj);
    void execute(const Stmt &stmt);
    void executeBlock(const std::vector<StmtPtr> &stmts, std::shared_ptr<Environment> environment);

    friend class InterpreterBlockManager;
  };

  class InterpreterBlockManager
  {
  public:
    InterpreterBlockManager(Interpreter &interpreter, std::shared_ptr<Environment> environment)
        : interpreter(interpreter), environment(environment), previous(interpreter.environment)
    {
      interpreter.environment = environment;
    }

    ~InterpreterBlockManager()
    {
      interpreter.environment = previous;
    }

  private:
    Interpreter &interpreter;
    std::shared_ptr<Environment> previous;
    std::shared_ptr<Environment> environment;
  };
}