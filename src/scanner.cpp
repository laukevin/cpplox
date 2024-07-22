
#include <any>
#include <string>
#include <vector>

#include "cpplox/lox.h"
#include "cpplox/token.h"
#include "cpplox/tokentype.h"
#include "cpplox/scanner.h"

namespace CppLox
{
  const std::vector<Token> &Scanner::scanTokens()
  {
    while (!Scanner::isAtEnd())
    {
      start = current;
      scanToken();
    }
    // Your implementation here

    tokens.push_back(Token(EOF_, "", nullptr, line));
    return tokens;
  }

  bool Scanner::isAtEnd()
  {
    return current >= source.length();
  }

  char Scanner::advance()
  {
    return source.at(current++);
  }

  void Scanner::addToken(TokenType type)
  {
    addToken(type, nullptr);
  }

  void Scanner::addToken(TokenType type, LiteralType literal)
  {
    std::string text = source.substr(start, current);
    tokens.push_back(Token(type, text, literal, line));
  }

  void Scanner::scanToken()
  {
    char c = advance();
    switch (c)
    {
    case '(':
      addToken(LEFT_PAREN);
      break;
    case ')':
      addToken(RIGHT_PAREN);
      break;
    case '{':
      addToken(LEFT_BRACE);
      break;
    case '}':
      addToken(RIGHT_BRACE);
      break;
    case ',':
      addToken(COMMA);
      break;
    case '.':
      addToken(DOT);
      break;
    case '-':
      addToken(MINUS);
      break;
    case '+':
      addToken(PLUS);
      break;
    case ';':
      addToken(SEMICOLON);
      break;
    case '*':
      addToken(STAR);
      break;
    default:
      lox::error(line, "Unexpected character.");
    }
  }
}