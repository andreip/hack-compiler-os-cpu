#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../utils.h"
#include "./builder.h"
#include "./instruction.h"

VMTranslationInstruction::VMTranslationInstruction(std::string line): Instruction(line), _builder(nullptr) {}

std::string VMTranslationInstruction::translate() {
  std::vector<std::string> lines = doTranslate();
  return join(lines, "\n");
}

void VMTranslationInstruction::accept(Builder *builder) {
  if (!builder)
    throw std::runtime_error("Something went wrong, no builder reference in accept of instruction: " + toString() + "\n");
  _builder = builder;  // keep a reference of builder.
}

Builder* VMTranslationInstruction::getBuilder() { return _builder; }

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
  VMTranslationInstruction::accept(builder);
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

std::vector<std::string> ConstantMemorySegment::doTranslate() {
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

std::vector<std::string> SegmentBaseMemorySegment::doTranslate() {
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

std::vector<std::string> TempMemorySegment::doTranslate() {
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

std::vector<std::string> PointerMemorySegment::doTranslate() {
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

StaticMemorySegment::StaticMemorySegment(std::string l): MemorySegment(l) { }

std::vector<std::string> StaticMemorySegment::doTranslate() {
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
  VMTranslationInstruction::accept(builder);
  dynamic_cast<HackBuilderVMTranslator*>(builder)->visit(this);
}

std::string ArithmeticLogic::value() {
  return trim_copy(toString());
}

// AddArithmeticLogic

AddArithmeticLogic::AddArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> AddArithmeticLogic::doTranslate() {
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

std::vector<std::string> SubArithmeticLogic::doTranslate() {
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

std::vector<std::string> NegArithmeticLogic::doTranslate() {
  return {"@SP",
          "A=M-1    // address SP[-1]",
          "M=-M     // SP[-1] = -SP[-1]",
  };
}

// EqArithmeticLogic

EqArithmeticLogic::EqArithmeticLogic(std::string l): ArithmeticLogic(l) {}

std::vector<std::string> EqArithmeticLogic::doTranslate() {
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

std::vector<std::string> GtArithmeticLogic::doTranslate() {
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

std::vector<std::string> LtArithmeticLogic::doTranslate() {
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

std::vector<std::string> AndArithmeticLogic::doTranslate() {
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

std::vector<std::string> OrArithmeticLogic::doTranslate() {
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

std::vector<std::string> NotArithmeticLogic::doTranslate() {
  return {"@SP",
          "M=M-1      // SP--",
          "A=M",
          "M=!M       // negate *SP",
          "@SP",
          "M=M+1      // SP++",
  };
}

// BranchingInstruction

BranchingInstruction::BranchingInstruction(std::string str)
  : VMTranslationInstruction(str) { }

void BranchingInstruction::accept(Builder *builder) {
  VMTranslationInstruction::accept(builder);
  dynamic_cast<HackBuilderVMTranslator*>(builder)->visit(this);
}

std::string BranchingInstruction::label() {
  std::vector<std::string> parts;
  split(parts, toString(), " ");
  return trim_copy(parts[1]);
}

std::string BranchingInstruction::cmd() {
  std::vector<std::string> parts;
  split(parts, toString(), " ");
  return trim_copy(parts[0]);
}

// LabelInstruction

LabelInstruction::LabelInstruction(std::string str)
  : BranchingInstruction(str) {}

LabelInstruction::LabelInstruction(BranchingInstruction &i): BranchingInstruction("label " + i.label()) {
  _builder = i.getBuilder();
}

bool LabelInstruction::isValid() {
  return cmd() == "label" && !label().empty();
}

std::string LabelInstruction::fullLabel() {
  std::string function = _builder->getCurrentFunction();
  std::string filename = getStem(_builder->getFilename());
  std::string prefix = function.empty() ? filename : function;
  return prefix + "$" + label();
}

std::vector<std::string> LabelInstruction::doTranslate() {
  return {
    "(" + fullLabel() + ")",
  };
}

// GotoInstruction

GotoInstruction::GotoInstruction(std::string str)
  : BranchingInstruction(str) {}

bool GotoInstruction::isValid() {
  return cmd() == "goto" && !label().empty();
}

std::vector<std::string> GotoInstruction::doTranslate() {
  LabelInstruction labelInstr(*this);
  return {
    "@" + labelInstr.fullLabel(),
    "0; JMP"    // jump <label>
  };
}

// IfGotoInstruction

IfGotoInstruction::IfGotoInstruction(std::string str)
  : BranchingInstruction(str) {}

bool IfGotoInstruction::isValid() {
  return cmd() == "if-goto" && !label().empty();
}

std::vector<std::string> IfGotoInstruction::doTranslate() {
  // jumps if pop() != 0
  LabelInstruction labelInstr(*this);
  return {
    "@SP",
    "M=M-1      // SP--",
    "@SP",
    "A=M",
    "D=M        // D = *SP",
    "@" + labelInstr.fullLabel(),
    "D; JNE     // jumps if D != 0",
  };
}

// BaseFunctionsInstruction

BaseFunctionsInstruction::BaseFunctionsInstruction(std::string str)
  : VMTranslationInstruction(str) { }

// FunctionInstruction

FunctionInstruction::FunctionInstruction(std::string str)
  : BaseFunctionsInstruction(str) { }

bool FunctionInstruction::isValid() {
  return !name().empty() && nVars() >= 0;
}

void FunctionInstruction::accept(Builder *builder) {
  VMTranslationInstruction::accept(builder);
  dynamic_cast<HackBuilderVMTranslator*>(builder)->visit(this);
}

std::vector<std::string> FunctionInstruction::doTranslate() {
  std::vector<std::string> output {
    "(" + name() + ") // repeat nVar times: push 0",
  };

  int n = nVars();
  while (n-- > 0)
    output.insert(
      output.end(),
      {
        "@SP",
        "A=M",
        "M=0        // *SP = 0",
        "@SP",
        "M=M+1      // SP++",
      }
    );

  return output;
}

std::string FunctionInstruction::name() {
  std::vector<std::string> parts;
  split(parts, toString(), " ");
  return trim_copy(parts[1]);
}

int FunctionInstruction::nVars() {
  std::vector<std::string> parts;
  split(parts, toString(), " ");
  return ::getNumber(trim_copy(parts[2]));
}

// ReturnInstruction

ReturnInstruction::ReturnInstruction(std::string line)
  : BaseFunctionsInstruction(line) { }

bool ReturnInstruction::isValid() {
  return true;
}

void ReturnInstruction::accept(Builder *builder) {
  VMTranslationInstruction::accept(builder);
  dynamic_cast<HackBuilderVMTranslator*>(builder)->visit(this);
}

std::vector<std::string> ReturnInstruction::doTranslate() {
  return {
    "@LCL",
    "D=M",
    "@R13",
    "M=D      // R13 = LCL, saves point where previous frame ends",
    "@5",
    "A=D-A    // D still contains LCL value",
    "D=M      // D = *(LCL-5), to get the return address",
    "@R14",
    "M=D      // R14 = return address, saves point of jump to at the end",
    "@SP",
    "M=M-1    // SP--",
    "A=M",
    "D=M      // D = return value of the function we are exiting",
    "@ARG",
    "A=M",
    "M=D      // *ARG = return value",
    "@ARG",
    "D=M+1",
    "@SP",
    "M=D      // SP = *ARG + 1, sets SP",
    "@R13",
    "D=M",
    "@4",
    "A=D-A",
    "D=M      // D = *(LCL-4), old local to restore",
    "@LCL",
    "M=D      // restores LCL",
    "@R13",
    "D=M",
    "@3",
    "A=D-A",
    "D=M      // D = *(LCL-3), old arg to restore",
    "@ARG",
    "M=D      // restores ARG",
    "@R13",
    "D=M",
    "@2",
    "A=D-A",
    "D=M      // D = *(LCL-2), old this to restore",
    "@THIS",
    "M=D      // restores THIS",
    "@R13",
    "A=M-1",
    "D=M      // D = *(LCL-1), old that to restore",
    "@THAT",
    "M=D      // restores THAT",
    "@R14",
    "A=M",
    "0; JMP   // jumps to old return address",
  };
}

// CallInstruction

CallInstruction::CallInstruction(std::string line)
  : BaseFunctionsInstruction(line) {
  parse();
}

CallInstruction::CallInstruction(const std::string &funcName, int nArgs)
  : BaseFunctionsInstruction(join(std::vector<std::string>{"call", funcName, ::toString(nArgs)}, " ")),
    _funcName(funcName),
    _nArgs(nArgs)
{
}

bool CallInstruction::isValid() {
  return nArgs() >= 0 && !funcName().empty();
}

void CallInstruction::accept(Builder *builder) {
  VMTranslationInstruction::accept(builder);
  dynamic_cast<HackBuilderVMTranslator*>(builder)->visit(this);
}

std::string CallInstruction::funcName() { return _funcName; }

int CallInstruction::nArgs() { return _nArgs; }

std::vector<std::string> CallInstruction::doTranslate() {
  std::string retAddr = getReturnAddress();
  return {
    "@" + retAddr,
    "D=A",
    "@SP",
    "A=M",
    "M=D      // *SP = retAddr, saves return address on stack",
    "@SP",
    "M=M+1    // SP++",
    "@LCL",
    "D=M",
    "@SP",
    "A=M",
    "M=D      // *SP = LCL, saves local address on stack",
    "@SP",
    "M=M+1    // SP++",
    "@ARG",
    "D=M",
    "@SP",
    "A=M",
    "M=D      // *SP = ARG, saves argument address on stack",
    "@SP",
    "M=M+1    // SP++",
    "@THIS",
    "D=M",
    "@SP",
    "A=M",
    "M=D      // *SP = THIS, saves this address on stack",
    "@SP",
    "M=M+1    // SP++",
    "@THAT",
    "D=M",
    "@SP",
    "A=M",
    "M=D      // *SP = THAT, saves that address on stack",
    "@SP",
    "M=M+1    // SP++",
    "@SP",
    "D=M",
    "@LCL",
    "M=D      // LCL = SP, sets local of new running function",
    "@SP",
    "D=M",
    "@" + ::toString(5 + _nArgs),
    "D=D-A",
    "@ARG",
    "M=D      // ARG = SP - 5 - nArgs, sets argument",
    "@" + funcName(),
    "0; JMP   // jumps to " + funcName(),
    "(" + retAddr + ")    // tricky, add return address",
  };
}

void CallInstruction::parse() {
  std::vector<std::string> parts;
  split(parts, toString(), " ");
  _funcName = trim_copy(parts[1]);
  _nArgs = ::getNumber(trim_copy(parts[2]));
}

std::string CallInstruction::getReturnAddress() {
  static int i = -1;
  return funcName() + "$ret." + ::toString(++i);
}
