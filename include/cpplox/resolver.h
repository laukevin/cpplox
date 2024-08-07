#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "expr.h"
#include "stmt.h"
#include "interpreter.h"

namespace CppLox
{
  enum class FunctionType
  {
    NONE,
    FUNCTION,
    INITIALIZER,
    METHOD
  };

  enum class ClassType
  {
    NONE,
    CLASS,
    SUBCLASS
  };

  using Scope = std::unordered_map<std::string, bool>;

  class Resolver : public ExprVisitor<std::any>, public StmtVisitor<std::any>
  {
    std::shared_ptr<Interpreter> interpreter;
    std::vector<Scope> scopes;
    FunctionType currentFunction = FunctionType::NONE;
    ClassType currentClass = ClassType::NONE;

  public:
    Resolver(std::shared_ptr<Interpreter> interpreter) : interpreter(interpreter) {}
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
    std::any visitCallExpr(const Call *expr) override;
    std::any visitFunctionStmt(const Function *stmt) override;
    std::any visitReturnStmt(const Return *stmt) override;
    std::any visitClassStmt(const Class *stmt) override;
    std::any visitGetExpr(const Get *expr) override;
    std::any visitSetExpr(const Set *expr) override;
    std::any visitThisExpr(const This *expr) override;
    void resolve(const std::vector<StmtPtr> &stmts);

  private:
    void resolve(const StmtPtr &stmt);
    void resolve(const ExprPtr &expr);
    void resolveLocal(const Expr *expr, const Token &name);
    void resolveFunction(const Function *function, FunctionType type);
    void beginScope();
    void endScope();
    Scope &topScope();
    Scope &getScopeByIndex(int index);
    void declare(const Token &name);
    void define(const Token &name);

    template <typename T>
    static void printUniquePtrType(const std::unique_ptr<T> &ptr);
  };
}