#ifndef __HACK_TRANSLATOR__H__
#define __HACK_TRANSLATOR__H__

#include "../translator.h"

class HackTranslator: public Translator {
public:
  HackTranslator(std::string inputFile,
                 std::string outputFile,
                 std::list<Builder*> *builders);
  HackTranslator(const std::vector<std::string> &inputFiles,
                 std::string outputFile,
                 std::list<Builder*> *builders);
  virtual std::list<std::string>* translateFile(const std::string&);
};

#endif
