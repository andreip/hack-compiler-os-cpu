#include <list>
#include <string>
#include <iostream>

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
  for (const auto &line: *lines) {
    Instruction *i = parseLine(line);
    // this will cause visitInstruction() to get called, where
    // you define in the Instruction subclass what the builder method's
    // name is that gets called.
    if (i) {
      if (!i->isValid())
        throw "Error parsing line: x";
      i->accept(this);
    }
    delete i;
  }
}

void Builder::visit(Instruction *i) { }
