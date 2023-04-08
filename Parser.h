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
  shared_ptr<Program> parse();
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
  shared_ptr<Program> ParseProgram();
  shared_ptr<MainClass> ParseMainClass();
  shared_ptr<ClassDecl> ParseClassDecl();
  shared_ptr<VarDecl> ParseVarDecl();
  shared_ptr<MethodDecl> ParseMethodDecl();

  shared_ptr<Argument> ParseArgument();

  shared_ptr<Type> ParseType();
  // We need this to parse int types.
  // Arrays and int types has the same start symbols
  // int x; 
  // int [] x;
  shared_ptr<Type> ParseTypeHelper();
  shared_ptr<BoolType> ParseBoolType();
  shared_ptr<IdentifierType> ParseIdType();

  shared_ptr<Statement> ParseStatement();
  shared_ptr<Block> ParseBlock();
  shared_ptr<If> ParseIf();
  shared_ptr<While> ParseWhile();
  shared_ptr<Print> ParsePrint();
  shared_ptr<Statement> ParseStatementHelper();

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
  shared_ptr<Expression> ParseExpression();
  shared_ptr<Expression> ParseAlpha(shared_ptr<Expression> LHS);
  // Parse binary operations using Operator-Precedence Parsing
  shared_ptr<BinOpExpression> ParseBinOpExpression(int ExpPerc,
                                                   shared_ptr<Expression> LHS);
  int GetPercedence(TOK op);
  shared_ptr<ArrayLookup> ParseArrayLookup(shared_ptr<Expression> LHS);
  shared_ptr<Expression> ParseDotHelper(shared_ptr<Expression> LHS);
  shared_ptr<ArrayLength> ParseArrayLength(shared_ptr<Expression> LHS);
  shared_ptr<Call> ParseCallExpression(shared_ptr<Expression> LHS);

  shared_ptr<Expression> ParseBeta();
  shared_ptr<IntegerLiteral> ParseIntLit();
  shared_ptr<True> ParseTrue();
  shared_ptr<False> ParseFalse();
  shared_ptr<IdentifierExp> ParseIdExp();
  shared_ptr<NewArray> ParseNewArray();
  shared_ptr<NewObject> ParseNewObject();
  shared_ptr<Not> ParseNot();
  shared_ptr<Expression> ParseParenExpr();
  shared_ptr<Expression> ParseNewHelper();
  shared_ptr<This> ParseThis();

  shared_ptr<Expression> ParseGoodExpression(shared_ptr<Expression> LHS);

  shared_ptr<Identifier> ParseIdentifier();

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