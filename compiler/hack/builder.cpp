#include <string>
#include <sstream>

#include "../utils.h"
#include "./utils.h"
#include "./builder.h"
#include "./instruction.h"

HackBuilder::HackBuilder(): Builder() {}

HackBuilder::HackBuilder(const std::string &filename): Builder(filename) {}

Instruction* HackBuilder::parseLine(const std::string &line) {
  std::string instr = trimComment(line);
  trim(instr);

  if (instr.empty())
    return nullptr;
  else if (instr[0] == '@')
    return new AInstruction(instr);
  else if (instr[0] == '(')
    return new Label(instr);
  else
    return new CInstruction(instr);
}

void HackBuilder::visit(HackInstruction *i) { }

// HackSymbolTranslator

HackSymbolTranslator::HackSymbolTranslator()
  : HackBuilder() {
  init();
}

HackSymbolTranslator::HackSymbolTranslator(const std::string &filename)
  : HackBuilder(filename) {
  init();
  initPredefinedSymbols();
  _crtVariableNo = VARIABLE_START;
}

void HackSymbolTranslator::init() {
  _firstPass = true;
  _crtInstructionNo = 0;
}

std::list<std::string>* HackSymbolTranslator::getResult() {
  HackBuilder::getResult();
  _firstPass = false;
  return HackBuilder::getResult();
}

void HackSymbolTranslator::visit(Label *i) {
  if (_firstPass) {
    std::ostringstream oss;
    oss << _crtInstructionNo;
    _symbolsTable[i->getName()] = oss.str();
  } else {
    // just ignore label from output, we're going to substitute it based
    // on the table.
  }
}

void HackSymbolTranslator::visit(CInstruction *i) {
  if (_firstPass)
    _crtInstructionNo++;
  else
    output->push_back(i->toString());
}

void HackSymbolTranslator::visit(AInstruction *i) {
  if (_firstPass)
    _crtInstructionNo++;
  else {
    // if it's not a numeric value, then we look it up in the table.
    // if it's present, we just replace it, otherwise we assume
    // it's a new variable so we just allocate it a memory location.
    if (!i->isNumericValue()) {
      std::string val = i->value();
      std::string newVal = "";
      if (_symbolsTable.find(val) != _symbolsTable.end()) {
        newVal = _symbolsTable[val];
      } else {
        newVal = toString(_crtVariableNo++);
        _symbolsTable[val] = newVal;
      }
      i->setValue(newVal);
    }

    output->push_back(i->toString());
  }
}

void HackSymbolTranslator::initPredefinedSymbols() {
  _symbolsTable = {
    {"SP", "0"},
    {"LCL", "1"},
    {"ARG", "2"},
    {"THIS", "3"},
    {"THAT", "4"},
    {"SCREEN", "16384"},
    {"KBD", "24576"},
  };
  // R0 = "0", R1 = "1", ..., R15 = "15"
  for (int i = 0; i < 16; ++i) {
    std::ostringstream oss;
    oss << i;
    std::string num = oss.str();
    _symbolsTable["R" + num] = num;
  }
}

// HackBinaryTranslator

HackBinaryTranslator::HackBinaryTranslator(): HackBuilder() { }

HackBinaryTranslator::HackBinaryTranslator(const std::string &filename)
  : HackBuilder(filename) { }

void HackBinaryTranslator::visit(Label *i) { }

void HackBinaryTranslator::visit(CInstruction *i) {
  output->push_back(i->translate());
}

void HackBinaryTranslator::visit(AInstruction *i) {
  output->push_back(i->translate());
}

