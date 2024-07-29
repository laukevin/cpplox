#ifndef CPPLOX_SCANNER_H
#define CPPLOX_SCANNER_H

#include <any>
#include <string>
#include <vector>
#include <unordered_map>

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
    inline static const std::unordered_map<std::string, TokenType> keywords = {
        {"and", AND},
        {"class", CLASS},
        {"else", ELSE},
        {"false", FALSE},
        {"for", FOR},
        {"fun", FUN},
        {"if", IF},
        {"nil", NIL},
        {"or", OR},
        {"print", PRINT},
        {"return", RETURN},
        {"super", SUPER},
        {"this", THIS},
        {"true", TRUE},
        {"var", VAR},
        {"while", WHILE}};
    int start = 0;
    int current = 0;
    int line = 1;

    bool isAtEnd();
    void scanToken();
    void addToken(TokenType type);
    void addToken(TokenType type, std::nullptr_t ptr);
    void addToken(TokenType type, std::string &&strLiteral);
    void addToken(TokenType type, LiteralType literal);
    char advance();
    bool match(char expected);
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();
  };
}

#endif