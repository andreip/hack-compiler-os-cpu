#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../utils.h"
#include "./builder.h"
#include "./instruction.h"

VMTranslationInstruction::VMTranslationInstruction(std::string line): Instruction(line) {}

std::string VMTranslationInstruction::translate() {
  std::vector<std::string> lines = _translate();
  return join(lines, "\n");
}


// MemorySegment

std::vector<std::string> MemorySegment::segments = {
  "local", "argument", "this", "that",
  "static", "constant", "pointer", "temp"
};

MemorySegment::MemorySegment(std::string line): VMTranslationInstruction(line) {
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

void MemorySegment::accept(Builder *builder) {
  dynamic_cast<HackBuilderVMTranslator*>(builder)->visit(this);
}

std::vector<std::string> MemorySegment::parse(const std::string &line) {
  std::vector<std::string> parts;
  split(parts, line, " ");
  trim(parts[0]);
  trim(parts[1]);
  trim(parts[2]);
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

// SegmentBaseMemorySegment

std::unordered_map<std::string, std::string> SegmentBaseMemorySegment::segmentToBase = {
  { "local", "LCL" },
  { "argument", "ARG" },
  { "this", "THIS" },
  { "that", "THAT" },
};

SegmentBaseMemorySegment::SegmentBaseMemorySegment(std::string s): MemorySegment(s) {
}

bool SegmentBaseMemorySegment::canHandleSegment(const std::string &seg) {
  return segmentToBase.find(seg) != segmentToBase.end();
}

std::vector<std::string> SegmentBaseMemorySegment::_translate() {
  std::string baseName = segmentToBase.at(segment());
  if (op() == "pop")
    return {
      "@" + ::toString(value()),
      "D=A",
      "@" + baseName,
      "D=D+M    // D = (baseName + i), saves address offset",
      "@R13     // a general purpose register to use",
      "M=D",
      "@SP",
      "M=M-1    // SP--",
      "A=M",
      "D=M      // D = *SP",
      "@R13",
      "A=M",
      "M=D      // *R13 = D",
    };

  if (op() == "push")
    return {
      "@" + ::toString(value()),
      "D=A",
      "@" + baseName,
      "A=D+M    // A = (baseName + i), go to that address",
      "D=M      // D = *(baseName + i), save the data from that segment",
      "@SP",
      "A=M",
      "M=D      // *SP = D puts the saved data into SP location",
      "@SP",
      "M=M+1    // SP++",
    };

  throw std::runtime_error("Invalid instruction: " + toString());
}

// TempMemorySegment

TempMemorySegment::TempMemorySegment(std::string l): MemorySegment(l) {}

bool TempMemorySegment::isValid() {
  int val = value();
  return val >= 0 && val < SIZE && MemorySegment::isValid();
}

std::vector<std::string> TempMemorySegment::_translate() {
  int offset = BASE_SEGMENT + value();
  if (op() == "push")
    return {
      "@" + ::toString(offset),
      "D=M    // D = TEMP[base + i], get value from temp segment",
      "@SP",
      "A=M",
      "M=D    // *SP = D",
      "@SP",
      "M=M+1  // SP++",
    };

  if (op() == "pop")
    return {
      "@SP",
      "M=M-1  // SP--",
      "A=M",
      "D=M    // D = *SP",
      "@" + ::toString(offset),
      "M=D    // TEMP[base+i] = D, puts what's in stack in temp",
    };

  throw std::runtime_error("Invalid instruction: " + toString());
}

// PointerMemorySegment

PointerMemorySegment::PointerMemorySegment(std::string l): MemorySegment(l) {}

bool PointerMemorySegment::isValid() {
  int val = value();
  return val >= 0 && val < SIZE && MemorySegment::isValid();
}

std::vector<std::string> PointerMemorySegment::_translate() {
  std::string name = value() == 0 ? "THIS" : "THAT";
  if (op() == "push")
    return {
      "@" + name,
      "D=M    // D = THIS/THAT",
      "@SP",
      "A=M",
      "M=D    // *SP = D",
      "@SP",
      "M=M+1  // SP++",
    };

  if (op() == "pop")
    return {
      "@SP",
      "M=M-1  // SP--",
      "A=M",
      "D=M    // D = *SP",
      "@" + name,
      "M=D    // THIS/THAT = D",
    };

  throw std::runtime_error("Invalid instruction: " + toString());
}

// StaticMemorySegment

StaticMemorySegment::StaticMemorySegment(std::string l): MemorySegment(l), _builder(nullptr) {}

void StaticMemorySegment::accept(Builder *builder) {
  _builder = builder;  // keep a reference of builder.
  MemorySegment::accept(builder);
}

std::vector<std::string> StaticMemorySegment::_translate() {
  if (!_builder)
    throw std::runtime_error("Something went wrong, no builder reference in static memory segment to get filename from.");

  std::string filename = getStem(_builder->getFilename());

  // "push static 5" gets converted into "Filename.5",
  // which we'll treat as a variable by prepending @ to it.
  // (assuming instruction is in Filename.vm)
  std::string varName = filename + "." + ::toString(value());

  if (op() == "push")
    return {
      "@" + varName,
      "D=M    // D = *(Foo.i), get data from static location",
      "@SP",
      "A=M",
      "M=D    // *SP = D, puts it on stack",
      "@SP",
      "M=M+1  // SP++",
    };

  if (op() == "pop")
    return {
      "@SP",
      "M=M-1  // SP--",
      "A=M",
      "D=M    // D = *SP",
      "@" + varName,
      "M=D    // *(Foo.i) = D, puts into static location sth from stack.",
    };

  throw std::runtime_error("Invalid command: " + toString());
}

// ArithmeticLogic

std::vector<std::string> ArithmeticLogic::ops = {
  "add", "sub", "neg", "eq",
  "gt", "lt", "and", "or", "not",
};

ArithmeticLogic::ArithmeticLogic(std::string line): VMTranslationInstruction(line) {}

bool ArithmeticLogic::isArithmeticLogicOp(const std::string &instr) {
  auto it = std::find(ops.begin(), ops.end(), instr);
  return it != ops.end();
}

bool ArithmeticLogic::isValid() {
  return isArithmeticLogicOp(value());
}

void ArithmeticLogic::accept(Builder *builder) {
  dynamic_cast<HackBuilderVMTranslator*>(builder)->visit(this);
}

std::string ArithmeticLogic::value() {
  return trim_copy(toString());
}

// AddArithmeticLogic

AddArithmeticLogic::AddArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> AddArithmeticLogic::_translate() {
  return {
    "@SP",
    "M=M-1    // SP--",
    "A=M",
    "D=M      // D = *SP",
    "@SP",
    "M=M-1    // SP--",
    "A=M",
    "M=D+M    // *SP += D",
    "@SP",
    "M=M+1    // SP++",
  };
}

// SubArithmeticLogic

SubArithmeticLogic::SubArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> SubArithmeticLogic::_translate() {
  return {"@SP",
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
  };
}

// NegArithmeticLogic

NegArithmeticLogic::NegArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> NegArithmeticLogic::_translate() {
  return {"@SP",
          "A=M-1    // address SP[-1]",
          "M=-M     // SP[-1] = -SP[-1]",
  };
}

// EqArithmeticLogic

EqArithmeticLogic::EqArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> EqArithmeticLogic::_translate() {
  static int i = 0;
  i++;
  std::string iStr = ::toString(i);
  return {"@SP",
          "M=M-1     // SP--",
          "A=M",
          "D=M       // D = *SP",
          "@SP",
          "M=M-1     // SP--",
          "A=M",
          "D=D-M     // D -= *SP",
          "@EQ_NOT_EQUAL" + iStr,
          "D; JNE",
          "D=-1      // writes 'true' in binary if D == 0",
          "@EQ_DONE" + iStr,
          "0; JMP",
       "(EQ_NOT_EQUAL" + iStr + ")",
          "D=0       // writes 'false' in binary if D != 0",
       "(EQ_DONE" + iStr + ")",
          "@SP",
          "A=M",
          "M=D       // *SP = true (-1) / false (0)",
          "@SP",
          "M=M+1     // SP++",
  };
}

// GtArithmeticLogic

GtArithmeticLogic::GtArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> GtArithmeticLogic::_translate() {
  static int i = 0;
  i++;
  std::string iStr = ::toString(i);
  return {"@SP",
          "M=M-1      // SP--",
          "A=M",
          "D=M        // D = *SP",
          "@SP",
          "M=M-1      // SP--",
          "A=M",
          "D=D-M",
          "D=-D       // D = SP[-2] - SP[-1]",
          "@GT_THAN_ZERO" + iStr,
          "D; JGT",
          "D=0        // writes 'false' in binary if D <= 0",
          "@GT_DONE" + iStr,
          "0; JMP",
       "(GT_THAN_ZERO" + iStr + ")",
          "D=-1       // writes 'true' in binary if D > 0",
       "(GT_DONE" + iStr + ")",
          "@SP",
          "A=M",
          "M=D        // *SP = true (-1) / false (0)",
          "@SP",
          "M=M+1      // SP++",
  };
}

// LtArithmeticLogic

LtArithmeticLogic::LtArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> LtArithmeticLogic::_translate() {
  static int i = 0;
  i++;
  std::string iStr = ::toString(i);
  return {"@SP",
          "M=M-1      // SP--",
          "A=M",
          "D=M        // D = *SP",
          "@SP",
          "M=M-1      // SP--",
          "A=M",
          "D=D-M",
          "D=-D       // D = SP[-2] - SP[-1]",
          "@LT_THAN_ZERO" + iStr,
          "D; JLT",
          "D=0        // writes 'false' in binary if D >= 0",
          "@LT_DONE" + iStr,
          "0; JMP",
       "(LT_THAN_ZERO" + iStr + ")",
          "D=-1       // writes 'true' in binary if D < 0",
       "(LT_DONE" + iStr + ")",
          "@SP",
          "A=M",
          "M=D        // *SP = true (-1) / false (0)",
          "@SP",
          "M=M+1      // SP++",
  };
}

// AndArithmeticLogic

AndArithmeticLogic::AndArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> AndArithmeticLogic::_translate() {
  return {"@SP",
          "M=M-1      // SP--",
          "A=M",
          "D=M        // D = *SP",
          "@SP",
          "M=M-1      // SP--",
          "A=M",
          "M=D&M      // puts result into sp",
          "@SP",
          "M=M+1      // SP++",
  };
}

// OrArithmeticLogic

OrArithmeticLogic::OrArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> OrArithmeticLogic::_translate() {
  return {"@SP",
          "M=M-1      // SP--",
          "A=M",
          "D=M        // D = *SP",
          "@SP",
          "M=M-1      // SP--",
          "A=M",
          "M=D|M      // puts result into sp",
          "@SP",
          "M=M+1      // SP++",
  };
}

// NotArithmeticLogic

NotArithmeticLogic::NotArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> NotArithmeticLogic::_translate() {
  return {"@SP",
          "M=M-1      // SP--",
          "A=M",
          "M=!M       // negate *SP",
          "@SP",
          "M=M+1      // SP++",
  };
}
