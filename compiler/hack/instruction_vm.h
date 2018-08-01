#ifndef __INSTRUCTION_VM__H__
#define __INSTRUCTION_VM__H__

#include <string>

#include "../instruction.h"

class MemoryAccessCmd: public Instruction {
public:
  MemoryAccessCmd(std::string);
  bool isValid() override;
  std::string translate() override;

  virtual void accept(Builder *builder) override;
private:
  //static std::vector<std::string> segments;
};

class ArithmeticLogicCmd: public Instruction {
public:
  ArithmeticLogicCmd(std::string);
  bool isValid() override;
  std::string translate() override;

  virtual void accept(Builder *builder) override;
private:
};

#endif
