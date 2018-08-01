#ifndef __INSTRUCTION_VM__H__
#define __INSTRUCTION_VM__H__

#include <string>
#include <unordered_map>
#include <vector>

#include "../instruction.h"

class MemoryAccessCmd: public Instruction {
public:
  MemoryAccessCmd(std::string);
  bool isValid() override;
  std::string translate() override;

  virtual void accept(Builder *builder) override;
private:
  // <op> <segment> <value>
  void parse();
  std::string segment();
  std::string op();
  int value();

  std::vector<std::string>* translateConstant();

private:
  bool _parsed;
  std::string _segment;
  std::string _op;
  int _value;
  static std::vector<std::string> segments;
};

class ArithmeticLogicCmd: public Instruction {
public:
  ArithmeticLogicCmd(std::string);
  bool isValid() override;
  std::string translate() override;

  virtual void accept(Builder *builder) override;
private:
  static std::unordered_map<std::string, std::vector<std::string>> to_asm;
};

#endif
