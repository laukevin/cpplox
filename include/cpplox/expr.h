
#pragma once
#include <memory>
#include <utility>
#include <any>

#include "token.h"

using namespace std;

namespace CppLox {

class Binary;
class Call;
class Grouping;
class Literal;
class Unary;
class Variable;
class Assign;
class Logical;



class BaseExprVisitor
{
public:
virtual ~BaseExprVisitor() = default;
};

template <typename R>
class ExprVisitor : public BaseExprVisitor
{
public:
        virtual R visitBinaryExpr(const Binary *expr) = 0;
    virtual R visitCallExpr(const Call *expr) = 0;
    virtual R visitGroupingExpr(const Grouping *expr) = 0;
    virtual R visitLiteralExpr(const Literal *expr) = 0;
    virtual R visitUnaryExpr(const Unary *expr) = 0;
    virtual R visitVariableExpr(const Variable *expr) = 0;
    virtual R visitAssignExpr(const Assign *expr) = 0;
    virtual R visitLogicalExpr(const Logical *expr) = 0;
};

class Expr
{
public:
    virtual ~Expr() = default;
    virtual std::any accept(BaseExprVisitor &visitor) const = 0;
};

using ExprPtr = std::unique_ptr<Expr>;



struct Binary : public Expr
{

Binary(ExprPtr left,  Token op,  ExprPtr right) : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

std::any accept(BaseExprVisitor &visitor) const override
{
  auto visitor_ptr = dynamic_cast<ExprVisitor<std::any>*>(&visitor);
  if (visitor_ptr)
    return std::any(visitor_ptr->visitBinaryExpr(this));
  return std::any();
}

    ExprPtr left;
     Token op;
     ExprPtr right;

};

using BinaryPtr = std::unique_ptr<Binary>;


struct Call : public Expr
{

Call(ExprPtr callee,  Token paren,  vector<ExprPtr> arguments) : callee(std::move(callee)), paren(std::move(paren)), arguments(std::move(arguments)) {}

std::any accept(BaseExprVisitor &visitor) const override
{
  auto visitor_ptr = dynamic_cast<ExprVisitor<std::any>*>(&visitor);
  if (visitor_ptr)
    return std::any(visitor_ptr->visitCallExpr(this));
  return std::any();
}

    ExprPtr callee;
     Token paren;
     vector<ExprPtr> arguments;

};

using CallPtr = std::unique_ptr<Call>;


struct Grouping : public Expr
{

Grouping(ExprPtr expression) : expression(std::move(expression)) {}

std::any accept(BaseExprVisitor &visitor) const override
{
  auto visitor_ptr = dynamic_cast<ExprVisitor<std::any>*>(&visitor);
  if (visitor_ptr)
    return std::any(visitor_ptr->visitGroupingExpr(this));
  return std::any();
}

    ExprPtr expression;

};

using GroupingPtr = std::unique_ptr<Grouping>;


struct Literal : public Expr
{

Literal(LiteralType value) : value(std::move(value)) {}

std::any accept(BaseExprVisitor &visitor) const override
{
  auto visitor_ptr = dynamic_cast<ExprVisitor<std::any>*>(&visitor);
  if (visitor_ptr)
    return std::any(visitor_ptr->visitLiteralExpr(this));
  return std::any();
}

    LiteralType value;

};

using LiteralPtr = std::unique_ptr<Literal>;


struct Unary : public Expr
{

Unary(Token op,  ExprPtr right) : op(std::move(op)), right(std::move(right)) {}

std::any accept(BaseExprVisitor &visitor) const override
{
  auto visitor_ptr = dynamic_cast<ExprVisitor<std::any>*>(&visitor);
  if (visitor_ptr)
    return std::any(visitor_ptr->visitUnaryExpr(this));
  return std::any();
}

    Token op;
     ExprPtr right;

};

using UnaryPtr = std::unique_ptr<Unary>;


struct Variable : public Expr
{

Variable(Token name) : name(std::move(name)) {}

std::any accept(BaseExprVisitor &visitor) const override
{
  auto visitor_ptr = dynamic_cast<ExprVisitor<std::any>*>(&visitor);
  if (visitor_ptr)
    return std::any(visitor_ptr->visitVariableExpr(this));
  return std::any();
}

    Token name;

};

using VariablePtr = std::unique_ptr<Variable>;


struct Assign : public Expr
{

Assign(Token name,  ExprPtr value) : name(std::move(name)), value(std::move(value)) {}

std::any accept(BaseExprVisitor &visitor) const override
{
  auto visitor_ptr = dynamic_cast<ExprVisitor<std::any>*>(&visitor);
  if (visitor_ptr)
    return std::any(visitor_ptr->visitAssignExpr(this));
  return std::any();
}

    Token name;
     ExprPtr value;

};

using AssignPtr = std::unique_ptr<Assign>;


struct Logical : public Expr
{

Logical(ExprPtr left,  Token op,  ExprPtr right) : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

std::any accept(BaseExprVisitor &visitor) const override
{
  auto visitor_ptr = dynamic_cast<ExprVisitor<std::any>*>(&visitor);
  if (visitor_ptr)
    return std::any(visitor_ptr->visitLogicalExpr(this));
  return std::any();
}

    ExprPtr left;
     Token op;
     ExprPtr right;

};

using LogicalPtr = std::unique_ptr<Logical>;

}
