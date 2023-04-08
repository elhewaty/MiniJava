#ifndef _TYPE_CHECKER_VISITOR_H_
#define _TYPE_CHECKER_VISITOR_H_

#include "AST.h"
#include "TypeVisitor.h"
#include "SymbolTable/SymbolTable.h"
#include "Error.h"

class TypeCheckerVisitor : public TypeVisitor {
public:
  TypeCheckerVisitor() = default;
  TypeCheckerVisitor(shared_ptr<SymbolTable> symTab) : SymTable(symTab) {}
  virtual string visit(Program* n) override;
  virtual string visit(MainClass* n) override;
  virtual string visit(ClassDeclSimple* n) override;
  virtual string visit(ClassDeclExtends* n) override;
  virtual string visit(VarDecl* n) override;
  virtual string visit(MethodDecl* n) override;
  virtual string visit(Argument* n) override;
  virtual string visit(ArrayType* n) override;
  virtual string visit(BoolType* n) override;
  virtual string visit(IntType* n) override;
  virtual string visit(IdentifierType* n) override;
  virtual string visit(BinOpExpression* n) override;
  virtual string visit(Block* n) override;
  virtual string visit(If* n) override;
  virtual string visit(While* n) override;
  virtual string visit(Print* n) override;
  virtual string visit(Assign* n) override;
  virtual string visit(ArrayAssign* n) override;
  virtual string visit(ArrayLookup* n) override;
  virtual string visit(ArrayLength* n) override;
  virtual string visit(Call* n) override;
  virtual string visit(IntegerLiteral* n) override;
  virtual string visit(True* n) override;
  virtual string visit(False* n) override;
  virtual string visit(IdentifierExp* n) override;
  virtual string visit(This* n) override;
  virtual string visit(NewArray* n) override;
  virtual string visit(NewObject* n) override;
  virtual string visit(Not* n) override;
  virtual string visit(Identifier* n) override;
private:
  shared_ptr<SymbolTable> SymTable = nullptr;
  ErrorHandling err;
  // This stack holds the class we are in its scope.
  // We use this stack to evaluate `this` Identifier.
  stack<shared_ptr<ClassSymbol>> CurClass;
  map<string, shared_ptr<ClassSymbol>> classList;
};

#endif
