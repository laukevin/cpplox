#include <vector>
#include <exception>
#include "expr.h"
#include "stmt.h"
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
    std::vector<StmtPtr> parse();

  private:
    std::vector<Token> tokens;
    int current = 0;
    ExprPtr expression();
    ExprPtr assignment();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();
    ExprPtr orExpr();
    ExprPtr andExpr();
    ExprPtr call();
    ExprPtr finishCall(ExprPtr callee);

    Token advance();
    bool check(TokenType type) const;
    bool match(std::vector<TokenType> tokens);
    bool isAtEnd() const;
    const Token peek() const;
    const Token previous() const;
    Token consume(TokenType type, std::string errorMessage);
    ParserError error(const Token &token, std::string errorMessage);
    void synchronize();
    StmtPtr statement();
    StmtPtr printStatement();
    StmtPtr returnStatement();
    StmtPtr expressionStatement();
    StmtPtr declaration();
    StmtPtr varDeclaration();
    StmtPtr ifStatement();
    StmtPtr whileStatement();
    StmtPtr forStatement();
    std::vector<StmtPtr> block();
    StmtPtr function(std::string kind);
    StmtPtr classDeclaration();
  };
}