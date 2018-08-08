#ifndef __HACK__BUILDER__VM__H__
#define __HACK__BUILDER__VM__H__

#include <vector>

#include "../../builder.h"
#include "../utils.h"
#include "./instruction.h"

class HackBuilderVMTranslator: public Builder {
public:
  HackBuilderVMTranslator();
  HackBuilderVMTranslator(const std::string&);

  virtual Instruction* parseLine(const std::string &line);
  virtual void visit(MemorySegment*);
  virtual void visit(ArithmeticLogic*);
  virtual void visit(BranchingInstruction*);
  virtual void visit(BaseFunctionsInstruction*);
  virtual void visit(FunctionInstruction*);
private:
  void defaultVisit(VMTranslationInstruction*);

private:
  static std::vector<std::string> arithmetic_ops;
  std::string _function;  // current active function
};

#endif
