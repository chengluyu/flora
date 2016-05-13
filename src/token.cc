#include "token.h"

namespace flora {

#define T(name, literal, precedence) #name
const char* const Tokens::name_[Tokens::TOKEN_COUNT] = {
  TOKEN_LIST(T)
};
#undef T

#define T(name, literal, precedence) #literal
const char* const Tokens::literal_[Tokens::TOKEN_COUNT] = {
  TOKEN_LIST(T)
};
#undef T

#define T(name, literal, precedence) precedence
const int Tokens::precedence_[Tokens::TOKEN_COUNT] = {
  TOKEN_LIST(T)
};
#undef T

}