#include <string>

#include "../../utils.h"

#include "./builder.h"
#include "./translator.h"

AsmHackTranslator::AsmHackTranslator(const std::string &path)
  : HackTranslator(path) {
  _builders.push_back(new HackSymbolTranslator(path));
  _builders.push_back(new HackBinaryTranslator(path));
}

std::string AsmHackTranslator::getOutputFile() {
  return replaceExtension(_path, "hack");
}

std::string AsmHackTranslator::extension() { return "asm"; }

