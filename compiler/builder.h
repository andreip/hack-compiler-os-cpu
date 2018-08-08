#ifndef __BUILDER__H__
#define __BUILDER__H__

#include <list>
#include <string>

class Instruction;

class Builder {
public:
  void setLines(const std::list<std::string> *lines);
  void setInputFile(const std::string &inputFile);
  void setCurrentFunction(const std::string &function);
  void reset();
  virtual std::list<std::string> *getResult();
  virtual void visit(Instruction*);
  std::string getFilename() const;
  std::string getCurrentFunction() const;

  virtual ~Builder();
protected:
  Builder();
  Builder(const std::string&);  // abstract
  virtual Instruction* parseLine(const std::string&) = 0;
  virtual void processLines(const std::list<std::string>*);

protected:
  std::list<std::string> *output;
private:
  const std::list<std::string> *_lines;
  std::string _filename;  // crt file we're into
  std::string _function;  // crt function we're into
};

#endif
