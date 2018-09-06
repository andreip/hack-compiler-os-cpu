#ifndef __JACK__GRAMMAR__H__
#define __JACK__GRAMMAR__H__

#include <string>
#include <vector>

// base class for all grammar elements from jack
class GrammarElement {
public:
  virtual std::string toXML() const = 0;
  std::string getType() const;
  std::string wrapXMLWithType(const std::string &xml) const;
protected:
  GrammarElement(std::string type);
private:
  std::string _type;
};

/* (static|field) <type> <varName> (, <varName>)* ; */
class ClassVarDec: public GrammarElement {
public:
  ClassVarDec(std::string kind, std::string type, std::vector<std::string> varNames);
  virtual std::string toXML() const override;
private:
  std::string _kind;  // static|field
  std::string _type;
  std::vector<std::string> _varNames;
};

class SubroutineDec: public GrammarElement {
public:
  SubroutineDec();
  virtual std::string toXML() const override;
};

/* class <className> { <classVarDec*> <subroutineDec*> } */
class ClassElement: public GrammarElement {
public:
  ClassElement(std::string, std::vector<ClassVarDec>, std::vector<SubroutineDec>);
  virtual std::string toXML() const override;
private:
  std::string className;
  std::vector<ClassVarDec> classVarDecs;
  std::vector<SubroutineDec> subroutineDecs;
};

#endif
