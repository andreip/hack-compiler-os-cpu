#ifndef __JACK__GRAMMAR__H__
#define __JACK__GRAMMAR__H__

#include <string>
#include <utility>
#include <vector>

#include "tokenizer.h"

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
  ClassVarDec(Token kind, Token type, std::vector<Token> varNames);
  virtual std::string toXML() const override;
private:
  Token _kind;  // static|field
  Token _type;
  std::vector<Token> _varNames;
};

// TODO
struct VarDec {};
struct Statement {};

/* '{' <varDec>* <statement>*) '}' */
class SubroutineBody : public GrammarElement {
public:
  SubroutineBody(std::vector<VarDec> varDecs,
                 std::vector<Statement> statements);
  virtual std::string toXML() const override;
private:
  std::vector<VarDec> _varDecs;
  std::vector<Statement> _statements;
};

/* ( (<type> <varName>) (, <type> <varName>)* )? */
class ParameterList : public GrammarElement {
public:
  ParameterList(std::vector<std::pair<Token, Token>>);
  virtual std::string toXML() const override;
private:
  // pairs of (type, varName)
  std::vector<std::pair<Token, Token>> _parameters;
};

/* (constructor|function|method) (void|<type>) <subroutineName> (<parameterList>) <subroutineBody> */
class SubroutineDec: public GrammarElement {
public:
  SubroutineDec(Token kind, Token _return, Token name,
                ParameterList parameters,
                SubroutineBody body);
  virtual std::string toXML() const override;
private:
  Token _kind;
  Token _return;
  Token _subroutineName;
  ParameterList _parameters;
  SubroutineBody _body;
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
