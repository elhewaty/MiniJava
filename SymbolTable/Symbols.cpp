#include "Symbols.h"
#include <cassert>
//===============================================//
//          Symbol class implementation          //
//===============================================//

Symbol::Symbol(string name, string type)
   : name(name), type(type) {}

string Symbol::getName() const {
  return name;
}

string Symbol::getType() const {
  return type;
}

//===============================================//
//        VarSymbol class implementation         //
//===============================================//

VarSymbol::VarSymbol(string name, string type) : Symbol(name, type) {}

bool VarSymbol::operator==(const Symbol& other) const {
  return this->stringize() == other.stringize();
}

bool VarSymbol::operator!=(const Symbol& other) const {
  return !(*this == other);
}

string VarSymbol::stringize() const {
  return "variable: " + getName() + " " + getType();
}

//===============================================//
//       MethodSymbol class implementation       //
//===============================================//

MethodSymbol::MethodSymbol(string name, string type) 
  : Symbol(name, type) {}

bool MethodSymbol::operator==(const Symbol& other) const {
  return this->stringize() == other.stringize();
}

bool MethodSymbol::operator!=(const Symbol& other) const {
  return !(*this == other);
}

string MethodSymbol::stringize() const {
  string s = "Method: " + getName() + " " + getType() + " ";
  for(int i = 0; i < (int)parameters.size(); i++) {
    s += parameters[i]->getType();
    if(i + 1 != (int)parameters.size()) {
      s += " ";
    }
  }
  return s;
}

vector<shared_ptr<VarSymbol>> MethodSymbol::getParameters() const {
  return parameters;
}

void MethodSymbol::addParameter(shared_ptr<VarSymbol> s) {
  parameters.push_back(s);
  s->setParent(static_pointer_cast<Symbol>(shared_from_this()));
}

void MethodSymbol::addLocal(shared_ptr<VarSymbol> s) {
  locals[s->stringize()] = 1;
  s->setParent(static_pointer_cast<Symbol>(shared_from_this()));
}

bool MethodSymbol::varExists(shared_ptr<VarSymbol> s) const {
  if(!s) return 0;
  return locals.count(s->stringize());
}

//===============================================//
//       ClassSymbol class implementation        //
//===============================================//

ClassSymbol::ClassSymbol(string name, string type)
  : Symbol(name, type){}

ClassSymbol::ClassSymbol(string name) : Symbol(name, "") {}

bool ClassSymbol::operator==(const Symbol& other) const {
  return this->stringize() == other.stringize();
}

bool ClassSymbol::operator!=(const Symbol& other) const {
  return !(*this == other);
}

string ClassSymbol::stringize() const {
  return "class: " + getName() + " " + getType();
}

vector<shared_ptr<MethodSymbol>> ClassSymbol::getMethods() const {
  return methods;
}

shared_ptr<MethodSymbol> ClassSymbol::getMethod(string s) const {
  for(int i = 0; i < (int)methods.size(); i++) {
    auto m = methods[i];
    if(m->getName() == s) {
      return m;
    }
  }
  return nullptr;
}

vector<shared_ptr<VarSymbol>> ClassSymbol::getVariables() const {
  return vars;
}

void ClassSymbol::addMethod(shared_ptr<MethodSymbol> ms) {
  methods.push_back(ms);
  ms->setParent(static_pointer_cast<Symbol>(shared_from_this()));
}

void ClassSymbol::addVariable(shared_ptr<VarSymbol> vs) {
  vars.push_back(vs);
  vs->setParent(static_pointer_cast<Symbol>(shared_from_this()));
}

void ClassSymbol::addVarInExtClass(shared_ptr<VarSymbol> vs) {
  for(int i = 0; i < (int)vars.size(); i++) {
    if(vars[i]->getName() == vs->getName()) {
      vars[i] = vs;
      return;
    }
  }
  vars.push_back(vs);
}

void ClassSymbol::parentClass(shared_ptr<ClassSymbol> cs) {
  auto m = cs->getMethods();
  auto v = cs->getVariables();
  methods.insert(methods.end(), m.begin(), m.end());
  vars.insert(vars.end(), v.begin(), v.end());
}

void ClassSymbol::replaceMethod(shared_ptr<MethodSymbol> ms) {
  for(int i = 0; i < methods.size(); i++) {
    if(methods[i]->getName() == ms->getName()) {
      methods[i] = ms;
      return;
    }
  }
}

int ClassSymbol::getMethodOffset(string s) {
  for(int i = 0; i < methods.size(); i++) {
    if(methods[i]->getName() == s) {
      return i + 1;
    }
  }
  assert(0);
}