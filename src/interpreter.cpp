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
#include "cpplox/loxcallable.h"
#include "cpplox/loxfunction.h"
#include "cpplox/loxreturn.h"
#include "cpplox/loxclass.h"
#include "cpplox/loxinstance.h"
#include "cpplox/runtime_error.h"
#include "cpplox/environment.h"
#include "cpplox/lox.h"

namespace CppLox
{
  class LoxClass;

  // Helper function to try casting to multiple types
  template <typename... Ts>
  std::shared_ptr<LoxCallable> try_cast(const std::any &value)
  {
    std::shared_ptr<LoxCallable> result;
    (... || [&]()
     {
        try {
            result = std::any_cast<std::shared_ptr<Ts>>(value);
            return true;
        } catch (const std::bad_any_cast&) {
            return false;
        } }());
    return result;
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
    if (obj.type() == typeid(std::shared_ptr<LoxInstance>))
      return std::any_cast<std::shared_ptr<LoxInstance>>(obj)->toString();
    if (obj.type() == typeid(std::shared_ptr<LoxClass>))
      return std::any_cast<std::shared_ptr<LoxClass>>(obj)->toString();
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
    return lookupVariable(expr->name, expr);
  }

  std::any Interpreter::visitAssignExpr(const Assign *expr)
  {
    std::any value = evaluate(*expr->value.get());

    int distance = locals.find(expr) != locals.end() ? locals[expr] : -1;
    if (distance > -1)
    {
      environment->assignAt(distance, expr->name, value);
    }
    else
    {
      globals->assign(expr->name, value);
    }
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

  std::any Interpreter::visitCallExpr(const Call *expr)
  {
    std::any callee = evaluate(*expr->callee);
    std::vector<std::any> arguments;
    for (const auto &argument : expr->arguments)
    {
      arguments.push_back(evaluate(*argument));
    }

    std::shared_ptr<LoxCallable> function = try_cast<LoxFunction, LoxClass, ClockCallable>(callee);
    if (!function)
    {
      throw RuntimeError(expr->paren, "Can only call functions and classes.");
    }

    if (arguments.size() != function->arity())
    {
      throw RuntimeError(
          expr->paren,
          "Expected " + std::to_string(function->arity()) +
              " arguments but got " + std::to_string(arguments.size()) + ".");
    }

    return function->call(shared_from_this(), std::move(arguments));
  }

  std::any Interpreter::visitFunctionStmt(const Function *stmt)
  {
    std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(stmt, environment, false);
    environment->define(stmt->name.lexeme, std::move(function));
    return std::any();
  }

  std::any Interpreter::visitReturnStmt(const Return *stmt)
  {
    std::any value;
    if (stmt->value != nullptr)
    {
      value = evaluate(*stmt->value);
    }
    throw LoxReturn(value);
  }

  void Interpreter::resolve(const Expr *expr, int depth)
  {
    locals[expr] = depth;
  }

  std::any Interpreter::lookupVariable(const Token &name, const Expr *expr)
  {
    int distance = locals.find(expr) != locals.end() ? locals[expr] : -1;
    if (distance > -1)
    {
      return environment->getAt(distance, name.lexeme);
    }
    return globals->get(name);
  }

  std::any Interpreter::visitClassStmt(const Class *stmt)
  {
    std::any superclass;
    std::shared_ptr<LoxClass> superclassPtr;
    if (stmt->superclass != nullptr)
    {
      superclass = evaluate(*stmt->superclass);
      auto loxSuperclassCast = try_cast<LoxClass>(superclass);
      if (loxSuperclassCast == nullptr)
      {
        auto superclassVar = dynamic_cast<Variable *>(stmt->superclass.get());
        throw RuntimeError(superclassVar->name, "Superclass must be a class.");
      }
      superclassPtr = std::dynamic_pointer_cast<LoxClass>(loxSuperclassCast);
    }

    environment->define(stmt->name.lexeme, std::any());

    if (stmt->superclass != nullptr)
    {
      environment = std::make_shared<Environment>(environment);
      environment->define("super", superclassPtr);
    }

    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;
    for (const auto &method : stmt->methods)
    {
      const Function *methodFn = dynamic_cast<Function *>(method.get());
      bool isInitializer = methodFn->name.lexeme == "init";
      std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(methodFn, environment, isInitializer);
      methods[methodFn->name.lexeme] = function;
    }

    std::shared_ptr<LoxClass> klass = std::make_shared<LoxClass>(stmt->name.lexeme, superclassPtr, std::move(methods));
    if (superclassPtr != nullptr)
    {
      environment = environment->enclosing;
    }

    environment->assign(stmt->name, klass);

    return std::any();
  }

  std::any Interpreter::visitGetExpr(const Get *expr)
  {
    std::any object = evaluate(*expr->object);
    if (object.type() == typeid(std::shared_ptr<LoxInstance>))
    {
      return std::any_cast<std::shared_ptr<LoxInstance>>(object)->get(expr->name);
    }
    throw RuntimeError(expr->name, "Only instances have properties.");
  }

  std::any Interpreter::visitSetExpr(const Set *expr)
  {
    std::any object = evaluate(*expr->object);
    if (object.type() != typeid(std::shared_ptr<LoxInstance>))
    {
      throw RuntimeError(expr->name, "Only instances have fields.");
    }
    std::any value = evaluate(*expr->value);
    std::shared_ptr<LoxInstance> instance = std::any_cast<std::shared_ptr<LoxInstance>>(object);
    instance->set(expr->name, value);
    return value;
  }

  std::any Interpreter::visitThisExpr(const This *expr)
  {
    return lookupVariable(expr->keyword, expr);
  }

  std::any Interpreter::visitSuperExpr(const Super *expr)
  {
    int distance = locals[expr];
    std::shared_ptr<LoxClass> superclass = std::any_cast<std::shared_ptr<LoxClass>>(environment->getAt(distance, "super"));
    std::shared_ptr<LoxInstance> object = std::any_cast<std::shared_ptr<LoxInstance>>(environment->getAt(distance - 1, "this"));
    auto method = superclass->findMethod(expr->method.lexeme);
    if (method == nullptr)
    {
      throw RuntimeError(expr->method, "Undefined property '" + expr->method.lexeme + "'.");
    }

    return method->bind(object);
  }

};