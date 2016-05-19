#include "debug.h"
#include "scanner.h"

namespace flora {

// Public Methods

Scanner::Scanner() {
  state_ = Scanner::State::Uninitialized;
  stream_ = nullptr;
  peek = 0;
}

Scanner::~Scanner() {
  
}

void Scanner::Initialize(CharacterStream *stream) {
  stream_ = stream;
  state_ = Scanner::State::Running;
  peek = stream->Advance();
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
    // Sequeeze the first recorded token
    std::pair<Token, std::string> &token = records_.front();
    records_.pop();
    // Check if no more recorded tokens
    if (records_.empty())
      state_ = Scanner::State::Running;
    SetTokenLiteral(token.second);
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

const std::string& Scanner::GetTokenLiteral() {
  return literal_;
}

// Private methods

char32_t Scanner::Next() {
  char32_t save = peek;
  peek = stream_->Advance();
  return save;
}

bool Scanner::Match(char32_t expected) {
  if (expected == peek) {
    peek = stream_->Advance();
    return true;
  }
  return false;
}

void Scanner::SetTokenLiteral(const char *literal) {
  literal_ = literal;
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
  state_ = Scanner::State::Error;
  SetTokenLiteral(message);
}

void Scanner::MarkEndOfSource() {
  state_ = Scanner::State::End;
}


Token Scanner::Scan() {
  ClearTokenLiteral();
  char32_t ch;
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
      ReportScannerError("unexpected end of source in multiple line comment");
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
  char32_t ch;
  while (true) {
    ch = Next();
    if (ch == character::EOS) {
      ReportScannerError("unexpected EOF in string literal");
      return Token::Illegal;
    } else if (ch == '\\') {
      ch = ScanCharacterEscape();
      if (ch == character::EOS)
        return Token::Illegal;
      literal.push_back(ch);
    } else if (ch == '"') {
      break;
    } else if (ch == '\n') {
      ReportScannerError("unexpected line feed in string literal");
      return Token::Illegal;
    } else {
      literal.push_back(ch);
    }
  }
  SetTokenLiteral(literal);
  return Token::String;
}

Token Scanner::ScanCharacterLiteral() {
  char32_t literal = Next();
  if (literal == character::EOS) {
    ReportScannerError("unexpected EOF in character literal");
    return Token::Illegal;
  } else if (literal == '\\') {
    literal = ScanCharacterEscape();
    if (literal == character::EOS)
      return Token::Illegal;
  } else if (literal == '\n') {
    ReportScannerError("unexpected line feed in character literal");
    return Token::Illegal;
  }
  // to ensure that there is only one character in literal
  if (Next() != '\'') {
    ReportScannerError("too more character in literal");
    return Token::Illegal;
  }
  SetTokenLiteral(literal);
  return Token::Character;
}

char32_t Scanner::ScanCharacterEscape() {
  char32_t codepoint = 0;
  switch (peek) {
  case 'n': return '\n';
  case 'r': return '\r';
  case 't': return '\t';
  case '\\': return '\\';
  case '\'': return '\'';
  case '\"': return '\"';
  case 'u':
    Next();
    while (character::IsDecimalDigit(peek)) {
      codepoint *= 10;
      codepoint += peek - '0';
      Next();
    }
    return codepoint;
  case 'x':
    Next();
    while (character::IsHexDigit(peek)) {
      codepoint *= 16;
      if (character::IsDecimalDigit(peek)) {
        codepoint += peek - '0';
      } else {
        codepoint += 10 + character::AsciiToLowerCase(peek) - 'a';
      }
      Next();
    }
    return codepoint;
  default:
    ReportScannerError("illegal character escape");
    return character::EOS;
  }
}

Token Scanner::ScanIdentifierOrKeyword(char32_t firstChar) {
  std::string identifier;
  identifier.push_back(firstChar);

  while (character::IsIdentifierBody(peek)) {
    identifier.push_back(Next());
  }
  Token token = Tokens::LookupKeyword(identifier);
  if (token == Token::Identifier)
    SetTokenLiteral(identifier);
  return token;
}

Token Scanner::ScanIntegerOrRealNumber(char32_t firstChar) {
  // There are 4 kinds of integer:
  // (1) Hexidecimal integer
  // (2) Decimal integer
  // (3) Octal integer
  // (4) Binary integer
  // (1), (3) and (4) has a prefix, so we can easily dintinguish them
  std::string num;
  if (firstChar == '0') {
    if (peek == 'x') {
      Next();
      return ScanHexInteger();
    } else if (peek == 'o') {
      Next();
      return ScanOctalInteger();
    } else if (peek == 'b') {
      Next();
      return ScanBinaryInteger();
    }
  }
  num.push_back(firstChar);
  // Scan the integral part
  while (character::IsDecimalDigit(peek)) {
    num.push_back(Next());
  }
  // Real number
  if (peek == '.' || character::AsciiToLowerCase(peek) == 'e') {
    return ScanRealNumber(&num);
  }
  // Integer
  SetTokenLiteral(num);
  return Token::Integer;
}

Token Scanner::ScanRealNumber(const std::string *integral_part,
                              bool scanned_period) {
  std::string num;
  if (integral_part) {
    num = *integral_part;
  } else {
    num.push_back('0');
  }
  // Fraction part
  if (Match('.')) {
    while (character::IsDecimalDigit(peek)) {
      num.push_back(Next());
    }
  }
  // Exponent part
  if (peek == 'E' || peek == 'e') {
    num.push_back(Next());
    if (peek == '+' || peek == '-')
      num.push_back(Next());
    // An error circumstance: 1.234E
    if (!character::IsDecimalDigit(peek)) {
      ReportScannerError("unexpected end of source in real number literal");
      return Token::Illegal;
    }
    while (character::IsDecimalDigit(peek)) {
      num.push_back(Next());
    }
  }
  SetTokenLiteral(num);
  return Token::RealNumber;
}

#define SCAN_INTEGER(base, ch, checker)\
  Token Scanner::Scan##base##Integer() {\
    std::string num;\
    num.push_back(ch);\
    if (!checker(peek)) {\
      ReportScannerError("unexpected end of source in integer literal");\
      return Token::Illegal;\
    }\
    while (checker(peek))\
      num.push_back(Next());\
    SetTokenLiteral(num);\
    return Token::Integer;\
  }

SCAN_INTEGER(Hex, 'x', character::IsHexDigit)
SCAN_INTEGER(Octal, 'o', character::IsOctalDigit)
SCAN_INTEGER(Binary, 'b', character::IsBinaryDigit)

}