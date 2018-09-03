#include <iostream>
#include <fstream>
#include <string>

#include "translator.h"
#include "tokenizer.h"
#include "builder.h"

Translator* getTranslatorFromPath(const std::string &path) {
  return new JackTranslator(path);
}

JackTranslator::JackTranslator(const std::string &path): HackTranslator(path) {
  _jack_builders = {
    new JackTokenizerBuilder(),
  };
}

JackTranslator::~JackTranslator() {
  for (auto it = _jack_builders.begin(); it != _jack_builders.end(); ++it)
    delete *it;
}

void JackTranslator::translate() {
  std::cout << "Processing " << _path << '\n';
  for (const std::string &inputFile: getInputFiles())
    for (JackBuilder *builder: _jack_builders)
      builder->build(inputFile);
}

std::string JackTranslator::extension() { return "jack"; }

// unused, since we're overwriting the translate.
std::string JackTranslator::getOutputFile() { return ""; }
