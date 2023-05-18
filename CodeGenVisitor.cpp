#include "CodeGenVisitor.h"
#include "Token.h"
#include <iostream>
#include <cassert>

void CodeGenVisitor::printTabs() const {
  std::string spaces = "";
  for (int i = 0; i < tabs * 4; i++) {
    spaces += " ";
  }
  std::cout << spaces;
}

void CodeGenVisitor::gen(string s) {
  printTabs();
  cout << s << endl;
}

void CodeGenVisitor::genBin(string opt, string src, string dst) {
  printTabs();
  cout << opt << " " << src << ", " << dst << endl;
}

string CodeGenVisitor::genLabel() {
  static int ln = 0;
  return ".L" + to_string(ln++);
}

string CodeGenVisitor::genLabel(string class_name, string method) {
  string label = method;
  if(class_name.size())
    label = class_name + "$" + label;
  return label;
}

void CodeGenVisitor::pushq(string s) {
  gen("pushq " + s);
}

void CodeGenVisitor::popq(string s) {
  gen("popq " + s);
}

void CodeGenVisitor::prologue() {
  pushq("%rbp");
  genBin("movq", "%rsp", "%rbp");
}

void CodeGenVisitor::epilogue() {
  genBin("movq", "%rbp", "%rsp");
  popq("%rbp");
  gen("ret");
}

int CodeGenVisitor::getObjSize(string type) {
  // all variables have 8-byte sizes, and pointers to other classes
  return (SymTab->classLookup(type)->getVariables().size() + 1) * 8;
}

void CodeGenVisitor::visit(Program *n) {
  gen(".data");
  for(int i = 0; i < n->cl.size(); i++) {
    auto c = n->cl.at(i);
    auto t = dynamic_pointer_cast<ClassDeclSimple>(c);
    if(t) {
      gen(t->i.s + "$$:");
      incTabs();
      gen(".quad 0");
      auto cs = SymTab->classLookup(t->i.s);
      if(cs) {
	auto mv = cs->getMethods();
        for(int i = 0; i < mv.size(); i++) {
          auto m = mv[i];
          gen(".quad " + t->i.s + "$" + m->getName());
        }
      }
      decTabs();
    } else {
      auto t = dynamic_pointer_cast<ClassDeclExtends>(c);
      gen(t->i.s + "$$:");
      incTabs();
      gen(".quad " + t->j.s+ "$$");
      auto cs = SymTab->classLookup(t->i.s);
      auto mv = cs->getMethods();
      for(int i = 0; i < mv.size(); i++) {
        auto m = mv[i];
        auto parent = m->getParent();
        string class_name = (parent != nullptr) ? parent->getName() : "";
        printTabs();
        cout << ".quad " + genLabel(class_name, m->getName()) << endl;
      }
      decTabs();
    }
  }
  cout << endl;
  gen(".text");
  gen(".globl asm_main");
  n->m.accept(*this);
  for(int i = 0; i < n->cl.size(); i++) {
    n->cl.at(i)->accept(*this);
  }
}

void CodeGenVisitor::visit(MainClass *n) {
  SymTab = SymTab->getScope(n->i.s);
  CurClass.push(n->i.s);
  SymTab = SymTab->getScope("main");
  stack_pos = 0;
  argument_idx = 0;
  CurMethod.push("main");
  gen("asm_main:");
  incTabs();
  stack_pos = 0;
  prologue();
  n->s->accept(*this);
  epilogue();
  SymTab = SymTab->exitScope();
  SymTab = SymTab->exitScope();
  CurMethod.pop();
  CurClass.pop();
}

void CodeGenVisitor::visit(ClassDeclSimple *n) {
  CurClass.push(n->i.s);
  SymTab = SymTab->getScope(n->i.s);
  // every class has a new stack frame, too
  stack_pos = 0;
  argument_idx = 0;
  for(int i = 0; i < n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
  }
  for(int i = 0; i < n->ml.size(); i++) {
    n->ml.at(i).accept(*this);
  }
  SymTab = SymTab->exitScope();
  CurClass.pop();
}

void CodeGenVisitor::visit(ClassDeclExtends *n) {
  SymTab = SymTab->getScope(n->i.s);
  CurClass.push(n->i.s);
  // likewise reset the position
  stack_pos = 0;
  argument_idx = 0;
  for(int i = 0; i < n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
  }
  for(int i = 0; i < n->ml.size(); i++) {
    n->ml.at(i).accept(*this);
  }
  SymTab = SymTab->exitScope();
  CurClass.pop();
}

void CodeGenVisitor::visit(VarDecl *n) {
  auto vs = SymTab->varLookup(n->i.s);
  if(vs) {
    stack_pos -= 8;
    var_loc[vs] = stack_pos;
  }
}

void CodeGenVisitor::visit(MethodDecl *n) {
  CurMethod.push(n->i.s);
  SymTab = SymTab->getScope(n->i.s);
  // the parameters are in: %rdi, %rsi, %rdx, %rcx, %r8, and %r9
  // respectively.
  // move the parameters to stack so we can use the registers.

  // function label
  string c = CurClass.top();
  stack_pos = 0;
  argument_idx = 0;
  cout << genLabel(c, n->i.s) << ":" << endl;
  prologue();
  // calculate the space needed for the arguments, and local variables
  int stack_size = 8 * (n->al.size() + n->vl.size() + 1);

  // We use 16-byte alignment
  if(stack_size > 0) {
    stack_size += (stack_size % 16);
    genBin("subq", "$" + to_string(stack_size), "%rsp");
  }
  // First we need to get the object that called the function
  // obj.foo(), or this.foo()
  // This is always stored as the first parameter
  // -8(%rbp)
  // movq %rdi, -8(%rbp)
  stack_pos -= 8;
  string dst = to_string(stack_pos) + "(%rbp)";
  genBin("movq", "%rdi", dst);
  argument_idx++;
  for(int i = 0; i < n->al.size(); i++) {
    n->al.at(i).accept(*this);
  }
  for(int i = 0; i < n->vl.size(); i++) {
    n->vl.at(i).accept(*this);
  }
  for(int i = 0; i < n->sl.size(); i++) {
    n->sl.at(i)->accept(*this);
  }
  // evaluate return expression, and put the return value in %rax.
  // return Expression;

  // evaluate expression
  n->e->accept(*this);
  // function epilogue
  epilogue();
  SymTab = SymTab->exitScope();
  CurMethod.pop();
}

void CodeGenVisitor::visit(Argument *n) {
  // movq arg_regs[argument_idx], offset(%rbp)
  // e.g movq %rdi, -8(%rbp)
  auto vs = SymTab->varLookup(n->i.s);
  if(vs) {
    stack_pos -= 8;
    setVarLoc(vs, stack_pos);
    string offset = to_string(stack_pos);
    string src = arg_regs[argument_idx++];
    genBin("movq", src, offset + "(%rbp)");
  }
}

void CodeGenVisitor::visit(ArrayType *n) {}
void CodeGenVisitor::visit(BoolType* n) {}
void CodeGenVisitor::visit(IntType* n) {}
void CodeGenVisitor::visit(IdentifierType* n) {}

void CodeGenVisitor::visit(BinOpExpression* n) {
  // Exp1 op Exp2
  n->LHS->accept(*this);
  pushq("%rax");
  n->RHS->accept(*this);
  // src --> %rbx, %dst --> %rax
  // Exp1 --> %rbx, Exp2 --> %rax
  switch(n->op) {
    case LTHAN:
      // %rbx < %rax
      popq("%rbx");
      genBin("cmpq", "%rbx", "%rax");
      gen(string("setg ") + "%al");
      genBin("movzbq", "%al", "%rax");
      break;
    case TIMES:
      // %rax * %r10 --> result in %rax
      popq("%rbx");
      genBin("imulq", "%rbx", "%rax");
      break;
    case PLUS:
      popq("%rbx");
      genBin("addq", "%rbx", "%rax");
      break;
    case MINUS:
      // Exp1 - Exp2 ---> %rbx - %rax
      // subq S, D subtracts `S` from `D`
      popq("%rbx");
      genBin("subq", "%rax", "%rbx");
      genBin("movq", "%rbx", "%rax");
      break;
    case AND:
      popq("%rbx");
      genBin("andq", "%rbx", "%rax");
      break;
    default:
      assert(0 && "Undefined binary operation");
  }
}

void CodeGenVisitor::visit(Block *n) {
  for(int i = 0; i < n->sl.size(); i++) {
    n->sl.at(i)->accept(*this);
  }
}

// if(exp) stat else stat
void CodeGenVisitor::visit(If *n) {
  n->e->accept(*this);
  // the test expression is in %rax
  string done_label = genLabel();
  string false_label = genLabel();
  genBin("testq ", "%rax", "%rax");
  gen("jz " + false_label);
  n->s1->accept(*this);
  gen("jmp " + done_label);
  cout << false_label << ":" << endl;
  n->s2->accept(*this);
  cout << done_label << ":" << endl;
}

void CodeGenVisitor::visit(While *n) {
  string loop_label = genLabel();
  string done_label = genLabel();
  cout << loop_label << ":" << endl;
  n->e->accept(*this);
  genBin("cmp", "$0", "%rax");
  gen(string("je ") + done_label);
  n->s->accept(*this);
  gen(string("jmp ") + loop_label);
  cout << done_label << ":" << endl;
}

void CodeGenVisitor::visit(Print *n) {
  n->e->accept(*this);
  // put the expression in the %rdi register so it can be
  // a parameter to printf
  genBin("movq", "%rax", "%rdi");
  gen("call put");
}

void CodeGenVisitor::visit(Assign *n) {
  // id = exp
  // get the value of exp in `%rax`
  n->e->accept(*this);
  // store it to be able to use it later, and use `rax`
  pushq("%rax");
  // this is tricky
  // let variable be `x`, `x` can apear in the following two forms:
  // 1) x = exp
  // 2) this.x = exp
  string dst = "";
  // let's search for the variable in the current function's scope
  auto vs = SymTab->varLookup(n->i.s);
  bool f
    = SymTab->classLookup(CurClass.top())->
        getMethod(CurMethod.top())->varExists(vs);
  if(f) {
    // Case 1:
    string offset = to_string(var_loc[vs]);
    dst = offset + "(%rbp)";
  } else {
    // Case 2:
    // get `this` in `rax`
    genBin("movq", "-8(%rbp)", "%rax");
    string offset = to_string(var_loc[vs]);
    dst = offset + "(%rax)";
  }
  // get the value in %r10
  popq("%r10");
  genBin("movq", "%r10", dst);
}


void CodeGenVisitor::visit(ArrayAssign *n) {
  // arr[exp1] = exp2
  // get index
  n->e1->accept(*this);
  // push index in the stack
  pushq("%rax");
  // get the value
  n->e2->accept(*this);
  genBin("movq", "%rax", "%r11");
  // ------------------- //
  // get the arr location two cases too.
  // arr[exp1] = exp2
  // this.arr[exp1] = exp2
  // movq exp2, 8(arr, exp1, 8)
  auto vs = SymTab->varLookup(n->i.s);
  bool f
    = SymTab->classLookup(CurClass.top())->
        getMethod(CurMethod.top())->varExists(vs);
  if(f) {
    // Case 1:
    string offset = to_string(var_loc[vs]);
    string arr = offset + "(%rbp)";
    pushq(arr);
  } else {
    // Case 2:
    // get `this` in `rax`
    genBin("movq", "-8(%rbp)", "%rax");
    string offset = to_string(var_loc[vs]);
    string arr = offset + "(%rax)";
    pushq(arr);
  }
  // get base in %r13
  popq("%r13");
  // index in %rbx
  popq("%rbx");
  // value in `%rax` ---> from n->e2->accept(*this)
  // movq %rax, 16(%r13, %rbx, 8)
  genBin("movq", "%r11", "16(%r13, %rbx, 8)");
}

void CodeGenVisitor::visit(ArrayLookup *n) {
  // Exp[Exp]
  n->e1->accept(*this);
  pushq("%rax");
  n->e2->accept(*this);
  popq("%rbx");
  // idx --> %rax, base --> %rbx
  // to get an element M[base + idx * 8 + 8]
  // it should be M[base + idx * 8], but the first 8 bytes
  // of the array contain the array length
  genBin("movq", "16(%rbx, %rax, 8)", "%rax");
}

void CodeGenVisitor::visit(ArrayLength *n) {
  n->e->accept(*this);
  genBin("movq", "0(%rax)", "%rax");
}

void CodeGenVisitor::visit(Call *n) {
  // exp.foo()
  n->e->accept(*this); // get the `object` or `this`.
  // push it as it should be the first parameter
  pushq("%rax");
  // evaluate the function parameters and store their values in the stack
  for(int i = 0; i < n->el.size(); i++){
    n->el.at(i)->accept(*this);
    pushq("%rax");
  }
  string class_name = "";
  // trivial way to execute the call we see which expression makes the call
  // as MiniJava has limited expression
  if(n->e->stringize() == "IdExp") {
    auto e = dynamic_pointer_cast<IdentifierExp>(n->e);
    auto sym = SymTab->symbolLookup(e->s);
    if(sym != nullptr && dynamic_pointer_cast<ClassSymbol>(sym)) {
      class_name = sym->getName();
    } else if(sym != nullptr && dynamic_pointer_cast<VarSymbol>(sym)){
      class_name = sym->getType();
    }
  } else if(n->e->stringize() == "call") {
    auto e = dynamic_pointer_cast<Call>(n->e);
    auto MethSym = SymTab->methodLookup(e->i.s);
    if(MethSym != nullptr) {
      class_name = MethSym->getParent()->getName();
    }
  } else if(n->e->stringize() == "this") {
    // We processing the current class so we need it's name
    class_name = CurClass.top();
  } else if(n->e->stringize() == "newObj") {
    auto e = dynamic_pointer_cast<NewObject>(n->e);
    class_name = e->i.s;
  } else {
    assert(0 && "Unhandled Expression for call, Please contact the author");
  }
  // now we have the class that called the function.
  // next step we need to know the function name.
  string function_name = n->i.s;
  // we need to know the function offset in the vtable
  int function_offset = -1;
  if(class_name.size()) {
    auto ClSym = tSymTab->classLookup(class_name);
    auto MethSym = ClSym ? ClSym->getMethod(function_name) : nullptr;
    assert(MethSym);
    function_offset
      = (MethSym && ClSym) ? 8 * ClSym->getMethodOffset(function_name) : -1;
  }
  // we need to pass the parametrs
  for(int i = n->el.size(); i >= 0; i--) {
    popq(arg_regs[i]);
  }
  if(function_offset != -1) {
    genBin("movq", "0(%rdi)", "%rax");
    gen("call *" + to_string(function_offset) + "(%rax)");
  } else {
    assert(0);
  }
}

void CodeGenVisitor::visit(IntegerLiteral *n) {
  if(n->numberValue == 0)
    genBin("xorq", "%rax", "%rax");
  else {
    genBin("movq", "$" + to_string(n->numberValue), "%rax");
  }
}

void CodeGenVisitor::visit(True *n) {
  genBin("movq", "$1", "%rax");
}

void CodeGenVisitor::visit(False *n) {
  genBin("xorq", "%rax", "%rax");
}

void CodeGenVisitor::visit(IdentifierExp *n) {
  string src = "";
  auto vs = SymTab->varLookup(n->s);
  bool f
    = SymTab->classLookup(CurClass.top())->
        getMethod(CurMethod.top())->varExists(vs);
  if(f) {
    // Case 1:
    string offset = to_string(var_loc[vs]);
    src = offset + "(%rbp)";
  } else {
    // Case 2:
    // get `this` in `rax`
    genBin("movq", "-8(%rbp)", "%rax");
    string offset = to_string(var_loc[vs]);
    src = offset + "(%rax)";
  }
  // get the value if this expression in `%rax`
  genBin("movq", src, "%rax");
}

void CodeGenVisitor::visit(This *n) {
  genBin("movq", "-8(%rbp)", "%rax");
}

void CodeGenVisitor::visit(NewArray *n) {
  // arr = new int[exp]
  // get exp in %rax
  n->e->accept(*this);
  // push the size of the array in the stack
  pushq("%rax");
  // multiply by 8
  gen("imulq $8, %rax");
  // the first slot for the array length
  genBin("addq", "$16", "%rax");
  // allocate memory
  gen("movq %rax, %rdi");
  gen("call mjcalloc");
  // we have a pointer to the allocated memory in %rax
  // put the size as the first element
  popq("0(%rax)");
  genBin("movq", "$16", "8(%rax)");
}

void CodeGenVisitor::visit(NewObject *n) {
  int objSize = getObjSize(n->i.s);
  objSize += (objSize % 16); // for alignment purposes
  genBin("movq", "$" + to_string(objSize), "%rdi");
  gen("call mjcalloc");
  // first 8 bytes in the object reserved for vtable
  genBin("movabs", "$" + n->i.s + "$$" , "%rcx");
  genBin("movq", "%rcx", "0(%rax)");
}

void CodeGenVisitor::visit(Not *n) {
  n->e->accept(*this);
  // now we have the value of the expression in %rax
  genBin("cmpq", "$0", "%rax"); // if `rax` has value
  gen("sete %al");
  gen("movzbl %al, %eax");
}

void CodeGenVisitor::visit(Identifier *n) {
  string src = "";
  auto vs = SymTab->varLookup(n->s);
  bool f
    = SymTab->classLookup(CurClass.top())->
        getMethod(CurMethod.top())->varExists(vs);
  if(f) {
    // Case 1:
    string offset = to_string(var_loc[vs]);
    src = offset + "(%rbp)";
  } else {
    // Case 2:
    // get `this` in `rax`
    genBin("movq", "-8(%rbp)", "%rax");
    string offset = to_string(var_loc[vs]);
    src = offset + "(%rax)";
  }
  // get the value if this expression in `%rax`
  genBin("movq", src, "%rax");
}

