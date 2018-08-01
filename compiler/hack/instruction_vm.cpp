#include "instruction_vm.h"
#include "builder_vm.h"

// MemoryAccessCmd

MemoryAccessCmd::MemoryAccessCmd(std::string line): Instruction(line) {}

bool MemoryAccessCmd::isValid() {
  return true;
}

std::string MemoryAccessCmd::translate() {
  return "todoMemAccess";
}

void MemoryAccessCmd::accept(Builder *builder) {
  dynamic_cast<HackVMTranslator*>(builder)->visit(this);
}

// ArithmeticLogicCmd

ArithmeticLogicCmd::ArithmeticLogicCmd(std::string line): Instruction(line) {}

bool ArithmeticLogicCmd::isValid() {
  return true;
}

std::string ArithmeticLogicCmd::translate() {
  return "todoMemAccess";
}

void ArithmeticLogicCmd::accept(Builder *builder) {
  dynamic_cast<HackVMTranslator*>(builder)->visit(this);
}
