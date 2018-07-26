#include <algorithm>
#include <string>
#include <sstream>
#include <unordered_map>

#include <boost/algorithm/string/trim.hpp>

#include "../utils.h"
#include "builder.h"
#include "instruction.h"

HackBuilder::HackBuilder() {}

Instruction* HackBuilder::parseLine(const std::string &line) {
  std::string instr = trimComment(line);
  boost::algorithm::trim(instr);

  if (instr.empty())
    return nullptr;
  else if (instr[0] == '@')
    return new AInstruction(instr);
  else if (instr[0] == '(')
    return new Label(instr);
  else
    return new CInstruction(instr);
}

std::string HackBuilder::trimComment(const std::string &line) {
  std::string comment = "//";
  std::string trimmed = line;  // clone it
  auto it = std::search(trimmed.begin(), trimmed.end(), comment.begin(), comment.end());
  trimmed.erase(it, trimmed.end());  // up to the end.
  return trimmed;
}

void HackBuilder::visit(HackInstruction *i) { }

HackSymbolTranslator::HackSymbolTranslator(): _firstPass(true), _crtInstructionNo(0) {
  initPredefinedSymbols();
  _crtVariableNo = VARIABLE_START;
}

std::list<std::string>* HackSymbolTranslator::getResult() {
  HackBuilder::getResult();
  _firstPass = false;
  std::cout << _symbolsTable << '\n';
  std::cout << "Doing a 2nd pass.\n";
  std::cout << _symbolsTable << '\n';
  return HackBuilder::getResult();
}

void HackSymbolTranslator::visit(Label *i) {
  std::cout << "processing a Label " << i->toString() << "\n";
  if (_firstPass) {
    std::cout << "Remembering label " << i->getName() << " at position " << _crtInstructionNo << "\n";
    std::ostringstream oss;
    oss << _crtInstructionNo;
    _symbolsTable[i->getName()] = oss.str();
  } else {
    // just ignore label from output, we're going to substitute it based
    // on the table.
  }
}

void HackSymbolTranslator::visit(CInstruction *i) {
  std::cout << "processing a C-Instruction " << i->toString() << "\n";
  if (_firstPass)
    _crtInstructionNo++;
  else
    output->push_back(i->toString());
}

void HackSymbolTranslator::visit(AInstruction *i) {
  std::cout << "processing an A-Instruction " << i->toString() << " in symbol translator.\n";
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
      std::cout << "Translating " << val << " to " << newVal << "\n";
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
