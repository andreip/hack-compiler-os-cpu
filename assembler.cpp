#include <string>
#include <list>

#include "assembler.h"

Assembler::Assembler(std::string inputFile, std::string outputExt,
                     std::list<Parser> *parsers)
    : _inputFile(inputFile), _outputExt(outputExt), _parsers(parsers) {}

void Assembler::assemble() {
  for (const Parser &p: *_parsers) {
    //std::string out = parsers.
  }
}

