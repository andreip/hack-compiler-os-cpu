#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include <string>
#include <list>

#include "builder.h"

class Assembler {
public:
  Assembler(std::string inputFile, std::string outputExt,
            std::list<Builder> *builders);
  void assemble();
protected:
  std::list<std::string> *splitLines();
  void writeToFile(std::list<std::string>*);
private:
  std::string _inputFile;
  std::string _outputExt;
  std::list<Builder> *_builders;
};

#endif
