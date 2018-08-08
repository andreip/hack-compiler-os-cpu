#ifndef __HACK__VM__TRANSLATOR__H__
#define __HACK__VM__TRANSLATOR__H__

#include "../translator.h"

class VMHackTranslator : public HackTranslator {
public:
  VMHackTranslator(const std::string &path);
protected:
  virtual std::string getOutputFile() override;
  virtual std::string extension() override;
};

#endif
