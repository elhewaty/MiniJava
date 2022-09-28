#include "Lexer.h"
#include <climits>
#include <cassert>

// Get the location of the current token
Location Lexer::getLocation() const {
  return curLocation;
}

TOK Lexer::getCurrentToken() const {
  return curToken;
}

std::string Lexer::tokenToString(TOK tk) const {
  if(isId(tk)) {
    return getIdToken();
  } else if(isKeyWord(tk)) {
    return tokenBuffer;
  } else if(isIntLiteral(tk)) {
    return std::to_string(getNumberToken());
  } else if(isOp(tk)) {
    if(tk == PLUS) {
      return "+";
    } else if(tk == MINUS) {
      return "-";
    } else if(tk == TIMES) {
      return "*";
    } else if(tk == ASSIGN) {
      return "=";
    } else if(tk == LTHAN) {
      return "<";
    } else if(tk == NOT) {
      return "!";
    } else if(tk == AND) {
      return "&&";
    }
  } else if(isSymbol(tk)) {
    if(tk == L_BRACKET) {
      return "{";
    } else if(tk == R_BRACKET) {
      return "}";
    } else if(tk == L_PAREN) {
      return "(";
    } else if(tk == R_PAREN) {
      return ")";
    } else if(tk == S_COLON) {
      return ";";
    } else if(tk == L_SBRACKET) {
      return "[";
    } else if(tk == R_SBRACKET) {
      return "]";
    } else if(tk == DOT) {
      return ".";
    } else if(tk == COMMA) {
      return ",";
    } else if(tk == QUOTE) {
      return "\"";
    }
  }
  assert(0);
}

TOK Lexer::lookahead() {
  // Store current token info
  TOK cur = curToken;
  Location curloc = curLocation;
  std::string pre = tokenToString(cur);
  // Get next token
  TOK re = getNextToken();
  // return the values back
  std::string s = tokenToString(re);
  for(int i = s.size() - 1; i >= 0; i--) {
    stream.push_front(s[i]);
  }
  curToken = cur;
  curLocation = curloc;
  curCol = curloc.col;
  curLine = curloc.line;
  if(isKeyWord(curToken)) {
    tokenBuffer = pre;
  } else if(isIntLiteral(curToken)) {
    numberValue = atoi(pre.c_str());
  } else if(curToken == ID) {
    idToken = pre;
  }
  return re;
}

TOK Lexer::getNextToken() {
  return curToken = getToken();
}

void Lexer::eat(TOK tk, std::string s) {
  if(tk != curToken) {
    err.emit(getLocation(),
             string("Expected `") + s + "`");
    if(isKeyWord(curToken)) {
      return;
    }
  }
  getNextToken();
}

long long int Lexer::getNumberToken() const {
  return numberValue;
}

string Lexer::getIdToken() const {
  return idToken;
}

int Lexer::getNextChar() {
  static int f = 0;
  if(stream.empty()) {
    if(f) {
      return EOF;
    }
    getline(cin, currentLineBuffer);
    if(cin.eof()) {
      f = 1;
    }
    curLine++;
    curCol = 0;
    if(currentLineBuffer.size()) {
      stream = deque(currentLineBuffer.begin(), currentLineBuffer.end());
      stream.push_back('\n');
    } else {
      if(cin.eof()) {
        return EOF;
      }
      stream.push_front('\n');
    }
  }
  char c = stream.front();
  curCol++;
  stream.pop_front();
  return c;
}

// alphabet = [a-z][A-Z]
// digits = [0-9]
// idToken = (_|alphabet)(alphabet | digits | _)*
// numberToken = (-|+)? digit*

TOK Lexer::getToken() {
  lastChar = ' ';
  // Ignore spaces
  while(isspace(lastChar)) {
    lastChar = getNextChar();
  }

  // Ignore comments
  if(lastChar == '/') {
    lastChar = getNextChar();
    if(lastChar == '/') {
      do {
      	lastChar = getNextChar();
      } while(lastChar != EOF && lastChar != '\n' && lastChar != '\r');
      if(lastChar != EOF) {
      	stream.push_front(lastChar);
        return getToken();
      }
    } else {
      err.emit(getLocation(), "Did you mean a comment? if so use '//'.");
      return getToken();
    }
  }

  setLocation(curCol, curLine);
  // Scan idToken
  if(isalpha(lastChar) || lastChar == '_') {
    tokenBuffer = lastChar;
    lastChar = getNextChar();
    while(lastChar == '_' || isdigit(lastChar) || isalpha(lastChar)) {
      tokenBuffer += lastChar;
      lastChar = getNextChar();
    }
    // Check print statement first
    if(tokenBuffer == "System" && lastChar == '.') {
      // System.out.println
      string needed = "System.out.println";
      int pos = 6;
      while(pos < needed.size()) {
      	if(lastChar != needed[pos]) {
      	  break;
      	}
      	pos++;
      	tokenBuffer += lastChar;
      	lastChar = getNextChar();
      }
      if(needed == tokenBuffer) {
      	stream.push_front(lastChar);
        curCol--;
      	return PRINT;
      }
      err.emit(getLocation(),
      	       "Unknown statemen did you mean '" + needed + "'?");
      return getToken();
    }
    // Check string tokens
    stream.push_front(lastChar);
    curCol--;
    if("class" == tokenBuffer) {
      return CLASS;
    } else if("public" == tokenBuffer) {
      return PUBLIC;
    } else if("static" == tokenBuffer) {
      return STATIC;
    } else if("void" == tokenBuffer) {
      return VOID;
    } else if("main" == tokenBuffer) {
      return MAIN;
    } else if("extends" == tokenBuffer) {
      return EXTENDS;
    } else if("return" == tokenBuffer) {
      return RETURN;
    } else if("boolean" == tokenBuffer) {
      return BOOL;
    } else if("int" == tokenBuffer) {
      return INT;
    } else if("if" == tokenBuffer) {
      return IF;
    } else if ("else" == tokenBuffer){
      return ELSE;
    }else if("while" == tokenBuffer) {
      return WHILE;
    } else if("length" == tokenBuffer) {
      return LENGTH;
    } else if("true" == tokenBuffer) {
      return TRUE;
    } else if("false" == tokenBuffer) {
      return FALSE;
    } else if("this" == tokenBuffer) {
      return THIS;
    } else if("new" == tokenBuffer){
      return NEW;
    } else if("String" == tokenBuffer){
      return STRING;
    } else {
      idToken = tokenBuffer;
      return ID;
    }
    stream.pop_front();
  }

  // Check integers
  if(isdigit(lastChar)) {
    numberValue = (lastChar - '0');
    lastChar = getNextChar();
    while(isdigit(lastChar)) {
      numberValue *= 10;
      numberValue += (lastChar - '0');
      if(numberValue > INT_MAX) {
        err.emit(getLocation(), "MiniJava only support integer formats.");
  	while(isdigit(lastChar)) {
  	  // Eat all digits
  	  lastChar = getNextChar();
	}
  	return getToken();
      }
      lastChar = getNextChar();
    }
    stream.push_front(lastChar);
    curCol--;
    return INTEGER_LITERAL;
  }

  // And (&&) should be handled too.
  if(lastChar == '&') {
    lastChar = getNextChar();
    if(lastChar == '&') {
      return AND;
    }
    err.emit(getLocation(), "Unknown symbol `&` did you mean `&&`?");
    return getToken();
  } else if(lastChar == '=') {
    return ASSIGN;
  } else if(lastChar == '<') {
    return LTHAN;
  } else if(lastChar == '*') {
    return TIMES;
  } else if(lastChar == '+') {
    return PLUS;
  } else if(lastChar == '-') {
    return MINUS;
  } else if(lastChar == '!') {
    return NOT;
  } else if(lastChar == '{') {
    return L_BRACKET;
  } else if(lastChar == '}') {
    return R_BRACKET;
  } else if(lastChar == '(') {
    return L_PAREN;
  } else if(lastChar == ')') {
    return R_PAREN;
  } else if(lastChar == ';') {
    return S_COLON;
  } else if(lastChar == '[') {
    return L_SBRACKET;
  } else if(lastChar == ']') {
    return R_SBRACKET;
  } else if(lastChar == '.') {
    return DOT;
  } else if(lastChar == ',') {
    return COMMA;
  } else if(lastChar == '"') {
    return QUOTE;
  } else {
    if(lastChar == EOF) {
      return TOK_EOF;
    }
    err.emit(getLocation(), "Unknown symbol ");
    return getToken();
  }

  return TOK_EOF;
}

void Lexer::lex() {
  freopen(curLocation.file.c_str(), "r", stdin);
  while(getNextToken() != TOK_EOF) {
    printToken(curToken);
    tokenBuffer = "";
  }
  fclose(stdin);
}

void Lexer::printToken(TOK tk) {
  if(isId(tk)) {
    cout << "IDENTIFIER\t\t\t";
    std::cout << getIdToken() << std::endl;
  } else if(isKeyWord(tk)) {
    cout << "KEY WORD\t\t\t";
    std::cout << tokenBuffer << std::endl;
  } else if(isIntLiteral(tk)) {
    cout << "INTEGER CONST\t\t\t";
    std::cout << getNumberToken() << std::endl;
  } else if(isOp(tk)) {
    cout << "Operation\t\t\t";
    if(tk == PLUS) {
      cout << "PLUS(+)" << std::endl;
    } else if(tk == MINUS) {
      cout << "MINUS(-)" << std::endl;
    } else if(tk == TIMES) {
      cout << "TIMES(*)" << std::endl;
    } else if(tk == ASSIGN) {
      cout << "ASSIGN(=)" << std::endl;
    } else if(tk == LTHAN) {
      cout << "LTHAN(<)" << endl;
    } else if(tk == NOT) {
      cout << "NOT(!)" << endl;
    } else if(tk == AND) {
      cout << "AND(&&)" << endl;
    }
  } else if(isSymbol(tk)) {
    cout << "Symbol\t\t\t\t";
    if(tk == L_BRACKET) {
      cout << "L_BRACKET '{'" << endl;
    } else if(tk == R_BRACKET) {
      cout << "R_BRACKET '}'" << endl;
    } else if(tk == L_PAREN) {
      cout << "L_PAREN '('" << endl;
    } else if(tk == R_PAREN) {
      cout << "R_PAREN ')'" << endl;
    } else if(tk == S_COLON) {
      cout << "S_COLON ';'" << endl;
    } else if(tk == L_SBRACKET) {
      cout << "L_SBRACKET '['" << endl;
    } else if(tk == R_SBRACKET) {
      cout << "R_SBRACKET ']'" << endl;
    } else if(tk == DOT) {
      cout << "DOT '.'" << endl;
    } else if(tk == COMMA) {
      cout << "COMMA ','" << endl;
    } else if(tk == QUOTE) {
      cout << "QUOTE '\"'" << endl;
    }
  }else {
    cout << "Unknown Token" << endl;
  }
}
