#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string>

// Keep track of the location

struct Location {
  std::string file;
  int line;
  int col;
};

enum TOK {
  // Symbol token
  __SYM_START,
  L_BRACKET,
  R_BRACKET,
  L_PAREN,
  R_PAREN,
  S_COLON,
  L_SBRACKET,
  R_SBRACKET,
  DOT,
  COMMA,
  QUOTE,
  __SYM_END,

  // Operations
  __OP_START,
  ASSIGN,
  LTHAN,
  TIMES,
  PLUS,
  MINUS,
  NOT,
  AND,
  __OP_END,

  // Key words
  __KEYWORD_START,
  CLASS,
  PUBLIC,
  STATIC,
  VOID,
  STRING,
  MAIN,
  EXTENDS,
  RETURN,
  BOOL,
  INT,
  IF,
  ELSE,
  WHILE,
  PRINT,
  LENGTH,
  TRUE,
  FALSE,
  THIS,
  NEW,
  __KEYWORD_END,

  // Others
  TOK_EOF,
  INTEGER_LITERAL,
  ID,
};


extern bool isKeyWord(TOK tk);
extern bool isIntLiteral(TOK tk);
extern bool isId(TOK tk);
extern bool isSymbol(TOK tk);
extern bool isOp(TOK tk);
extern bool isEOF(TOK tk);
#endif
