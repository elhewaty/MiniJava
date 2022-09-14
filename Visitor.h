#ifndef _VISITOR_H_
#define _VISITOR_H_

#include <memory>

class Program;
class MainClass;
class ClassDeclSimple;
class ClassDeclExtends;
class VarDecl;
class MethodDecl;
class Argument;
class ArrayType;
class BoolType;
class IntType;
class IdentifierType;
class BinOpExpression;
class Block;
class If;
class While;
class Print;
class Assign;
class ArrayAssign;
class ArrayLookup;
class ArrayLength;
class Call;
class IntegerLiteral;
class True;
class False;
class IdentifierExp;
class This;
class NewArray;
class NewObject;
class Not;
class Identifier;

// AST Visitor

class Visitor {
public:
  virtual void visit(Program* n) = 0;
  virtual void visit(MainClass* n) = 0;
  virtual void visit(ClassDeclSimple* n) = 0;
  virtual void visit(ClassDeclExtends* n) = 0;
  virtual void visit(VarDecl* n) = 0;
  virtual void visit(MethodDecl* n) = 0;
  virtual void visit(Argument* n) = 0;
  virtual void visit(ArrayType* n) = 0;
  virtual void visit(BoolType* n) = 0;
  virtual void visit(IntType* n) = 0;
  virtual void visit(IdentifierType* n) = 0;
  virtual void visit(BinOpExpression* n) = 0;
  virtual void visit(Block* n) = 0;
  virtual void visit(If* n) = 0;
  virtual void visit(While* n) = 0;
  virtual void visit(Print* n) = 0;
  virtual void visit(Assign* n) = 0;
  virtual void visit(ArrayAssign* n) = 0;
  virtual void visit(ArrayLookup* n) = 0;
  virtual void visit(ArrayLength* n) = 0;
  virtual void visit(Call* n) = 0;
  virtual void visit(IntegerLiteral* n) = 0;
  virtual void visit(True* n) = 0;
  virtual void visit(False* n) = 0;
  virtual void visit(IdentifierExp* n) = 0;
  virtual void visit(This* n) = 0;
  virtual void visit(NewArray* n) = 0;
  virtual void visit(NewObject* n) = 0;
  virtual void visit(Not* n) = 0;
  virtual void visit(Identifier* n) = 0;
};

#endif