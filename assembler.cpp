#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>

#include <boost/filesystem.hpp>

#include "assembler.h"

Assembler::Assembler(std::string inputFile, std::string outputExt,
                     std::list<Builder*> *builders)
    : _inputFile(inputFile), _outputExt(outputExt), _builders(builders) {
  std::cout << "Assembling " << inputFile << "\n";
  std::list<std::string> *_lines = splitLines(inputFile);
}

Assembler::~Assembler() {
  delete _lines;
}

void Assembler::assemble() {
  const std::list<std::string> *crtLines = _lines;
  for (Builder *builder: *_builders) {
    builder->init(crtLines);
    crtLines = builder->computeResult();
  }
  writeToFile(crtLines);
}

void Assembler::writeToFile(const std::list<std::string> *lines) {
  // extract base filename from path, w/o extension
  boost::filesystem::path path(_inputFile);
  std::ofstream out(path.stem().string() + _outputExt);

  if (lines)
    for (const std::string &s: *lines)
      out << s << '\n';
  out.close();
}

std::list<std::string>* Assembler::splitLines(const std::string &inputFile) {
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
