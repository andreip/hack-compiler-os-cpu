#include <iostream>
#include <fstream>
#include <string>

#include "../../utils.h"

#include "translator.h"
#include "tokenizer.h"

Translator* getTranslatorFromPath(const std::string &path) {
  return new JackTranslator(path);
}

JackTranslator::JackTranslator(const std::string &path): HackTranslator(path) {
}

void JackTranslator::translate() {
  std::cout << "Processing " << _path << '\n';
  for (const std::string &inputFile: getInputFiles()) {
    std::string outputFile = replaceExtension(inputFile, "T.xml");
    std::cout << "Extracting tokens from " << inputFile << " into " << outputFile << '\n';
    std::ifstream in(inputFile);
    JackTokenizer tok(in);
    std::ofstream out(outputFile);
    while (tok.hasMore()) {
      out << tok.getCurrentToken() << "\n";
      tok.advance();
    }
  }
}

std::string JackTranslator::extension() { return "jack"; }

// unused, since we're overwriting the translate.
std::string JackTranslator::getOutputFile() { return ""; }
