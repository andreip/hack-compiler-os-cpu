// https://en.wikipedia.org/wiki/Translator_(computing)
// A translator or programming language processor is a computer program that
// performs the translation of a program written in a given programming
// language into a functionally equivalent program in another computer language
// (the target language), without losing the functional or logical structure of
// the original code (the "essence" of each program).

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "translator.h"

Translator::Translator(std::string inputFile, std::string outputExt,
                     std::list<Builder*> *builders)
    : _inputFile(inputFile), _builders(builders) {

  // extract base filename from path, w/o extension
  boost::filesystem::path path(_inputFile);
  // outputs in current directory with the output extension.
  _outputFile = path.stem().string() + outputExt;

  std::cout << "Translating " << _inputFile << " into "
            << _outputFile << "\n";
  _lines = splitLines(inputFile);
}

Translator::~Translator() {
  delete _lines;
}

void Translator::translate() {
  const std::list<std::string> *crtLines = _lines;
  for (Builder *builder: *_builders) {
    builder->init(crtLines);
    crtLines = builder->getResult();
  }
  writeToFile(crtLines);
}

void Translator::writeToFile(const std::list<std::string> *lines) {
  std::ofstream out(_outputFile);
  if (lines && !lines->empty())
    for (const std::string &s: *lines)
      out << s << '\n';
  out.close();
}

std::list<std::string>* Translator::splitLines(const std::string &inputFile) {
  std::ifstream in(inputFile);
  std::string line;

  auto lines = new std::list<std::string>;
  while (in) {
    std::getline(in, line);
    lines->push_back(line);
  }
  in.close();
  return lines;
}
