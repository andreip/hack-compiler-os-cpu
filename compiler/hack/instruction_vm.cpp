#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include <boost/algorithm/string.hpp>

#include "../utils.h"
#include "./instruction_vm.h"
#include "./builder_vm.h"

// MemoryAccessCmd

std::vector<std::string> MemoryAccessCmd::segments = {
  "local", "argument", "this", "that",
  "static", "constant", "pointer", "temp"
};

MemoryAccessCmd::MemoryAccessCmd(std::string line): Instruction(line) {
  _parsed = false;
}

bool MemoryAccessCmd::isValid() {
  try {
    parse();
  } catch (std::runtime_error &e) {
    return false;
  }

  std::string o = op();
  std::string s = segment();
  int v = value();

  // invalid segment given.
  auto found_it = std::find(segments.begin(), segments.end(),
                            segment());
  if (found_it == segments.end())
    return false;

  if (o != "pop" && o != "push")
    return false;

  // edge case, cannot do "pop constant"
  if (o == "pop" && s == "constant")
    return false;

  // 16-bit value
  if (v < 0 || v > ((1 << 16) - 1))
    return false;

  return true;
}

std::string MemoryAccessCmd::translate() {
  std::string s = segment();
  std::vector<std::string> *lines;

  std::cout << "Translating " << toString() << '\n';
  if (s == "constant")
    lines = std::move(translateConstant());
  std::string res = boost::algorithm::join(*lines, "\n");
  delete lines;
  return res;
}

void MemoryAccessCmd::accept(Builder *builder) {
  dynamic_cast<HackVMTranslator*>(builder)->visit(this);
}

void MemoryAccessCmd::parse() {
  if (_parsed)
    return;

  std::string val = toString();
  std::vector<std::string> parts;
  boost::algorithm::split(parts, val, boost::is_space());
  _op = boost::algorithm::trim_copy(parts[0]);
  _segment = boost::algorithm::trim_copy(parts[1]);
  std::string valueStr = boost::algorithm::trim_copy(parts[2]);
  if (!isNumber(valueStr))
    throw std::runtime_error("Invalid command " + val);
  _value = getNumber(valueStr);

  std::cout << "Parsed " << "op=" << _op << ", _seg=" << _segment << ", _val=" << _value << "\n";

  _parsed = true;
}

std::string MemoryAccessCmd::segment() {
  parse();
  return _segment;
}

std::string MemoryAccessCmd::op() {
  parse();
  return _op;
}

int MemoryAccessCmd::value() {
  parse();
  return _value;
}

std::vector<std::string>* MemoryAccessCmd::translateConstant() {
  if (op() == "push")
    return new std::vector<std::string> {
      "@" + ::toString(value()),
      "D=A      // save value in D",
      "@SP",
      "A=M",
      "M=D      // *SP = D, set the new value",
      "@SP",
      "M=M+1    // SP++",
    };
  return nullptr;
}

// ArithmeticLogicCmd

ArithmeticLogicCmd::ArithmeticLogicCmd(std::string line): Instruction(line) {}

bool ArithmeticLogicCmd::isValid() {
  return true;
}

std::string ArithmeticLogicCmd::translate() {
  std::string val = toString();
  if (to_asm.find(val) != to_asm.end())
    return boost::algorithm::join(to_asm[val], "\n");
  return "";
}

void ArithmeticLogicCmd::accept(Builder *builder) {
  dynamic_cast<HackVMTranslator*>(builder)->visit(this);
}

std::unordered_map<std::string, std::vector<std::string>> ArithmeticLogicCmd::to_asm {
  { "add", {"@SP",
            "M=M-1    // SP--",
            "A=M",
            "D=M      // D = *SP",
            "@SP",
            "M=M-1    // SP--",
            "A=M",
            "D=D+M    // D += *SP, holds the addition",
            "@SP",
            "A=M",
            "M=D      // *SP = D, set addition",
            "@SP",
            "M=M+1    // SP++ back to how it was",
           },
  },
  { "sub", {"@SP",
            "M=M-1    // SP--",
            "A=M",
            "D=M      // D = *SP",
            "@SP",
            "M=M-1    // SP--",
            "A=M",
            "D=D-M    // D -= *SP, holds the inverse subtraction",
            "D=-D     // makes D = SP[-2] - SP[-1]",
            "@SP",
            "A=M",
            "M=D      // *SP = D, set rezult",
            "@SP",
            "M=M+1    // SP++ back to how it was",
           },
  },
  { "neg", {"@SP",
            "A=M-1   // address SP[-1]",
            "M=-M    // SP[-1] = -SP[-1]",
           },
  // TODO do more here.
  //{ "eq", {"
  },
};
