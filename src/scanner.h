#ifndef FLORA_SCANNER_H
#define FLORA_SCANNER_H

#include <cctype>

#include "flora.h"
#include "token.h"
#include "character-predicates.h"

namespace flora {

class Scanner {
public:
  // Forbid copy behaviors
  NOCOPY_CLASS(Scanner)

  Scanner();
  ~Scanner();
  // TODO
  void Initialize();
  Token Peek();
  Token Advance();
private:
  // Scan a token
  Token Scan();
  // Skip comments
  void SkipMultipleLineComment();
  void SkipSingleLineComment();
  // Scan literals and escapees of strings and characters
  Token ScanStringLiteral();
  Token ScanCharacterLiteral();
  char ScanStringEscape();
  char ScanCharacterEscape();
  // Scan identifiers and keywords
  Token ScanIdentifierOrKeyword();
  // Scan integers and real numebers
  Token ScanIntegerOrRealNumber();
  Token ScanRealNumber();
};

}

#endif