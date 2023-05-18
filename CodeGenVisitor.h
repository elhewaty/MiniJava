#ifndef __CODEGEN__
#define __CODEGEN__

#include "AST.h"
#include "Visitor.h"
#include "SymbolTable/SymbolTable.h"

class CodeGenVisitor final : public Visitor {
public:
  CodeGenVisitor(shared_ptr<SymbolTable> symTab)
    : SymTab(symTab), tSymTab(SymTab){}
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
private:
  int tabs = 0;
  // available stack position
  int stack_pos = 0;
  // index of a used parameter register we need save this,
  // to be able to move the parameters to the stack.
  int argument_idx = 0;
  shared_ptr<SymbolTable> SymTab;
  shared_ptr<SymbolTable> tSymTab;
  // We need a to know the location of the local variable in the stack
  map<shared_ptr<VarSymbol>, int> var_loc;
  void setVarLoc(shared_ptr<VarSymbol> vs, int pos) { var_loc[vs] = pos; }
  // Print with cool identation
  void printTabs() const;
  void incTabs() { tabs++; }
  void decTabs() {
    tabs--;
    if(tabs < 0) {
      tabs = 0;
    }
  }
  // write code s to .asm file
  void gen(string s);
  // trivial way to get the current class we're processing
  stack<string> CurClass;
  // method
  stack<string> CurMethod;
  // write "op src, dst" to .asm file
  void genBin(string op, string src, string dst);
  // generate label to .asm file L ---> .L(number)
  string genLabel();
  // generate label for methods in classes
  string genLabel(string class_name, string method);
  // Method prologue
  void prologue();
  // Method epilogue
  void epilogue();
  void pushq(string s);
  void popq(string s);
  // function to get the value of some `MiniJava` expression
  string evaluateExpression(shared_ptr<Expression> e);
  const vector<string> arg_regs
    = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

  // to get the size of an object to allocate the correct memory
  int getObjSize(string t);
  // functions to align the stack on 16-byte boundary
};

#endif
