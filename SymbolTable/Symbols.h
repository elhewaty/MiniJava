#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include <string>
#include <vector>
#include <map>
#include <memory>

using namespace std;

// Symbol class holds information about Symbol table entries.
// We have three types of symbols:
// 1) Variable symbol
// 2) Method symbol
// 3) Class symbol
// Each Symbol has a name and a type. If the class is simple,
// then it does't have a type. If it's a sub class, it's type is
// the parent class.

class Symbol : public enable_shared_from_this<Symbol> {
public:
  Symbol(string name, string type);
  string getName() const;
  string getType() const;
  void setParent(shared_ptr<Symbol> s) { parent = s; }
  shared_ptr<Symbol> getParent() const { return parent; }
  virtual bool operator==(const Symbol& other) const = 0;
  virtual bool operator!=(const Symbol& other) const = 0;
  virtual string stringize() const = 0;
private:
  string name;
  string type;
  // We need this to know where exactly the symbol is defined
  // e.g, the parent class of the method
  shared_ptr<Symbol> parent;
};

class VarSymbol : public Symbol {
public:
  VarSymbol(string name, string type);
  virtual bool operator==(const Symbol& other) const override;
  virtual bool operator!=(const Symbol& other) const override;
  virtual string stringize() const override;
};

class MethodSymbol : public Symbol {
public:
  MethodSymbol(string name, string type);
  virtual bool operator==(const Symbol& other) const override;
  virtual bool operator!=(const Symbol& other) const override;
  virtual string stringize() const override;
  vector<shared_ptr<VarSymbol>> getParameters() const;
  void addParameter(shared_ptr<VarSymbol> s);
  void addLocal(shared_ptr<VarSymbol> s);
  bool varExists(shared_ptr<VarSymbol> s) const;
private:
  vector<shared_ptr<VarSymbol>> parameters;
  map<string, bool> locals;
};

class ClassSymbol : public Symbol {
public:
  ClassSymbol(string name, string type);
  ClassSymbol(string name);
  virtual bool operator==(const Symbol& other) const override;
  virtual bool operator!=(const Symbol& other) const override;
  virtual string stringize() const override;
  vector<shared_ptr<MethodSymbol>> getMethods() const;
  vector<shared_ptr<VarSymbol>> getVariables() const;
  void addMethod(shared_ptr<MethodSymbol> ms);
  void addVariable(shared_ptr<VarSymbol> vs);
  void addVarInExtClass(shared_ptr<VarSymbol> vs);
  shared_ptr<MethodSymbol> getMethod(string s) const;
  // This function adds the Symbols in parent class to the child classs.
  void parentClass(shared_ptr<ClassSymbol> cs);
  void replaceMethod(shared_ptr<MethodSymbol> ms);
  int getMethodOffset(string s);

private:
  vector<shared_ptr<MethodSymbol>> methods;
  vector<shared_ptr<VarSymbol>> vars;
};

#endif
