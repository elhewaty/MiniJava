#include "SymbolTableVisitor.h"

shared_ptr<SymbolTable> SymbolTableVisitor::getSymbolTable() const {
  return SymTable;
}

// Mainclass m
// ClassDeclList cl
void SymbolTableVisitor::visit(Program* n) {
  map<string, shared_ptr<ClassDecl>>m;
  mainClassName = n->m.i.s;
  for(int i = 0; i < n->cl.size(); i++) {
    string s = "", t = "";
    auto c = dynamic_pointer_cast<ClassDeclSimple>(n->cl.at(i));
    if(c) {
      if(c->i.s == "") {
        continue;
      }
      s = c->i.s;
      m[c->i.s] = n->cl.at(i);
    } else {
      auto c = dynamic_pointer_cast<ClassDeclExtends>(n->cl.at(i));
      if(c->i.s == "") {
        continue;
      }
      s = c->i.s;
      t = c->j.s;
      m[c->i.s] = n->cl.at(i);
    }
    if(s == "") {
      continue;
    }
    SymTable->addClass(make_shared<ClassSymbol>(s, t));
    exist[s] = 1;
  }
  vector<shared_ptr<ClassDecl>> cl;
  for(auto u : ord) {
    cl.push_back(m[u]);
  }
  for(auto u : cl) {
    if(u == nullptr) {
      continue;
    }
    u->accept(*this);
  }
  n->m.accept(*this);
}

// main function
void SymbolTableVisitor::visit(MainClass* n) {
  shared_ptr<ClassSymbol>ClSym = make_shared<ClassSymbol>(n->i.s, "");
  SymTable->addClass(ClSym);
  // Enter class Scope
  SymTable = SymTable->enterScope(n->i.s);
  shared_ptr<MethodSymbol> MethSym = make_shared<MethodSymbol>("main", "void");
  // Enter main function scope
  SymTable = SymTable->enterScope("main");
  CurMeth.push(MethSym);
  SymTable->addMethod(MethSym);
  shared_ptr<VarSymbol> VarSym = make_shared<VarSymbol>(n->j.s, "String[]");
  MethSym->addParameter(VarSym);
  SymTable->addVar(VarSym);
  n->s->accept(*this);
  // Exit main function scope
  SymTable = SymTable->exitScope();
  CurMeth.pop();
  // Exit class Scope
  SymTable = SymTable->exitScope();
}

// Identifier i;
// VarDeclList vl;
// MethodDeclList ml;
void SymbolTableVisitor::visit(ClassDeclSimple* n) {
  if(n->i.s == mainClassName) {
    err.emit(n->i.getLocation(), "class has the same name as the main class");
  }
  shared_ptr<ClassSymbol> ClSym = make_shared<ClassSymbol>(n->i.s, "");
  SymTable->addClass(ClSym);
  // Enter the class scope
  SymTable = SymTable->enterScope(n->i.s);
  // Collect member variables information
  for(int i = 0; i < (int)n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
    // Add the Variable
    auto sym = SymTable->getVarTable()[n->vl.at(i).i.s];
    if(sym != nullptr) {
      ClSym->addVariable(sym);
    }
  }
  // Collect member functions information
  for(int i = 0; i < (int)n->ml.size(); i++) {
    n->ml.at(i).accept(*this);
    auto sym = SymTable->getMethodTable()[n->ml.at(i).i.s];
    if(sym != nullptr) {
      ClSym->addMethod(sym);
    }
  }
  SymTable = SymTable->exitScope();
}

// Identifier i;
// Identifier j;
// VarDeclList vl;
// MethodDeclList ml;
void SymbolTableVisitor::visit(ClassDeclExtends* n) {
  if(n->i.s == mainClassName) {
    err.emit(n->i.getLocation(), "class has the same name as the main class");
  }
  shared_ptr<ClassSymbol> ClSym = make_shared<ClassSymbol>(n->i.s, n->j.s);
  if(n->j.s == mainClassName) {
    err.emit(n->i.getLocation(), "Main class can not be extended");
  }
  SymTable->addClass(ClSym);
  // Add inherited members
  auto extScope = SymTable->getSymbolTable(n->j.s);
  if(extScope != nullptr) {
    auto sym = extScope->classLookup(n->j.s);
    if(sym != nullptr) {
      ClSym->parentClass(sym);
    }
  } else {
    err.emit(n->i.getLocation(), "class " + n->j.s + " doesn't exist");
    err.failure();
  }

  SymTable = SymTable->enterScope(n->i.s);
  
  // Collect member variables information
  for(int i = 0; i < (int)n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
    // Add the Variable
    auto sym = SymTable->getVarTable()[n->vl.at(i).i.s];
    if(sym != nullptr) {
      ClSym->addVarInExtClass(sym);
    }
  }
  auto tab = SymTable->getVarTable();
  auto v = ClSym->getVariables();
  for(auto u : v) {
    if(tab.count(u->getName())) {
      continue;
    }
    SymTable->addVar(u);
  }
  // Collect member functions information
  for(int i = 0; i < (int)n->ml.size(); i++) {
    MethodDecl m = n->ml.at(i);
    m.accept(*this);
    auto MethSym = SymTable->getMethodTable()[m.i.s];
    if(MethSym != nullptr) {
      // If the function is extended.
      // Check if the function is overridden.
      auto MethSymExt = ClSym->getMethod(MethSym->getName());
      if(MethSymExt != nullptr &&
	MethSymExt->stringize() != MethSym->stringize()) {
	cerr << BOLDRED << "Error:" << RESET
	     << " Method override of function `" + m.i.s + "` doesn't match "
	     << "the parent class method." << endl;
        cerr << "location:" << endl;
        cerr << "class: " << n->i.s << endl;
	err.error();
      } else {
        SymTable->addMethod(MethSym);
	// If it's new function add it.
        if(MethSymExt == nullptr) {
          ClSym->addMethod(MethSym);
        } else {
          ClSym->replaceMethod(MethSym);
        }
      }
    }
  }
  SymTable = SymTable->exitScope();
}

// Type* t
// Identifier i
void SymbolTableVisitor::visit(VarDecl* n) {
  // Check if the variable defined before.
  // If we are in a method scope, and the var is declared
  // in the class scope we should ignore the class scope var.
  string t = n->t->stringize();
  if(!isBuiltInType(t) && !exist.count(t)) {
    if(t == mainClassName) {
      err.emit(n->t->getLocation(), "main class can not be instantiated");
    } else {
      err.emit(n->t->getLocation(), "class " + t + " is undefined");
      err.failure();
    }
  }
  bool declared = false;
  shared_ptr<VarSymbol> v = make_shared<VarSymbol>(n->i.s, t);
  if(!CurMeth.empty()) {
    declared = CurMeth.top()->varExists(v);
    CurMeth.top()->addLocal(v);
  } else {
    declared = (SymTable->varLookup(n->i.s)) != nullptr;
  }
   
  if(declared) {
    err.emit(n->i.getLocation(), "redifinition of variable " + n->i.s);
  }
  SymTable->addVar(v);
}

// Type* t
// Identifier i
// ArgList al
// VarDeclList vl
// StatementList sl
// Expression *e
void SymbolTableVisitor::visit(MethodDecl* n) {
  // Check if the function declared before
  if(n->t->stringize() == mainClassName) {
    err.emit(n->getLocation(),
             "can use main class types as a return types");
  }
  auto sym = SymTable->methodLookup(n->i.s);
  if(sym != nullptr) {
    err.emit(n->i.getLocation(),
	     "Method " + sym->getName() + " was declared before.");
    return;
  }
  shared_ptr<MethodSymbol> MethSym
    = make_shared<MethodSymbol>(n->i.s, n->t->stringize());
  if(n->al.size() > 6) {
    err.emit(n->getLocation(), "Too many args. Please Pass at most 6 args.");
  }
  SymTable->addMethod(MethSym);
  SymTable = SymTable->enterScope(MethSym->getName());
  CurMeth.push(MethSym);
  for(int i = 0; i < (int)n->al.size(); i++) {
    MethSym->addParameter(make_shared<VarSymbol>(n->al.at(i).i.s,
                          n->al.at(i).t->stringize()));
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
  SymTable = SymTable->exitScope();
  CurMeth.pop();
}

// Type *t
// Identifier i
void SymbolTableVisitor::visit(Argument* n) {
  bool declared = false;
  string t = n->t->stringize();
  if(!isBuiltInType(t) && !exist.count(t)) {
    if(t == mainClassName) {
      err.emit(n->t->getLocation(), "main class can not be instantiated");
    } else {
      err.emit(n->t->getLocation(), "class " + t + " is undefined");
      err.failure();
    }
  }
  shared_ptr<VarSymbol> v = make_shared<VarSymbol>(n->i.s, t);
  if(!CurMeth.empty()) {
    declared = CurMeth.top()->varExists(v);
    CurMeth.top()->addLocal(v);
  } else {
    declared = (SymTable->varLookup(n->i.s)) != nullptr;
  }

  if(declared) {
    err.emit(n->getLocation(), "redifinition of variable " + n->i.s);
  }
  SymTable->addVar(v);
}


void SymbolTableVisitor::visit(ArrayType* n) {
}

void SymbolTableVisitor::visit(BoolType* n) {
}

void SymbolTableVisitor::visit(IntType* n) {
}

void SymbolTableVisitor::visit(IdentifierType* n) {
}

void SymbolTableVisitor::visit(BinOpExpression* n) {
  n->LHS->accept(*this);
  n->RHS->accept(*this);
}

void SymbolTableVisitor::visit(Block* n) {
  for(int i = 0; i < n->sl.size(); i++) {
    n->sl.at(i)->accept(*this);
  }
}

void SymbolTableVisitor::visit(If* n) {
  n->e->accept(*this);
  n->s1->accept(*this);
  n->s2->accept(*this);
}

void SymbolTableVisitor::visit(While* n) {
  n->e->accept(*this);
  n->s->accept(*this);
}

void SymbolTableVisitor::visit(Print* n) {
  n->e->accept(*this);
}

void SymbolTableVisitor::visit(Assign* n) {
  n->i.accept(*this);
  n->e->accept(*this);
}

void SymbolTableVisitor::visit(ArrayAssign* n) {
  n->i.accept(*this);
  n->e1->accept(*this);
  n->e2->accept(*this);
}

void SymbolTableVisitor::visit(ArrayLookup* n) {
  n->e1->accept(*this);
  n->e2->accept(*this);
}

void SymbolTableVisitor::visit(ArrayLength* n) {
  n->e->accept(*this);
}

void SymbolTableVisitor::visit(Call* n) {
  n->e->accept(*this);
  n->i.accept(*this);
  for (int i = 0; i < n->el.size(); i++) {
    n->el.at(i)->accept(*this);
  }
}

void SymbolTableVisitor::visit(IntegerLiteral* n) {
}

void SymbolTableVisitor::visit(True* n) {
}

void SymbolTableVisitor::visit(False* n) {
}

void SymbolTableVisitor::visit(IdentifierExp* n) {
}

void SymbolTableVisitor::visit(This* n) {
}

void SymbolTableVisitor::visit(NewArray* n) {
  n->e->accept(*this);
}

void SymbolTableVisitor::visit(NewObject* n) {
  n->i.accept(*this);
}

void SymbolTableVisitor::visit(Not* n) {
  n->e->accept(*this);
}

void SymbolTableVisitor::visit(Identifier* n) {
}

