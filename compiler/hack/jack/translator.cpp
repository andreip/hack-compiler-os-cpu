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
    JackTokenizer tokenizer(in);
    std::ofstream out(outputFile);
    out << "<tokens>\n";
    while (tokenizer.hasMore()) {
      const Token token = tokenizer.getCurrentToken();
      out << token.toXML() << "\n";
      std::cout << "Got xml token '" << token.toXML() << "'\n";
      tokenizer.advance();
    }
    out << "</tokens>\n";
  }
}

std::string JackTranslator::extension() { return "jack"; }

// unused, since we're overwriting the translate.
std::string JackTranslator::getOutputFile() { return ""; }
