
#include <vector>
#include <iostream>

#include "cpplox/expr.h"
#include "cpplox/stmt.h"
#include "cpplox/parser.h"
#include "cpplox/lox.h"
#include "cpplox/tokentype.h"

namespace CppLox
{
  const Token Parser::peek() const
  {
    return tokens[current];
  }

  bool Parser::isAtEnd() const
  {
    return peek().type == TokenType::EOF_;
  }

  const Token Parser::previous() const
  {
    return tokens[current - 1];
  }

  bool Parser::check(TokenType type) const
  {
    if (isAtEnd())
      return false;
    return peek().type == type;
  }

  Token Parser::advance()
  {
    if (!isAtEnd())
      current++;
    return previous();
  }

  bool Parser::match(std::vector<TokenType> types)
  {
    for (auto &type : types)
    {
      if (check(type))
      {
        advance();
        return true;
      }
    }
    return false;
  }

  ExprPtr Parser::expression()
  {
    return assignment();
  }

  ExprPtr Parser::assignment()
  {
    ExprPtr expr = orExpr();
    if (match({TokenType::EQUAL}))
    {
      Token equals = previous();
      ExprPtr value = expression();

      Variable *varExpr = dynamic_cast<Variable *>(expr.get());
      if (varExpr)
      {
        Token name = varExpr->name;
        return std::make_unique<Assign>(name, std::move(value));
      }
      Get *getExpr = dynamic_cast<Get *>(expr.get());
      if (getExpr)
      {
        return std::make_unique<Set>(std::move(getExpr->object), getExpr->name, std::move(value));
      }
      lox::error(equals, "Invalid assignment target.");
    }
    return expr;
  }

  ExprPtr Parser::primary()
  {
    if (match({TokenType::FALSE}))
    {
      return std::make_unique<Literal>(false);
    }

    if (match({TokenType::TRUE}))
    {
      return std::make_unique<Literal>(true);
    }

    if (match({TokenType::THIS}))
    {
      return std::make_unique<This>(previous());
    }

    if (match({TokenType::IDENTIFIER}))
    {
      return std::make_unique<Variable>(previous());
    }

    if (match({TokenType::NIL}))
    {
      return std::make_unique<Literal>(nullptr);
    }

    if (match({TokenType::NUMBER, TokenType::STRING}))
    {
      return std::make_unique<Literal>(previous().literal);
    }

    if (match({TokenType::LEFT_PAREN}))
    {
      ExprPtr expr = expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_unique<Grouping>(std::move(expr));
    }

    throw error(peek(), "Expect expression");
  }

  Token Parser::consume(TokenType type, std::string errorMessage)
  {
    if (check(type))
      return advance();
    throw error(peek(), errorMessage);
  }

  ParserError Parser::error(const Token &token, std::string errorMessage)
  {
    lox::error(token, errorMessage);
    return ParserError();
  }

  ExprPtr Parser::unary()
  {
    if (match({TokenType::BANG, TokenType::MINUS}))
    {
      Token op = previous();
      ExprPtr right = unary();
      return std::make_unique<Unary>(std::move(op), std::move(right));
    }
    return call();
  }

  ExprPtr Parser::finishCall(ExprPtr callee)
  {
    std::vector<ExprPtr> arguments;
    if (!check(TokenType::RIGHT_PAREN))
    {
      do
      {
        if (arguments.size() >= 255)
        {
          error(peek(), "Cannot have more than 255 arguments.");
        }
        arguments.push_back(expression());
      } while (match({TokenType::COMMA}));
    }

    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return std::make_unique<Call>(std::move(callee), paren, std::move(arguments));
  }

  ExprPtr Parser::call()
  {
    ExprPtr expr = primary();
    while (true)
    {
      if (match({TokenType::LEFT_PAREN}))
      {
        expr = finishCall(std::move(expr));
      }
      else if (match({TokenType::DOT}))
      {
        Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
        expr = std::make_unique<Get>(std::move(expr), name);
      }
      else
      {
        break;
      }
    }
    return expr;
  }

  ExprPtr Parser::term()
  {
    ExprPtr expr = factor();
    while (match({TokenType::MINUS,
                  TokenType::PLUS}))
    {
      Token op = previous();
      ExprPtr right = factor();
      expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
  }

  ExprPtr Parser::factor()
  {
    ExprPtr expr = unary();
    while (match({TokenType::SLASH,
                  TokenType::STAR}))
    {
      Token op = previous();
      ExprPtr right = unary();
      expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
  }

  ExprPtr Parser::comparison()
  {
    ExprPtr expr = term();
    while (match({TokenType::GREATER,
                  TokenType::GREATER_EQUAL,
                  TokenType::LESS,
                  TokenType::LESS_EQUAL}))
    {
      Token op = previous();
      ExprPtr right = term();
      expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
  }

  ExprPtr Parser::orExpr()
  {
    ExprPtr expr = andExpr();
    while (match({TokenType::OR}))
    {
      Token op = previous();
      ExprPtr right = andExpr();
      expr = std::make_unique<Logical>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
  }

  ExprPtr Parser::andExpr()
  {
    ExprPtr expr = equality();
    while (match({TokenType::AND}))
    {
      Token op = previous();
      ExprPtr right = equality();
      expr = std::make_unique<Logical>(std::move(expr), std::move(op), std::move(right));
    }

    return expr;
  }

  ExprPtr Parser::equality()
  {
    ExprPtr expr = comparison();

    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
    {
      Token op = previous();
      ExprPtr right = comparison();
      expr = std::make_unique<Binary>(std::move(expr), std::move(op), std::move(right));
    }
    return expr;
  }

  void Parser::synchronize()
  {
    advance();
    while (!isAtEnd())
    {
      if (previous().type == TokenType::SEMICOLON)
        return;
      switch (peek().type)
      {
      case CLASS:
      case FUN:
      case VAR:
      case FOR:
      case IF:
      case WHILE:
      case PRINT:
      case RETURN:
        return;
      }

      advance();
    }
  }

  std::vector<StmtPtr> Parser::parse()
  {
    std::vector<StmtPtr> statements;

    while (!isAtEnd())
    {
      statements.push_back(declaration());
    }
    return statements;
  }

  StmtPtr Parser::statement()
  {
    if (match({TokenType::PRINT}))
    {
      return printStatement();
    }
    if (match({TokenType::RETURN}))
    {
      return returnStatement();
    }
    if (match({TokenType::IF}))
    {
      return ifStatement();
    }
    if (match({TokenType::WHILE}))
    {
      return whileStatement();
    }
    if (match({TokenType::FOR}))
    {
      return forStatement();
    }
    if (match({TokenType::LEFT_BRACE}))
    {
      return std::make_unique<Block>(std::move(block()));
    }
    return expressionStatement();
  }

  StmtPtr Parser::printStatement()
  {
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<Print>(std::move(value));
  }

  StmtPtr Parser::expressionStatement()
  {
    ExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<Expression>(std::move(expr));
  }

  StmtPtr Parser::classDeclaration()
  {
    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");

    ExprPtr superclass;
    if (match({TokenType::LESS}))
    {
      consume(TokenType::IDENTIFIER, "Expect superclass name.");
      superclass = std::make_unique<Variable>(previous());
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");
    std::vector<StmtPtr> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
      methods.push_back(function("method"));
    }
    consume(RIGHT_BRACE, "Expect '}' after class body.");
    return std::make_unique<Class>(name, std::move(superclass), std::move(methods));
  }

  StmtPtr Parser::declaration()
  {
    try
    {
      if (match({TokenType::CLASS}))
      {
        return classDeclaration();
      }
      if (match({TokenType::FUN}))
      {
        return function("function");
      }
      if (match({TokenType::VAR}))
      {
        return varDeclaration();
      }
      return statement();
    }
    catch (const ParserError &error)
    {
      synchronize();
      return nullptr;
    }
  }

  StmtPtr Parser::function(std::string kind)
  {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check(TokenType::RIGHT_PAREN))
    {
      do
      {
        if (parameters.size() >= 255)
        {
          error(peek(), "Cannot have more than 255 parameters.");
        }
        parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
      } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<StmtPtr> body = block();
    return std::make_unique<Function>(name, std::move(parameters), std::move(body));
  }

  StmtPtr Parser::varDeclaration()
  {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    ExprPtr initializer = nullptr;
    if (match({TokenType::EQUAL}))
    {
      initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<Var>(name, std::move(initializer));
  }

  std::vector<StmtPtr> Parser::block()
  {
    std::vector<StmtPtr> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
    {
      statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
  }

  StmtPtr Parser::ifStatement()
  {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    StmtPtr thenBranch = statement();
    StmtPtr elseBranch = nullptr;

    if (match({TokenType::ELSE}))
    {
      elseBranch = statement();
    }
    return std::make_unique<If>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
  }

  StmtPtr Parser::whileStatement()
  {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if' .");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    StmtPtr body = statement();
    return std::make_unique<While>(std::move(condition), std::move(body));
  }

  StmtPtr Parser::forStatement()
  {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for' .");
    StmtPtr initializer;
    if (match({TokenType::SEMICOLON}))
    {
      initializer = nullptr;
    }
    else if (match({TokenType::VAR}))
    {
      initializer = varDeclaration();
    }
    else
    {
      initializer = expressionStatement();
    }

    ExprPtr condition;
    if (!check(TokenType::SEMICOLON))
    {
      condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    ExprPtr increment;
    if (!check(TokenType::RIGHT_PAREN))
    {
      increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
    StmtPtr body = statement();

    std::vector<StmtPtr> stmts;

    if (initializer != nullptr)
    {
      stmts.push_back(std::move(initializer));
    }

    if (increment != nullptr)
    {
      Block *block = dynamic_cast<Block *>(body.get());
      block->statements.push_back(std::make_unique<Expression>(std::move(increment)));
    }

    if (condition == nullptr)
    {
      ExprPtr trueExpr = std::make_unique<Literal>(true);
      stmts.push_back(std::make_unique<While>(std::move(trueExpr), std::move(body)));
    }
    else
    {
      stmts.push_back(std::make_unique<While>(std::move(condition), std::move(body)));
    }

    return std::make_unique<Block>(std::move(stmts));
  }

  StmtPtr Parser::returnStatement()
  {
    Token keyword = previous();
    ExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON))
    {
      value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<Return>(keyword, std::move(value));
  }

}