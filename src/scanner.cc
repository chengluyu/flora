#include "scanner.h"

namespace flora {

// Public Methods

Scanner::Scanner() {

}

void Scanner::Initialize() {
  UNIMPLEMENTED();
}

Token Scanner::Advance() {
  switch (state_) {
  case Scanner::State::Uninitialized:
    ReportScannerError("the scanner is uninitialized");
    return Token::Illegal;

  case Scanner::State::Running:
    return Scan();

  case Scanner::State::Recording: {
    Token token = Scan();
    records_.push(std::make_pair(token, literal_));
    return token;
  }

  case Scanner::State::Restoring: {
    std::pair<Token, std::string> &token = records_.front();
    SetTokenLiteral(token.second);
    if (records_.empty())
      state_ = Scanner::State::Running;
    return token.first;
  }

  case Scanner::State::End:
    return Token::EndOfSource;

  case Scanner::State::Error:
    return Token::Illegal;
  }
  UNREACHABLE();
}

void Scanner::SaveBookmark() {
  // Clean the recording queue
  while (!records_.empty())
    records_.pop();
  // Set state to recording
  state_ = Scanner::State::Recording;
}

void Scanner::LoadBookmark() {
  // Set state to restoring
  state_ = Scanner::State::Restoring;
}

void Scanner::ClearBookmark() {
  // Clean the recording queue
  while (!records_.empty())
    records_.pop();
  // Set state to running
  state_ = Scanner::State::Running;
}


// Private methods

void Scanner::SetTokenLiteral(const char *literal) {
  literal_.assign(literal);
}

void Scanner::SetTokenLiteral(std::string &literal) {
  literal_ = std::move(literal);
}

void Scanner::SetTokenLiteral(char literal) {
  literal_.clear();
  literal_.push_back(literal);
}


void Scanner::ClearTokenLiteral() {
  literal_.clear();
}

void Scanner::ReportScannerError(const char *message) {
  SetTokenLiteral(message);
}

void Scanner::MarkEndOfSource() {
  state_ = Scanner::State::End;
}


Token Scanner::Scan() {
  ClearTokenLiteral();
  char ch;
  while (true) {
    switch (ch = Next()) {
    case character::EOS:
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
      } else if (Match('/')) {
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
      } else if (character::IsDecimalDigit(ch)) {
        return ScanIntegerOrRealNumber(ch);
      } else {
        ReportScannerError("unknown token");
        return Token::Illegal;
      }
    }
  }
}

bool Scanner::SkipMultipleLineComment() {
  int cascade = 1;
  char ch;
  while (true) {
    ch = Next();
    if (ch == '/') {
      if (peek == '*') {
        Next();
        cascade++;
      }
    } else if (ch == '*') {
      if (peek == '/') {
        Next();
        cascade--;
        if (cascade == 0) break;
      }
    } else if (ch == character::EOS) {
      ReportScannerError("unexpected EOF in multiple line comment");
      return false;
    }
  }
  return true;
}

void Scanner::SkipSingleLineComment() {
  while (!character::IsLineFeed(peek) && peek != character::EOS)
    Next();
}

Token Scanner::ScanStringLiteral() {
  std::string literal;
  char ch;
  while (true) {
    ch = Next();
    if (ch == character::EOS) {
      ReportScannerError("unexpected EOF in string literal");
      return Token::Illegal;
    } else if (ch == '\\') {
      literal.push_back(ScanStringEscape());
    } else if (ch == '"') {
      break;
    } else {
      literal.push_back(ch);
    }
  }
  SetTokenLiteral(literal);
  return Token::String;
}

Token Scanner::ScanCharacterLiteral() {
  char literal = Next();
  if (literal == character::EOS) {
    ReportScannerError("unexpected EOF in character literal");
    return Token::Illegal;
  } else if (literal == '\\') {
    literal = ScanCharacterEscape();
  }
  // to ensure that there is only one character in literal
  if (Next() != '\'') {
    ReportScannerError("too more character in literal");
    return Token::Illegal;
  }
  SetTokenLiteral(literal);
  return Token::Character;
}

Token Scanner::ScanIdentifierOrKeyword(char firstChar) {
  std::string identifier;
  identifier.push_back(firstChar);

  while (character::IsIdentifierBody(peek)) {
    identifier.push_back(Next());
  }
  Token token = Tokens::LookupKeyword(identifier);
  if (token != Token::Identifier) SetTokenLiteral(identifier);
  return token;
}

Token Scanner::ScanIntegerOrRealNumber(char firstChar) {
  // There are 4 kinds of integer:
  // (1) Hexidecimal integer
  // (2) Decimal integer
  // (3) Octal integer
  // (4) Binary integer
  // (1), (3) and (4) has a prefix, so we can easily dintinguish them
  if (firstChar == '0') {
    if (peek == 'x') {
      return ScanHexInteger();
    } else if (peek == 'o') {
      return ScanOctalInteger();
    } else if (peek == 'b') {
      return ScanBinaryInteger();
    }
    // Skip leading zeros
    while (Match('0'))
      ;
  }
}

Token Scanner::ScanRealNumber(const std::string *integerPart,
                              bool scannedPeriod = false) {
  std::string number = integerPart ? *integerPart : '0';
  
}

}