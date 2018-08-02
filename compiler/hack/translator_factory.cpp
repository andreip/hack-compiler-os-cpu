#include <list>
#include <stdexcept>

#include "../utils.h"
#include "./builder.h"
#include "./builder_vm.h"
#include "./translator_factory.h"

HACKTranslatorFactory::HACKTranslatorFactory(): TranslatorFactory() {}

HACKTranslatorFactory::~HACKTranslatorFactory() {
  for (Builder *b: *_builders)
    delete b;
  delete _builders;
}

Translator* HACKTranslatorFactory::getTranslator(const std::string &filename) {
  std::string extension = pathExtension(filename);
  if (extension == ".asm")
    return getAssembler(filename);
  if (extension == ".vm")
    return getVMTranslator(filename);

  throw std::runtime_error("Unknown extension type " + extension);
}

Translator* HACKTranslatorFactory::getAssembler(const std::string &filename) {
  _builders = new std::list<Builder*> {new HackSymbolTranslator(filename), new HackBinaryTranslator(filename)};
  return new Translator(filename, ".hack", _builders);
}

Translator* HACKTranslatorFactory::getVMTranslator(const std::string &filename) {
  _builders = new std::list<Builder*> {new HackVMTranslator(filename)};
  return new Translator(filename, ".asm", _builders);
}
