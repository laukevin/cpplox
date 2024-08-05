#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "cpplox/lox.h"
#include "cpplox/expr.h"
#include "cpplox/stmt.h"
#include "cpplox/astprinter.h"
#include "cpplox/scanner.h"
#include "cpplox/parser.h"
#include "cpplox/token.h"
#include "cpplox/tokentype.h"
#include "cpplox/runtime_error.h"
#include "cpplox/interpreter.h"

static int hadError = false;
static int hadRuntimeError = false;

namespace CppLox
{
  static Interpreter interpreter;

  void lox::error(int line, const std::string &message)
  {
    report(line, "", message);
  }

  void lox::error(const Token &token, const std::string &message)
  {
    if (token.type == TokenType::EOF_)
    {
      report(token.line, " at end", message);
    }
    else
    {
      report(token.line, " at '" + token.lexeme + "'", message);
    }
  }

  void lox::report(int line, const std::string &where, const std::string &message)
  {
    std::string error_message = "[line " + std::to_string(line) + "] Error " + where + ": " + message;

    std::cout << error_message << std::endl;
    hadError = true;
  }

  void lox::runtimeError(const RuntimeError &error)
  {
    std::cout << "Runtime error: " << error.what() << "[line " << error.op.line << "]" << std::endl;
    hadRuntimeError = true;
  }

  void run(const std::string &source)
  {
    Scanner scanner = Scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    Parser parser = Parser(tokens);
    std::vector<StmtPtr> stmts = parser.parse();

    if (hadError)
      return;
    if (hadRuntimeError)
      return;
    interpreter.interpret(stmts);
  }

  bool readFile(const std::string &file_loc, std::string &content)
  {
    std::ifstream infile(file_loc);
    if (!infile.is_open())
    {
      return false;
    }

    std::stringstream buffer;
    buffer << infile.rdbuf();
    content = buffer.str();
    return true;
  }

  void runFile(const std::string &file_loc)
  {
    std::string contents;
    readFile(file_loc, contents);
    run(contents);
    if (hadError)
    {
      std::exit(1);
    }
  }

  void runPrompt()
  {
    while (true)
    {
      std::string line;
      if (std::getline(std::cin, line))
      {
        run(line);
      }
      else
      {
        std::cout << "entered line exiting " << line << std::endl;
        std::exit(1);
      }
    }
  }
}

int main(int argc, char *argv[])
{
  // using namespace CppLox;

  // ExprPtr unaryA = std::make_unique<Unary>(
  //     Token(TokenType::MINUS, "-", nullptr, 1),
  //     std::make_unique<Literal>(123));

  // Token op = Token(TokenType::STAR, "*", nullptr, 1);
  // ExprPtr unaryB = std::make_unique<Grouping>(
  //     std::make_unique<Literal>(45.67));

  // Binary expression = Binary(
  //     std::move(unaryA),
  //     std::move(op),
  //     std::move(unaryB));

  // AstPrinter printer;
  // std::any res = printer.print(expression);
  // std::cout << std::any_cast<std::string>(res) << std::endl;

  if (argc > 2)
  {
    std::cout << "test program" << std::endl;
    std::exit(1);
  }
  else if (argc == 2)
  {
    CppLox::runFile(argv[1]);
  }
  else
  {
    CppLox::runPrompt();
  }
}