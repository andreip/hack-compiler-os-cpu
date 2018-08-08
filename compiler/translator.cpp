// https://en.wikipedia.org/wiki/Translator_(computing)
// A translator or programming language processor is a computer program that
// performs the translation of a program written in a given programming
// language into a functionally equivalent program in another computer language
// (the target language), without losing the functional or logical structure of
// the original code (the "essence" of each program).

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#include <string>

#include "translator.h"
#include "utils.h"

Translator::Translator(const std::string &path)
    : _path(path) { }

Translator::~Translator() {
  for (Builder *b: _builders)
    delete b;
}

void Translator::translate() {
  const std::list<std::string> *outputLines;
  outputLines = translateFile(_path);
  writeToFile(*outputLines);
}

std::list<std::string>* Translator::translateFile(const std::string &path) {
  std::list<std::string> *crtLines, *initLines;
  initLines = crtLines = splitLines(path);
  for (Builder *builder: _builders) {
    builder->reset();
    builder->setLines(crtLines);
    builder->setInputFile(path);
    crtLines = builder->getResult();
  }
  delete initLines;
  return crtLines;
}

void Translator::writeToFile(const std::list<std::string> &lines) {
  std::cout << "Translating " << _path << " into "
            << getOutputFile() << "\n";

  std::ofstream out(getOutputFile());
  if (!lines.empty())
    for (const std::string &s: lines)
      out << s << '\n';
  out.close();
}

std::list<std::string>* Translator::splitLines(const std::string &path) {
  std::ifstream in(path);
  std::string line;

  auto lines = new std::list<std::string>;
  while (in) {
    std::getline(in, line);
    lines->push_back(line);
  }
  in.close();
  return lines;
}
