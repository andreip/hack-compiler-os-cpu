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

struct Expression { };
struct SubroutineCall { };

/* statement = letStatement | ifStatement | whileStatement |
 *             doStatement | returnStatement
 *
 * letStatement = let <varName>([<expression>])? = expression ;
 */
class Statement : public GrammarElement {
public:
  Statement(std::string type, std::string varName, std::vector<Expression>);  // let
  Statement(std::string type, std::vector<Expression>, std::vector<Statement>, std::vector<Statement>);  // if/while/return
  Statement(std::string type, SubroutineCall);  // do
  virtual std::string toXML() const override;
private:
  std::string _type;  // let/if/while/do/return
  std::string _varName;  // for let
  std::vector<Expression> _expressions;
  std::vector<Statement> _statements1;
  std::vector<Statement> _statements2;
  SubroutineCall _subroutineCall;  // for do
};

/* 'var' <type> <varName> (, <varName>)* ; */
class VarDec : public GrammarElement {
public:
  VarDec(Token type, std::vector<Token> varNames);
  virtual std::string toXML() const override;
private:
  Token _type;
  std::vector<Token> _varNames;
};

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