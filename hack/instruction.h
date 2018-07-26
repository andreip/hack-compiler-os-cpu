#ifndef __HACK__INSTRUCTION__H__
#define __HACK__INSTRUCTION__H__

#include <string>

#include "../instruction.h"

class HackBuilder;

class HackInstruction: public Instruction {
protected:
  HackInstruction(std::string);  // abstract
  virtual void accept(const Builder*) override;
};

class AInstruction: public HackInstruction {
public:
  AInstruction(std::string);
  std::string value();
  void setValue(std::string);
  bool isValid() override;
  std::string toBinary() override;

  virtual void accept(const Builder*) override;
private:
};

class CInstruction: public HackInstruction {
public:
  CInstruction(std::string);
  bool isValid() override;
  std::string toBinary() override;

  virtual void accept(const Builder*) override;
};

class Label: public HackInstruction {
public:
  Label(std::string);
  bool isValid() override;
  std::string toBinary() override;

  virtual void accept(const Builder*) override;
};

#endif
