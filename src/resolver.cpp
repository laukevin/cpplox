#pragma once

#include <any>
#include "cpplox/resolver.h"
#include "cpplox/interpreter.h"
#include "cpplox/lox.h"

namespace CppLox
{
  void Resolver::beginScope()
  {
    scopes.push_back(Scope());
  }

  void Resolver::endScope()
  {
    if (!scopes.empty())
    {
      scopes.pop_back();
    }
  }

  Scope &Resolver::topScope()
  {
    if (scopes.empty())
    {
      throw std::out_of_range("No scopes available");
    }
    return scopes.back();
  }

  Scope &Resolver::getScopeByIndex(int index)
  {
    if (index >= scopes.size())
    {
      throw std::out_of_range("Index out of range");
    }
    return scopes[index];
  }

  std::any Resolver::visitBlockStmt(const Block *stmt)
  {
    beginScope();
    resolve(stmt->statements);
    endScope();
    return std::any();
  }

  void Resolver::resolve(const StmtPtr &stmt)
  {
    stmt->accept(*this);
  }

  void Resolver::resolve(const ExprPtr &expr)
  {
    expr->accept(*this);
  }

  void Resolver::resolve(const std::vector<StmtPtr> &stmts)
  {
    for (const auto &stmt : stmts)
    {
      resolve(stmt);
    }
  }

  std::any Resolver::visitVarStmt(const Var *stmt)
  {
    declare(stmt->name);
    if (stmt->initializer)
    {
      resolve(stmt->initializer);
    }
    define(stmt->name);
    return std::any();
  }

  void Resolver::declare(const Token &name)
  {
    if (scopes.empty())
    {
      return;
    }
    Scope &scope = topScope();

    if (scope.find(name.lexeme) != scope.end())
    {
      lox::error(name, "Variable with this name already declared in this scope.");
    }

    scope[name.lexeme] = false;
  }

  void Resolver::define(const Token &name)
  {
    if (scopes.empty())
    {
      return;
    }
    Scope &scope = topScope();
    scope[name.lexeme] = true;
  }

  std::any Resolver::visitVariableExpr(const Variable *expr)
  {
    if (!scopes.empty())
    {
      Scope &top = topScope();
      if (top.find(expr->name.lexeme) != top.end() && !top[expr->name.lexeme])
      {
        lox::error(expr->name, "Cannot read local variable in its own initializer.");
      }
    }
    resolveLocal(expr, expr->name);
    return std::any();
  }

  void Resolver::resolveLocal(const Expr *expr, const Token &name)
  {
    for (int i = scopes.size() - 1; i >= 0; i--)
    {
      Scope &scope = getScopeByIndex(i);
      if (scope.find(name.lexeme) != scope.end())
      {
        interpreter->resolve(expr, scopes.size() - 1 - i);
        return;
      }
    }
  }

  std::any Resolver::visitAssignExpr(const Assign *expr)
  {
    resolve(expr->value);
    resolveLocal(expr, expr->name);
    return std::any();
  }

  std::any Resolver::visitFunctionStmt(const Function *stmt)
  {
    declare(stmt->name);
    define(stmt->name);
    resolveFunction(stmt, FunctionType::FUNCTION);
    return std::any();
  }

  void Resolver::resolveFunction(const Function *function, FunctionType type)
  {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;
    beginScope();
    for (const auto &param : function->params)
    {
      declare(param);
      define(param);
    }
    resolve(function->body);
    endScope();
    currentFunction = enclosingFunction;
  }

  std::any Resolver::visitExpressionStmt(const Expression *stmt)
  {
    resolve(stmt->expression);
    return std::any();
  }

  std::any Resolver::visitIfStmt(const If *stmt)
  {
    resolve(stmt->condition);
    resolve(stmt->thenBranch);
    if (stmt->elseBranch)
    {
      resolve(stmt->elseBranch);
    }
    return std::any();
  }

  std::any Resolver::visitPrintStmt(const Print *stmt)
  {
    resolve(stmt->expression);
    return std::any();
  }

  std::any Resolver::visitReturnStmt(const Return *stmt)
  {
    if (currentFunction == FunctionType::NONE)
    {
      lox::error(stmt->keyword, "Cannot return from top-level code.");
    }
    if (!scopes.empty())
    {
      if (stmt->value != nullptr)
      {
        if (currentFunction == FunctionType::INITIALIZER)
        {
          lox::error(stmt->keyword, "Cannot return a value from an initializer.");
        }
        resolve(stmt->value);
      }
    }
    return std::any();
  }

  std::any Resolver::visitWhileStmt(const While *stmt)
  {
    resolve(stmt->condition);
    resolve(stmt->body);
    return std::any();
  }

  std::any Resolver::visitBinaryExpr(const Binary *expr)
  {
    resolve(expr->left);
    resolve(expr->right);
    return std::any();
  }

  std::any Resolver::visitCallExpr(const Call *expr)
  {
    Variable *varPtr = dynamic_cast<Variable *>(expr->callee.get());

    resolve(expr->callee);
    for (const auto &argument : expr->arguments)
    {
      resolve(argument);
    }
    return std::any();
  }

  std::any Resolver::visitGroupingExpr(const Grouping *expr)
  {
    resolve(expr->expression);
    return std::any();
  }

  std::any Resolver::visitLiteralExpr(const Literal *expr)
  {
    return std::any();
  }

  std::any Resolver::visitLogicalExpr(const Logical *expr)
  {
    resolve(expr->left);
    resolve(expr->right);
    return std::any();
  }

  std::any Resolver::visitUnaryExpr(const Unary *expr)
  {
    resolve(expr->right);
    return std::any();
  }

  template <typename T>
  void Resolver::printUniquePtrType(const std::unique_ptr<T> &ptr)
  {
    std::cout << "Type: " << lox::demangle(typeid(*ptr).name()) << std::endl;
  }

  std::any Resolver::visitClassStmt(const Class *stmt)
  {
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::CLASS;
    declare(stmt->name);
    define(stmt->name);

    if (stmt->superclass != nullptr)
    {
      auto *superclass = dynamic_cast<Variable *>(stmt->superclass.get());
      if (superclass->name.lexeme == stmt->name.lexeme)
      {
        lox::error(superclass->name, "A class cannot inherit from itself.");
      }
    }

    if (stmt->superclass != nullptr)
    {
      resolve(stmt->superclass);
    }

    beginScope();
    topScope()["this"] = true;

    for (const auto &method : stmt->methods)
    {
      FunctionType declaration = FunctionType::METHOD;
      Function *methodFn = dynamic_cast<Function *>(method.get());
      if (methodFn->name.lexeme == "init")
      {
        declaration = FunctionType::INITIALIZER;
      }
      resolveFunction(methodFn, declaration);
    }
    endScope();
    currentClass = enclosingClass;

    return std::any();
  }

  std::any Resolver::visitGetExpr(const Get *expr)
  {
    resolve(expr->object);
    return std::any();
  }

  std::any Resolver::visitSetExpr(const Set *expr)
  {
    resolve(expr->value);
    resolve(expr->object);
    return std::any();
  }

  std::any Resolver::visitThisExpr(const This *expr)
  {
    if (currentClass == ClassType::NONE)
    {
      lox::error(expr->keyword, "Cannot use 'this' outside of a class.");
      return std::any();
    }
    resolveLocal(expr, expr->keyword);
    return std::any();
  }
}