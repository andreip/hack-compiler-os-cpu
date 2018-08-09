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
  virtual void visit(FunctionInstruction*);
  virtual void visit(ReturnInstruction*);
  virtual void visit(CallInstruction*);
private:
  void defaultVisit(VMTranslationInstruction*);

private:
  static std::vector<std::string> arithmetic_ops;
};

#endif
