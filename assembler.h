#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include <string>
#include <list>

#include "parser.h"

class Assembler {
public:
  Assembler(std::string inputFile, std::string outputExt,
            std::list<Parser> *parsers);
  void assemble();
private:
  std::string _inputFile;
  std::string _outputExt;
  std::list<Parser> *_parsers;
};

#endif
