#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "builder.h"
#include "instruction.h"

Builder::Builder() {
  output = new std::list<std::string>;
}
Builder::~Builder() {
  delete output;
}

void Builder::init(const std::list<std::string> *lines) {
  _lines = lines;
}

std::list<std::string>* Builder::getResult() {
  processLines(_lines);
  return output;
}

void Builder::processLines(const std::list<std::string> *lines) {
  std::cout << "visiting " << lines->size() << " lines\n";
  int lineNo = 1;
  for (const auto &line: *lines) {
    Instruction *instr = parseLine(line);
    // this will cause visitInstruction() to get called, where
    // you define in the Instruction subclass what the builder method's
    // name is that gets called.
    if (instr) {
      if (!instr->isValid()) {
        std::ostringstream oss;
        oss << "Error parsing line: " << lineNo << '\n';
        std::string msg = oss.str();
        throw std::runtime_error(msg);
      }
      instr->accept(this);
    }
    delete instr;
    ++lineNo;
  }
}

void Builder::visit(Instruction *i) { }
