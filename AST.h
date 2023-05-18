#ifndef _AST_H_
#define _AST_H_

#include "Token.h"
#include <vector>
#include <memory>

using namespace std;

class Visitor;
class TypeVisitor;
class CodeGenVisitor;

class ASTNode {
public:
  ASTNode(Location pos): loc(pos) {}
  ASTNode() = default;
  virtual Location getLocation() const { return loc; }
  virtual ~ASTNode() {};
  Location loc;
  shared_ptr<ASTNode> parent;
};

class Type : public ASTNode {
public:
  Type(Location l) : ASTNode(l) {}
  Type() = default;
  virtual void accept(Visitor& v) = 0;
  virtual string accept(TypeVisitor& v) = 0;
  virtual string stringize() = 0;
};

class ArrayType : public Type {
public:
  ArrayType(Location l) : Type(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() override { return "int[]"; }
};

class IntType: public Type {
public:
  IntType(Location l) : Type(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() override { return "int"; }
};

class BoolType: public Type {
public:
  BoolType(Location l) : Type(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() override { return "boolean"; }
};

class IdentifierType : public Type {
public:
  IdentifierType(string s, Location l) : s(s), Type(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() override { return s; }
  string s;
};

class Identifier : public ASTNode {
public:
  Identifier(string s, Location l) : s(s), ASTNode(l) {}
  virtual void accept(Visitor& v);
  virtual string accept(TypeVisitor& v);
  string s;
};

class VarDecl : public ASTNode {
public:
  VarDecl(shared_ptr<Type> t, Identifier i, Location l) : t(t), i(i), ASTNode(l) {}
  VarDecl() = default;
  virtual void accept(Visitor& v);
  virtual string accept(TypeVisitor& v);
  shared_ptr<Type> t;
  Identifier i;
};

class VarDeclList : public ASTNode {
public:
  VarDeclList() {}
  VarDecl at(int idx) const { return vl[idx]; }
  size_t size() const { return vl.size(); }
  void add(VarDecl v) { vl.push_back(v); }
  vector<VarDecl> vl;
};

class Statement : public ASTNode {
public:
  Statement(Location l) : ASTNode(l) {}
  Statement() = default;
  virtual void accept(Visitor& v) = 0;
  virtual string accept(TypeVisitor& v) = 0;
};

class StatementList : public ASTNode {
public:
  StatementList() {}
  shared_ptr<Statement> at(int idx) const { return sl[idx]; }
  size_t size() const { return sl.size(); }
  void add(shared_ptr<Statement> s) { sl.push_back(s); }
  vector<shared_ptr<Statement>>sl;
};

class Expression : public ASTNode {
public:
  Expression(Location l) : ASTNode(l) {}
  Expression() = default;
  virtual void accept(Visitor& v) = 0;
  virtual string accept(TypeVisitor& v) = 0;
  // we use this to get the type of the expression
  // for conditional statements, e.g,
  // if(exp->stringize() == "binOp") { do something }
  // instead of
  // if(dynamic_pointer_cast<BinOpExpression>(exp)) { do something }
  // Although we need the dynamic cast in the body, but it's better
  // to test with strings, instead of testing every test with dynamic_cast
  // seems dummy: but still ok.
  virtual string stringize() = 0;
};

class ExpressionList : public ASTNode {
public:
  ExpressionList() {}
  shared_ptr<Expression> at(int idx) const { return el[idx]; }
  size_t size() const { return el.size(); }
  void add(shared_ptr<Expression> e) { el.push_back(e); }
  vector<shared_ptr<Expression>> el;
};

// This class build the AST for binary operations.
// We have two kinds of binary expression:
// 1) one that works only for IntgerTypes (+ | - | *)
// 2) other works with boolean expression (&&)
class BinOpExpression : public Expression {
public:
  BinOpExpression(TOK op, shared_ptr<Expression> LHS,
                  shared_ptr<Expression> RHS, Location l)
    : op(op), LHS(LHS), RHS(RHS), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "BinOp"; }
  shared_ptr<Expression> LHS;
  shared_ptr<Expression> RHS;
  TOK op;
};

class ArrayLookup : public Expression {
public:
  ArrayLookup(shared_ptr<Expression> e1, shared_ptr<Expression> e2, Location l)
    : e1(e1), e2(e2), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "lookup"; }
  shared_ptr<Expression> e1;
  shared_ptr<Expression> e2;
};

class ArrayLength : public Expression {
public:
  ArrayLength(shared_ptr<Expression> e, Location l)
    : e(e), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "length"; }
  shared_ptr<Expression> e;
};

class Call : public Expression {
public:
  Call(shared_ptr<Expression> e, Identifier i, ExpressionList el, Location l)
    : e(e), i(i), el(el), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "call"; }
  shared_ptr<Expression> e;
  Identifier i;
  ExpressionList el;
  Location l;
};

class IntegerLiteral : public Expression {
public:
  IntegerLiteral(long long int n, Location l) : numberValue(n), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "intlit"; }
  long long int numberValue;
};

class True : public Expression {
public:
  True(Location l) : Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "true"; }
};

class False : public Expression {
public:
  False(Location l) : Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "false"; }
};

class IdentifierExp : public Expression {
public:
  IdentifierExp(string s, Location l) : s(s), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "IdExp"; }
  string s;
};

class This : public Expression {
public:
  This(Location l) : Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "this"; }
};

class NewArray : public Expression {
public:
  NewArray(shared_ptr<Expression> e, Location l)
    : e(e), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "newArr"; }
  shared_ptr<Expression> e;
};

class NewObject : public Expression {
public:
  NewObject(Identifier i, Location l) : i(i), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "newObj"; }
  Identifier i;
};

class Not : public Expression {
public:
  Not(shared_ptr<Expression> e, Location l)
    : e(e), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "not"; }
  shared_ptr<Expression> e;
};

class ParenExpression : public Expression {
public:
  ParenExpression(shared_ptr<Expression> e, Location l)
    : e(e), Expression(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  virtual string stringize() { return "paren"; }
  shared_ptr<Expression> e;
};

class If : public Statement {
public:
  If(shared_ptr<Expression> e, shared_ptr<Statement> s1, shared_ptr<Statement> s2,
     Location l)
    : e(e), s1(s1), s2(s2), Statement(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  shared_ptr<Expression> e;
  shared_ptr<Statement> s1;
  shared_ptr<Statement> s2;
};

class While : public Statement {
public:
  While(shared_ptr<Expression> e, shared_ptr<Statement> s, Location l)
    : e(e), s(s), Statement(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  shared_ptr<Expression> e;
  shared_ptr<Statement> s;
};

class Print : public Statement {
public:
  Print(shared_ptr<Expression> e, Location l) : e(e), Statement(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  shared_ptr<Expression> e;
};

class Assign : public Statement {
public:
  Assign(Identifier i, shared_ptr<Expression> e, Location l)
    : i(i), e(e), Statement(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  Identifier i;
  shared_ptr<Expression> e;
};

class ArrayAssign : public Statement {
public:
  ArrayAssign(Identifier i, shared_ptr<Expression> e1, shared_ptr<Expression> e2,
              Location l)
    : i(i), e1(e1), e2(e2), Statement(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  Identifier i;
  shared_ptr<Expression> e1;
  shared_ptr<Expression> e2;
};

class Block : public Statement {
public:
  Block(StatementList sl, Location l) : sl(sl), Statement(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  StatementList sl;
};

class Argument : public ASTNode {
public:
  Argument(shared_ptr<Type> t, Identifier i, Location l) : t(t), i(i), ASTNode(l) {}
  virtual void accept(Visitor& v);
  virtual string accept(TypeVisitor& v);
  shared_ptr<Type> t;
  Identifier i;
};

class ArgList : public ASTNode {
public:
  ArgList() {}
  Argument at(int idx) const { return al[idx]; }
  size_t size() const { return al.size(); }
  void add(Argument a) { al.push_back(a); }
  vector<Argument> al;
};

class MethodDecl : public ASTNode {
public:
  MethodDecl(shared_ptr<Type> t, Identifier i, ArgList al, VarDeclList vl,
    StatementList sl, shared_ptr<Expression> e, Location l)
    : t(t), i(i), al(al), vl(vl), sl(sl), e(e) {}
  MethodDecl() = default;
  virtual void accept(Visitor& v);
  virtual string accept(TypeVisitor& v);
  shared_ptr<Type> t;
  Identifier i;
  ArgList al;
  VarDeclList vl;
  StatementList sl;
  shared_ptr<Expression> e;
};

class MethodDeclList : public ASTNode {
public:
  MethodDeclList() {}
  MethodDecl at(int idx) const { return ml[idx]; }
  size_t size() const { return ml.size(); }
  void add(MethodDecl m) { ml.push_back(m); }
  vector<MethodDecl> ml;
};

class ClassDecl : public ASTNode {
public:
  ClassDecl(Location l) : ASTNode(l) {}
  ClassDecl() = default;
  virtual void accept(Visitor& v) = 0;
  virtual string accept(TypeVisitor& v) = 0;
};

class ClassDeclSimple : public ClassDecl {
public:
  ClassDeclSimple(Identifier i, VarDeclList vl, MethodDeclList ml, Location l)
    : i(i), vl(vl), ml(ml), ClassDecl(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  Identifier i;
  VarDeclList vl;
  MethodDeclList ml;
};

class ClassDeclExtends : public ClassDecl {
public:
  ClassDeclExtends(Identifier i, VarDeclList vl, MethodDeclList ml,
    Identifier j, Location l)
    : i(i), vl(vl), ml(ml), j(j), ClassDecl(l) {}
  virtual void accept(Visitor& v) override;
  virtual string accept(TypeVisitor& v) override;
  Identifier i;
  Identifier j;
  VarDeclList vl;
  MethodDeclList ml;
};

class ClassDeclList : public ASTNode {
public:
  ClassDeclList() : cl() {}
  shared_ptr<ClassDecl> at(int idx) const { return cl[idx]; }
  size_t size() const { return cl.size(); }
  void add(shared_ptr<ClassDecl> c) { cl.push_back(c); }
  vector<shared_ptr<ClassDecl>> cl;
};

class MainClass : public ASTNode {
public:
  MainClass(Identifier i, Identifier j, shared_ptr<Statement> s, Location l)
    : i(i), j(j), s(s), ASTNode(l) {}
  virtual void accept(Visitor& v);
  virtual string accept(TypeVisitor& v);
  Identifier i;
  Identifier j;
  shared_ptr<Statement> s;
};

class Program : public ASTNode {
public:
  Program(MainClass mc, ClassDeclList c, Location l)
    : m(mc), cl(c), ASTNode(l) {}
  virtual void accept(Visitor& v);
  virtual string accept(TypeVisitor& v);
  MainClass m;
  ClassDeclList cl;
};


#endif
