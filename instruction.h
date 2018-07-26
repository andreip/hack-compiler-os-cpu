#ifndef __INSTRUCTION__H__
#define __INSTRUCTION__H__

#include <string>

#include "builder.h"

class Instruction {
public:
  std::string toString() const;

  virtual ~Instruction();
  virtual bool isValid() = 0;
  virtual std::string toBinary() = 0;

  // visitor pattern. You should call a method like
  // builder->visitMyInstructionType(this) on the builder
  // in the new instruction subclasses you define.
  virtual void accept(Builder *builder);
protected:
  Instruction(std::string);  // abstract
  void set(std::string line);
private:
  std::string _line;
};

#endif
