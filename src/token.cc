#include "token.h"

namespace flora {

Token Tokens::LookupKeyword(const std::string &identifier) {
  auto result = lookup_table_.find(identifier);
  return result == lookup_table_.end() ? result->second : Token::Identifier;
}

#define T(name, literal, precedence) #name,
const char *Tokens::name_[Tokens::TOKEN_COUNT] = {
  TOKEN_LIST(T, T)
};
#undef T

#define T(name, literal, precedence) #literal,
const char *Tokens::literal_[Tokens::TOKEN_COUNT] = {
  TOKEN_LIST(T, T)
};
#undef T

#define T(name, literal, precedence) precedence,
const int Tokens::precedence_[Tokens::TOKEN_COUNT] = {
  TOKEN_LIST(T, T)
};
#undef T

std::unordered_map<std::string, Token> Tokens::lookup_table_;

void Tokens::InitializeKeywordLookupTable() {
#define K(name, literal, precedence) lookup_table_[std::string(literal)] = Token::name;
#define T(name, literal, precedence)
  TOKEN_LIST(K, T)
#undef K
#undef T
}

Tokens::Tokens() {
  InitializeKeywordLookupTable();
}

}