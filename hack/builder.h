#ifndef __HACK__BUILDER__H__
#define __HACK__BUILDER__H__

#include <list>
#include <iostream>
#include <string>
#include <unordered_map>

#include "../builder.h"

#include "./instruction.h"

class HackBuilder: public Builder {
public:
  virtual void visit(Label *i) = 0;
  virtual void visit(CInstruction *i) = 0;
  virtual void visit(AInstruction *i) = 0;
  virtual Instruction* parseLine(const std::string &line);

  virtual void visit(HackInstruction *i);
protected:
  HackBuilder();  // abstract
private:
  std::string trimComment(const std::string &line);
};

class HackSymbolTranslator: public HackBuilder {
public:
  HackSymbolTranslator();
  virtual std::list<std::string>* getResult() override;
  virtual void visit(Label *i) override;
  virtual void visit(CInstruction *i) override;
  virtual void visit(AInstruction *i) override;
private:
  void initPredefinedSymbols();
private:
  bool _firstPass;
  int _crtInstructionNo;
  int _crtVariableNo;
  std::unordered_map<std::string, std::string> _symbolsTable;

  // memory address where variables start.
  static constexpr int VARIABLE_START = 16;
};

#endif
