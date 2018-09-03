#ifndef __JACK__TRANSLATOR__H__
#define __JACK__TRANSLATOR__H__

#include <list>
#include <string>

#include "../translator.h"

class JackBuilder;

class JackTranslator: public HackTranslator {
public:
  JackTranslator(const std::string &path);
  ~JackTranslator();
  virtual void translate() override;
  virtual std::string extension() override;
  virtual std::string getOutputFile() override;
private:
  std::list<JackBuilder*> _jack_builders;
};

#endif
