#ifndef __HACK__ASM__TRANSLATOR__H__
#define __HACK__ASM__TRANSLATOR__H__

#include "hack/translator.h"

class AsmHackTranslator : public HackTranslator {
public:
  AsmHackTranslator(const std::string &path);
protected:
  virtual std::string getOutputFile() override;
  virtual std::string extension() override;
};

#endif
