#include <algorithm>
#include <string>

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

void HackBuilder::visitHackInstruction() const { }

HackSymbolTranslator::HackSymbolTranslator() {}

void HackSymbolTranslator::visitLabel(const Label *i) const {
  std::cout << "processing a Label " << i->toString() << "\n";
}

void HackSymbolTranslator::visitCInstruction(const CInstruction *i) const {
  std::cout << "processing a C-Instruction " << i->toString() << "\n";
}

void HackSymbolTranslator::visitAInstruction(const AInstruction *i) const {
  std::cout << "processing an A-Instruction " << i->toString() << " in symbol translator.\n";
}
