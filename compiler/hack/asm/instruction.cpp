#include <algorithm>
#include <bitset>
#include <string>
#include <sstream>
#include <stdexcept>

#include "../../utils.h"
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
    return num >= 0 && num < AInstruction::MAX_VALUE;
  }
  return true;
}

std::string AInstruction::translate() {
  if (!isNumericValue())
    throw std::runtime_error("Cannot call translate() yet on symbol AInstruction: " + toString());

  std::string val = value();
  int num = getNumber(val);
  std::bitset<AInstruction::BITS_VALUE> numBits(num);

  return "0" + numBits.to_string();

}

void AInstruction::accept(Builder *builder) {
  dynamic_cast<HackBuilder*>(builder)->visit(this);
}

// CInstruction

CInstruction::CInstruction(std::string line): HackInstruction(line) { }

bool CInstruction::isValid() {
  std::string c = comp();
  std::string d = dest();
  std::string j = jmp();

  if (_compToBinary.find(c) == _compToBinary.end())
    return false;

  if (_destToBinary.find(d) == _destToBinary.end())
    return false;

  if (_jumpToBinary.find(j) == _jumpToBinary.end())
    return false;

  return true;
}

std::string CInstruction::translate() {
  std::string c = comp();
  std::string d = dest();
  std::string j = jmp();
  return (
    "111" +
    CInstruction::_compToBinary[c] +
    CInstruction::_destToBinary[d] +
    CInstruction::_jumpToBinary[j]
  );
}

void CInstruction::accept(Builder *builder) {
  dynamic_cast<HackBuilder*>(builder)->visit(this);
}

std::string CInstruction::dest() {
  std::string val = toString();
  auto start = val.begin();
  auto end = std::find(val.begin(), val.end(), '=');
  if (end == val.end()) {
    start = end;
  }
  std::string d(start, end);
  trim(d);

  return d;
}

std::string CInstruction::comp() {
  std::string val = toString();
  auto start = std::find(val.begin(), val.end(), '=');
  if (start != val.end())
    start = start + 1;
  else
    start = val.begin();

  auto end = std::find(start, val.end(), ';');

  std::string c(start, end);
  trim(c);

  return c;
}

std::string CInstruction::jmp() {
  std::string val = toString();
  auto start = std::find(val.begin(), val.end(), ';');
  if (start != val.end())
    start = start + 1;

  std::string j(start, val.end());
  trim(j);

  return j;
}

// CInstruction tables

std::unordered_map<std::string, std::string> CInstruction::createComputationTable() {
  std::unordered_map<std::string, std::string> map;
  map = {
    //         acccccc
    {"0",     "0101010"},
    {"1",     "0111111"},
    {"-1",    "0111010"},
    {"D",     "0001100"},
    {"A",     "0110000"}, {"M",     "1110000"},
    {"!D",    "0001101"},
    {"!A",    "0110001"}, {"!M",    "1110001"},
    {"-D",    "0001111"},
    {"-A",    "0110011"}, {"-M",    "1110011"},
    {"D+1",   "0011111"},
    {"A+1",   "0110111"}, {"M+1",   "1110111"},
    {"D-1",   "0001110"},
    {"A-1",   "0110010"}, {"M-1",   "1110010"},
    {"D+A",   "0000010"}, {"D+M",   "1000010"},
    {"D-A",   "0010011"}, {"D-M",   "1010011"},
    {"A-D",   "0000111"}, {"M-D",   "1000111"},
    {"D&A",   "0000000"}, {"D&M",   "1000000"},
    {"D|A",   "0010101"}, {"D|M",   "1010101"},
  };
  std::string commutative = "+|&";
  for (const auto &it : map) {
    std::string k = it.first;
    std::string v = it.second;
    char op = k[1];
    auto found = std::find(commutative.begin(), commutative.end(), op);
    if (found != commutative.end()) {
      // assuming operation is very easy, like XopY, so 3 chars in
      // total, since op is +, | or &.
      std::ostringstream oss;
      oss << k[2] << op << k[0];
      std::string kCommutative = oss.str();
      map[kCommutative] = v;
    }
  }
  return map;
}

std::unordered_map<std::string, std::string> CInstruction::_compToBinary = \
    CInstruction::createComputationTable();

std::unordered_map<std::string, std::string> CInstruction::_destToBinary = {
  {"", "000"},
  {"M", "001"},
  {"D", "010"},
  {"MD", "011"},
  {"A", "100"},
  {"AM", "101"},
  {"AD", "110"},
  {"AMD", "111"},
};

std::unordered_map<std::string, std::string> CInstruction::_jumpToBinary = {
  {"", "000"},
  {"JGT", "001"},
  {"JEQ", "010"},
  {"JGE", "011"},
  {"JLT", "100"},
  {"JNE", "101"},
  {"JLE", "110"},
  {"JMP", "111"},
};

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

std::string Label::translate() {
  throw std::runtime_error("Label doesn't support translate()");
}

void Label::accept(Builder *builder) {
  dynamic_cast<HackBuilder*>(builder)->visit(this);
}

std::string Label::getName() {
  std::string name = toString();
  return name.substr(1, name.size() - 2);
}
