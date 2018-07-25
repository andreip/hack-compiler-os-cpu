#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

#include <string>
#include <list>

#include "builder.h"

class Assembler {
public:
  Assembler(std::string inputFile, std::string outputExt,
            std::list<Builder*> *builders);
  virtual ~Assembler();
  void assemble();
protected:
  std::list<std::string> *splitLines(const std::string&);
  void writeToFile(const std::list<std::string>*);
private:
  std::string _inputFile;
  std::string _outputExt;
  std::list<Builder*> *_builders;
  std::list<std::string> *_lines;
};

#endif
