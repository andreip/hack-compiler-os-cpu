#include <string>

#include "../utils.h"
#include "./instruction.h"
#include "./builder.h"

HackInstruction::HackInstruction(std::string line): Instruction(line) {}

void HackInstruction::accept(Builder *builder) {
  dynamic_cast<HackBuilder*>(builder)->visit(this);
}

// AInstruction

AInstruction::AInstruction(std::string line): HackInstruction(line) {}

std::string AInstruction::value() {
  // everything except first char.
  return toString().substr(1);
}

void AInstruction::setValue(std::string value) {
  set("@" + value);
}

bool AInstruction::isNumericValue() {
  return isNumber(value());
}

bool AInstruction::isValid() {
  std::string val = value();
  if (isNumber(val)) {
    int num = getNumber(val);
    return num >= 0 && num < 32768;  // at most a 15-bit number
  }
  return true;
}

std::string AInstruction::toBinary() {
  return "0todo";
}

void AInstruction::accept(Builder *builder) {
  dynamic_cast<HackBuilder*>(builder)->visit(this);
}

// CInstruction

CInstruction::CInstruction(std::string line): HackInstruction(line) {}

bool CInstruction::isValid() {
  return true;
}

std::string CInstruction::toBinary() {
  return "111todo";
}

void CInstruction::accept(Builder *builder) {
  dynamic_cast<HackBuilder*>(builder)->visit(this);
}

// Label

Label::Label(std::string line): HackInstruction(line) {}

bool Label::isValid() {
  std::string name = toString();
  return (
    name.size() > 2 &&
    name[0] == '(' &&
    name[name.size()-1] == ')'
  );
}

std::string Label::toBinary() {
  throw "Label doesn't support toBinary()";
}

void Label::accept(Builder *builder) {
  dynamic_cast<HackBuilder*>(builder)->visit(this);
}

std::string Label::getName() {
  std::string name = toString();
  return name.substr(1, name.size() - 2);
}
