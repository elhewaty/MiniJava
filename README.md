# MiniJava
MiniJava is a subset of Java. It's a simple language for educational purposes.</br>
Here is the [grammar](http://www.cs.tufts.edu/~sguyer/classes/comp181-2006/minijava.html) that I used to build the compiler. The Compiler in this repo is MiniJava to X64_86.</br>
**THIS COMPILER IS FULLY HAND WRITTEN. NO TOOLS WERE USED.**

# Development Status
The compiler is still under development here is the stages: </br>
```
Lexer: finished.
Parser: finished.
AST Building: finished.
Type Checker has to passes:
  Symbol Table building: Not yet.
  Checking phase by consulting symbol table: Not yet.
Code Generator: Not yet.
```
# Build and Run
`$ make`
run `./MiniJava` with `-h` flag to see the help list.</br>
You can Run with the following flags:</br>
* `-h` to show the help list.
* `-S` to see the program from the `Lexer` prospective (Tokens).
* `-P` to see the program from the `Parser` prospective for example:</br>
if we have `x + y * z` it becomes `(x + (y * z))`.
* I will add more flags for every finished phase.
# Motivation
This is a side-project I totally built myself. The project</br>
is for educational purposes. I studied compiler theories, and</br>
I wanted to implement the theories I learned.
# What you should know
To under stand the code, you don't have to understande Compilers' theories.</br>
But here are some highlights, that might help:
* Regular expressions
* Compiler components: Lexer, Parser, Type checking, IR, and Code generation.
* Know the order of compiler phases, and why we use this order?
* What the AST? Why We use it?
* What is the recursive-descent parser?
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
## Error Recovery:
The compiler Prints **Colored** error messages. it continues parsing always, but</br>
It exits if the source has 10 errors, couldn't parse the main class,</br>
found any data outside classes, or found any data outside a function.</br>
## Error Locations
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
The implementation is `Error.h` and `Error.cpp`
# AST
The Parser has two important jobs:</br>
* Check the syntax and make sure it's error free.
* Build the AST.
The AST is very important we use it in type-checking, and code generation.</br>
AST implementaion is in `AST.h`, and `AST.cpp`
