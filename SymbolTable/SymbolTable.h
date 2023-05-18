#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_

#include "Symbols.h"
#include <map>
#include <stack>

class SymbolTable : public enable_shared_from_this<SymbolTable> {
public:
  SymbolTable();
  SymbolTable(const SymbolTable &);
  void addClass(shared_ptr<ClassSymbol>);
  void addMethod(shared_ptr<MethodSymbol>);
  void addVar(shared_ptr<VarSymbol>);
  shared_ptr<SymbolTable> enterScope(string);
  shared_ptr<SymbolTable> getScope(string);
  shared_ptr<SymbolTable> exitScope();
  shared_ptr<Symbol> symbolLookup(string);
  shared_ptr<ClassSymbol> classLookup(string);
  shared_ptr<MethodSymbol> methodLookup(string);
  shared_ptr<VarSymbol> varLookup(string);
  //shared_ptr<SymbolTable> getSymbol(string s) { return symbolLookupHelper(s); }
  map<string, shared_ptr<VarSymbol>> getVarTable() const;
  map<string, shared_ptr<MethodSymbol>> getMethodTable() const;
  map<string, shared_ptr<ClassSymbol>> getClassTable() const;
  map<string, shared_ptr<SymbolTable>> getScopeTable() const;
  shared_ptr<SymbolTable> getSymbolTable(string);
  shared_ptr<SymbolTable> getParent() const;

private:
  map<string, shared_ptr<SymbolTable>> ScopeTable;
  map<string, shared_ptr<VarSymbol>> varTable;
  map<string, shared_ptr<MethodSymbol>> methodTable;
  map<string, shared_ptr<ClassSymbol>> classTable;
  shared_ptr<SymbolTable> parent;
  shared_ptr<Symbol> symbolLookupHelper(string);
  void addScopeTableEntry(string, shared_ptr<SymbolTable>);
  void setParent(shared_ptr<SymbolTable>);
};

#endif
