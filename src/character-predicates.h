#ifndef FLORA_CHAR_PREDICATES_H
#define FLORA_CHAR_PREDICATES_H

namespace flora {
namespace character {

// End of source
const char32_t EOS = -1;

// Unicode-ready character predicates

inline char32_t AsciiToLowerCase(char32_t ch) {
  return ch | 0x20;
}

// Returns true if ch is in range [lower, upper]
inline bool IsInRange(char32_t ch, char32_t lower, char32_t upper) {
  return static_cast<unsigned int>(ch - lower) <=
      static_cast<unsigned int>(upper - lower);
}

inline bool IsLineFeed(char32_t ch) {
  return ch == 0xA;
} 

inline bool IsDecimalDigit(char32_t ch) {
  return IsInRange(ch, '0', '9');
}

inline bool IsHexDigit(char32_t ch) {
  return IsDecimalDigit(ch) || IsInRange(AsciiToLowerCase(ch), 'a', 'f');
}

inline bool IsOctalDigit(char32_t ch) {
  return IsInRange(ch, '0', '7');
}

inline bool IsBinaryDigit(char32_t ch) {
  return ch == '0' || ch == '1';
}

inline bool IsAlphaNumeric(uc32 c) {
  return IsInRange(AsciiToLowerCase(c), 'a', 'z') || IsDecimalDigit(c);
}

inline bool IsIdentifierStart(char32_t ch) {
  return IsInRange(AsciiToLowerCase(ch), 'a', 'z') || ch == '_';
}

inline bool IsIdentifierBody(char32_t ch) {
  return IsAlphaNumeric(ch) || ch == '_';
}

}
}

#endif