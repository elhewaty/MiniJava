#include "SymbolTable.h"
#include <cassert>

SymbolTable::SymbolTable()
  : ScopeTable(), varTable(), methodTable(), classTable() {}

SymbolTable::SymbolTable(const SymbolTable &st) {
  ScopeTable = st.getScopeTable();
  varTable = st.getVarTable();
  methodTable = st.getMethodTable();
  classTable = st.getClassTable();
  parent = st.getParent();
}

void SymbolTable::addClass(shared_ptr<ClassSymbol> cs) {
  classTable[cs->getName()] = cs;
}

void SymbolTable::addMethod(shared_ptr<MethodSymbol> ms) {
  methodTable[ms->getName()] = ms;
}

void SymbolTable::addVar(shared_ptr<VarSymbol> vs) {
  varTable[vs->getName()] = vs;
}

shared_ptr<SymbolTable> SymbolTable::enterScope(string name) {
  if(ScopeTable.count(name)) {
    return shared_from_this();
  }
  shared_ptr<SymbolTable> st = make_shared<SymbolTable>(*this);
  this->addScopeTableEntry(name, st);
  return st;
}

shared_ptr<SymbolTable> SymbolTable::getScope(string name) {
  shared_ptr<SymbolTable> SymTab = this->getSymbolTable(name);
  if(SymTab != nullptr) {
    return SymTab;
  }
  return shared_from_this();
}

shared_ptr<SymbolTable> SymbolTable::exitScope() {
  return this->getParent();
}

shared_ptr<Symbol> SymbolTable::symbolLookupHelper(string s) {
  if(classTable.count(s) ) {
    return classTable[s];
  }
  if(methodTable.count(s)) {
    return methodTable[s];
  }
  if(varTable.count(s)) {
    return varTable[s];
  }
  return nullptr;
}

shared_ptr<Symbol> SymbolTable::symbolLookup(string s) {
  auto st = shared_from_this();
  while(st != nullptr) {
    shared_ptr<Symbol> sym = symbolLookupHelper(s);
    if(sym != nullptr) {
      return sym;
    }
    st = st->getParent();
  }
  return nullptr;
}

shared_ptr<ClassSymbol> SymbolTable::classLookup(string s) {
  return classTable.count(s) ? classTable[s] : nullptr;
}

shared_ptr<MethodSymbol> SymbolTable::methodLookup(string s) {
  return methodTable.count(s) ? methodTable[s] : nullptr;
}

shared_ptr<VarSymbol> SymbolTable::varLookup(string s) {
  return varTable.count(s) ? varTable[s] : nullptr;
}

map<string, shared_ptr<VarSymbol>> SymbolTable::getVarTable() const {
  return varTable;
}

map<string, shared_ptr<MethodSymbol>> SymbolTable::getMethodTable() const {
  return methodTable;
}

map<string, shared_ptr<ClassSymbol>> SymbolTable::getClassTable() const {
  return classTable;
}

shared_ptr<SymbolTable> SymbolTable::getParent() const {
  return parent;
}

void SymbolTable::setParent(shared_ptr<SymbolTable> st) {
  this->parent = st;
}

map<string, shared_ptr<SymbolTable>> SymbolTable::getScopeTable() const {
  return ScopeTable;
}

void SymbolTable::addScopeTableEntry(string s, shared_ptr<SymbolTable> st) {
  ScopeTable[s] = st;
  st->parent = shared_from_this();
}

shared_ptr<SymbolTable> SymbolTable::getSymbolTable(string s) {
  return ScopeTable[s];
}
