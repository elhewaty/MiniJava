#include "Token.h"

bool isKeyWord(TOK tk) {
  return tk >= __KEYWORD_START && tk <= __KEYWORD_END;
}

bool isIntLiteral(TOK tk) {
  return tk == INTEGER_LITERAL;
}

bool isId(TOK tk) {
  return tk == ID;
}

bool isSymbol(TOK tk) {
  return tk >= __SYM_START && tk <= __SYM_END;
}

bool isOp(TOK tk) {
  return tk >= __OP_START && tk <= __OP_END;
}

bool isEOF(TOK tk) {
  return tk == TOK_EOF;
}