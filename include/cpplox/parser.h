#include <vector>
#include <exception>
#include "expr.h"
#include "token.h"

namespace CppLox
{
  class ParserError : public std::exception
  {
  };

  class Parser
  {
  public:
    Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}
    ExprPtr parse();

  private:
    std::vector<Token> tokens;
    int current = 0;
    ExprPtr expression();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();
    Token advance();
    bool check(TokenType type) const;
    bool match(std::vector<TokenType> tokens);
    bool isAtEnd() const;
    const Token peek() const;
    const Token previous() const;
    Token consume(TokenType type, std::string errorMessage);
    ParserError error(Token token, std::string errorMessage);
    void synchronize();
  };
}