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
  if(n->cl.size() == 0) {
    return;
  }
  for(int i = 0; i < n->cl.size(); i++) {
    std::cout << std::endl;    
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
  n->s->accept(*this);
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
  n->t->accept(*this);
  std::cout << " ";
  n->i.accept(*this);
  std::cout << ";\n";
}

void PrettyPrintVisitor::visit(MethodDecl* n) {
  std::cout << std::endl;
  printTabs();
  std::cout << "public ";
  n->t->accept(*this);
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
  n->e->accept(*this);
  std::cout << ";" << std::endl;
  decTabs();
  printTabs();
  std::cout << "}\n";
}

void PrettyPrintVisitor::visit(Argument* n) {
  n->t->accept(*this);
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
  n->LHS->accept(*this);
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
  n->RHS->accept(*this);
  std::cout << ")";
}

void PrettyPrintVisitor::visit(Block* n) {
  std::cout << "{\n";
  incTabs();
  for(int i = 0; i < n->sl.size(); i++) {
    n->sl.at(i)->accept(*this);
  }
  decTabs();
  printTabs();
  std::cout << "}\n";
}

void PrettyPrintVisitor::visit(If* n) {
  printTabs();
  std::cout << "If (";
  n->e->accept(*this);
  std::cout << ") ";
  if(!dynamic_cast<Block*>(n->s1)) {
    std::cout << std::endl;
    incTabs();
  }
  n->s1->accept(*this);
  if(!dynamic_cast<Block*>(n->s1)) {
    decTabs();
  }
  printTabs();
  std::cout << "else ";
  if(!dynamic_cast<Block*>(n->s2)) {
    std::cout << std::endl;
    incTabs();
  }
  n->s2->accept(*this);
  if(!dynamic_cast<Block*>(n->s2)) {
    decTabs();
  }
}

void PrettyPrintVisitor::visit(While* n) {
  printTabs();
  std::cout << "while (";
  n->e->accept(*this);
  std::cout << ")";
  n->s->accept(*this);
}

void PrettyPrintVisitor::visit(Print* n) {
  printTabs();
  std::cout << "System.out.println(";
  n->e->accept(*this);
  std::cout << ");" << std::endl;
}

void PrettyPrintVisitor::visit(Assign* n) {
  printTabs();
  n->i.accept(*this);
  std::cout << " = ";
  n->e->accept(*this);
  std::cout << ";\n";
}

void PrettyPrintVisitor::visit(ArrayAssign* n) {
  printTabs();
  n->i.accept(*this);
  std::cout << "[";
  n->e1->accept(*this);
  std::cout << "]";
  std::cout << " = ";
  n->e2->accept(*this);
  std::cout << ";\n";
}

void PrettyPrintVisitor::visit(ArrayLookup* n) {
  n->e1->accept(*this);
  std::cout << "[";
  n->e2->accept(*this);
  std::cout << "]";
}

void PrettyPrintVisitor::visit(ArrayLength* n) {
  n->e->accept(*this);
  std::cout << ".length";
}

void PrettyPrintVisitor::visit(Call* n) {
  n->e->accept(*this);
  std::cout << ".";
  n->i.accept(*this);
  std::cout << "(";
  for (int i = 0; i < n->el.size(); i++) {
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
  n->e->accept(*this);
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
