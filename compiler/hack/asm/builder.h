#ifndef __HACK__BUILDER__H__
#define __HACK__BUILDER__H__

#include <list>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "../../builder.h"

#include "./instruction.h"

class HackBuilder: public Builder {
public:
  virtual void visit(Label *i) = 0;
  virtual void visit(CInstruction *i) = 0;
  virtual void visit(AInstruction *i) = 0;
  virtual Instruction* parseLine(const std::string &line);

  virtual void visit(HackInstruction *i);
protected:
  HackBuilder();
  HackBuilder(const std::string&);  // abstract
};

class HackSymbolTranslator: public HackBuilder {
public:
  HackSymbolTranslator();
  HackSymbolTranslator(const std::string&);
  virtual void init();
  virtual std::list<std::string>* getResult() override;
  virtual void visit(Label *i) override;
  virtual void visit(CInstruction *i) override;
  virtual void visit(AInstruction *i) override;
private:
  void initPredefinedSymbols();
  void writeDebugOutputFile();
  void writeDebugInstruction(Instruction *i);
  void incrementInstructionNo();
  void incrementVariableNo();
private:
  bool _firstPass;
  int _crtInstructionNo;
  int _crtVariableNo;
  std::unordered_map<std::string, std::string> _symbolsTable;

  // filename to output mapping of line numbers to symbols
  std::string _debugFilename;
  std::ostringstream _debugStream;

  // memory address for variables are in [start,end)
  static constexpr int VARIABLE_START = 16;
  static constexpr int VARIABLE_END = 256;
  // this is the maximum number we can load in
  // an A-Instruction since we only have 15 out of 16 bits
  // for the value.
  static constexpr int MAX_INT = (1 << 15) - 1;
};

class HackBinaryTranslator: public HackBuilder {
public:
  HackBinaryTranslator();
  HackBinaryTranslator(const std::string&);
  virtual void visit(Label *i) override;
  virtual void visit(CInstruction *i) override;
  virtual void visit(AInstruction *i) override;
};

#endif
