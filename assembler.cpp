#include <string>
#include <list>
#include <fstream>
#include <iostream>

#include "assembler.h"

Assembler::Assembler(std::string inputFile, std::string outputExt,
                     std::list<Builder> *builders)
    : _inputFile(inputFile), _outputExt(outputExt), _builders(builders) {
    std::cout << "Assembling " << inputFile << "\n";
  }

void Assembler::assemble() {
  std::list<std::string> *lines = splitLines();
  for (const Builder &builder: *_builders) {
    //builder.init(lines);
    //// modify inline the lines until we get the final result.
    //builder.computeResult();
  }

  writeToFile(lines);

  delete lines;
}

void Assembler::writeToFile(std::list<std::string> *lines) {
  std::string inputWithoutExt = _inputFile.substr(0, _inputFile.rfind("."));
  std::string outputFile = inputWithoutExt + _outputExt;
  std::ofstream out(outputFile);

  for (const std::string &s: *lines)
    out << s << '\n';

  out.close();
}

std::list<std::string>* Assembler::splitLines() {
  std::ifstream in(_inputFile);
  std::string line;

  auto lines = new std::list<std::string>;
  while (in) {
    std::getline(in, line);
    lines->push_back(line);
  }
  in.close();
  return lines;
}
