#include "PrettyPrintVisitor.h"


void PrettyPrintVisitor::printTabs() const {
  std::string spaces = "";
  for (int i = 0; i < tabs * 4; i++) {
    spaces += " ";
  }
  std::cout << spaces;
}

void PrettyPrintVisitor::visit(Program* n) {
  n->m.accept(*this);
  for(int i = 0; i < n->cl.size(); i++) {
    std::cout << std::endl;
    if(n->cl.at(i) == nullptr) {
      continue;
    }
    n->cl.at(i)->accept(*this);
  }
}

void PrettyPrintVisitor::visit(MainClass* n) {
  printTabs();
  std::cout << "class ";
  n->i.accept(*this);
  std::cout << " {" << std::endl;
  incTabs();
  printTabs();
  std::cout << "public static void main (String [] ";
  n->j.accept(*this);
  std::cout << ") {" << std::endl;
  incTabs();
  if(n->s) {
    n->s->accept(*this);
  }
  decTabs();
  printTabs();
  std::cout << "}" << std::endl;
  decTabs();
  printTabs();
  std::cout << "}" << std::endl;
}

void PrettyPrintVisitor::visit(ClassDeclSimple* n) {
  printTabs();
  std::cout << "class ";
  n->i.accept(*this);
  std::cout << " {" << std::endl;
  incTabs();
  for(int i = 0; i < n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
  }
  for(int i = 0; i < n->ml.size(); i++) {
    n->ml.at(i).accept(*this);
  }
  decTabs();
  std::cout << std::endl;
  printTabs();
  std::cout << "}" << std::endl;
}

void PrettyPrintVisitor::visit(ClassDeclExtends* n) {
  printTabs();
  n->i.accept(*this);
  std::cout << " extends ";
  n->j.accept(*this);
  std::cout << " {" << std::endl;
  incTabs();
  for(int i = 0; i < n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
  }
  std::cout << "\n";
  for(int i = 0; i < n->ml.size(); i++) {
    n->ml.at(i).accept(*this);
  }
  decTabs();
  std::cout << "\n";
  printTabs();
  std::cout << " }\n";
}

void PrettyPrintVisitor::visit(VarDecl* n) {
  printTabs();
  if(n->t) {
    n->t->accept(*this);
  }
  std::cout << " ";
  n->i.accept(*this);
  std::cout << ";\n";
}

void PrettyPrintVisitor::visit(MethodDecl* n) {
  std::cout << std::endl;
  printTabs();
  std::cout << "public ";
  if(n->t) {
    n->t->accept(*this);
  }
  std::cout << " ";
  n->i.accept(*this);
  std::cout << " (";
  for(int i = 0; i < n->al.size(); i++) {
    n->al.at(i).accept(*this);
    if(i + 1 < n->al.size()) {
      std::cout << ", ";
    }
  }
  std::cout << ") {\n";
  incTabs();
  for(int i = 0; i < n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
  }
  std::cout << std::endl;
  for(int i = 0; i < n->sl.size(); i++) {
    n->sl.at(i)->accept(*this);
  }
  printTabs();
  std::cout << "return ";
  if(n->e) {
    n->e->accept(*this);
  }
  std::cout << ";" << std::endl;
  decTabs();
  printTabs();
  std::cout << "}\n";
}

void PrettyPrintVisitor::visit(Argument* n) {
  if(n->t) {
    n->t->accept(*this);
  }
  std::cout << " ";
  n->i.accept(*this);
}

void PrettyPrintVisitor::visit(ArrayType* n) {
  std::cout << "int []";
}

void PrettyPrintVisitor::visit(BoolType* n) {
  std::cout << "boolean";
}

void PrettyPrintVisitor::visit(IntType* n) {
  std::cout << "int";
}

void PrettyPrintVisitor::visit(IdentifierType* n) {
  std::cout << n->s;
}

void PrettyPrintVisitor::visit(BinOpExpression* n) {
  std::cout << "(";
  if(n->LHS) {
    n->LHS->accept(*this);
  }
  switch(n->op) {
    case AND:
      std::cout << " && ";
      break;
    case TIMES:
      std::cout << " * ";
      break;
    case PLUS:
      std::cout << " + ";
      break;
    case MINUS:
      std::cout << " - ";
      break;
    case LTHAN:
      std::cout << " < ";
      break;
    default:
      std::cerr << "Unknown binary operation" << std::endl;
      break;
  }
  if(n->RHS) {
    n->RHS->accept(*this);
  }
  std::cout << ")";
}

void PrettyPrintVisitor::visit(Block* n) {
  std::cout << "{\n";
  incTabs();
  for(int i = 0; i < n->sl.size(); i++) {
    if(n->sl.at(i) == nullptr) {
      continue;
    } 
    n->sl.at(i)->accept(*this);
  }
  decTabs();
  printTabs();
  std::cout << "}\n";
}

void PrettyPrintVisitor::visit(If* n) {
  printTabs();
  std::cout << "If (";
  if(n->e) {
    n->e->accept(*this);
  }
  std::cout << ") ";
  if(!dynamic_cast<Block*>(n->s1.get())) {
    std::cout << std::endl;
    incTabs();
  }
  if(n->s1) {
    n->s1->accept(*this);
    if(!dynamic_cast<Block*>(n->s1.get())) {
      decTabs();
    }
  }
  printTabs();
  std::cout << "else ";
  if(n->s2) {
    if(!dynamic_cast<Block*>(n->s2.get())) {
      std::cout << std::endl;
      incTabs();
    }
    n->s2->accept(*this);
    if(!dynamic_cast<Block*>(n->s2.get())) {
      decTabs();
    }
  }
}

void PrettyPrintVisitor::visit(While* n) {
  printTabs();
  std::cout << "while (";
  if(n->e) {
    n->e->accept(*this);
  }
  std::cout << ")";
  if(n->s) {
    n->s->accept(*this);
  }
}

void PrettyPrintVisitor::visit(Print* n) {
  printTabs();
  std::cout << "System.out.println(";
  if(n->e) {
    n->e->accept(*this);
  }
  std::cout << ");" << std::endl;
}

void PrettyPrintVisitor::visit(Assign* n) {
  printTabs();
  n->i.accept(*this);
  std::cout << " = ";
  if(n->e) {
    n->e->accept(*this);
  }
  std::cout << ";\n";
}

void PrettyPrintVisitor::visit(ArrayAssign* n) {
  printTabs();
  n->i.accept(*this);
  std::cout << "[";
  if(n->e1) {
    n->e1->accept(*this);
  }
  std::cout << "]";
  std::cout << " = ";
  if(n->e2) {
    n->e2->accept(*this);
  }
  std::cout << ";\n";
}

void PrettyPrintVisitor::visit(ArrayLookup* n) {
  if(n->e1) {
    n->e1->accept(*this);
  }
  std::cout << "[";
  if(n->e2) {
    n->e2->accept(*this);
  }
  std::cout << "]";
}

void PrettyPrintVisitor::visit(ArrayLength* n) {
  if(n->e) {
    n->e->accept(*this);
  }
  std::cout << ".length";
}

void PrettyPrintVisitor::visit(Call* n) {
  if(n->e) {
    n->e->accept(*this);
  }
  std::cout << ".";
  n->i.accept(*this);
  std::cout << "(";
  for (int i = 0; i < n->el.size(); i++) {
    if(n->el.at(i) == nullptr) {
      continue;
    }
    n->el.at(i)->accept(*this);
    if(i + 1 < n->el.size()) {
      std::cout << ", ";
    }
  }
  std::cout << ")";
}

void PrettyPrintVisitor::visit(IntegerLiteral* n) {
  std::cout << n->numberValue;
}

void PrettyPrintVisitor::visit(True* n) {
  std::cout << "true";
}

void PrettyPrintVisitor::visit(False* n) {
  std::cout << "false";
}

void PrettyPrintVisitor::visit(IdentifierExp* n) {
  std::cout << n->s;
}

void PrettyPrintVisitor::visit(This* n) {
  std::cout << "this";
}

void PrettyPrintVisitor::visit(NewArray* n) {
  std::cout << "new int [";
  if(n->e) {
    n->e->accept(*this);
  }
  std::cout << "]";
}

void PrettyPrintVisitor::visit(NewObject* n) {
  std::cout << "new ";
  std::cout << n->i.s;
  std::cout << "()";  
}

void PrettyPrintVisitor::visit(Not* n) {
  std::cout << "!";
  n->e->accept(*this);
}

void PrettyPrintVisitor::visit(Identifier* n) {
  std::cout << n->s;
}
