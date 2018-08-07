#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

#include "compiler/hack/translator_factory.h"
#include "compiler/utils.h"

const std::string EXEC = "cpl";

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

  std::string path(argv[1]);
  TranslatorFactory *factory = new HACKTranslatorFactory;
  Translator *translator = factory->getTranslator(path);
  try {
    translator->translate();
  } catch (std::runtime_error &e) {
    std::cout << e.what();
    usage();
  }

  delete factory;
  delete translator;

  return 0;
}
