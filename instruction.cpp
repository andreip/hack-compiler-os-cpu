#include <string>
#include <iostream>

#include "instruction.h"
#include "builder.h"

Instruction::Instruction(std::string line): _line(line) {}
Instruction::~Instruction() {}

std::string Instruction::toString() const {
  return _line;
}

void Instruction::accept(const Builder *builder) {
  builder->visitInstruction(this);
}

void Instruction::set(std::string line) {
  _line = line;
}
