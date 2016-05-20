
Token Parser::Advance() {
#ifdef FLORA_DEBUG
  debug_logger_.Log("advance $1", Tokens::Name(peek));
#endif
  peek = scanner_.Advance();
}

bool Parser::Match(Token expected) {
  if (peek == expected) {
    Advance();
#ifdef FLORA_DEBUG
    debug_logger_.Log("match $1: success", Tokens::Name(expected));
#endif
    return true;
  }
#ifdef FLORA_DEBUG
  debug_logger_.Log("match $1: failure, met $2 instead",
                    Tokens::Name(expected), Tokens::Name(peek));
#endif
  return false;
}

TranslationUnit* Parser::ParseProgram(bool &ok) {
  SetUpParsing();

  Scope *global = NewScope(ScopeType::Global);
  Declaration *decl;
  std::vector<Declaration*> decls;
  while (true) {
    if (peek == Token::EndOfSource) {
      break;
    } else if (peek == Token::Using) {
      ParseUsingClause(ok);
    } else if (peek == Token::Namespace) {
      decl = ParseNamespaceDeclaration(ok);
      if (!ok) break;
    } else if (peek == Token::Class) {
      decl = ParseClassDeclaration(ok);
      if (!ok) break;
    } else if (peek == Token::Const) {
      decl = ParseConstantDeclaration(ok);
      if (!ok) break;
    } else { // Variable and function declaration
      decl = ParseVariableOrFunctionDeclaration(ok);
      if (!ok) break;
    }
  }
  if (!ok) {
    CleanUpParsing();
  }
  TranslationUnit *unit = new ast::TranslationUnit();
  unit->scope_tree_ = global;
  unit->declarations_ = std::move(decls);
  return unit;
}

UsingDeclaration* ParseUsingClause(bool &ok) {
  // using A.B.C.D [as E];
  Expect(Token::Using);

}

NamespaceDeclaration* ParseNamespace(bool &ok) {
  Expect(Token::Namespace);
  Scope *the_scope = NewScope(ScopeType::Namespace);
  if (peek != Token::Identifier) {
    ReportSyntaxError("missing name of the namespace");
    ok = false;
    return nullptr;
  }
  std::string the_name = scanner_.GetTokenLiteral();
  Advance();
  Expect(Token::LeftBrace);

  std::vector<Declaration*> decls;
  while (true) {
    switch (peek) {
    case Token::Namespace:
      decls.push_back(ParseNamespace(ok));
      break;
    case Token::Class:
      decls.push_back(ParseClass(ok));
      break;
    default:
      decls.push_back(ParseVariableOrFunction(ok));
      break;
    }
    if (!ok) return nullptr;
  }

  Expect(Token::RightBrace);

  return new ast::NamespaceDeclaration(the_name, the_scope ,decls);
}

ClassDeclaration* ParseClass(bool &ok) {
  Expect(Token::Class);
  std::string class_name = scanner_->GetTokenLiteral();
  // Parent classes
  if (peek == Token::LessThan) {

  }
  // Class body
  Expect(Token::LeftBrace);
  while (true) {
    MemberVisibility visibility;
    if (peek == Token::Public) {
      visibility = MemberVisibility::Public;
    } else if (peek == Token::Private) {
      visibility = MemberVisibility::Private;
    } else if (peek == Token::Protected) {
      visibility = MemberVisibility::Protected;
    } else if (peek == Token::Internal) {
      visibility = MemberVisibility::Internal;
    } else {

    }
    bool is_static = Match(Token::Static);
    
  }
  Expect(Token::RightBrace);
}

Block* Parser::ParseBlock(Scope *the_scope, bool &ok) {
  if (the_scope = nullptr) {
    the_scope = NewScope();
  }
  Expect(Token::LeftBrace);
  std::vector<Statement*> stmts;
  while (true) {
    switch (peek) {
    case Token::If:
      stmts.push_back(ParseIfStatement(ok));
      CHECK_ERROR(ok);
    case Token::For:
      stmts.push_back(ParseForStatement(ok));
      CHECK_ERROR(ok);
    case Token::While:
      stmts.push_back(ParseWhileStatement(ok));
      CHECK_ERROR(ok);
    case Token::DoWhile:
      stmts.push_back(ParseDoWhileStatement(ok));
      CHECK_ERROR(ok);
    default:

    }
  }
  Expect(Token::RightBrace);
}

Expression* ParseExpression(int rightBindingPower, bool &ok) {
  Expression *left = ParseNullDenotation(ok);
  if (!ok) return nullptr;
  while (rightBindingPower < Tokens::Precedence(peek)) {
    left = ParseLeftDenotation(left, ok);
    if (!ok) return nullptr;
  }
  return left;
}

Expression* Parser::ParseLeftDenotation(Expression *left, bool &ok) {
  Token op;
  switch (peek) {
  // Function application:
  case Token::LeftParenthesis:
    return ParseFunctionApplication(left, ok);
  // Indexing
  case Token::LeftBracket:
    return ParseIndex(left, ok);
  // Assignment operators
  case Token::Assignment:
  case Token::AssignmentBitwiseOr:
  case Token::AssignmentBitwiseXor:
  case Token::AssignmentBitwiseAnd:
  case Token::AssignmentShiftLeft:
  case Token::AssignmentShiftRight:
  case Token::AssignmentAddition:
  case Token::AssignmentSubtraction:
  case Token::AssignmentMultiplication:
  case Token::AssignmentDivision:
  case Token::AssignmentModulus:
    return new ast::AssignmentOperation()
  // Binary operators
  case Token::LogicalOr:
  case Token::LogicalAnd:
  case Token::BitwiseOr:
  case Token::BitwiseXor:
  case Token::BitwiseAnd:
  case Token::ShiftLeft:
  case Token::ShiftRight:
  case Token::Addition:
  case Token::Subtraction:
  case Token::Multiplication:
  case Token::Division:
  case Token::Modulus:
    op = peek;
    Advance();
    return new ast::ArithmeticOperation(op, left, ParseExpression())
  // Compare operators
  case Token::Equal:
  case Token::NotEqual:
  case Token::LessThan:
  case Token::GreaterThan:
  case Token::LessThanOrEqual:
  case Token::GreaterThanOrEqual:

  default:
    ReportSyntaxError("");
    ok = false;
    return nullptr;
  }
}

Expression* Parser::ParseNullDenotation(bool &ok) {
  switch (peek) {
  case Token::EndOfSource:
    ReportSyntaxError("expect expressions instead of end of source");
    ok = false;
    return nullptr;
  case Token::LeftParenthesis:
    return ParseExpressionOrTupleOrConversion();
  case Token::LeftBracket:
    return ParseArrayLiteral();
  case Token::Addition:
  case Token::Subtraction:
  case Token::LogicalNot:
  case Token::BitwiseNot:
  case Token::Increment:
  case Token::Decrement:

  case Token::Integer:
  case Token::RealNumber:
  case Token::String:
  case Token::Character:
  }
}

Expression* ParseExpressionOrTupleOrConversion(bool &ok) {
  bool success = false;
  // conversion
  TypeSpecifier type = ParseTypeSpecifier(success);
  if (success) {
    Expect(Token::RightParenthesis);
    Expression *expr = ParseExpression();
    return new ast::Conversion(type, expr);
  }
  // parenthesis expression or tuple
  std::vector<Expression*> tuples;
  Expression *expr = nullptr;
  while (true) {
    expr = ParseExpression(0, ok);
    if ()
  }
}

Expression* ParseFunctionApplication(Expression *func, bool &ok) {
  std::vector<Expression*> arguments;
  Expect(Token::LeftParenthesis);
  if (peek != Token::RightParenthesis) {
    do {
      Expression *arg = ParseExpression(0, ok);
      CHECK_ERROR(ok);
      arguments.push_back(arg);
    } while (Match(Token::Comma));
  }
  Expect(Token::RightParenthesis);
  return new ast::Invoke(func, arguments);
}

Expression* ParseArrayLiteral(bool &ok) {
  std::vector<Expression*> array_exprs;
  while (true) {
    if (Match(Token::RightBracket)) break;
    array_exprs.push_back(ParseExpression(0, ok));
    if (!ok) return nullptr;
    Expect
  }
}

ForStatement* ParseForStatement(bool &ok) {
  // Create the scope
  Scope *the_scope = NewScope(ScopeType::Block);
  // Parse for (
  Expect(Token::For);
  Expect(Token::LeftParenthesis);
  union {
    Expression *init_expr;
    VariableDeclaration *init_decl;
  };
  bool init_is_decl;
  scanner_->SaveBookmark();
  init_decl = ParseVariableDeclaration(init_is_decl);
  if (init_is_decl) {
    scanner_->ClearBookmark();
  } else {
    scanner_->LoadBookmark();
    init_expr = ParseExpression(0, ok);
  }
  // Parse ; cond
  Expect(Token::Semicolon);
  Expression *cond = ParseExpression(0, ok);
  Expect(Token::Semicolon);
  Expression *extra = ParseExpression(0, ok);
  Expect(Token::RightParenthesis);
  // Parse the loop body
  if (peek == Token::LeftBrace) {
    Block *body = ParseBlock(the_scope, ok);
  } else {
    Statement *body = ParseStatement(ok);
  }
  return new ast::ForStatement();
}

WhileStatement* ParseWhileStatement(bool &ok) {
  Expect(Token::While);
  Expect(Token::LeftParenthesis);
  Expression *cond = ParseExpression(0, ok);
  RETURN_IF_ERROR(ok);
  Expect(Token::RightParenthesis);
  if (peek == Token::LeftBrace) {
    Block *body = ParseBlock(the_scope, ok);
    RETURN_IF_ERROR(ok);
  } else {
    Statement *body = ParseStatement(ok);
    RETURN_IF_ERROR(ok);
  }
}