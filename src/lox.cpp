#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "cpplox/lox.h"
#include "cpplox/scanner.h"
#include "cpplox/token.h"

static int hadError = false;

namespace CppLox
{
  void lox::error(int line, const std::string &message)
  {
    report(line, "", message);
  }

  void lox::report(int line, const std::string &where, const std::string &message)
  {
    std::string error_message = "[line " + std::to_string(line) + "] Error " + where + ": " + message;

    std::cout << error_message << std::endl;
    hadError = true;
  }

  void run(const std::string &source)
  {
    Scanner scanner = Scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    for (const auto &token : tokens)
    {
      std::cout << token.toString() << std::endl;
    }
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
    std::cout << "file loc" << file_loc << std::endl;
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
        std::cout << "You entered: " << line << std::endl;
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