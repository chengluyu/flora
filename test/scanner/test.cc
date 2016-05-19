#include <iostream>

#include "../../src/character-stream.h"
#include "../../src/scanner.h"
#include "../../src/token.h"

using flora::FileCharacterStream;
using flora::Scanner;
using flora::Token;
using flora::Tokens;

int main(int argc, char const *argv[]) {
  const char *test_cases[] = {
    "test_case_all_tokens.txt",
    // "test_case_error_comments.txt",
    // "test_case_error_string.txt",
    // "test_case_error_character.txt",
    // "test_case_5.txt",
    // "test_case_6.txt"
  };
  for (int i = 0; i < sizeof(test_cases) / sizeof(const char*); i++) {
    std::cout << "Now testing " << test_cases[i] << std::endl;
    FileCharacterStream *stream = new FileCharacterStream(test_cases[i]);
    Scanner *scanner = new Scanner();
    scanner->Initialize(stream);
    std::cout << "Strat scanning..." << std::endl;
    while (true) {
      Token tok = scanner->Advance();
      std::cout << Tokens::Name(tok) << " (\"";
      if (scanner->GetTokenLiteral().size() > 0) {
        std::cout << scanner->GetTokenLiteral();
      } else {
        std::cout << Tokens::Literal(tok);
      }
      std::cout << "\")" << std::endl;
      if (tok == Token::EndOfSource || tok == Token::Illegal)
        break;
    }
    delete scanner;
    delete stream;
  }
  return 0;
}