#include "TypeCheckerVisitor.h"

// Mainclass m
// ClassDeclList cl
string TypeCheckerVisitor::visit(Program* n) {
  classList = SymTable->getClassTable();
  n->m.accept(*this);
  for(int i = 0; i < n->cl.size(); i++) {
    n->cl.at(i)->accept(*this);
  }
  if(err.hasErrors()) {
    err.failure();
  }
  return "";
}

string TypeCheckerVisitor::visit(MainClass* n) {
  string s = n->i.accept(*this);
  SymTable = SymTable->getScope(n->i.s);
  CurClass.push(new ClassSymbol(n->i.s, ""));
  SymTable = SymTable->getScope("main");
  n->s->accept(*this);
  SymTable = SymTable->exitScope();
  SymTable = SymTable->exitScope();
  CurClass.pop();
  return s;
}

// Identifier i;
// VarDeclList vl;
// MethodDeclList ml;
string TypeCheckerVisitor::visit(ClassDeclSimple* n) {
  string s = n->i.accept(*this);
  CurClass.push(new ClassSymbol(n->i.s, ""));
  SymTable = SymTable->getScope(n->i.s);
  for(int i = 0; i < (int)n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
  }
  for(int i = 0; i < (int)n->ml.size(); i++) {
    n->ml.at(i).accept(*this);
  }
  SymTable = SymTable->exitScope();
  CurClass.pop();
  return s;
}

// Identifier i;
// Identifier j;
// VarDeclList vl;
// MethodDeclList ml;
string TypeCheckerVisitor::visit(ClassDeclExtends* n) {
  string s = n->i.accept(*this);
  string t = n->j.accept(*this);
  CurClass.push(new ClassSymbol(n->i.s, n->j.s));
  SymTable = SymTable->getScope(n->i.s);
  for(int i = 0; i < (int)n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
  }
  for(int i = 0; i < (int)n->ml.size(); i++) {
    n->ml.at(i).accept(*this);
  }
  SymTable = SymTable->exitScope();
  CurClass.pop();
  return s;
}

// Type *t
// Identifier
string TypeCheckerVisitor::visit(VarDecl* n) {
  return n->t->accept(*this);
}

// Type* t
// Identifier i
// ArgList al
// VarDeclList vl
// StatementList sl
// Expression *e
string TypeCheckerVisitor::visit(MethodDecl* n) {
  string s = n->i.accept(*this);
  SymTable = SymTable->getScope(n->i.s);
  if(n->al.size() > 6) {
    auto c = CurClass.top();
    cerr << BOLDRED << "Error:" << RESET << " function cannot have more than 6 "
    	 << "arguments." << endl;
    cerr << "location" << endl;
    cerr << "class: " << c->getName() << endl;
    cerr << "method: " << n->i.s << endl;
    err.error();
  }
  for(int i = 0; i < (int)n->al.size(); i++) {
    n->al.at(i).accept(*this);
  }
  for(int i = 0; i < (int)n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
  }
  for(int i = 0; i < (int)n->sl.size(); i++) {
    n->sl.at(i)->accept(*this);
  }
  string t = n->e->accept(*this);
  string tt = "";
  if(classList.count(t)) {
    tt = classList[t]->getType();
  }
  if(t != s && tt != s) {
    auto c = CurClass.top();
    cerr << BOLDRED << "Error" << RESET << ": return type mismatched"
    	 << " the actual type." << endl;
    if(t == "") {
      t = "Undefined type";
    }
    cerr << "       Reason: expect " << s << " found " << t << endl;
    err.error();
    cerr << "location" << endl;
    cerr << "class: " << c->getName() << endl;
    cerr << "method: " << n->i.s << endl;
    err.failure();
  }
  SymTable = SymTable->exitScope();
  return s;
}

// Type *t
// Identifier i
string TypeCheckerVisitor::visit(Argument* n) {
  return n->t->accept(*this);
}

string TypeCheckerVisitor::visit(ArrayType* n) {
  return "int[]";
}

string TypeCheckerVisitor::visit(BoolType* n) {
  return "boolean";
}

string TypeCheckerVisitor::visit(IntType* n) {
  return "int";
}

string TypeCheckerVisitor::visit(IdentifierType* n) {
  return n->s;
}

// + - * && <
string TypeCheckerVisitor::visit(BinOpExpression* n) {
  string s = n->LHS->accept(*this);
  string t = n->RHS->accept(*this);
  switch(n->op) {
    case AND:
      if(s != t || s != "boolean") {
      	err.emit(n->getLocation(),
      		 "Expected `boolean` operands for `&&`");
      }
    return "boolean";
    case LTHAN:
      if(s != t || s != "int") {
      	err.emit(n->getLocation(), "Expected `int` operands for `<`");
      }
    return "boolean";
    case PLUS:
    case TIMES:
    case MINUS:
      if(s != t || s != "int") {
      	err.emit(n->getLocation(),
      		 "Maths operations must have integer operands");
      }
      return "int";
   }
  return s;
}


string TypeCheckerVisitor::visit(Block* n) {
  for(int i = 0; i < (int)n->sl.size(); i++) {
    n->sl.at(i)->accept(*this);
  }
  return "";
}

// if(exp) stat else stat 
string TypeCheckerVisitor::visit(If* n) {
  string s = n->e->accept(*this);
  if(s != "boolean") {
    err.emit(n->e->getLocation(), "Expected boolean expression in `if`");
  }
  n->s1->accept(*this);
  n->s2->accept(*this);
  return "";
}

// while(exp) stat
string TypeCheckerVisitor::visit(While* n) {
  string s = n->e->accept(*this);
  if(s != "boolean") {
    err.emit(n->getLocation(), "Expected boolean expression in `while`");
  }
  n->s->accept(*this);
  return "";
}

// System.out.print(intexp)
string TypeCheckerVisitor::visit(Print* n) {
  string s = n->e->accept(*this);
  if(s != "int") {
    err.emit(n->getLocation(),
    	       "Expected integer expression in `System.out.println`");
  }
  return "";
}
// id = exp
string TypeCheckerVisitor::visit(Assign* n) {
  string s = n->i.accept(*this);
  string t = n->e->accept(*this);
  bool f = 0;
  if(s != t) {
    if(s == "int" && t == "boolean") {
      f = 1;
    } else if(s == "boolean" && t == "int") {
      f = 1;
    }
  }
  if(s == "" || f) {
    if(s == "") s = "Undefined type";
    err.emit(n->getLocation(),
    	     "can not assign " + t + " to " + s);
  }
  return s;
}

// id[exp] = exp
string TypeCheckerVisitor::visit(ArrayAssign* n) {
  string s = n->i.accept(*this);
  string t = n->e1->accept(*this);
  if(t != "int") {
    err.emit(n->getLocation(), "array index must be integer");
    return "";
  }
  if(s != "int[]") {
    err.emit(n->getLocation(), "MiniJava only supports integer arrays");
    return "";
  }
  string ss = n->e2->accept(*this);
  if(ss != "int") {
    err.emit(n->getLocation(),
    	     "can not assign " + ss + " to int in array" + n->i.s);
    return "";
  }
  return s;
}

// exp[exp]
string TypeCheckerVisitor::visit(ArrayLookup* n) {
  string s = n->e1->accept(*this);
  if(s != "int[]") {
    err.emit(n->getLocation(), "Undefined array type");
    return "";
  }
  string t = n->e2->accept(*this);
  if(t != "int") {
    err.emit(n->getLocation(), "array index must be integer");
    return "";
  }
  return t;
}

string TypeCheckerVisitor::visit(ArrayLength* n) {
  n->e->accept(*this);
  return "int";
}


// exp.func(exp, exp, ...)
string TypeCheckerVisitor::visit(Call* n) {
  string s = n->e->accept(*this);
  if(s == "") {
    err.emit(n->getLocation(), "Expected object name to call the function");
    return "";
  }
  if(classList.count(s) == 0) {
    err.emit(n->getLocation(), "Undefined class object" );
    return "";
  }
  MethodSymbol* MethSym = classList[s]->getMethod(n->i.s);
  if(MethSym == nullptr) {
    cerr << BOLDRED << "Error:" << RESET << " class " << s
         << " doesn't have function " + n->i.s << "." << endl;
    err.error();
    err.failure();
  }
  if(MethSym->getParameters().size() != n->el.size()) {
    err.emit(n->getLocation(),
    	     "function " + n->i.s + " expects "
    	     + to_string(MethSym->getParameters().size()) + " but "
    	     + to_string(n->el.size()) + " provided");
    return "";
  }
  if(n->el.size() > 6) {
    err.emit(n->getLocation(),
    	     "at function " + n->i.s + " MiniJava only allows 6 paramaters");
    return "";
  }
  auto v = MethSym->getParameters();
  for(int i = 0; i < (int)n->el.size(); i++) {
    string needed = v[i]->getType();
    string found = n->el.at(i)->accept(*this);
    Symbol* sym = (found != "") ? SymTable->symbolLookup(found) : nullptr;
    if(sym != nullptr && dynamic_cast<ClassSymbol*>(sym)) {
      string found__ = sym->getType();
      if(found != needed && found__ != needed) {
        err.emit(n->getLocation(),
      	         "function call " + n->i.s + " expects " + needed
      	         + " found " + found);
      }
    } else if(needed != found) {
      err.emit(n->getLocation(),
      	       "function call " + n->i.s + " expects " + needed
      	       + " found " + found);
    }
  }
  return MethSym->getType();
}

string TypeCheckerVisitor::visit(IntegerLiteral* n) {
  return "int";
}

string TypeCheckerVisitor::visit(True* n) {
  return "boolean";
}

string TypeCheckerVisitor::visit(False* n) {
  return "boolean";
}

string TypeCheckerVisitor::visit(IdentifierExp* n) {
  auto sym = SymTable->varLookup(n->s);
  if(sym == nullptr) {
    err.emit(n->getLocation(), "Symbol " + n->s + " was not declared");
    return "";
  }
  return sym->getType();
}

string TypeCheckerVisitor::visit(This* n) {
  ClassSymbol* ClSym = CurClass.top();
  return ClSym->getName();
}


// new int[e]
string TypeCheckerVisitor::visit(NewArray* n) {
  string s = n->e->accept(*this);
  if(s != "int") {
    err.emit(n->getLocation(), "array size must be integer");
    return "";
  }
  return "int[]";
}

// new class()
string TypeCheckerVisitor::visit(NewObject* n) {
  auto s = SymTable->classLookup(n->i.s);
  if(s == nullptr) {
    err.emit(n->getLocation(), "object " + n->i.s + " was not declared");
    return "";
  }
  return n->i.s;
}

string TypeCheckerVisitor::visit(Not* n) {
  return n->e->accept(*this);
}

string TypeCheckerVisitor::visit(Identifier* n) {
  Symbol* sym = SymTable->symbolLookup(n->s);
  if(sym == nullptr) {
    err.emit(n->getLocation(), "Symbol " + n->s + " was not declared");
    return "";
  }
  return sym->getType();
}
