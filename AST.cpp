#include "PrettyPrintVisitor.h"
#include "SymbolTable/SymbolTableVisitor.h"
#include "TypeCheckerVisitor.h"
#include "CodeGenVisitor.h"

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

std::string Program::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string MainClass::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string ClassDeclSimple::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string ClassDeclExtends::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string VarDecl::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string MethodDecl::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string Argument::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string ArrayType::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string BoolType::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string IntType::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string IdentifierType::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string BinOpExpression::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string Block::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string If::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string While::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string Print::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string Assign::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string ArrayAssign::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string ArrayLookup::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string ArrayLength::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string Call::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string IntegerLiteral::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string True::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string False::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string IdentifierExp::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string This::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string NewArray::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string NewObject::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string Not::accept(TypeVisitor& v) {
  return v.visit(this);
}

std::string Identifier::accept(TypeVisitor& v) {
  return v.visit(this);
}
