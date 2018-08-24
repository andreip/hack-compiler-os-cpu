#include <string>
#include <iostream>

#include "translator.h"

Translator* getTranslatorFromPath(const std::string &path) {
  return new JackTranslator(path);
}

JackTranslator::JackTranslator(const std::string &path): HackTranslator(path) {
  //_builders.push_back();
}

void JackTranslator::translate() {
  std::cout << "Translating " << _path << '\n';
}

std::string JackTranslator::extension() { return "jack"; }

// unused, since we're overwriting the translate.
std::string JackTranslator::getOutputFile() { return ""; }
