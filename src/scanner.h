#ifndef FLORA_SCANNER_H
#define FLORA_SCANNER_H

#include <queue>
#include <utility>

#include "flora.h"
#include "checks.h"
#include "token.h"
#include "character-predicates.h"
#include "character-stream.h"

namespace flora {

class Scanner {
public:
  // Forbid copy behaviors
  NOCOPY_CLASS(Scanner)

  Scanner();
  ~Scanner();
  // TODO
  void Initialize(CharacterStream *stream);
  Token Advance();
  void SaveBookmark();
  void LoadBookmark();
  void ClearBookmark();
  const std::string& GetTokenLiteral();
private:
  enum class State {
    Uninitialized, Running, Recording, Restoring, Error, End
  };
  // The recording queue for bookmark function
  std::queue<std::pair<Token, std::string>> records_;
  // The current state of scanner
  State state_;
  // The stream
  CharacterStream *stream_;
  // Next character
  char32_t peek;
  // The literal of token
  std::string literal_;
  // Advance next character.
  char32_t Next();
  // Returns true if the next character is expected character
  bool Match(char32_t expected);
  // Set the literal of token
  inline void SetTokenLiteral(const char *literal);
  inline void SetTokenLiteral(std::string &literal);
  inline void SetTokenLiteral(char literal);
  inline void ClearTokenLiteral();
  // Report error, call before returning Token::Illegal
  inline void ReportScannerError(const char *message);
  inline void MarkEndOfSource();
  // Scan a token
  Token Scan();
  // Skip comments, returns true if error occurs
  bool SkipMultipleLineComment();
  void SkipSingleLineComment();
  // Scan literals and escapees of strings and characters
  Token ScanStringLiteral();
  Token ScanCharacterLiteral();
  char32_t ScanCharacterEscape();
  // Scan identifiers and keywords
  Token ScanIdentifierOrKeyword(char32_t firstChar);
  // Scan integers and real numebers
  Token ScanIntegerOrRealNumber(char32_t firstChar);
  Token ScanRealNumber(const std::string *integral_part,
                       bool scanned_period = false);
  // Scan integer in different base
  Token ScanHexInteger();
  Token ScanOctalInteger();
  Token ScanBinaryInteger();
};

}

#endif