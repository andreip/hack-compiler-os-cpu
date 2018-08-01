#ifndef __HACK__BUILDER__VM__H__
#define __HACK__BUILDER__VM__H__

#include "../builder.h"
#include "./utils.h"

class HackVMTranslator: public Builder {
public:
  HackVMTranslator();

  virtual Instruction* parseLine(const std::string &line);
};

#endif
