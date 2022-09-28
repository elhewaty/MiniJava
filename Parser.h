#ifndef _PARSER_H_
#define _PARSER_H_

#include "Lexer.h"
#include "AST.h"
#include <utility>
#include <string>
#include <stack>

using namespace std;

class Parser {
public:
  Parser(std::string filename): l(filename) {}
  Program* parse();
  vector<pair<string, string>> getEdges() const;
  vector<pair<string, string>> getOrder() const;
private:
  Lexer l;
  ErrorHandling err;
  vector<pair<string, string>> edges;
  // Used in top-sort to build symbol table in top-sort order.
  // See ClassFilter.
  vector<pair<string, string>> OrderEdge;

  // Store the value of the current class we parse.
  stack<string>CurClass;
  Program* ParseProgram();
  MainClass* ParseMainClass();
  ClassDecl* ParseClassDecl();
  VarDecl* ParseVarDecl();
  MethodDecl* ParseMethodDecl();

  Argument* ParseArgument();

  Type* ParseType();
  // We need this to parse int types.
  // Arrays and int types has the same start symbols
  // int x; 
  // int [] x;
  Type* ParseTypeHelper();
  BoolType* ParseBoolType();
  IdentifierType* ParseIdType();

  Statement* ParseStatement();
  Block* ParseBlock();
  If* ParseIf();
  While* ParseWhile();
  Print* ParsePrint();
  Statement* ParseStatementHelper();

  // To parse expression we should elimenate left recuresion.
  // Expression ::= Expression ( "&&" | "<" | "+" | "-" | "*" ) Expression
  // | 	Expression "[" Expression "]"
  // | 	Expression "." "length"
  // | 	Expression "." Identifier "(" ( Expression ( "," Expression )* )? ")"
  // | 	<INTEGER_LITERAL>
  // | 	"true"
  // | 	"false"
  // | 	Identifier
  // | 	"this"
  // | 	"new" "int" "[" Expression "]"
  // | 	"new" Identifier "(" ")"
  // | 	"!" Expression
  // | 	"(" Expression ")"
  // This is equivelent to:
  // Expression ::= Expression Alpha | Beta
  // We convert this grammar to:
  // Expression ::= Beta GoodExpression
  // GoodExpression ::= Alpha GoodExpression | epselon

  // Alpha ::= ( "&&" | "<" | "+" | "-" | "*" ) Expression
  // | "[" Expression "]"
  // | "." "length"
  // | "." Identifier "(" ( Expression ( "," Expression )* )? ")"

  // Beta ::= <INTEGER_LITERAL>
  // | "true"
  // | 	"false"
  // | 	Identifier
  // | 	"this"
  // | 	"new" "int" "[" Expression "]"
  // | 	"new" Identifier "(" ")"
  // | 	"!" Expression
  // | 	"(" Expression ")"
  Expression* ParseExpression();
  Expression* ParseAlpha(Expression* LHS);
  // Parse binary operations using Operator-Precedence Parsing
  BinOpExpression* ParseBinOpExpression(int ExpPerc, Expression* LHS);
  int GetPercedence(TOK op);
  ArrayLookup* ParseArrayLookup(Expression* LHS);
  Expression* ParseDotHelper(Expression* LHS);
  ArrayLength* ParseArrayLength(Expression* LHS);
  Call* ParseCallExpression(Expression* LHS);

  Expression* ParseBeta();
  IntegerLiteral* ParseIntLit();
  True* ParseTrue();
  False* ParseFalse();
  IdentifierExp* ParseIdExp();
  NewArray* ParseNewArray();
  NewObject* ParseNewObject();
  Not* ParseNot();
  Expression* ParseParenExpr();
  Expression* ParseNewHelper();
  This* ParseThis();

  Expression* ParseGoodExpression(Expression* LHS);

  Identifier* ParseIdentifier();

  // Here we define some helper functions
  // to help us decide which grammer production
  // we should follow.

  // Variable Declarations
  bool IsVarStart(TOK tk) {
    return tk == INT || tk == BOOL || tk == ID;
  }

  // Method
  bool IsMethodStart(TOK tk) {
    return tk == PUBLIC;
  }

  // Class
  bool IsClassStart(TOK tk) {
    return tk == CLASS;
  }

  // Statement
  bool IsStatementStart(TOK tk) {
    return tk == L_BRACKET
           || tk == IF
           || tk == WHILE
           || tk == PRINT
           || tk == ID;
  }

  // Binary operations
  bool IsBinOp(TOK tk) {
    return tk > __OP_START && tk < __OP_END && tk != NOT;
  }

  // epsilon
  bool IsStillGood(TOK tk) {
    return tk == DOT || tk == L_SBRACKET|| IsBinOp(tk);
  }
};

#endif