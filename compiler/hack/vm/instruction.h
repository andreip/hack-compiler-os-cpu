#ifndef __INSTRUCTION_VM__H__
#define __INSTRUCTION_VM__H__

#include <string>
#include <unordered_map>
#include <vector>

#include "../../instruction.h"

// base

class VMTranslationInstruction: public Instruction {
public:
  std::string translate() override;
  virtual void accept(Builder *builder) override;
  Builder *getBuilder();
protected:
  VMTranslationInstruction(std::string);
  virtual std::vector<std::string> _translate() = 0;
protected:
  Builder *_builder;
};

// memory segment instructions

class MemorySegment: public VMTranslationInstruction {
public:
  static std::vector<std::string> parse(const std::string&);
  bool isValid() override;
  virtual void accept(Builder *builder) override;
protected:
  // <op> <segment> <value>
  MemorySegment(std::string);
  virtual void parse();
  virtual std::string segment();
  virtual std::string op();
  virtual int value();
protected:
  bool _parsed;
  std::string _segment;
  std::string _op;
  int _value;
  static std::vector<std::string> segments;
};

class ConstantMemorySegment : public MemorySegment {
public:
  ConstantMemorySegment(std::string);
protected:
  std::vector<std::string> _translate() override;
};

class SegmentBaseMemorySegment : public MemorySegment {
public:
  SegmentBaseMemorySegment(std::string);
  static bool canHandleSegment(const std::string&);
protected:
  std::vector<std::string> _translate() override;
private:
  static std::unordered_map<std::string, std::string> segmentToBase;
};

class TempMemorySegment : public MemorySegment {
public:
  TempMemorySegment(std::string);
  bool isValid() override;
protected:
  std::vector<std::string> _translate() override;
private:
  static constexpr int BASE_SEGMENT = 5;  // fixed
  static constexpr int SIZE = 8;
};

class PointerMemorySegment: public MemorySegment {
public:
  PointerMemorySegment(std::string);
  bool isValid() override;
protected:
  std::vector<std::string> _translate() override;
private:
  static constexpr int BASE_SEGMENT = 3;  // fixed
  static constexpr int SIZE = 2;
};

class StaticMemorySegment: public MemorySegment {
public:
  StaticMemorySegment(std::string);
protected:
  std::vector<std::string> _translate() override;
};

// arithmetic & logic instructions

class ArithmeticLogic: public VMTranslationInstruction {
public:
  static bool isArithmeticLogicOp(const std::string &);
  bool isValid() override;
  virtual void accept(Builder *builder) override;
protected:
  ArithmeticLogic(std::string);
  std::string value();
private:
  static std::vector<std::string> ops;
};

class AddArithmeticLogic: public ArithmeticLogic {
public:
  AddArithmeticLogic(std::string);
protected:
  std::vector<std::string> _translate() override;
};

class SubArithmeticLogic: public ArithmeticLogic {
public:
  SubArithmeticLogic(std::string);
protected:
  std::vector<std::string> _translate() override;
};

class NegArithmeticLogic: public ArithmeticLogic {
public:
  NegArithmeticLogic(std::string);
protected:
  std::vector<std::string> _translate() override;
};

class EqArithmeticLogic: public ArithmeticLogic {
public:
  EqArithmeticLogic(std::string);
protected:
  std::vector<std::string> _translate() override;
};

class GtArithmeticLogic: public ArithmeticLogic {
public:
  GtArithmeticLogic(std::string);
protected:
  std::vector<std::string> _translate() override;
};

class LtArithmeticLogic: public ArithmeticLogic {
public:
  LtArithmeticLogic(std::string);
protected:
  std::vector<std::string> _translate() override;
};

class AndArithmeticLogic: public ArithmeticLogic {
public:
  AndArithmeticLogic(std::string);
protected:
  std::vector<std::string> _translate() override;
};

class OrArithmeticLogic: public ArithmeticLogic {
public:
  OrArithmeticLogic(std::string);
protected:
  std::vector<std::string> _translate() override;
};

class NotArithmeticLogic: public ArithmeticLogic {
public:
  NotArithmeticLogic(std::string);
protected:
  std::vector<std::string> _translate() override;
};

// branching instructions

class BranchingInstruction: public VMTranslationInstruction {
public:
  virtual void accept(Builder *builder) override;
  // label|goto|if-goto <label>
  virtual std::string cmd();
  virtual std::string label();
protected:
  BranchingInstruction(std::string);
};

class LabelInstruction: public BranchingInstruction {
public:
  LabelInstruction(std::string);
  LabelInstruction(BranchingInstruction&);  // copy constructor
  bool isValid() override;
  std::string fullLabel();
protected:
  virtual std::vector<std::string> _translate() override;
};

class GotoInstruction: public BranchingInstruction {
public:
  GotoInstruction(std::string);
  bool isValid() override;
protected:
  virtual std::vector<std::string> _translate() override;
};

class IfGotoInstruction: public BranchingInstruction {
public:
  IfGotoInstruction(std::string);
  bool isValid() override;
protected:
  virtual std::vector<std::string> _translate() override;
};

#endif
