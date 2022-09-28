#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "Symbols.h"
#include <map>
#include <stack>

class SymbolTable {
public:
  SymbolTable();
  SymbolTable(const SymbolTable &);
  void addClass(ClassSymbol*);
  void addMethod(MethodSymbol*);
  void addVar(VarSymbol*);
  SymbolTable* enterScope(string);
  SymbolTable* getScope(string);
  SymbolTable* exitScope();
  Symbol* symbolLookup(string);
  ClassSymbol* classLookup(string);
  MethodSymbol* methodLookup(string);
  VarSymbol* varLookup(string);
  map<string, VarSymbol*> getVarTable() const;
  map<string, MethodSymbol*> getMethodTable() const;
  map<string, ClassSymbol*> getClassTable() const;
  map<string, SymbolTable*> getScopeTable() const;
  SymbolTable* getSymbolTable(string);
  SymbolTable* getParent() const;

private:
  map<string, SymbolTable*> ScopeTable;
  map<string, VarSymbol*> varTable;
  map<string, MethodSymbol*> methodTable;
  map<string, ClassSymbol*> classTable;
  SymbolTable* parent = nullptr;
  Symbol* symbolLookupHelper(string);
  void addScopeTableEntry(string, SymbolTable*);
  void setParent(SymbolTable*);
};

#endif
