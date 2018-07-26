#include <algorithm>
#include <string>
#include <unordered_map>

#include <boost/algorithm/string/trim.hpp>

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

HackSymbolTranslator::HackSymbolTranslator(): _firstPass(true), _crtInstructionNo(0) { }

void HackSymbolTranslator::visit(Label *i) {
  std::cout << "processing a Label " << i->toString() << "\n";
  //if (_firstPass) {
  //}
}

void HackSymbolTranslator::visit(CInstruction *i) {
  std::cout << "processing a C-Instruction " << i->toString() << "\n";
  //if (_firstPass)
  //  _crtInstructionNo++;
}

void HackSymbolTranslator::visit(AInstruction *i) {
  std::cout << "processing an A-Instruction " << i->toString() << " in symbol translator.\n";
  //if (_firstPass)
  //  _crtInstructionNo++;
}
