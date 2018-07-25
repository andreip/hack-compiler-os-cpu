#ifndef __BUILDER__H__
#define __BUILDER__H__

#include <list>
#include <string>

class Instruction;

class Builder {
public:
  void init(const std::list<std::string> *lines);
  std::list<std::string> *computeResult();

  void visitInstruction(const Instruction*) const;
protected:
  Builder();  // abstract
  std::list<std::string>* computeOnce(const std::list<std::string>*);
  virtual Instruction* parseLine(const std::string&) = 0;

private:
  const std::list<std::string> *_lines;
};

#endif
