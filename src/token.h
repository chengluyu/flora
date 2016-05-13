#ifndef FLORA_TOKEN_H
#define FLORA_TOKEN_H

#include "flora.h"

namespace flora {

#define TOKEN_LIST(K)\
  K(EndOfSource)\
  /* Punctuation marks */ \
  K(LeftParenthesis, "(", 0)\
  K(RightParenthesis, ")", 0)\
  K(LeftBracket, "[", 0)\
  K(RightBracket, "]", 0)\
  K(LeftBrace, "{", 0)\
  K(RightBrace, "}", 0)\
  K(Colon, ":", 0)\
  K(Semicolon, ";", 0)\
  K(Period, ".", 0)\
  K(Ellipsis, "...", 0)\
  K(Conditional, "?", 0)\
  K(Increment, "++", 0)\
  K(Decrement, "--", 0)\
  K(Arrow, "=>", 0)\
  /* Assignment operators */\
  K(Assignment, "=", 2)\
  K(AssignmentBitwiseOr, "|=", 2)\
  K(AssignmentBitwiseXor, "^=", 2)\
  K(AssignmentBitwiseAnd, "&=", 2)\
  K(AssignmentShiftLeft, "<=<",2)\
  K(AssignmentShiftRight, ">=>",2)\
  K(AssignmentAddition, "+=", 2)\
  K(AssignmentSubtraction, "-=", 2)\
  K(AssignmentMultiplication, "*=", 2)\
  K(AssignmentDivision, "/=", 2)\
  K(AssignmentModulus, "%=", 2)\
  /* Binary operators */\
  K(Comma, ",", 1)\
  K(LogicalOr, "||", 4)\
  K(LogicalAnd, "&&", 4)\
  K(BitwiseOr, "|", 6)\
  K(BitwiseXor, "^", 7)\
  K(BitwiseAnd, "&", 8)\
  K(ShiftLeft, "<<", 11)\
  K(ShiftRight, ">>", 11)\
  K(Addition, "+", 12)\
  K(Subtraction, "-", 12)\
  K(Multiplication, "*", 12)\
  K(Division, "/", 13)\
  K(Modulus, "%", 13)\
  /* Compare operators */\
  K(Equal, "==", 9)\
  K(NotEqual, "!=", 9)\
  K(LessThan, "<", 10)\
  K(GreaterThan, ">", 10)\
  K(LessThanOrEqual, "<=", 10)\
  K(GreaterThanOrEqual, ">=", 10)\
  /* Unary operators */\
  K(Not, "!", 0)\
  K(BitwiseNot, "~", 0)\
  /* Keywords */\
  K(As, "as", 0)\
  K(Break, "break", 0)\
  K(Case, "case", 0)\
  K(Catch, "catch", 0)\
  K(Class, "class", 0)\
  K(Const, "const", 0)\
  K(Continue, "continue", 0)\
  K(Default, "default", 0)\
  K(Do, "do", 0)\
  K(Else, "else", 0)\
  K(Enum, "enum", 0)\
  K(Export, "export", 0)\
  K(Finally, "finally", 0)\
  K(For, "for", 0)\
  K(From, "from", 0)\
  K(If, "if", 0)\
  K(Import, "import", 0)\
  K(New, "new", 0)\
  K(Private, "private", 0)\
  K(Public, "public", 0)\
  K(Return, "return", 0)\
  K(Static, "static", 0)\
  K(Switch, "switch", 0)\
  K(This, "this", 0)\
  K(Throw, "throw", 0)\
  K(Try, "try", 0)\
  K(While, "while", 0)\
  /* Built-in types */\
  K(Bool, "bool", 0)\
  K(Char, "char", 0)\
  K(Double, "double", 0)\
  K(Float, "float", 0)\
  /* Literals */\
  K(Null, "null", 0)\
  K(True, "true", 0)\
  K(False, "false", 0)\
  K(Integer, nullptr, 0)\
  K(RealNumber, nullptr, 0)\
  K(Character, nullptr, 0)\
  K(String, nullptr, 0)\
  /* Identifier */\
  K(Identifier, nullptr, 0)\
  /* Illegal token */\
  K(Illegal, nullptr, 0)


enum class Token : int {
#define T(name, string, precedence) name,
  TOKEN_LIST(T),
  TOKEN_COUNT
#undef T
};

class Tokens {

  STATIC_CLASS(Tokens)

  static const char* Name(Token token) {
    return name_[static_cast<int>(token)];
  }

  static const char* Literal(Token token) {
    return literal_[static_cast<int>(token)];
  }

  static int Precedence(Token token) {
    return precedence_[static_cast<int>(token)];
  }

private:
  static const int TOKEN_COUNT = static_cast<int>(TOKEN_COUNT);

  static const char *name_[TOKEN_COUNT];
  static const char *literal_[TOKEN_COUNT];
  static const int precedence_[TOKEN_COUNT];
};

}
#endif