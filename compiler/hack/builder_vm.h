#ifndef __HACK__BUILDER__VM__H__
#define __HACK__BUILDER__VM__H__

#include <vector>

#include "../builder.h"
#include "./instruction_vm.h"
#include "./utils.h"

class HackBuilderVMTranslator: public Builder {
public:
  HackBuilderVMTranslator();
  HackBuilderVMTranslator(const std::string&);

  virtual Instruction* parseLine(const std::string &line);
  virtual void visit(MemorySegment*);
  virtual void visit(ArithmeticLogic*);
private:
  static std::vector<std::string> arithmetic_ops;
};

#endif
