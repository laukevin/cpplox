
#include <vector>

#include "cpplox/expr.h"
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
        true;
      }
    }
    return false;
  }

  ExprPtr Parser::expression()
  {
    return equality();
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

  ParserError Parser::error(Token token, std::string errorMessage)
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
    return primary();
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

  ExprPtr Parser::parse()
  {
    try
    {
      return expression();
    }
    catch (const ParserError &e)
    {
      return nullptr;
    }
  }
}