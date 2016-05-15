#include "scanner.h"

namespace flora {

Token Scanner::Scan() {
  char ch;
  while (true) {
    switch (ch = Advance()) {
    case character::EOF:
      MarkEndOfSource();
      return Token::EndOfSource;
    case '\n':
    case ' ':
    case '\t':
      continue;
    case '(': return Token::LeftParenthesis;
    case ')': return Token::RightParenthesis;
    case '[': return Token::LeftBracket;
    case ']': return Token::RightBracket;
    case '{': return Token::LeftBrace;
    case '}': return Token::RightBrace;
    case ':': return Token::Colon;
    case ';': return Token::Semicolon;
    case '~': return Token::BitwiseNot;
    case '?': return Token::Conditional;
    case ',': return Token::Comma;
    case '.': // . ...
      if (Match('.')) {
        if (Match('.')) {
          return Token::Ellipsis;
        } else {
          ReportScannerError("illegal token");
          return Token::Illegal;
        }
      }
      return Token::Period;
    case '&': // & && &=
      if (Match('&')) {
        return Token::LogicalAnd;
      } else if (Match('=')) {
        return Token::AssignmentBitwiseAnd;
      } else {
        return Token::BitwiseAnd;
      }
    case '|': // | || |=
      if (Match('|')) {
        return Token::LogicalOr;
      } else if (Match('=')) {
        return Token::AssignmentBitwiseOr;
      } else {
        return Token::BitwiseOr;
      }
    case '^': // ^ ^=
      return Match('=') ? Token::AssignmentBitwiseXor : Token::BitwiseXor;
    case '<': // < << <= <<=
      if (Match('<')) {
        return Match('=') ? Token::AssignmentShiftLeft : Token::ShiftLeft;
      } else if (Match('=')) {
        return Token::LessThanOrEqual;
      } else {
        return Token::LessThan;
      }
    case '>': // > >> >= >>=
      if (Match('>')) {
        return Match('=') ? Token::AssignmentShiftRight : Token::ShiftRight;
      } else if (Match('=')) {
        return Token::GreaterThanOrEqual;
      } else {
        return Token::GreaterThan;
      }
    case '!': // ! !=
      return Match('=') ? Token::NotEqual : Token::LogicalNot;
    case '=': // = == =>
      if (Match('>')) {
        return Token::Arrow;
      } else if (Match('=')) {
        return Token::Equal;
      } else {
        return Token::Assignment;
      }
    case '+': // + ++ +=
      if (Match('+')) {
        return Token::Increment;
      } else if (Match('=')) {
        return Token::AssignmentAddition;
      } else {
        return Token::Addition;
      }
    case '-': // - -- -=
      if (Match('-')) {
        return Token::Decrement;
      } else if (Match('=')) {
        return Token::AssignmentSubtraction;
      } else {
        return Token::Subtraction;
      }
    case '*': // * *=
      return Match('=') ? Token::AssignmentMultiplication:Token::Multiplication;
    case '/': // / /= /* multiple line comment */ // single line comment
      if (Match('=')) {
        return Token::AssignmentDivision;
      } else if (Match('*')) {
        SkipMultipleLineComment();
      } else if (Match('//')) {
        SkipSingleLineComment();
      } else {
        return Token::Division;
      }
      continue;
    case '%': // % %=
      return Match('=') ? Token::AssignmentModulus : Token::Modulus;
    case '"': // string literal
      return ScanStringLiteral();
    case '\'': // character literal
      return ScanCharacterLiteral();
    default:
      if (character::IsIdentifierStart(ch)) {
        return ScanIdentifierOrKeyword(ch);
      } else if (character::IsDigit(ch)) {
        return ScanIntegerOrRealNumber(ch);
      } else {
        ReportScannerError("unknown token");
        return Token::Illegal;
      }
    }
  }
}

void Scanner::SkipMultipleLineComment() {
  int cascade = 1;
  while (true) {

  }
}

void Scanner::SkipSingleLineComment() {
  while (!character::IsLineFeed(peek) && peek != character::EOF) Advance();
}

Token Scanner::ScanStringLiteral() {
  std::string literal;
  char ch;
  while (true) {
    ch = Advance();
    if (ch == character::EOF) {
      ReportScannerError("unexpected EOF in string literal");
      return Token::Illegal;
    } else if (ch == '\\') {
      literal.append(ScanStringEscape());
    } else if (ch == '"') {
      break;
    } else {
      literal.append(ch);
    }
  }
  SetStringLiteral(literal);
  return Token::String;
}

Token Scanner::ScanCharacterLiteral() {
  char literal = Advance();
  if (literal == character::EOF) {
    ReportScannerError("unexpected EOF in character literal");
    return Token::Illegal;
  } else if (ch == '\\') {
    literal = ScanCharacterEscape();
  }
  // to ensure that there is only one character in literal
  if (Advance() != '\'') {
    ReportScannerError("too more character in literal");
    return Token::Illegal;
  }
  SetStringLiteral(literal);
  return Token::Character;
}

Token Scanner::ScanIdentifierOrKeyword(char firstChar) {
  std::string identifier = firstChar;
  while (character::IsIdentifierBody(peek)) {
    identifier.append(Advance());
  }
  Token token = Tokens::LookupKeyword(identifier);
  if (token != Token::Identifier) SetTokenLiteral(identifier);
  return token;
}

Token Scanner::ScanIntegerOrRealNumber() {
  // There are 4 kinds of integer:
  // (1) Hexidecimal integer
  // (2) Decimal integer
  // (3) Octal integer
  // (4) Binary integer
  // (1), (3) and (4) has a prefix, so we can easily dintinguish
  char ch = Advance();
  if (ch == '0') {
    if (peek == 'x') {
      return ScanHexInteger();
    } else if (peek == 'o') {
      return ScanOctalInteger();
    } else if (peek == 'b') {
      return ScanBinaryInteger();
    }
    while (Match('0'));
  }
}

Token Scanner::ScanRealNumber(const std::string *integerPart,
                              bool scannedPeriod = false) {
  std::string number = integerPart ? *integerPart : '0';
  if (!scannedPeriod)
}

}