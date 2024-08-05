#pragma once
#include <any>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

#include <typeinfo>
#include <cxxabi.h>
#include <memory>

#include "cpplox/interpreter.h"
#include "cpplox/runtime_error.h"
#include "cpplox/environment.h"
#include "cpplox/lox.h"

namespace CppLox
{
  // Function to demangle type names
  std::string demangle(const char *name)
  {
    int status = -1;
    std::unique_ptr<char[], void (*)(void *)> res{
        abi::__cxa_demangle(name, nullptr, nullptr, &status),
        std::free};
    return (status == 0) ? res.get() : name;
  }

  // Function to print type of std::any
  void printType(const std::any &a)
  {
    const std::type_info &typeInfo = a.type();
    std::cout << "Type: " << demangle(typeInfo.name()) << std::endl;
  }

  template <typename T>
  void printUniquePtrType(const std::unique_ptr<T> &ptr)
  {
    std::cout << "Type: " << demangle(typeid(*ptr).name()) << std::endl;
  }

  std::any Interpreter::visitBinaryExpr(const Binary *expr)
  {
    std::any left = evaluate(*expr->left);
    std::any right = evaluate(*expr->right);

    switch (expr->op.type)
    {
    case TokenType::GREATER:
      checkNumberOperands(expr->op, left, right);
      return std::any(std::any_cast<double>(left) > std::any_cast<double>(right));
    case TokenType::GREATER_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return std::any(std::any_cast<double>(left) >= std::any_cast<double>(right));
    case TokenType::LESS:
      checkNumberOperands(expr->op, left, right);
      return std::any(std::any_cast<double>(left) < std::any_cast<double>(right));
    case TokenType::LESS_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return std::any(std::any_cast<double>(left) <= std::any_cast<double>(right));
    case TokenType::MINUS:
      checkNumberOperands(expr->op, left, right);
      return std::any(std::any_cast<double>(left) - std::any_cast<double>(right));
    case TokenType::SLASH:
      checkNumberOperands(expr->op, left, right);
      return std::any(std::any_cast<double>(left) / std::any_cast<double>(right));
    case TokenType::STAR:
      checkNumberOperands(expr->op, left, right);
      return std::any(std::any_cast<double>(left) * std::any_cast<double>(right));
    case TokenType::PLUS:
    {
      auto &leftType = left.type();
      auto &rightType = right.type();

      if ((leftType == typeid(int) || leftType == typeid(double)) && ((right.type() == typeid(int) || rightType == typeid(double))))
        return std::any(std::any_cast<double>(left) + std::any_cast<double>(right));
      if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
        return std::any(std::any_cast<std::string>(left) + std::any_cast<std::string>(right));
      break;
    }
    case TokenType::BANG_EQUAL:
      return std::any(isEqual(left, right));
    case TokenType::EQUAL_EQUAL:
      return std::any(!isEqual(left, right));
    }
    throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
  }

  std::any Interpreter::visitGroupingExpr(const Grouping *expr)
  {
    return evaluate(*expr->expression);
  }

  std::any Interpreter::visitLiteralExpr(const Literal *expr)
  {
    return toAny(expr->value);
  }

  std::any Interpreter::visitUnaryExpr(const Unary *expr)
  {
    std::any right = evaluate(*expr->right);

    switch (expr->op.type)
    {
    case TokenType::BANG:
      return std::any(!isTruthy(right));
    case TokenType::MINUS:
      checkNumberOperand(expr->op, right);
      return std::any(-std::any_cast<double>(right));
    }
    // Unreachable.
    std::cout << "Unreachable code reached" << std::endl;
    return std::any();
  }

  std::any Interpreter::evaluate(Expr &expr)
  {
    return expr.accept(*this);
  }

  bool Interpreter::isTruthy(const std::any &value)
  {
    if (value.type() == typeid(std::nullptr_t))
      return false;
    if (value.type() == typeid(bool))
      return std::any_cast<bool>(value);
    return true;
  }

  bool Interpreter::isEqual(const std::any &left, const std::any &right)
  {
    if (left.type() != right.type())
      return false;
    if (left.type() == typeid(std::nullptr_t))
      return true;
    if (left.type() == typeid(double))
      return std::any_cast<double>(left) == std::any_cast<double>(right);
    if (left.type() == typeid(bool))
      return std::any_cast<bool>(left) == std::any_cast<bool>(right);
    if (left.type() == typeid(std::string))
      return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
    if (left.type() == typeid(int))
      return std::any_cast<int>(left) == std::any_cast<int>(right);
    return false;
  }

  void Interpreter::checkNumberOperand(const Token &op, const std::any &operand)
  {
    if (operand.type() == typeid(double))
      return;
    throw RuntimeError(op, "Operand must be a number.");
  }

  void Interpreter::checkNumberOperands(const Token &op, const std::any &left, const std::any &right)
  {
    if (left.type() == typeid(double) && right.type() == typeid(double))
      return;
    throw RuntimeError(op, "Both Operands must be a number.");
  }

  void Interpreter::interpret(std::vector<StmtPtr> &stmts)
  {
    try
    {
      for (auto &stmt : stmts)
      {
        execute(*stmt.get());
      }
    }
    catch (const RuntimeError &error)
    {
      lox::runtimeError(error);
    }
  }

  std::string Interpreter::stringify(std::any &obj)
  {
    if (obj.type() == typeid(std::nullptr_t))
      return "nil";
    if (obj.type() == typeid(double))
      return std::to_string(std::any_cast<double>(obj));
    if (obj.type() == typeid(bool))
      return std::any_cast<bool>(obj) ? "true" : "false";
    if (obj.type() == typeid(std::string))
      return std::any_cast<std::string>(obj);
    return "Unknown type";
  }

  std::any Interpreter::visitExpressionStmt(const Expression *stmt)
  {
    evaluate(*stmt->expression);
    return std::any();
  }

  std::any Interpreter::visitPrintStmt(const Print *stmt)
  {
    std::any value = evaluate(*stmt->expression);
    std::cout << stringify(value) << std::endl;
    return std::any();
  }

  void Interpreter::execute(const Stmt &stmt)
  {
    stmt.accept(*this);
  }

  std::any Interpreter::visitVarStmt(const Var *stmt)
  {
    std::any value = std::any();
    if (stmt->initializer)
    {
      value = evaluate(*stmt->initializer.get());
    }
    environment->define(stmt->name.lexeme, value);
    return std::any();
  }

  std::any Interpreter::visitVariableExpr(const Variable *expr)
  {
    return environment->get(expr->name);
  }

  std::any Interpreter::visitAssignExpr(const Assign *expr)
  {
    std::any value = evaluate(*expr->value.get());
    environment->assign(expr->name, value);
    return value;
  }

  std::any Interpreter::visitBlockStmt(const Block *stmt)
  {
    std::shared_ptr<Environment> new_env = std::make_shared<Environment>(this->environment);
    executeBlock(stmt->statements, new_env);
    return std::any();
  }

  void Interpreter::executeBlock(const std::vector<StmtPtr> &stmts, std::shared_ptr<Environment> new_env)
  {
    InterpreterBlockManager blockManager(*this, new_env);
    try
    {
      for (const auto &stmt : stmts)
      {
        execute(*stmt);
      }
    }
    catch (const RuntimeError &error)
    {
      lox::runtimeError(error);
    }
  }

  std::any Interpreter::visitIfStmt(const If *stmt)
  {
    if (isTruthy(evaluate(*stmt->condition)))
    {
      execute(*stmt->thenBranch);
    }
    else if (stmt->elseBranch != nullptr)
    {
      execute(*stmt->elseBranch);
    }
    return std::any();
  }

  std::any Interpreter::visitLogicalExpr(const Logical *expr)
  {
    std::any left = evaluate(*expr->left);
    if (expr->op.type == TokenType::OR)
    {
      if (isTruthy(left))
        return left;
    }
    else
    {
      if (!isTruthy(left))
        return left;
    }

    return evaluate(*expr->right);
  }

  std::any Interpreter::visitWhileStmt(const While *stmt)
  {
    while (isTruthy(evaluate(*stmt->condition)))
    {
      execute(*stmt->body);
    }
    return std::any();
  }
};