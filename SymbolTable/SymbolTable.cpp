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

void SymbolTable::addClass(ClassSymbol* cs) {
  classTable[cs->getName()] = cs;
}

void SymbolTable::addMethod(MethodSymbol* ms) {
  methodTable[ms->getName()] = ms;
}

void SymbolTable::addVar(VarSymbol* vs) {
  varTable[vs->getName()] = vs;
}

SymbolTable* SymbolTable::enterScope(string name) {
  if(ScopeTable.count(name)) {
    return this;
  }
  SymbolTable *st = new SymbolTable(*this);
  this->addScopeTableEntry(name, st);
  return st;
}

SymbolTable* SymbolTable::getScope(string name) {
  SymbolTable* SymTab = this->getSymbolTable(name);
  if(SymTab != nullptr) {
    return SymTab;
  }
  return this;
}

SymbolTable* SymbolTable::exitScope() {
  return this->getParent();
}

Symbol* SymbolTable::symbolLookupHelper(string s) {
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

Symbol* SymbolTable::symbolLookup(string s) {
  SymbolTable *st = this;
  while(st != nullptr) {
    Symbol* sym = symbolLookupHelper(s);
    if(sym != nullptr) {
      return sym;
    }
    st = st->getParent();
  }
  return nullptr;
}

ClassSymbol* SymbolTable::classLookup(string s) {
  return classTable.count(s) ? classTable[s] : nullptr;
}

MethodSymbol* SymbolTable::methodLookup(string s) {
  return methodTable.count(s) ? methodTable[s] : nullptr;
}

VarSymbol* SymbolTable::varLookup(string s) {
  return varTable.count(s) ? varTable[s] : nullptr;
}

map<string, VarSymbol*> SymbolTable::getVarTable() const {
  return varTable;
}

map<string, MethodSymbol*> SymbolTable::getMethodTable() const {
  return methodTable;
}

map<string, ClassSymbol*> SymbolTable::getClassTable() const {
  return classTable;
}

SymbolTable* SymbolTable::getParent() const {
  return parent;
}

void SymbolTable::setParent(SymbolTable* st) {
  this->parent = st;
}

map<string, SymbolTable*> SymbolTable::getScopeTable() const {
  return ScopeTable;
}

void SymbolTable::addScopeTableEntry(string s, SymbolTable* st) {
  ScopeTable[s] = st;
  st->parent = this;
}

SymbolTable* SymbolTable::getSymbolTable(string s) {
  return ScopeTable[s];
}
