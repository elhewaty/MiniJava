#ifndef _LEXER_H_
#define _LEXER_H_

// MiniJava Lexer
// referece: http://www.cs.tufts.edu/~sguyer/classes/comp181-2006/minijava.html
// visit the previous link to see minijava grammar

#include "Error.h"
#include <deque>

using namespace std;

class Lexer {
public:
  Lexer(string filename)
    : curLocation({filename, 0, 0}) {}

  ~Lexer() = default;

  Location getLocation() const;
  void setLocation(int col, int line) {
    curLocation.col = col;
    curLocation.line = line;
  }

  TOK getNextToken();
  TOK getCurrentToken() const;

  void eat(TOK tok, std::string);

  // This function to see the next token for (LL(1))
  // to decide which rule to apply.
  TOK lookahead();

  std::string tokenToString(TOK tk) const;

  string getIdToken() const;
  long long int getNumberToken() const;

  void lex();
  void printToken(TOK tk);

private:
  Location curLocation;
  int curCol = 0;
  int curLine = 0;
  string tokenBuffer; // the current token we have
  string idToken;     // if the current token is id, this is its value
  string currentLineBuffer; // temp buffer for the current line
  // we read the charecters line by line. this the holder of the line
  deque<char> stream;
  // if the current token is integer literal, then here it's value
  long long int numberValue;
  char lastChar; // last character we read
  TOK curToken = TOK_EOF;
  ErrorHandling err; // for error handlin purposing
  TOK getToken();  // get the tokens
  int getNextChar(); // get the next char in the stream
};

#endif // _LEXER_H_