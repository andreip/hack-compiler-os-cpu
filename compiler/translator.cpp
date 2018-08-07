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

Translator::Translator(std::string inputFile, std::string outputFile,
                     std::list<Builder*> *builders)
    : _outputFile(outputFile), _builders(builders) {

  _inputFiles.push_back(inputFile);
  std::cout << "Translating " << inputFile << " into "
            << _outputFile << "\n";
}

Translator::Translator(const std::vector<std::string> &inputFiles,
                       std::string outputFile,
                       std::list<Builder*> *builders)
    : _outputFile(outputFile), _builders(builders), _inputFiles(inputFiles) {
  std::string inputFilesStr = join(inputFiles, ", ");
  std::cout << "Translating input files " << inputFilesStr << " into "
            << _outputFile << "\n";
}

Translator::~Translator() { }

void Translator::translate() {
  std::list<std::string> allLines;
  const std::list<std::string> *crtLines;
  for (const std::string &inputFile: _inputFiles) {
    std::cout << "Translating single file " << inputFile << "\n";
    crtLines = translateFile(inputFile);
    std::cout << "Got back " << crtLines->size() << " lines\n";
    std::copy(
      crtLines->cbegin(), crtLines->cend(),
      std::back_inserter(allLines)
    );
    // insert empty line between them.
    allLines.push_back("");
  }
  writeToFile(allLines);
}

std::list<std::string>* Translator::translateFile(const std::string &inputFile) {
  std::list<std::string> *crtLines, *initLines;
  initLines = crtLines = splitLines(inputFile);
  for (Builder *builder: *_builders) {
    builder->reset();
    builder->setLines(crtLines);
    builder->setInputFile(inputFile);
    crtLines = builder->getResult();
  }
  delete initLines;
  return crtLines;
}

void Translator::writeToFile(const std::list<std::string> &lines) {
  std::ofstream out(_outputFile);
  if (!lines.empty())
    for (const std::string &s: lines)
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
