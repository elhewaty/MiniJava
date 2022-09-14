#include "Error.h"

int ErrorHandling::errors = 0;

void ErrorHandling::emit(Location loc, std::string message) {
  std::cerr << BOLDRED << "Error: " << RESET << message << " at " << loc.line
            << " : " << loc.col <<  " in file " << loc.file << std::endl;
  errors++;
  if(errors == 10) {
    failure();
  }
}

void ErrorHandling::failure() {
  std::cerr << BOLDRED << errors << " errors." << RESET << std::endl;
  std::cerr << BOLDRED << "Build failed " << RESET << std::endl;
  exit(1);
}