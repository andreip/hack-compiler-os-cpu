#include <algorithm>
#include <iostream>
#include <string>

#include <boost/algorithm/string/trim.hpp>

#include "./instruction_vm.h"
#include "./builder_vm.h"
#include "./utils.h"

HackVMTranslator::HackVMTranslator(): Builder() {}

std::vector<std::string> HackVMTranslator::arithmetic_ops = {
  "eq", "add", "neg", "sub"
};

Instruction* HackVMTranslator::parseLine(const std::string &line) {
  std::string instr = trimComment(line);
  boost::algorithm::trim(instr);

  if (instr.empty())
    return nullptr;

  if (instr.substr(0, 3) == "pop" || instr.substr(0, 4) == "push")
    return new MemoryAccessCmd(instr);

  auto arithmetic_it = std::find(arithmetic_ops.begin(),
                                 arithmetic_ops.end(),
                                 instr);
  if (arithmetic_it != arithmetic_ops.end())
    return new ArithmeticLogicCmd(instr);

  return nullptr;
}

void HackVMTranslator::visit(MemoryAccessCmd *i) {
  std::cout << "Visiting " << i->toString() << '\n';
}

void HackVMTranslator::visit(ArithmeticLogicCmd *i) {
  std::cout << "Visiting " << i->toString() << '\n';
}
