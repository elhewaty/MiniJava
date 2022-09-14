#include "Lexer.h"
#include "Parser.h"
#include "PrettyPrintVisitor.h"
#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char const *argv[]) {
  string help = string("you can run MiniJava with these flags:\n") +
	        "\t-h to show this list\n" +
	        "\t-S to run the Scanner\n" +
	        "\t-P to run the Parser\n" +
	        "\t-A to print th AST\n" +
	        "\t-T to run the type checker\n" +
	        "\t-C to convert the code to assembly.\n";

  if(argc <= 2) {
    cout << "Error..\n";
    cout << help << endl;
    exit(1);
  }
  if(strcmp("-h", argv[1]) == 0) {
    cout << help;
  } else if(strcmp("-S", argv[1]) == 0) {
    Lexer l(argv[2]);
    l.lex();
  } else if(strcmp("-P", argv[1]) == 0) {
    Parser p(argv[2]);
    auto pr = p.parse();
    PrettyPrintVisitor pret;
    pr->accept(pret);
  }
  return 0;
}

