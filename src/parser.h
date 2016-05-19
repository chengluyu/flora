#ifndef FLORA_PARSER_H
#define FLORA_PARSER_H

#include "flora.h"
#include "token.h"
#include "scanner.h"

#define CHECK_ERROR(ok) if (!ok) return nullptr

namespace flora {

class Parser {
public:
  NOCOPY_CLASS(Parser)

  Parser();
  ~Parser();

private:

#ifdef FLORA_DEBUG
  // debugger printer
  ParserDebugLogger debug_logger_;
  ParserCallStackTracer call_stack_tracer_;
#endif
  // The scanner
  Scanner scanner_;
  // The current token
  Token peek;
  INLINE(Token Advance());
  INLINE(void Expect(Token expected));
  INLINE(bool Match(Token expected));

  // current scope
  Scope *current_;

  Scope* NewScope();

  void ReportError(const char *message, Array<const char*> args);
  void ReportErrorAt(Scanner::Location location,
                     const char *message,
                     Array<const char*> args);
  void ReportUnexpectedToken(Token expected);

  NamespaceDeclararion* ParseNamespaceDeclaration(bool &ok);
  ClassDeclararion* ParseClassDeclaration(bool &ok);
  FunctionDeclararion* ParseFunctionDeclaration(bool &ok);
  ConstantDeclararion* ParseConstantDeclaration(bool &ok);
  VariableDeclararion* ParseVariableDeclaration(bool &ok);

  Block* ParseBlock(Scope *the_scope, bool &ok);
  Statement* ParseStatement(bool &ok);
  IfStatement* ParseIfStatement(bool &ok);
  ForStatement* ParseForStatement(bool &ok);
  WhileStatement* ParseWhileStatement(bool &ok);
  DoWhileStatement* ParseDoWhileStatement(bool &ok);
  CaseClause* ParseCaseClause(bool &ok);
  SwitchStatement* ParseSwitchStatement(bool &ok);

  TypeSpecifier* ParseTypeSpecifier(bool &ok);

  Expression* ParseExpression(bool &ok);
  Expression* ParseNullDenotation(bool &ok);
  Expression* ParseLeftDenotation(Expression *left, bool &ok);
  Expression* ParseCondition(Expression *cond, bool &ok);

  Expression* ParseArrayLiteral(bool &ok);
  Expression* ParseLambdaExpression(bool &ok);

};

}

#endif