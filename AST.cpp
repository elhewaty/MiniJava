
#include "PrettyPrintVisitor.h"

void Program::accept(Visitor& v) {
  v.visit(this);
}

void MainClass::accept(Visitor& v) {
  v.visit(this);
}

void ClassDeclSimple::accept(Visitor& v) {
  v.visit(this);
}

void ClassDeclExtends::accept(Visitor& v) {
  v.visit(this);
}

void VarDecl::accept(Visitor& v) {
  v.visit(this);
}

void MethodDecl::accept(Visitor& v) {
  v.visit(this);
}

void Argument::accept(Visitor& v) {
  v.visit(this);
}

void ArrayType::accept(Visitor& v) {
  v.visit(this);
}

void BoolType::accept(Visitor& v) {
  v.visit(this);
}

void IntType::accept(Visitor& v) {
  v.visit(this);
}

void IdentifierType::accept(Visitor& v) {
  v.visit(this);
}

void BinOpExpression::accept(Visitor& v) {
  v.visit(this);
}

void Block::accept(Visitor& v) {
  v.visit(this);
}

void If::accept(Visitor& v) {
  v.visit(this);
}

void While::accept(Visitor& v) {
  v.visit(this);
}

void Print::accept(Visitor& v) {
  v.visit(this);
}

void Assign::accept(Visitor& v) {
  v.visit(this);
}

void ArrayAssign::accept(Visitor& v) {
  v.visit(this);
}

void ArrayLookup::accept(Visitor& v) {
  v.visit(this);
}

void ArrayLength::accept(Visitor& v) {
  v.visit(this);
}

void Call::accept(Visitor& v) {
  v.visit(this);
}

void IntegerLiteral::accept(Visitor& v) {
  v.visit(this);
}

void True::accept(Visitor& v) {
  v.visit(this);
}

void False::accept(Visitor& v) {
  v.visit(this);
}

void IdentifierExp::accept(Visitor& v) {
  v.visit(this);
}

void This::accept(Visitor& v) {
  v.visit(this);
}

void NewArray::accept(Visitor& v) {
  v.visit(this);
}

void NewObject::accept(Visitor& v) {
  v.visit(this);
}

void Not::accept(Visitor& v) {
  v.visit(this);
}

void Identifier::accept(Visitor& v) {
  v.visit(this);
}
