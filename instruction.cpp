#include <string>

#include "instruction.h"
#include "builder.h"

std::string Instruction::toString() {
  return _line;
}

void Instruction::accept(const Builder *builder) {
  builder->visitInstruction(this);
}
