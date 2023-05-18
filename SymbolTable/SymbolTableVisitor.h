#ifndef _SYMBOL_TABLE_VISITOR_H_
#define _SYMBOL_TABLE_VISITOR_H_

#include "../Visitor.h"
#include "../AST.h"
#include "../Error.h"
#include "SymbolTable.h"
#include <stack>

class SymbolTableVisitor final : public Visitor {
public:
  SymbolTableVisitor() : SymTable(make_shared<SymbolTable>()) {}
  SymbolTableVisitor(vector<string>v) : ord(v),
    SymTable(make_shared<SymbolTable>()) {}
  virtual void visit(Program* n) override;
  virtual void visit(MainClass* n) override;
  virtual void visit(ClassDeclSimple* n) override;
  virtual void visit(ClassDeclExtends* n) override;
  virtual void visit(VarDecl* n) override;
  virtual void visit(MethodDecl* n) override;
  virtual void visit(Argument* n) override;
  virtual void visit(ArrayType* n) override;
  virtual void visit(BoolType* n) override;
  virtual void visit(IntType* n) override;
  virtual void visit(IdentifierType* n) override;
  virtual void visit(BinOpExpression* n) override;
  virtual void visit(Block* n) override;
  virtual void visit(If* n) override;
  virtual void visit(While* n) override;
  virtual void visit(Print* n) override;
  virtual void visit(Assign* n) override;
  virtual void visit(ArrayAssign* n) override;
  virtual void visit(ArrayLookup* n) override;
  virtual void visit(ArrayLength* n) override;
  virtual void visit(Call* n) override;
  virtual void visit(IntegerLiteral* n) override;
  virtual void visit(True* n) override;
  virtual void visit(False* n) override;
  virtual void visit(IdentifierExp* n) override;
  virtual void visit(This* n) override;
  virtual void visit(NewArray* n) override;
  virtual void visit(NewObject* n) override;
  virtual void visit(Not* n) override;
  virtual void visit(Identifier* n) override;
  shared_ptr<SymbolTable> getSymbolTable() const;

private:
  shared_ptr<SymbolTable> SymTable;
  ErrorHandling err;
  stack<shared_ptr<MethodSymbol>> CurMeth;
  stack<shared_ptr<ClassSymbol>> CurClass;
  vector<string> ord;
  map<string, bool> exist;
  string mainClassName;
  bool isBuiltInType(string t) {
    return t == "int" || t == "boolean" || t == "int[]";
  }
};

#endif
