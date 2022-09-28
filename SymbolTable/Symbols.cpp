#include "Symbols.h"

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

vector<VarSymbol*> MethodSymbol::getParameters() const {
  return parameters;
}

void MethodSymbol::addParameter(VarSymbol* s) {
  parameters.push_back(s);
}

void MethodSymbol::addLocal(VarSymbol* s) {
  locals[s->stringize()] = 1;;
}

bool MethodSymbol::varExists(VarSymbol* s) const {
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

vector<MethodSymbol*> ClassSymbol::getMethods() const {
  return methods;
}

MethodSymbol* ClassSymbol::getMethod(string s) const {
  for(int i = 0; i < (int)methods.size(); i++) {
    auto m = methods[i];
    if(m->getName() == s) {
      return m;
    }
  }
  return nullptr;
}

vector<VarSymbol*> ClassSymbol::getVariables() const {
  return vars;
}

void ClassSymbol::addMethod(MethodSymbol* ms) {
  methods.push_back(ms);
}

void ClassSymbol::addVariable(VarSymbol* vs) {
  vars.push_back(vs);
}

void ClassSymbol::addVarInExtClass(VarSymbol* vs) {
  for(int i = 0; i < (int)vars.size(); i++) {
    if(vars[i]->getName() == vs->getName()) {
      vars[i] = vs;
      return;
    }
  }
  vars.push_back(vs);
}

void ClassSymbol::parentClass(ClassSymbol* cs) {
  auto m = cs->getMethods();
  auto v = cs->getVariables();
  methods.insert(methods.end(), m.begin(), m.end());
  vars.insert(vars.end(), v.begin(), v.end());
}

void ClassSymbol::replaceMethod(MethodSymbol* ms) {
  for(int i = 0; i < methods.size(); i++) {
    if(methods[i]->getName() == ms->getName()) {
      methods[i] = ms;
      return;
    }
  }
}
