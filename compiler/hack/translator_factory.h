#ifndef __HACK_TRANSLATOR__FACTORY__H__
#define __HACK_TRANSLATOR__FACTORY__H__

#include <string>

#include "../translator.h"
#include "../translator_factory.h"

class HACKTranslatorFactory: public TranslatorFactory {
public:
  HACKTranslatorFactory();
  virtual Translator* getTranslator(const std::string&) override;
protected:
  Translator* getAssembler(const std::string&);
  Translator* getVMTranslator(const std::string&);
  // Translator* getJackCompiler();
};

#endif
