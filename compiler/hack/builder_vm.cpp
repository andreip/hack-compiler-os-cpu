#include <algorithm>
#include <iostream>
#include <string>

#include <boost/algorithm/string/trim.hpp>

#include "./instruction_vm.h"
#include "./builder_vm.h"
#include "./utils.h"

HackVMTranslator::HackVMTranslator(): Builder() {}

std::vector<std::string> HackVMTranslator::arithmetic_ops = {
  "add", "sub", "neg", "eq",
  "gt", "lt", "and", "or", "not",
};

Instruction* HackVMTranslator::parseLine(const std::string &line) {
  std::string instr = trimComment(line);
  boost::algorithm::trim(instr);

  if (instr.empty())
    return nullptr;

  if (instr.substr(0, 3) == "pop" || instr.substr(0, 4) == "push") {
    std::string segment = MemorySegment::parse(instr)[1];
    if (segment == "constant")
      return new ConstantMemorySegment(instr);
    // else if TODO rest
  }

  auto arithmetic_it = std::find(arithmetic_ops.begin(),
                                 arithmetic_ops.end(),
                                 instr);
  if (arithmetic_it != arithmetic_ops.end())
    return new ArithmeticLogic(instr);

  return nullptr;
}

void HackVMTranslator::visit(MemorySegment *i) {
  // adding a comment about what generated that code is going to be
  // helpful.
  output->push_back(getComment(i->toString()));
  output->push_back(i->translate());
}

void HackVMTranslator::visit(ArithmeticLogic *i) {
  // adding a comment about what generated that code is going to be
  // helpful.
  output->push_back(getComment(i->toString()));
  output->push_back(i->translate());
}
