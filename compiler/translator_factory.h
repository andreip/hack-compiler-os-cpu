#ifndef __TRANSLATOR__FACTORY__H__
#define __TRANSLATOR__FACTORY__H__

#include <string>

#include "translator.h"

class TranslatorFactory {
public:
  virtual Translator* getTranslator(const std::string&) = 0;
  virtual ~TranslatorFactory();
protected:
  TranslatorFactory();
};

#endif
