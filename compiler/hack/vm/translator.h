#ifndef __HACK__VM__TRANSLATOR__H__
#define __HACK__VM__TRANSLATOR__H__

#include <list>
#include <string>

#include "../translator.h"

class VMHackTranslator : public HackTranslator {
public:
  VMHackTranslator(const std::string &path);
  // override to add file's name to the output.
  std::list<std::string>* translateFile(const std::string &path) override;
protected:
  virtual std::string getOutputFile() override;
  virtual std::string extension() override;
  // before writing to file, add bootstrap code
  virtual void beforeWriteToFile(std::list<std::string>&) override;
private:
  std::list<std::string> getBootstrapCode();  // calls Sys.init
};

#endif
