#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <string>
#include <list>

#include "builder.h"

class Translator {
public:
  Translator(std::string inputFile, std::string outputExt,
            std::list<Builder*> *builders);
  virtual ~Translator();
  void translate();
protected:
  std::list<std::string> *splitLines(const std::string&);
  void writeToFile(const std::list<std::string>*);
private:
  std::string _inputFile;
  std::string _outputFile;
  std::list<Builder*> *_builders;
  std::list<std::string> *_lines;
};

#endif
