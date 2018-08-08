#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../utils.h"
#include "../utils.h"
#include "./instruction.h"
#include "./builder.h"

HackBuilderVMTranslator::HackBuilderVMTranslator()
  : Builder(), _function("") { }

HackBuilderVMTranslator::HackBuilderVMTranslator(const std::string &filename)
  : Builder(filename), _function("") { }

Instruction* HackBuilderVMTranslator::parseLine(const std::string &line) {
  std::string instr = trimComment(line);
  trim(instr);

  if (instr.empty())
    return nullptr;

  if (instr.substr(0, 3) == "pop" || instr.substr(0, 4) == "push") {
    std::string segment = MemorySegment::parse(instr)[1];
    if (segment == "constant")
      return new ConstantMemorySegment(instr);
    else if (SegmentBaseMemorySegment::canHandleSegment(segment))
      return new SegmentBaseMemorySegment(instr);
    else if (segment == "temp")
      return new TempMemorySegment(instr);
    else if (segment == "pointer")
      return new PointerMemorySegment(instr);
    else if (segment == "static")
      return new StaticMemorySegment(instr);

    throw std::runtime_error("Unknown instruction " + instr + "\n");
  }

  if (ArithmeticLogic::isArithmeticLogicOp(instr)) {
    if (instr == "add")
      return new AddArithmeticLogic(instr);
    else if (instr == "sub")
      return new SubArithmeticLogic(instr);
    else if (instr == "neg")
      return new NegArithmeticLogic(instr);
    else if (instr == "eq")
      return new EqArithmeticLogic(instr);
    else if (instr == "gt")
      return new GtArithmeticLogic(instr);
    else if (instr == "lt")
      return new LtArithmeticLogic(instr);
    else if (instr == "and")
      return new AndArithmeticLogic(instr);
    else if (instr == "or")
      return new OrArithmeticLogic(instr);
    else if (instr == "not")
      return new NotArithmeticLogic(instr);
    else
      throw std::runtime_error("Unknown instruction " + instr + "\n");
  }

  // branching instructions
  if (startsWith(instr, "label")) {
    return new LabelInstruction(instr);
  } else if (startsWith(instr, "goto")) {
    return new GotoInstruction(instr);
  } else if (startsWith(instr, "if-goto")) {
    return new IfGotoInstruction(instr);
  }

  // function instructions
  if (startsWith(instr, "function")) {
    return new FunctionInstruction(instr);
  }


  return nullptr;
}

void HackBuilderVMTranslator::visit(MemorySegment *i) {
  HackBuilderVMTranslator::defaultVisit(i);
}

void HackBuilderVMTranslator::visit(ArithmeticLogic *i) {
  HackBuilderVMTranslator::defaultVisit(i);
  std::cout << "in arithm instr visit: " << i->toString() << "\n";
}

void HackBuilderVMTranslator::visit(BranchingInstruction *i) {
  HackBuilderVMTranslator::defaultVisit(i);
  std::cout << "in branch instr visit: " << i->toString() << "\n";
}

void HackBuilderVMTranslator::visit(BaseFunctionsInstruction *i) {
  HackBuilderVMTranslator::defaultVisit(i);
  std::cout << "in base functions visit: " << i->toString() << "\n";
}

void HackBuilderVMTranslator::visit(FunctionInstruction *i) {
  HackBuilderVMTranslator::defaultVisit(i);
  std::cout << "in function visit: " << i->name()
            << ", nVars: " << i->nVars() << "\n";
  _function = i->name();
}

void HackBuilderVMTranslator::defaultVisit(VMTranslationInstruction *i) {
  // adding a comment about what generated that code is going to be
  // helpful.
  output->push_back(getComment(i->toString()));
  output->push_back(i->translate());
}

