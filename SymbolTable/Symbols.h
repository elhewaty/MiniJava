#ifndef _SYMBOLS_H_
#define _SYMBOLS_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

// Symbol class holds information about Symbol table entries.
// We have three types of symbols:
// 1) Variable symbol
// 2) Method symbol
// 3) Class symbol
// Each Symbol has a name and a type. If the class is simple,
// then it does't have a type. If it's a sub class, it's type is
// the parent class.

class Symbol {
public:
  Symbol(string name, string type);
  string getName() const;
  string getType() const;
  virtual bool operator==(const Symbol& other) const = 0;
  virtual bool operator!=(const Symbol& other) const = 0;
  virtual string stringize() const = 0;
private:
  string name;
  string type;
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
  vector<VarSymbol*> getParameters() const;
  void addParameter(VarSymbol* s);
  void addLocal(VarSymbol* s);
  bool varExists(VarSymbol* s) const;
private:
  vector<VarSymbol*> parameters;
  map<string, bool> locals;
};

class ClassSymbol : public Symbol {
public:
  ClassSymbol(string name, string type);
  ClassSymbol(string name);
  virtual bool operator==(const Symbol& other) const override;
  virtual bool operator!=(const Symbol& other) const override;
  virtual string stringize() const override;
  vector<MethodSymbol*> getMethods() const;
  vector<VarSymbol*> getVariables() const;
  void addMethod(MethodSymbol* ms);
  void addVariable(VarSymbol* vs);
  void addVarInExtClass(VarSymbol* vs);
  MethodSymbol* getMethod(string s) const;
  // This function adds the Symbols in parent class to the child classs.
  void parentClass(ClassSymbol* cs);
  void replaceMethod(MethodSymbol* ms);

private:
  vector<MethodSymbol*> methods;
  vector<VarSymbol*> vars;
};

#endif
