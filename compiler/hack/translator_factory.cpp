#include <list>
#include <stdexcept>

#include "../utils.h"

#include "./asm/builder.h"
#include "./asm/translator.h"
#include "./translator_factory.h"
#include "./vm/translator.h"
#include "./vm/builder.h"

HACKTranslatorFactory::HACKTranslatorFactory(): TranslatorFactory() { }

HACKTranslatorFactory::~HACKTranslatorFactory() { }

Translator* HACKTranslatorFactory::getTranslator(const std::string &path) {
  std::string extension = getExtension(path);
  if (extension == "asm")
    return new AsmHackTranslator(path);
  else if (extension == "vm" || getPathType(path) == PathType::DIR_TYPE)
    return new VMHackTranslator(path); 
  throw std::runtime_error("Unknown extension type " + extension);
}
