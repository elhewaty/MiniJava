#include "Parser.h"

vector<pair<string, string>> Parser::getEdges() const {
  return edges;
}

vector<pair<string, string>> Parser::getOrder() const {
  return OrderEdge;
}

shared_ptr<Identifier> Parser::ParseIdentifier() {
  if(l.getCurrentToken() != ID && l.getCurrentToken() != MAIN) {
    err.emit(l.getLocation(), "expected identifier");
    return nullptr;
  }
  string IdName = l.getIdToken();
  l.getNextToken(); // eat ID
  return make_shared<Identifier>(IdName, l.getLocation());
}

shared_ptr<Type> Parser::ParseType() {
  shared_ptr<Type> re;
  switch(l.getCurrentToken()) {
    case INT:
      re = ParseTypeHelper();
      break;
    case BOOL:
      re = ParseBoolType();
      break;
    case ID:
      re = ParseIdType();
      OrderEdge.push_back({re->stringize(), CurClass.top()});
      break;
    default:
      err.emit(l.getLocation(), "expected type declaration.");
      break;
  }
  return re;
}

shared_ptr<IdentifierType> Parser::ParseIdType() {
  string IdName = l.getIdToken();
  l.eat(ID, "type name");
  return make_shared<IdentifierType>(IdName, l.getLocation());
}

shared_ptr<Type> Parser::ParseTypeHelper() {
  l.eat(INT, "int");
  if(l.getCurrentToken() != L_SBRACKET) {
    return make_shared<IntType>(l.getLocation());
  }
  l.eat(L_SBRACKET, "[");
  l.eat(R_SBRACKET, "]");
  return make_shared<ArrayType>(l.getLocation());
}

shared_ptr<BoolType> Parser::ParseBoolType() {
  l.eat(BOOL, "boolean");
  return make_shared<BoolType>(l.getLocation());
}

shared_ptr<VarDecl> Parser::ParseVarDecl() {
  auto t = ParseType();
  if(!t) {
    return nullptr;
  }
  auto i = ParseIdentifier();
  if(!i) {
    if(!IsVarStart(l.getCurrentToken())) {
      l.getNextToken(); // Eat the Token
    }
    return nullptr;
  }
  l.eat(S_COLON, ";");
  return make_shared<VarDecl>(t, *i, l.getLocation());
}


shared_ptr<Program> Parser::ParseProgram() {
  l.getNextToken();
  if(!IsClassStart(l.getCurrentToken())) {
    cerr << "syntax error...\nPlease create a class for your project"
         << endl;
    err.error();
    err.failure();
  }
  auto m = ParseMainClass();
  if(!m) {
    return nullptr;
  }
  ClassDeclList cl;
  while(IsClassStart(l.getCurrentToken())) {
    auto c = ParseClassDecl();
    if(c) {
      cl.add(c);
    } else {
      cerr << "Skipping Parsing operation for This class." << endl;
      while(l.getCurrentToken() != CLASS) {
        l.getNextToken();
      }
    }
  }
  if(l.getCurrentToken() != TOK_EOF) {
    err.emit(l.getLocation(), "You can only define classes here");
    err.failure();
  }
  return make_shared<Program>(*m, cl, l.getLocation());
}

shared_ptr<MainClass> Parser::ParseMainClass() {
  // class class_name{ public static void main(string [] a) {statement}}
  l.eat(CLASS, "class");
  auto c = ParseIdentifier();
  l.eat(L_BRACKET, "{");
  l.eat(PUBLIC, "public"); l.eat(STATIC, "static"); l.eat(VOID, "void");
  l.eat(MAIN, "main"); l.eat(L_PAREN, "("); l.eat(STRING, "String");
  l.eat(L_SBRACKET, "["); l.eat(R_SBRACKET, "]");
  auto a = ParseIdentifier();
  l.eat(R_PAREN, ")");
  l.eat(L_BRACKET, "{");
  auto s = ParseStatement();
  if(!c || !s || !a) {
    if(!s) {
      cerr << "Maybe the main function is empty" << endl;
    }
    cerr << "Cannot compile Main class, please hava a look at the `main` "
              << "funcation" << endl;
    err.failure();
  }
  l.eat(R_BRACKET, "}");
  l.eat(R_BRACKET, "}");
  return make_shared<MainClass>(*c, *a, s, l.getLocation());
}

shared_ptr<ClassDecl> Parser::ParseClassDecl() {
  // "class" Identifier ( "extends" Identifier )?
  //    "{" ( VarDeclaration )* ( MethodDeclaration )* "}"
  l.eat(CLASS, "class");
  auto i = ParseIdentifier();
  if(!i) {
    return nullptr;
  }
  shared_ptr<Identifier> j = nullptr;
  if(l.getCurrentToken() == EXTENDS) {
    l.eat(EXTENDS, "extends");
    j = ParseIdentifier();
  }
  l.eat(L_BRACKET, "{");
  VarDeclList vl;
  CurClass.push(i->s);
  while(IsVarStart(l.getCurrentToken())) {
    if(l.getCurrentToken() == ID) {
      TOK t = l.lookahead();
      if(t == ASSIGN || t == L_SBRACKET){
        err.emit(l.getLocation(), "Assign Statement in class body");
        err.failure();
      }
    }
    auto v = ParseVarDecl();
    if(!v) {
      while(l.getCurrentToken() != S_COLON) {
        l.getNextToken();
      }
      l.getNextToken();
      continue;
    }
    vl.add(*v);
  }
  MethodDeclList ml;
  while(IsMethodStart(l.getCurrentToken())) {
    auto m = ParseMethodDecl();
    if(!m) {
      cerr << "Skipping Parsing operation for this method" << endl;
      while(!IsMethodStart(l.getCurrentToken())
            && l.getCurrentToken() != RETURN) {
        l.getNextToken();
      }
      l.getNextToken(); // Eat return
      while(l.getCurrentToken() != R_BRACKET) {
        l.getNextToken();
      }
      l.getNextToken(); // Eat `}`
      continue;
    }
    ml.add(*m);
  }
  l.eat(R_BRACKET, "}");
  CurClass.pop();
  if(j) {
    edges.push_back({i->s, j->s});
    OrderEdge.push_back({j->s, i->s});
    return make_shared<ClassDeclExtends>(*i, vl, ml, *j, l.getLocation());
  }
  OrderEdge.push_back({i->s, ""});
  edges.push_back({i->s, ""});
  return make_shared<ClassDeclSimple>(*i, vl, ml, l.getLocation());
}

shared_ptr<Argument> Parser::ParseArgument() {
  // Type Identifier
  auto t = ParseType();
  if(!t) {
    return nullptr;
  }
  auto i = ParseIdentifier();
  if(!i) {
    return nullptr;
  }
  return make_shared<Argument>(t, *i, l.getLocation());
}

shared_ptr<MethodDecl> Parser::ParseMethodDecl() {
  // "public" Type Identifier
  // "(" ( Type Identifier ( "," Type Identifier )* )? ")"
  // "{" ( VarDeclaration )* ( Statement )* "return" Expression ";" "}"
  l.eat(PUBLIC, "public"); 
  auto t = ParseType();
  if(!t) {
    return nullptr;
  }
  auto i = ParseIdentifier();
  if(!i) {
    return nullptr;
  }
  l.eat(L_PAREN, "(");
  ArgList al;
  if(l.getCurrentToken() != R_PAREN) {
    shared_ptr<Argument> a ;
    while(IsVarStart(l.getCurrentToken())) {
      a = ParseArgument();
      if(a) {
        al.add(*a);
      } else {
        while(l.getCurrentToken() != COMMA
              || l.getCurrentToken() != R_PAREN) {
          l.getNextToken();
        }
        continue;
      }
      if(l.getCurrentToken() == R_PAREN) {
        break;
      }
      l.eat(COMMA, ",");
    }
  }
  l.eat(R_PAREN, ")");
  l.eat(L_BRACKET, "{");
  VarDeclList vl;
  while(IsVarStart(l.getCurrentToken())) {
    if(l.getCurrentToken() == ID) {
      TOK t = l.lookahead();
      if(t == ASSIGN || t == L_SBRACKET){
        break;
      }
    }
    auto v = ParseVarDecl();
    if(!v) {
      while(l.getCurrentToken() != S_COLON) {
        l.getNextToken();
      }
      l.getNextToken();
      continue;
    }
    vl.add(*v);
  }
  StatementList sl;
  while(IsStatementStart(l.getCurrentToken())) {
    auto s = ParseStatement();
    if(!s) {
      return nullptr;
    }
    sl.add(s);
  }
  l.eat(RETURN, "return");
  auto e = ParseExpression();
  l.eat(S_COLON, ";");
  l.eat(R_BRACKET, "}");
  shared_ptr<MethodDecl> x = make_shared<MethodDecl>(t, *i, al, vl, sl, e, l.getLocation());
  return x;
}

shared_ptr<If> Parser::ParseIf() {
  // "if" "(" Expression ")" Statement "else" Statement
  l.eat(IF, "if"); l.eat(L_PAREN, "(");
  auto e = ParseExpression();
  if(!e) {
    return nullptr;
  }
  l.eat(R_PAREN, ")");
  auto s = ParseStatement();
  if(!s) {
    return nullptr;
  }
  l.eat(ELSE, "else");
  auto ss = ParseStatement();
  if(!ss) {
    return nullptr;
  }
  return make_shared<If>(e, s, ss, l.getLocation());
}

shared_ptr<While> Parser::ParseWhile() {
  // "while" "(" Expression ")" Statement
  l.eat(WHILE, "while"); l.eat(L_PAREN, "(");
  auto e = ParseExpression();
  if(!e) {
    return nullptr;
  }
  l.eat(R_PAREN, ")");
  auto s = ParseStatement();
  if(!s) {
    return nullptr;
  }
  return make_shared<While>(e, s, l.getLocation());
}

shared_ptr<Print> Parser::ParsePrint() {
  // "System.out.println" "(" Expression ")" ";"
  l.eat(PRINT, "System.out.println");
  l.eat(L_PAREN, "(");
  auto e = ParseExpression();
  if(!e) {
    return nullptr;
  }
  l.eat(R_PAREN, ")");
  l.eat(S_COLON, ";");
  return make_shared<Print>(e, l.getLocation());
}

shared_ptr<Statement> Parser::ParseStatementHelper() {
  auto i = ParseIdentifier();
  if(!i) {
    return nullptr;
  }
  shared_ptr<Statement> re;
  shared_ptr<Expression> e, ee;
  switch(l.getCurrentToken()) {
    case ASSIGN:
      l.eat(ASSIGN, "=");
      e = ParseExpression();
      break;
    case L_SBRACKET:
      l.eat(L_SBRACKET, "[");
      e = ParseExpression();
      l.eat(R_SBRACKET, "]");
      l.eat(ASSIGN, "=");
      ee = ParseExpression();
      break;
    default:
      err.emit(l.getLocation(), "expected Assignment Statement");
      return nullptr;
  }
  l.eat(S_COLON, ";");
  if(e && ee) {
    return make_shared<ArrayAssign>(*i, e, ee, l.getLocation());
  } else if(e) {
    return make_shared<Assign>(*i, e, l.getLocation());
  }
  return nullptr;
}

shared_ptr<Block> Parser::ParseBlock() {
  l.eat(L_BRACKET, "{");
  StatementList sl;
  while(IsStatementStart(l.getCurrentToken())) {
    auto s = ParseStatement();
    if(!s) {
      return nullptr;
    }
    sl.add(s);
  }
  l.eat(R_BRACKET, "}");
  return make_shared<Block>(sl, l.getLocation());
}

shared_ptr<Statement> Parser::ParseStatement() {
  shared_ptr<Statement> re;
  switch(l.getCurrentToken()) {
    case IF:
      re = ParseIf();
      break;
    case WHILE:
      re = ParseWhile();
      break;
    case PRINT:
      re = ParsePrint();
      break;
    case ID:
      re = ParseStatementHelper();
      break;
    case L_BRACKET:
      re = ParseBlock();
      break;
    default:
      err.emit(l.getLocation(), "Undefined Statement");
      break;
  }
  if(!re) {
    return nullptr;
  }
  return re;
}

shared_ptr<Expression> Parser::ParseExpression() {
  auto LHS = ParseBeta();
  return ParseGoodExpression(LHS);
}

shared_ptr<Expression> Parser::ParseGoodExpression(shared_ptr<Expression> LHS) {
  if(!IsStillGood(l.getCurrentToken())) {
    return LHS;
  }
  auto e = ParseAlpha(LHS);
  return ParseGoodExpression(e);
}

shared_ptr<Expression> Parser::ParseAlpha(shared_ptr<Expression> LHS) {
  TOK tk = l.getCurrentToken();
  if(IsBinOp(tk)) {
    return ParseBinOpExpression(0, LHS);
  } else if(tk == L_SBRACKET) {
    return ParseArrayLookup(LHS);
  } else if(tk == DOT) {
    return ParseDotHelper(LHS);
  }
  return nullptr;
}

// && < + - *
int Parser::GetPercedence(TOK op) {
  switch(op) {
    case AND:
      return 10;
    case LTHAN:
      return 20;
    case PLUS:
    case MINUS:
      return 30;
    case TIMES:
      return 40;
  }
  return -1;
}

// We parse Binary operation using Operator-precedence parser
// https://en.wikipedia.org/wiki/Operator-precedence_parser
shared_ptr<BinOpExpression>
Parser::ParseBinOpExpression(int ExpPrec, shared_ptr<Expression> LHS) {
  while(true) {
    int CurPrec = GetPercedence(l.getCurrentToken());
    if(CurPrec < ExpPrec) {
      shared_ptr<BinOpExpression> lhs
        = static_pointer_cast<BinOpExpression>(LHS);
      return lhs;
    }
    TOK op = l.getCurrentToken();
    l.getNextToken();
    auto RHS = ParseExpression();
    int NxtPrec = GetPercedence(l.getCurrentToken());
    if(CurPrec < NxtPrec) {
      RHS = ParseBinOpExpression(CurPrec + 1, RHS);
      if(!RHS) {
        return nullptr;
      }
    }
    LHS = make_shared<BinOpExpression>(op, LHS, RHS, l.getLocation());
  }
}

shared_ptr<ArrayLookup> Parser::ParseArrayLookup(shared_ptr<Expression> LHS) {
  l.eat(L_SBRACKET, "[");
  auto e = ParseExpression();
  l.eat(R_SBRACKET, "]");
  return make_shared<ArrayLookup>(LHS, e, l.getLocation());
}

shared_ptr<Expression> Parser::ParseDotHelper(shared_ptr<Expression> LHS) {
  l.eat(DOT, ".");
  switch(l.getCurrentToken()) {
    case LENGTH:
      return ParseArrayLength(LHS);
    case ID:
      return ParseCallExpression(LHS);
    default:
      err.emit(l.getLocation(), "Undefined Expression");
      break;
  }
  return nullptr;
}

shared_ptr<ArrayLength> Parser::ParseArrayLength(shared_ptr<Expression> LHS) {
  l.eat(LENGTH, "length");
  return make_shared<ArrayLength>(LHS, l.getLocation());
}

shared_ptr<Call> Parser::ParseCallExpression(shared_ptr<Expression> LHS) {
  auto i = ParseIdentifier();
  if(!i) {
    return nullptr;
  }
  l.eat(L_PAREN, "(");
  ExpressionList el;
  if(l.getCurrentToken() == R_PAREN) {
    l.eat(R_PAREN, ")");
  } else {
    while(true) {
      auto e = ParseExpression();
      if(!e) {
        return nullptr;
      }
      el.add(e);
      if(l.getCurrentToken() == R_PAREN) {
        break;
      }
      l.eat(COMMA, ",");
    }
    l.eat(R_PAREN, ")");
  }
  return make_shared<Call>(LHS, *i, el, l.getLocation());
}

shared_ptr<Expression> Parser::ParseBeta() {
  switch(l.getCurrentToken()) {
    case INTEGER_LITERAL:
      return ParseIntLit();
    case TRUE:
      return ParseTrue();
    case FALSE:
      return ParseFalse();
    case ID:
      return ParseIdExp();
    case THIS:
      return ParseThis();
    case NEW:
      return ParseNewHelper();
    case NOT:
      return ParseNot();
    case L_PAREN:
      return ParseParenExpr();
    default:
      err.emit(l.getLocation(), "Undefined Expression");
      break;
  }
  return nullptr;
}

shared_ptr<IntegerLiteral> Parser::ParseIntLit() {
  l.eat(INTEGER_LITERAL, "integer value");
  return make_shared<IntegerLiteral>(l.getNumberToken(), l.getLocation());
}

shared_ptr<True> Parser::ParseTrue() {
  l.eat(TRUE, "true");
  return make_shared<True>(l.getLocation());
}

shared_ptr<False> Parser::ParseFalse() {
  l.eat(FALSE, "false");
  return make_shared<False>(l.getLocation());
}

shared_ptr<IdentifierExp> Parser::ParseIdExp() {
  auto i = ParseIdentifier();
  return make_shared<IdentifierExp>(i->s, l.getLocation());
}

shared_ptr<This> Parser::ParseThis() {
  l.eat(THIS, "this");
  return make_shared<This>(l.getLocation());
}

shared_ptr<Expression> Parser::ParseParenExpr() {
  l.eat(L_PAREN, "(");
  auto e = ParseExpression();
  l.eat(R_PAREN, ")");
  return e;
}

shared_ptr<Expression> Parser::ParseNewHelper() {
  l.eat(NEW, "new");
  shared_ptr<Expression> re;
  switch(l.getCurrentToken()) {
    case INT:
      re = ParseNewArray();
      break;
    case ID:
      re = ParseNewObject();
      break;
    default:
      err.emit(l.getLocation(), "Unknown Expression");
      return nullptr;
  }
  return re;
}

shared_ptr<NewArray> Parser::ParseNewArray() {
  l.eat(INT, "int");
  l.eat(L_SBRACKET, "[");
  auto e = ParseExpression();
  l.eat(R_SBRACKET, "]");
  return make_shared<NewArray>(e, l.getLocation());
}

shared_ptr<NewObject> Parser::ParseNewObject() {
  auto i = ParseIdentifier();
  l.eat(L_PAREN, "(");
  l.eat(R_PAREN, ")");
  return make_shared<NewObject>(*i, l.getLocation());
}

shared_ptr<Not> Parser::ParseNot() {
  l.eat(NOT, "!");
  auto e = ParseExpression();
  return make_shared<Not>(e, l.getLocation());
}

shared_ptr<Program> Parser::parse() {
  freopen(l.getLocation().file.c_str(), "r", stdin);
  auto re = ParseProgram();
  fclose(stdin);
  if(err.hasErrors()) {
    err.failure();
  }
  return re;
}
