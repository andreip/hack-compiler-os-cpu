#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>

#include "utils.h"
#include "translator.h"

extern Translator* getTranslatorFromPath(const std::string &path);

void usage(char *exec) {
  std::cout << "Usage:\n" << exec << " <path|file>\n";
}

int main(int argc, char **argv) {
  if (argc != 2) {
    usage(argv[0]);
    std::exit(1);
  }

  std::string path(argv[1]);
  Translator *translator = getTranslatorFromPath(path);
  try {
    translator->translate();
  } catch (std::runtime_error &e) {
    std::cout << e.what();
    usage(argv[0]);
  }

  delete translator;
  return 0;
}
