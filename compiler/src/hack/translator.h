#ifndef __HACK_TRANSLATOR__H__
#define __HACK_TRANSLATOR__H__

#include <list>
#include <string>

#include "generic/translator.h"

class HackTranslator: public Translator {
public:
  // adds directory loop logic, to bundle multiple files
  // into one output file.
  virtual void translate();
protected:
  HackTranslator(const std::string &path);  // abstract
  virtual std::list<std::string> getInputFiles();
  virtual std::string extension() = 0;
};

#endif
