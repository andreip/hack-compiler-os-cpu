#include <list>
#include <string>

#include "builder.h"
#include "instruction.h"

void Builder::init(const std::list<std::string> *lines) {
  _lines = lines;
}

// in case your builder needs to do several passes through
// the data, call computeOnce() several times and feed its output
// back as its new input.
std::list<std::string>* Builder::computeResult() {
  return computeOnce(_lines);
}

std::list<std::string>* Builder::computeOnce(const std::list<std::string> *lines) {
  for (const auto &line: *lines) {
    Instruction *i = parseLine(line);
    // this will cause visitInstruction() to get called, where
    // you define in the Instruction subclass what the builder method's
    // name is that gets called.
    i->accept(this);
  }

  return nullptr;
}

void Builder::visitInstruction(const Instruction *i) const { }
