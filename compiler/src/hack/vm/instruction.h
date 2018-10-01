#ifndef __INSTRUCTION_VM__H__
#define __INSTRUCTION_VM__H__

#include <string>
#include <unordered_map>
#include <vector>

#include "generic/instruction.h"

// base

class VMTranslationInstruction: public Instruction {
public:
  std::string translate() override;
  virtual void accept(Builder *builder) override;
  Builder *getBuilder();
  virtual std::vector<std::string> doTranslate() = 0;
protected:
  VMTranslationInstruction(std::string line);
protected:
  Builder *_builder;
};

// memory segment instructions

class MemorySegment: public VMTranslationInstruction {
public:
  static std::vector<std::string> parse(const std::string&);
  virtual bool isValid() override;
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
  std::vector<std::string> doTranslate() override;
};

class SegmentBaseMemorySegment : public MemorySegment {
public:
  SegmentBaseMemorySegment(std::string);
  static bool canHandleSegment(const std::string&);
  std::vector<std::string> doTranslate() override;
private:
  static std::unordered_map<std::string, std::string> segmentToBase;
};

class TempMemorySegment : public MemorySegment {
public:
  TempMemorySegment(std::string);
  virtual bool isValid() override;
  std::vector<std::string> doTranslate() override;
private:
  static constexpr int BASE_SEGMENT = 5;  // fixed
  static constexpr int SIZE = 8;
};

class PointerMemorySegment: public MemorySegment {
public:
  PointerMemorySegment(std::string);
  virtual bool isValid() override;
  std::vector<std::string> doTranslate() override;
private:
  static constexpr int BASE_SEGMENT = 3;  // fixed
  static constexpr int SIZE = 2;
};

class StaticMemorySegment: public MemorySegment {
public:
  StaticMemorySegment(std::string);
  std::vector<std::string> doTranslate() override;
};

// arithmetic & logic instructions

class ArithmeticLogic: public VMTranslationInstruction {
public:
  static bool isArithmeticLogicOp(const std::string &);
  virtual bool isValid() override;
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
  std::vector<std::string> doTranslate() override;
};

class SubArithmeticLogic: public ArithmeticLogic {
public:
  SubArithmeticLogic(std::string);
  std::vector<std::string> doTranslate() override;
};

class NegArithmeticLogic: public ArithmeticLogic {
public:
  NegArithmeticLogic(std::string);
  std::vector<std::string> doTranslate() override;
};

class EqArithmeticLogic: public ArithmeticLogic {
public:
  EqArithmeticLogic(std::string);
  std::vector<std::string> doTranslate() override;
};

class GtArithmeticLogic: public ArithmeticLogic {
public:
  GtArithmeticLogic(std::string);
  std::vector<std::string> doTranslate() override;
};

class LtArithmeticLogic: public ArithmeticLogic {
public:
  LtArithmeticLogic(std::string);
  std::vector<std::string> doTranslate() override;
};

class AndArithmeticLogic: public ArithmeticLogic {
public:
  AndArithmeticLogic(std::string);
  std::vector<std::string> doTranslate() override;
};

class OrArithmeticLogic: public ArithmeticLogic {
public:
  OrArithmeticLogic(std::string);
  std::vector<std::string> doTranslate() override;
};

class NotArithmeticLogic: public ArithmeticLogic {
public:
  NotArithmeticLogic(std::string);
  std::vector<std::string> doTranslate() override;
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
  virtual bool isValid() override;
  std::string fullLabel();
  virtual std::vector<std::string> doTranslate() override;
};

class GotoInstruction: public BranchingInstruction {
public:
  GotoInstruction(std::string);
  virtual bool isValid() override;
  virtual std::vector<std::string> doTranslate() override;
};

class IfGotoInstruction: public BranchingInstruction {
public:
  IfGotoInstruction(std::string);
  virtual bool isValid() override;
  virtual std::vector<std::string> doTranslate() override;
};

// function instructions

class BaseFunctionsInstruction: public VMTranslationInstruction {
protected:
  BaseFunctionsInstruction(std::string);
};

class FunctionInstruction: public BaseFunctionsInstruction {
public:
  FunctionInstruction(std::string);
  std::string name();   // function name we're defining
  int nVars();          // number of local variables

  virtual bool isValid() override;
  virtual void accept(Builder *builder) override;
  virtual std::vector<std::string> doTranslate() override;
};

class ReturnInstruction: public BaseFunctionsInstruction {
public:
  ReturnInstruction(std::string);
  virtual bool isValid() override;
  virtual void accept(Builder*) override;
  virtual std::vector<std::string> doTranslate() override;
};

class CallInstruction: public BaseFunctionsInstruction {
public:
  CallInstruction(std::string line);
  CallInstruction(const std::string &funcName, int nArgs);
  virtual bool isValid() override;
  virtual void accept(Builder*) override;
  std::string funcName();
  int nArgs();
  virtual std::vector<std::string> doTranslate() override;
private:
  void parse();
  std::string getReturnAddress();
private:
  std::string _funcName;
  int _nArgs;
};

#endif
