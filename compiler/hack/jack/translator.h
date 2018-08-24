#ifndef __JACK__TRANSLATOR__H__
#define __JACK__TRANSLATOR__H__

#include <string>

#include "../translator.h"

class JackTranslator: public HackTranslator {
public:
  JackTranslator(const std::string &path);
  virtual void translate() override;
  virtual std::string extension() override;
  virtual std::string getOutputFile() override;
};

#endif
