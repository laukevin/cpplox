
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

  void Scanner::addToken(TokenType type, std::nullptr_t ptr)
  {
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, std::move(text), ptr, line));
  }

  void Scanner::addToken(TokenType type, std::string &&strLiteral)
  {
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, std::move(text), strLiteral, line));
  }

  void Scanner::addToken(TokenType type, LiteralType literal)
  {
    std::string text = source.substr(start, current - start);
    tokens.push_back(Token(type, std::move(text), std::move(literal), line));
  }

  bool Scanner::match(char expected)
  {
    if (isAtEnd())
      return false;
    if (source.at(current) != expected)
      return false;

    current++;
    return true;
  }

  bool Scanner::isDigit(char c)
  {
    return c >= '0' && c <= '9';
  }

  bool Scanner::isAlpha(char c)
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  bool Scanner::isAlphaNumeric(char c)
  {
    return isDigit(c) || isAlpha(c);
  }

  char Scanner::peek()
  {
    if (isAtEnd())
      return '\0';
    return source.at(current);
  }

  char Scanner::peekNext()
  {
    if (current + 1 >= source.length())
      return '\0';
    return source.at(current + 1);
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
    case '!':
      addToken(match('=') ? BANG_EQUAL : BANG);
      break;
    case '=':
      addToken(match('=') ? EQUAL_EQUAL : EQUAL);
      break;
    case '<':
      addToken(match('=') ? LESS_EQUAL : LESS);
      break;
    case '>':
      addToken(match('=') ? GREATER_EQUAL : GREATER);
      break;
    case '/':
      if (match('/'))
      {
        // A comment goes until the end of the line.
        while (peek() != '\n' && !isAtEnd())
          advance();
      }
      else
      {
        addToken(SLASH);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      // Ignore whitespace.
      break;

    case '\n':
      line++;
      break;
    case 'o':
      if (match('r'))
      {
        addToken(TokenType::OR);
      }
      break;
    case '"':
      string();
      break;
    default:
      if (isDigit(c))
      {
        number();
      }
      else if (isAlpha(c))
      {
        identifier();
      }
      else
      {

        lox::error(line, "Unexpected character.");
      }
      break;
    }
  }

  void Scanner::string()
  {
    while (peek() != '"' && !isAtEnd())
    {
      if (peek() == '\n')
        line++;
      advance();
    }

    if (isAtEnd())
    {
      lox::error(line, "Unterminated string.");
      return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(STRING, std::move(value));
  }

  void Scanner::number()
  {
    while (isDigit(peek()))
    {
      advance();
    }

    if (peek() == '.' && isDigit(peekNext()))
    {
      advance();
      while (isDigit(peek()))
      {
        advance();
      }
    }

    double num = std::stod(source.substr(start, current - start));
    addToken(TokenType::NUMBER, num);
  }

  void Scanner::identifier()
  {
    while (isAlphaNumeric(peek()))
    {
      advance();
    }

    std::string text = source.substr(start, current - start);
    auto it = keywords.find(text);
    TokenType type = IDENTIFIER;
    if (it != keywords.end())
    {
      type = it->second;
    }
    addToken(type);
  }
}