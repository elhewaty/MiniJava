# MiniJava
[MiniJava](https://www.cambridge.org/resources/052182060X/) is a subset of Java. It's a simple language for educational purposes.</br>
Here is the [grammar](http://www.cs.tufts.edu/~sguyer/classes/comp181-2006/minijava.html) that I used to build the compiler. The Compiler in this repo is MiniJava to X64_86.</br>
**THIS COMPILER IS FULLY HAND WRITTEN. NO TOOLS WERE USED.**

# Development Status
The compiler is still under development here is the stages: </br>
```
Lexer: finished.
Parser: finished.
AST Building: finished.
Type Checker has to passes:
  Symbol Table building: finished.
  Checking phase by consulting symbol table: finished.
Code Generator: finished.
```
# Build and Run
`$ make` to build `MiniJava` from the source you will get `MiniJava` executable</br>
run `./MiniJava` with `-h` flag to see the help list.</br>
You can Run with the following flags:</br>
* `-h` to show the help list.
* `-S` to see the program from the `Lexer` prospective (Tokens).
* `-P` to see the program from the `Parser` prospective for example:</br>
if we have `x + y * z` it becomes `(x + (y * z))`.
* `-T` to run the type checker.
* `-C` to run the Code Generator, you will get a file called demo.s</br>
under `runtime/` this has the assembly code for your program.</br>
* After Generatin the assembly code you can get the executable of your program
</br>by executing `make run`, the executable name is `out`</br>
run the executable by typing `./out`
# Motivation
This is a side-project I totally built myself. The project</br>
is for educational purposes. I studied compiler theories, and</br>
I wanted to implement the theories I learned.
# Acknowledgement
I realy thank **University of Washington** for making [**CSE P 501**](https://courses.cs.washington.edu/courses/csep501/18sp/) Open source. This course is a great help and helped me a lot.</br>
# What you should know
To understand the code, you don't have to know Compilers' theories.</br>
But here are some highlights, that might help:
* Regular expressions
* Compiler components: Lexer, Parser, Type checking, IR, and Code generation.
* Know the order of compiler phases, and why we use this order?
* What the AST? Why We use it?
* What is the recursive-descent parser?
* Hash tables
* Graph theory
# Lexical Analysis
The lexer does the Lexical Analysis. Lexer implementaion is in `Lexer.h` and `Lexer.cpp`</br>
Here are most important functions in the Lexer:
* `getToken()`: Reads the Token stream, and put each group of</br>
charachters in the suitable group e.g, Key word, operation, ...etc.
* `getNextToken()`: Gets the next token in the input stream.
* `lookahead()`: Retuerns the next token in the input stream</br>
used in Parser as it's [LL](https://en.wikipedia.org/wiki/LL_parser)(1) type
* `eat(TOK, string)`: Takes two arguments the token, and the string form of it.</br>
Consumes the token if it's the expexted token, or emits an error.
# Parsing
The Parser is hand-crafted `LL(1)`. I didn't use any tools to build it,</br>
as I wanted to learn how Parsers are built internally.</br>
We needed to use `lookahead()` to check parse variable declarations as</br>
they have the same start tokens as assignment statements see [grammar](http://www.cs.tufts.edu/~sguyer/classes/comp181-2006/minijava.html)</br>
**NOTE**: The grammar is not LL but I converted it to be LL.</br>
Files `Parser.cpp`, and `Parser.h`
# Error Recovery:
The compiler Prints **Colored** error messages. it continues parsing always, but</br>
It exits if the source has 10 errors, couldn't parse the main class,</br>
If there's an error in a class, or method declaration, the compiler skips </br>
the class, or the method, but it emits a message of the error and a note</br>
that it's skipping the operation of the current class or method.</br>
If there is an error in a variable declaration, it skips till the next</br>
semi-colon.</br>
You can see the source program from the parser view by passing the `-P` flag.
# Error Locations
The Error message has a correct location for the parse error, **but**</br>
if the error is the last charchter in the line it expects the correct token to</br>
be in the next line for example:</br>
```
1 int x
2 int z;
output:
expected `;` at line 2
```
it expectes `;` instead of int.</br>
The implementation is in `Error.h` and `Error.cpp`
# AST
The Parser has two important jobs:</br>
* Check the syntax and make sure it's error free.
* Build the AST.
The AST is very important we use it in type-checking, and code generation.</br>
AST implementaion is in `AST.h`, and `AST.cpp`.</br>
**NOTE**:
For Any AST traversing in the compiler I used visitor pattern:</br>
* Print the source from the parser view.
* Building The symbol table.
* Type-checking.
* Code generation.
# Type-Checking / Semantic Analysis
After done with parsing we should check the semantics of the source code.</br>
We do this by two passes:
* Build a symbol table of every symbol in the program, and store its scope.
* Perform the type-checking consulting the symbol table.
## Building Symbol Table
We use the builtin hashmap `std::map` for the symbol table enteries.
This phase is seriously challenging assume the following problems:
* Variables and methods' Scopes, and overriding.
  Assume the following case:
  ```
  class A {
    int a;
    int fii() {int a; return a;}
    int tii() {retuen 0;}
  }
  class B extends A {
    int a;
    int foo(int a) {return a;}
    int fee() {return a;}
    int tii() {return 6;}
    int a() {return a;}
  }

  ```
  The compiler must handle the above case perfectly.
  in `SymbolTable/SymbolTable.h` and `SymbolTable/SymbolTable.cpp`, you see</br>
  the interface, and implementation of symbol table.</br>
  but the functions that interests you are `enterScope(string)`, and</br>
  `exitScope()`, these functions store the information of every scope it</br>
  hits. before entering a new scope, `enterScope` creates a Symbol table</br>
  entry, store the parent scope for the current scope and start storing it's</br>information.</br>
  After done with the current scope `exitScope` return the control to the</br>
  parent scope. 
* Cyclic inheritance.
  Assume the following case:
  ```
  class A extends B {}
  class B extends C {}
  class C extends A {}
  ```
  How can we know this? the solution is simple, just build a graph using
  the class relations. every node is a class, and every directed edge
  represents `extends`, and check for the cycles using simple `dfs`.
  * Class appears before it's used.
  This problem doesn't exist in C / C++, for example:
  ```
  class A extends B {}
  class B {}
  ```
  or
  ```
  Class A {B b;}
  Class B {A a;}
  ```
  How can we know this? answer:
  * For the first case, class `B` must be in the symbol table before class</br>
    `A`, so we use `topological sort` to know the order.
  * For the second case, If class `B` doesn't use class `A` object,</br>
    we could use `topological sort`, but there's a **cycle** in declaration.</br>
    how could we solve this? answer, just use a `map` e.g, `map<string, bool>`</br>and before building the symbol table just store all classes.</br>
    If we find the class in map then we don't emit an error. For example:</br>
    the map has two values: {{A, true}, {B, true}}, the we start building the</br>
    symbol table.</br>
  Merging the above two solutions, sort, and store, we can build the symbol table</br>successfully.
## Type-Checking
After building the Symbol table this phase becomes easy. Just use</br>
`getScope`, and `exiteScope` for every scope you hit. The type-checker</br>
consults the symbol table about variables and classes, e.g:</br>
* Return type mismatch.
* Use of undefined variable.
* Check if the object has the method e.g, `a.foo()`.
* Declare variable of undefined types e.g:
  ```
  class A {B b;}
  ```
* non-integer array index.
* non-boolean `if` expression.
* and many more...

You can find The Symentic Phase code in:
`SymbolTable/`, `TypeCheckerVisitor.h`, `TypeCheckerVisitor.cpp`,</br>
and `TypeVisitor.h`
## Code Generation
This phase was the most difficult although it seems easy, but the generated</br>
code had many `Segmentaion fault` errors most of them was because of `stack alignment`.</br>
The technique used here is so simple **traverse** the AST in execution order</br>
and emit the code in visitor methods.</br>
**One strategy that made the implementation easy is to treat X86 as a 1-register</br>
language with a stack for intermediate values** all operations writes `%rax` register</br>
The code however uses other register for some operations but not much.</br>
function calls was a little bit trickt to implement but the code uses **O(1) dynamic dispatch**.</br>
you can find the code in `CodeGeneratorVistor.h` and `CodeGeneratorVisitor.cpp`
## Language Runtime
The MiniJava program should have runtime environment e.g space for heap, and stack, </br>
`%rsp` and the other registers should have correct initial values, and a way </br>
to allocate memory for objects, and arrays.</br>
## Bootstrapping
We use the existing `C` runtime, under directory `runtime/` you will find</br>
a `C` program `boot.c`. This program calls the MiniJava main function</br>
as if it were a `C` function. By using this technique, it's possible</br>
to call standard `C` functions, and the program will now use the `C` ennvironment.
# Copyrights
I wrote All files in this project except `runtime`, and `SamplePrograms`</br>
they are from `CSE P 501` starter code, however you can use this code freely.</br>
