#include <list>
#include <stdexcept>

#include "../utils.h"
#include "./builder.h"
#include "./builder_vm.h"
#include "./translator_factory.h"

HACKTranslatorFactory::HACKTranslatorFactory(): TranslatorFactory() {}

Translator* HACKTranslatorFactory::getTranslator(const std::string &filename) {
  std::string extension = pathExtension(filename);
  if (extension == ".asm")
    return getAssembler(filename);
  if (extension == ".vm")
    return getVMTranslator(filename);

  throw std::runtime_error("Unknown extension type " + extension);
}

Translator* HACKTranslatorFactory::getAssembler(const std::string &filename) {
  std::list<Builder*> builders {new HackSymbolTranslator, new HackBinaryTranslator};
  return new Translator(filename, ".hack", &builders);
}

Translator* HACKTranslatorFactory::getVMTranslator(const std::string &filename) {
  std::list<Builder*> builders {new HackVMTranslator};
  return new Translator(filename, ".asm", &builders);
}
