#ifndef CPPLOX_SCANNER_H
#define CPPLOX_SCANNER_H

#include <any>
#include <string>
#include <vector>

#include "token.h"

namespace CppLox
{
  class Scanner
  {
  public:
    Scanner(const std::string &source) : source(source) {}

    const std::vector<Token> &scanTokens();

  private:
    const std::string &source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();
    void addToken(TokenType type);
    void addToken(TokenType type, LiteralType literal);
    char advance();
  };
}

#endif