#ifndef __BUILDER__H__
#define __BUILDER__H__

#include <list>
#include <string>

class Instruction;

class Builder {
public:
  void init(const std::list<std::string> *lines);
  std::list<std::string> *getResult();
  void visitInstruction(const Instruction*) const;

  virtual ~Builder();
protected:
  Builder();  // abstract
  virtual Instruction* parseLine(const std::string&) = 0;
private:
  virtual void visit(const std::list<std::string>*);

protected:
  std::list<std::string> *output;
private:
  const std::list<std::string> *_lines;
};

#endif
