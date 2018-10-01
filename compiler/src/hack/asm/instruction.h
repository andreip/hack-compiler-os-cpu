#ifndef __HACK__INSTRUCTION__H__
#define __HACK__INSTRUCTION__H__

#include <string>

#include "generic/instruction.h"

class HackBuilder;

class HackInstruction: public Instruction {
protected:
  HackInstruction(std::string);  // abstract
  virtual void accept(Builder*) override;
};

class AInstruction: public HackInstruction {
public:
  AInstruction(std::string);
  std::string value();
  void setValue(std::string);
  bool isValid() override;
  bool isNumericValue();
  std::string translate() override;

  virtual void accept(Builder*) override;
private:
  // A-instruction's value can hold at most a 15-bit integer.
  static constexpr int BITS_VALUE = 15;
  static constexpr int MAX_VALUE = 1 << BITS_VALUE;
};

class CInstruction: public HackInstruction {
public:
  CInstruction(std::string);
  bool isValid() override;
  std::string translate() override;

  virtual void accept(Builder*) override;
private:
  std::string dest();
  std::string comp();
  std::string jmp();

  static std::unordered_map<std::string, std::string> createComputationTable();

private:
  static std::unordered_map<std::string, std::string> _compToBinary;
  static std::unordered_map<std::string, std::string> _destToBinary;
  static std::unordered_map<std::string, std::string> _jumpToBinary;
};

class Label: public HackInstruction {
public:
  Label(std::string);
  bool isValid() override;
  std::string translate() override;

  virtual void accept(Builder*) override;

  std::string getName();
private:
};

#endif
