#ifndef __INSTRUCTION__H__
#define __INSTRUCTION__H__

#include <string>

#include "builder.h"

class Instruction {
public:
  std::string toString();

  virtual bool isValid() = 0;
  virtual std::string toBinary() = 0;

  // visitor pattern. You should call a method like
  // builder->visitMyInstructionType(this) on the builder
  // in the new instruction subclasses you define.
  virtual void accept(const Builder *builder);
protected:
  Instruction();  // abstract
private:
  std::string _line;
};

#endif
