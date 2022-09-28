#ifndef _ERROR_H_
#define _ERROR_H_

#include <iostream>
#include <cstdio>
#include "Token.h"

#define RESET   "\033[0m"
#define BOLDRED     "\033[1m\033[31m"

class ErrorHandling {
public:
  ErrorHandling() {}
  void emit(Location loc, std::string message);
  void error() { errors++; }
  int hasErrors() { return errors; }
  void failure();
private:
  static int errors;
};

#endif
