#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "builder.h"
#include "instruction.h"
#include "utils.h"

Builder::Builder()
  : _filename(""), _function("") {
  output = new std::list<std::string>;
}

Builder::Builder(const std::string &filename): Builder() {
  _filename = filename;
}

Builder::~Builder() {
  delete output;
}

void Builder::setLines(const std::list<std::string> *lines) {
  _lines = lines;
}

void Builder::setInputFile(const std::string &inputFile) {
  _filename = inputFile;
  setCurrentFunction("");
}

void Builder::setCurrentFunction(const std::string &function) {
  _function = function;
}

void Builder::reset() {
  output->clear();
  setLines(nullptr);
  setInputFile("");
}

std::list<std::string>* Builder::getResult() {
  processLines(_lines);
  return output;
}

void Builder::processLines(const std::list<std::string> *lines) {
  int lineNo = 1;
  for (const auto &line: *lines) {
    Instruction *instr = parseLine(line);
    // this will cause visitInstruction() to get called, where
    // you define in the Instruction subclass what the builder method's
    // name is that gets called.
    if (instr) {
      if (!instr->isValid()) {
        debug("Invalid line", line);
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

std::string Builder::getFilename() const {
  return _filename;
}

std::string Builder::getCurrentFunction() const {
  return _function;
}
