#ifndef _TYPE_VISITOR_H_
#define _TYPE_VISITOR_H_

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

#include <string>

using namespace std;

// Type Checker Visitor

class TypeVisitor {
public:
  virtual string visit(Program* n) = 0;
  virtual string visit(MainClass* n) = 0;
  virtual string visit(ClassDeclSimple* n) = 0;
  virtual string visit(ClassDeclExtends* n) = 0;
  virtual string visit(VarDecl* n) = 0;
  virtual string visit(MethodDecl* n) = 0;
  virtual string visit(Argument* n) = 0;
  virtual string visit(ArrayType* n) = 0;
  virtual string visit(BoolType* n) = 0;
  virtual string visit(IntType* n) = 0;
  virtual string visit(IdentifierType* n) = 0;
  virtual string visit(BinOpExpression* n) = 0;
  virtual string visit(Block* n) = 0;
  virtual string visit(If* n) = 0;
  virtual string visit(While* n) = 0;
  virtual string visit(Print* n) = 0;
  virtual string visit(Assign* n) = 0;
  virtual string visit(ArrayAssign* n) = 0;
  virtual string visit(ArrayLookup* n) = 0;
  virtual string visit(ArrayLength* n) = 0;
  virtual string visit(Call* n) = 0;
  virtual string visit(IntegerLiteral* n) = 0;
  virtual string visit(True* n) = 0;
  virtual string visit(False* n) = 0;
  virtual string visit(IdentifierExp* n) = 0;
  virtual string visit(This* n) = 0;
  virtual string visit(NewArray* n) = 0;
  virtual string visit(NewObject* n) = 0;
  virtual string visit(Not* n) = 0;
  virtual string visit(Identifier* n) = 0;
};

#endif
