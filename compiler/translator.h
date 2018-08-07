#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <string>
#include <list>
#include <vector>

#include "builder.h"

class Translator {
public:
  Translator(std::string inputFile, std::string outputExt,
             std::list<Builder*> *builders);
  Translator(const std::vector<std::string> &inputFiles,
             std::string outputExt,
             std::list<Builder*> *builders);
  virtual ~Translator();
  void translate();
protected:
  std::list<std::string>* splitLines(const std::string&);
  void writeToFile(const std::list<std::string>&);
  virtual std::list<std::string>* translateFile(const std::string&);
private:
  std::vector<std::string> _inputFiles;
  std::string _outputFile;
  std::list<Builder*> *_builders;
};

#endif
