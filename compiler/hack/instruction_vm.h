#ifndef __INSTRUCTION_VM__H__
#define __INSTRUCTION_VM__H__

#include <string>
#include <unordered_map>
#include <vector>

#include "../instruction.h"


class MemorySegment: public Instruction {
public:
  static std::vector<std::string> parse(const std::string&);

  MemorySegment(std::string);
  bool isValid() override;
  std::string translate() override;

  virtual void accept(Builder *builder) override;
protected:
  // <op> <segment> <value>
  virtual void parse();
  virtual std::string segment();
  virtual std::string op();
  virtual int value();
  virtual std::vector<std::string> _translate() = 0;
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


class ArithmeticLogic: public Instruction {
public:
  ArithmeticLogic(std::string);
  bool isValid() override;
  std::string translate() override;

  virtual void accept(Builder *builder) override;
private:
  static std::unordered_map<std::string, std::vector<std::string>> to_asm;
};

#endif
