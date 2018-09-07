#ifndef __JACK__GRAMMAR__H__
#define __JACK__GRAMMAR__H__

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "tokenizer.h"

enum class Op {
  ADD, SUB, MUL, DIV, AND, OR, LT, GT, EQ,
  NONE
};
enum class UnaryOp {
  NEG, // -
  NOT, // ~
  NONE,
};
extern std::unordered_map<Op, std::string> opStr;
extern std::unordered_map<std::string, Op> strOp;
extern std::unordered_map<UnaryOp, std::string> unaryOpStr;
extern std::unordered_map<std::string, UnaryOp> strUnaryOp;

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

// forward declaration for expression list.
class Expression;
class ExpressionList : public GrammarElement {
public:
  ExpressionList();
  ExpressionList(std::vector<Expression> expressions);
  virtual std::string toXML() const override;
  operator bool() const;
private:
  std::vector<Expression> _expressions;
};

class SubroutineCall: public GrammarElement {
public:
  SubroutineCall();
  SubroutineCall(Token subroutineName, Token classOrVarName, ExpressionList);
  virtual std::string toXML() const override;
  operator bool() const;
private:
  Token _subroutineName;
  Token _classOrVarName;  // <classOrVarName>.<subroutineName>
  ExpressionList _expressionList;
};

// Expression must come before Term, since Term uses an expression
// directly inside it, while Expression uses pointers of Terms in
// a vector, so that's ok, Expression works only w/ a forward
// declaration of Term.
class Term;
/* term (op term)* */
class Expression : public GrammarElement {
public:
  Expression();
  Expression(std::vector<Term> terms, std::vector<Op> ops);
  virtual std::string toXML() const override;
  std::string opToXML(Op op) const;
  bool operator!() const;
  operator bool() const;
private:
  std::vector<Term> _terms;
  std::vector<Op> _ops;
};

/* Term = intConstant | stringConstant | keywordConstant |
 *        varName | varName[expression] | subroutineCall |
 *        (expression) | unaryOp Term
 */
class Term : public GrammarElement {
public:
  // int/str/keyword constant or varname and unary operation
  Term(Token type, UnaryOp op=UnaryOp::NONE);
  // subroutine call and unary op
  Term(SubroutineCall, UnaryOp op=UnaryOp::NONE);
  // varname[expr] and unary op
  Term(Token type, Expression, UnaryOp op=UnaryOp::NONE);
  // (expr) and unary operation
  Term(Expression expression, UnaryOp op=UnaryOp::NONE);
  virtual std::string toXML() const override;
  std::string unaryOpToXML(UnaryOp op) const;
private:
  // if integer/string constant, or
  // keyword constant (this,true,false,null) or
  // varName if identifier type
  Token _type;
  UnaryOp _unaryOp;  // if NONE, it's not set
  Expression _expression;
  SubroutineCall _subroutineCall;
};

/* statement = letStatement | ifStatement | whileStatement |
 *             doStatement | returnStatement
 *
 * letStatement = let <varName>([<expression>])? = expression ;
 */
class Statement : public GrammarElement {
public:
  Statement(Token type, Token varName, std::vector<Expression>);  // let
  Statement(Token type, std::vector<Expression>, std::vector<Statement>, std::vector<Statement>);  // if/while/return
  Statement(Token type, SubroutineCall);  // do
  virtual std::string toXML() const override;
private:
  Token _type;  // let/if/while/do/return
  Token _varName;  // for let
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
