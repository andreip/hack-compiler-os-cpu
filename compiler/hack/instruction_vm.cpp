#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "../utils.h"
#include "./instruction_vm.h"
#include "./builder_vm.h"

// MemorySegment

std::vector<std::string> MemorySegment::segments = {
  "local", "argument", "this", "that",
  "static", "constant", "pointer", "temp"
};

MemorySegment::MemorySegment(std::string line): Instruction(line) {
  _parsed = false;
}

bool MemorySegment::isValid() {
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

std::string MemorySegment::translate() {
  std::cout << "Translating " << toString() << '\n';
  std::vector<std::string> lines = _translate();
  return boost::algorithm::join(lines, "\n");
}

void MemorySegment::accept(Builder *builder) {
  dynamic_cast<HackVMTranslator*>(builder)->visit(this);
}

std::vector<std::string> MemorySegment::parse(const std::string &line) {
  std::vector<std::string> parts;
  boost::algorithm::split(parts, line, boost::is_space());
  boost::algorithm::trim(parts[0]);
  boost::algorithm::trim(parts[1]);
  boost::algorithm::trim(parts[2]);
  if (!isNumber(parts[2]))
    throw std::runtime_error("Invalid command: " + line);
  return parts;
}

void MemorySegment::parse() {
  if (_parsed)
    return;

  std::vector<std::string> parts = MemorySegment::parse(toString());
  _op = parts[0];
  _segment = parts[1];
  _value = getNumber(parts[2]);
  _parsed = true;

  std::cout << "Parsed " << "op=" << _op << ", _seg=" << _segment << ", _val=" << _value << "\n";
}

std::string MemorySegment::segment() {
  parse();
  return _segment;
}

std::string MemorySegment::op() {
  parse();
  return _op;
}

int MemorySegment::value() {
  parse();
  return _value;
}

// ConstantMemorySegment

ConstantMemorySegment::ConstantMemorySegment(std::string s): MemorySegment(s) {}

std::vector<std::string> ConstantMemorySegment::_translate() {
  std::vector<std::string> v;
  if (op() == "push")
    v = {
      "@" + ::toString(value()),
      "D=A      // save value in D",
      "@SP",
      "A=M",
      "M=D      // *SP = D, set the new value",
      "@SP",
      "M=M+1    // SP++",
    };
  else
    throw std::runtime_error("Invalid instruction: " + toString());
  return v;
}

// ArithmeticLogic

ArithmeticLogic::ArithmeticLogic(std::string line): Instruction(line) {}

bool ArithmeticLogic::isValid() {
  return true;
}

std::string ArithmeticLogic::translate() {
  std::string val = toString();
  if (to_asm.find(val) != to_asm.end())
    return boost::algorithm::join(to_asm[val], "\n");
  return "";
}

void ArithmeticLogic::accept(Builder *builder) {
  dynamic_cast<HackVMTranslator*>(builder)->visit(this);
}

std::unordered_map<std::string, std::vector<std::string>> ArithmeticLogic::to_asm {
  { "add", {"@SP",
            "M=M-1    // SP--",
            "A=M",
            "D=M      // D = *SP",
            "@SP",
            "M=M-1    // SP--",
            "A=M",
            "M=D+M    // *SP += D",
            "@SP",
            "M=M+1    // SP++",
           },
  },
  { "sub", {"@SP",
            "M=M-1    // SP--",
            "A=M",
            "D=M      // D = *SP",
            "@SP",
            "M=M-1    // SP--",
            "A=M",
            "M=D-M",
            "M=-M     // *SP = -(D - *SP)",
            "@SP",
            "M=M+1    // SP++ back to how it was",
           },
  },
  { "neg", {"@SP",
            "A=M-1    // address SP[-1]",
            "M=-M     // SP[-1] = -SP[-1]",
           },
  },
  { "eq", {"@SP",
           "M=M-1     // SP--",
           "A=M",
           "D=M       // D = *SP",
           "@SP",
           "M=M-1     // SP--",
           "A=M",
           "D=D-M     // D -= *SP",
           // TODO: move into its own class and generate
           // unique labels for each eq call, with a suffix number.
           "@EQ_NOT_EQUAL",
           "D; JNE",
           "D=-1      // writes 'true' in binary if D == 0",
           "@EQ_DONE",
           "0; JMP",
       "(EQ_NOT_EQUAL)",
           "D=0       // writes 'false' in binary if D != 0",
       "(EQ_DONE)",
           "@SP",
           "A=M",
           "M=D       // *SP = true (-1) / false (0)",
           "@SP",
           "M=M+1     // SP++",
          },
  },
};
