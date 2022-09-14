#ifndef _AST_H_
#define _AST_H_

#include "Token.h"
#include <vector>

class Visitor;

class ASTNode {
public:
  ASTNode(Location pos): loc(pos) {}
  ASTNode() = default;
  virtual ~ASTNode() {};
  Location loc;
};

class Type : public ASTNode {
public:
  Type(Location l) : ASTNode(l) {}
  Type() = default;
  virtual void accept(Visitor& v) = 0;
};

class ArrayType : public Type {
public:
  ArrayType(Location l) : Type(l) {}
  virtual void accept(Visitor& v);
};

class IntType: public Type {
public:
  IntType(Location l) : Type(l) {}
  virtual void accept(Visitor& v);
};

class BoolType: public Type {
public:
  BoolType(Location l) : Type(l) {}
  virtual void accept(Visitor& v);
};

class IdentifierType : public Type {
public:
  IdentifierType(std::string s, Location l) : s(s), Type(l) {}
  virtual void accept(Visitor& v);
  std::string s;
};

class Identifier : public ASTNode {
public:
  Identifier(std::string s, Location l) : s(s), ASTNode(l) {}
  std::string toString() { return s; }
  void accept(Visitor& v);
  std::string s;
};

class VarDecl : public ASTNode {
public:
  VarDecl(Type* t, Identifier i, Location l) : t(t), i(i), ASTNode(l) {}
  VarDecl() = default;
  void accept(Visitor& v);
  Type* t;
  Identifier i;
};

class VarDeclList : public ASTNode {
public:
  VarDeclList() {}
  VarDecl at(int idx) const { return vl[idx]; }
  size_t size() const { return vl.size(); }
  void add(VarDecl v) { vl.push_back(v); }
  std::vector<VarDecl> vl;
};

class Statement : public ASTNode {
public:
  Statement(Location l) : ASTNode(l) {}
  Statement() = default;
  virtual void accept(Visitor& v) {}
};

class StatementList : public ASTNode {
public:
  StatementList() {}
  Statement* at(int idx) const { return sl[idx]; }
  size_t size() const { return sl.size(); }
  void add(Statement* s) { sl.push_back(s); }
  std::vector<Statement*>sl;
};

class Expression : public ASTNode {
public:
  Expression(Location l) : ASTNode(l) {}
  Expression() = default;
  virtual void accept(Visitor& v) = 0;
};

class ExpressionList : public ASTNode {
public:
  ExpressionList() {}
  Expression* at(int idx) const { return el[idx]; }
  size_t size() const { return el.size(); }
  void add(Expression* e) { el.push_back(e); }
  std::vector<Expression*> el;
};

// This class build the AST for binary operations.
// We have two kinds of binary expression:
// 1) one that works only for IntgerTypes (+ | - | *)
// 2) other works with boolean expression (&&)
class BinOpExpression : public Expression {
public:
  BinOpExpression(TOK op, Expression* LHS, Expression* RHS, Location l)
    : op(op), LHS(LHS), RHS(RHS), Expression(l) {}
  virtual void accept(Visitor& v);
  Expression* LHS;
  Expression* RHS;
  TOK op;
};

class ArrayLookup : public Expression {
public:
  ArrayLookup(Expression* e1, Expression* e2, Location l)
    : e1(e1), e2(e2), Expression(l) {}
  virtual void accept(Visitor& v);
  Expression* e1;
  Expression* e2;
};

class ArrayLength : public Expression {
public:
  ArrayLength(Expression* e, Location l)
    : e(e), Expression(l) {}
  virtual void accept(Visitor& v);
  Expression* e;
};

class Call : public Expression {
public:
  Call(Expression* e, Identifier i, ExpressionList el)
    : e(e), i(i), el(el) {}
  virtual void accept(Visitor& v);
  Expression* e;
  Identifier i;
  ExpressionList el;
};

class IntegerLiteral : public Expression {
public:
  IntegerLiteral(int n, Location l) : numberValue(n), Expression(l) {}
  virtual void accept(Visitor& v);
  int numberValue;
};

class True : public Expression {
public:
  True(Location l) : Expression(l) {}
  virtual void accept(Visitor& v);
};

class False : public Expression {
public:
  False(Location l) : Expression(l) {}
  virtual void accept(Visitor& v);
};

class IdentifierExp : public Expression {
public:
  IdentifierExp(std::string s, Location l) : s(s), Expression(l) {}
  virtual void accept(Visitor& v);
  std::string s;
};

class This : public Expression {
public:
  This(Location l) : Expression(l) {}
  virtual void accept(Visitor& v);
};

class NewArray : public Expression {
public:
  NewArray(Expression* e, Location l)
    : e(e), Expression(l) {}
  virtual void accept(Visitor& v);
  Expression* e;
};

class NewObject : public Expression {
public:
  NewObject(Identifier i, Location l) : i(i), Expression(l) {}
  virtual void accept(Visitor& v);
  Identifier i;
};

class Not : public Expression {
public:
  Not(Expression* e, Location l)
    : e(e), Expression(l) {}
  virtual void accept(Visitor& v);
  Expression* e;
};

class ParenExpression : public Expression {
public:
  ParenExpression(Expression* e, Location l)
    : e(e), Expression(l) {}
  virtual void accept(Visitor& v);
  Expression* e;
};

class If : public Statement {
public:
  If(Expression* e, Statement* s1, Statement* s2, Location l)
    : e(e), s1(s1), s2(s2), Statement(l) {}
  virtual void accept(Visitor& v);
  Expression* e;
  Statement* s1;
  Statement* s2;
};

class While : public Statement {
public:
  While(Expression* e, Statement* s, Location l)
    : e(e), s(s), Statement(l) {}
  virtual void accept(Visitor& v);
  Expression* e;
  Statement* s;
};

class Print : public Statement {
public:
  Print(Expression* e, Location l) : e(e), Statement(l) {}
  virtual void accept(Visitor& v);
  Expression* e;
};

class Assign : public Statement {
public:
  Assign(Identifier i, Expression* e, Location l)
    : i(i), e(e), Statement(l) {}
  virtual void accept(Visitor& v);
  Identifier i;
  Expression* e;
};

class ArrayAssign : public Statement {
public:
  ArrayAssign(Identifier i, Expression* e1, Expression* e2, Location l)
    : i(i), e1(e1), e2(e2), Statement(l) {}
  virtual void accept(Visitor& v);
  Identifier i;
  Expression* e1;
  Expression* e2;
};

class Block : public Statement {
public:
  Block(StatementList sl, Location l) : sl(sl), Statement(l) {}
  void accept(Visitor& v);
  StatementList sl;
};

class Argument : public ASTNode {
public:
  Argument(Type* t, Identifier i, Location l) : t(t), i(i), ASTNode(l) {}
  void accept(Visitor& v);
  Type* t;
  Identifier i;
};

class ArgList : public ASTNode {
public:
  ArgList() {}
  Argument at(int idx) const { return al[idx]; }
  size_t size() const { return al.size(); }
  void add(Argument a) { al.push_back(a); }
  std::vector<Argument> al;
};

class MethodDecl : public ASTNode {
public:
  MethodDecl(Type* t, Identifier i, ArgList al, VarDeclList vl,
    StatementList sl, Expression *e, Location l)
    : t(t), i(i), al(al), vl(vl), sl(sl), e(e) {}
  MethodDecl() = default;
  void accept(Visitor& v);
  Type* t;
  Identifier i;
  ArgList al;
  VarDeclList vl;
  StatementList sl;
  Expression *e;
};

class MethodDeclList : public ASTNode {
public:
  MethodDeclList() {}
  MethodDecl at(int idx) const { return ml[idx]; }
  size_t size() const { return ml.size(); }
  void add(MethodDecl m) { ml.push_back(m); }
  std::vector<MethodDecl> ml;
};

class ClassDecl : ASTNode {
public:
  ClassDecl(Location l) : ASTNode(l) {}
  ClassDecl() = default;
  virtual void accept(Visitor& v) {};
};

class ClassDeclSimple : public ClassDecl {
public:
  ClassDeclSimple(Identifier i, VarDeclList vl, MethodDeclList ml, Location l)
    : i(i), vl(vl), ml(ml), ClassDecl(l) {}
  void accept(Visitor& v) override;
  Identifier i;
  VarDeclList vl;
  MethodDeclList ml;
};

class ClassDeclExtends : public ClassDecl {
public:
  ClassDeclExtends(Identifier i, VarDeclList vl, MethodDeclList ml,
    Identifier j, Location l)
    : i(i), vl(vl), ml(ml), j(j), ClassDecl(l) {}
  void accept(Visitor& v) override;
  Identifier i;
  Identifier j;
  VarDeclList vl;
  MethodDeclList ml;
};

class ClassDeclList : public ASTNode {
public:
  ClassDeclList() : cl() {}
  ClassDecl* at(int idx) const { return cl[idx]; }
  size_t size() const { return cl.size(); }
  void add(ClassDecl* c) { cl.push_back(c); }
  std::vector<ClassDecl*> cl;
};

class MainClass : public ASTNode {
public:
  MainClass(Identifier i, Identifier j, Statement* s, Location l)
    : i(i), j(j), s(s), ASTNode(l) {}
  void accept(Visitor& v);
  Identifier i;
  Identifier j;
  Statement* s;
};

class Program : public ASTNode {
public:
  Program(MainClass mc, ClassDeclList c, Location l)
    : m(mc), cl(c), ASTNode(l) {}
  void accept(Visitor& v);
  MainClass m;
  ClassDeclList cl;
};


#endif