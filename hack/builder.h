#ifndef __HACK__BUILDER__H__
#define __HACK__BUILDER__H__

#include <list>
#include <iostream>

#include "../builder.h"

#include "./instruction.h"

class HackBuilder: public Builder {
public:
  virtual void visitLabel(const Label *i) const = 0;
  virtual void visitCInstruction(const CInstruction *i) const = 0;
  virtual void visitAInstruction(const AInstruction *i) const = 0;
  virtual Instruction* parseLine(const std::string &line);

  void visitHackInstruction() const;
protected:
  HackBuilder();  // abstract
private:
  std::string trimComment(const std::string &line);
};

class HackSymbolTranslator: public HackBuilder {
public:
  HackSymbolTranslator();
  virtual void visitLabel(const Label *i) const override;
  virtual void visitCInstruction(const CInstruction *i) const override;
  virtual void visitAInstruction(const AInstruction *i) const override;
private:
};

#endif
