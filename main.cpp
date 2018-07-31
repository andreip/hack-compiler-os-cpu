#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

#include "compiler/assembler.h"
#include "compiler/builder.h"
#include "compiler/hack/builder.h"

const std::string EXEC = "compile";

void usage() {
  std::vector<std::string> ways {
    "./" + EXEC + " file.asm",
    "Assemble .asm files and get .hack files out",
    "./" + EXEC + " file.vm",
    "Compile .vm files and get .asm files out",
  };

  std::cout << "Usage:\n";
  for (auto it = ways.begin(); it != ways.end(); it += 2)
    std::cout << "\t" << *it << "\t" << *(it+1) << "\n";
}

int main(int argc, char **argv) {
  if (argc != 2) {
    usage();
    std::exit(1);
  }

  std::string filename(argv[1]);
  std::list<Builder*> builders {new HackSymbolTranslator, new HackBinaryTranslator};

  Assembler hack(filename, ".hack", &builders);
  try {
    hack.assemble();
  } catch (std::runtime_error &e) {
    std::cout << e.what();
  }

  return 0;
}
