#ifndef CPPLOX_TOKENTYPE_H
#define CPPLOX_TOKENTYPE_H

#include <iostream>
#include <vector>
#include <string>

namespace CppLox
{
  enum TokenType
  {
    // Single-character tokens.
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,
    EOF_,
    TOKEN_TYPE_COUNT
  };

  const std::vector<std::string> tokenTypeStrings = {
      "LEFT_PAREN",
      "RIGHT_PAREN",
      "LEFT_BRACE",
      "RIGHT_BRACE",
      "COMMA",
      "DOT",
      "MINUS",
      "PLUS",
      "SEMICOLON",
      "SLASH",
      "STAR",

      // One or two character tokens.
      "BANG",
      "BANG_EQUAL",
      "EQUAL",
      "EQUAL_EQUAL",
      "GREATER",
      "GREATER_EQUAL",
      "LESS",
      "LESS_EQUAL",

      // Literals.
      "IDENTIFIER",
      "STRING",
      "NUMBER",

      // Keywords.
      "AND",
      "CLASS",
      "ELSE",
      "FALSE",
      "FUN",
      "FOR",
      "IF",
      "NIL",
      "OR",
      "PRINT",
      "RETURN",
      "SUPER",
      "THIS",
      "TRUE",
      "VAR",
      "WHILE",
      "EOF_"
      // Add other token strings here
  };

  inline const std::string &tokenTypeToString(TokenType type)
  {
    if (type >= 0 && type < TOKEN_TYPE_COUNT)
    {
      return tokenTypeStrings[type];
    }
    return "UNKNOWN";
  }
}

#endif