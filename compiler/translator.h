#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <string>
#include <list>
#include <vector>

#include "builder.h"

class Translator {
public:
  virtual ~Translator();
  virtual void translate();
protected:
  Translator(const std::string &path);  // abstract
  std::list<std::string>* splitLines(const std::string&);
  void writeToFile(const std::list<std::string>&);

  virtual std::list<std::string>* translateFile(const std::string&);
  virtual std::string getOutputFile() = 0;
protected:
  std::string _path;
  std::list<Builder*> _builders;
};

#endif
