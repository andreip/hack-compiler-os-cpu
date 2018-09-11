#include <algorithm>
#include <cstdio>
#include <iterator>
#include <string>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../../utils.h"

#include "grammar.h"
#include "symbol_table.h"

std::unordered_map<Op, std::string, EnumClassHash> opStr {
  {Op::ADD, "+"}, {Op::SUB, "-"}, {Op::MUL, "*"}, {Op::DIV, "/"},
  {Op::AND, "&"}, {Op::OR, "|"}, {Op::LT, "<"}, {Op::GT, ">"}, {Op::EQ, "="},
};
std::unordered_map<std::string, Op> strOp {
  {"+", Op::ADD}, {"-", Op::SUB}, {"*", Op::MUL}, {"/", Op::DIV},
  {"&", Op::AND}, {"|", Op::OR}, {"<", Op::LT}, {">", Op::GT}, {"=", Op::EQ},
};
std::unordered_map<UnaryOp, std::string, EnumClassHash> unaryOpStr {
  {UnaryOp::NEG, "-"},
  {UnaryOp::NOT, "~"},
};
std::unordered_map<std::string, UnaryOp> strUnaryOp {
  {"-", UnaryOp::NEG},
  {"~", UnaryOp::NOT},
};

// GrammarElement

GrammarElement::GrammarElement(std::string type): _type(type) { }

std::string GrammarElement::getType() const { return _type; }

std::string GrammarElement::wrapXMLWithType(const std::string &xml) const {
  std::ostringstream out;
  out << "<" << getType() << ">\n";
  out << xml;
  out << "</" << getType() << ">\n";
  return out.str();
}

// ClassVarDec

ClassVarDec::ClassVarDec(Token kind,
                         Token type,
                         std::vector<Token> varNames)
  : GrammarElement("classVarDec"), _kind(kind), _type(type), _varNames(varNames) { }

std::string ClassVarDec::getKind() const { return _kind.value(); }
std::string ClassVarDec::getType() const { return _type.value(); }
std::vector<std::string> ClassVarDec::getVarNames() const {
  std::vector<std::string> out;
  std::transform(
    _varNames.begin(), _varNames.end(), std::back_inserter(out),
    [](const Token &t) { return t.value(); }
  );
  return out;
}

std::vector<std::string> ClassVarDec::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string ClassVarDec::toXML() const {
  if (_varNames.empty())
    throw_and_debug("Class var declaration cannot be w/ empty variables.");

  std::ostringstream out;
  out << _kind.toXML() << '\n';
  out << _type.toXML() << '\n';
  out << _varNames[0].toXML() << '\n';
  for (int i = 1; i < _varNames.size(); ++i) {
    out << "<symbol>,</symbol>\n";
    out << _varNames[i].toXML() << '\n';
  }
  out << "<symbol>;</symbol>\n";
  return wrapXMLWithType(out.str());
}

// ExpressionList

ExpressionList::ExpressionList() : GrammarElement("expressionList") { }

ExpressionList::ExpressionList(std::vector<Expression> expressions)
  : GrammarElement("expressionList"), _expressions(expressions) { }

std::vector<std::string> ExpressionList::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string ExpressionList::toXML() const {
  std::ostringstream out;
  if (!_expressions.empty()) {
    out << _expressions[0].toXML();
    for (int i = 1; i < _expressions.size(); ++i) {
      out << "<symbol>,</symbol>\n";
      out << _expressions[i].toXML();
    }
  }
  return wrapXMLWithType(out.str());
}

ExpressionList::operator bool() const { return !_expressions.empty(); }

// SubroutineCall

SubroutineCall::SubroutineCall(): GrammarElement("subroutineCall") { }

SubroutineCall::SubroutineCall(Token subroutineName, Token classOrVarName, ExpressionList expressionList)
  : GrammarElement("subroutineCall"),
    _subroutineName(subroutineName), _classOrVarName(classOrVarName),
    _expressionList(expressionList) { }

std::vector<std::string> SubroutineCall::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string SubroutineCall::toXML() const {
  std::ostringstream out;
  if (_classOrVarName) {
    out << _classOrVarName.toXML() << '\n';
    out << "<symbol>.</symbol>\n";
  }
  out << _subroutineName.toXML() << '\n';
  out << "<symbol>(</symbol>\n";
  out << _expressionList.toXML();
  out << "<symbol>)</symbol>\n";
  // no wrap, don't want <subroutineCall> element.
  return out.str();
}

SubroutineCall::operator bool() const { return _subroutineName; }

// Term

// empty term, usually not called directly.
Term::Term()
  : GrammarElement("term"),
    _unaryOp(UnaryOp::NONE), _term(nullptr) { }

// int/str/keyword constant or varname
Term::Term(Token type)
  : GrammarElement("term"),
    _type(type), _unaryOp(UnaryOp::NONE), _term(nullptr) { }

// varname[expr]
Term::Term(Token type, Expression expression)
  : GrammarElement("term"),
    _type(type), _expression(expression), _unaryOp(UnaryOp::NONE),
    _term(nullptr) { }

// subroutine call
Term::Term(SubroutineCall subroutineCall)
  : GrammarElement("term"), _subroutineCall(subroutineCall), _unaryOp(UnaryOp::NONE), _term(nullptr) { }

// (expr)
Term::Term(Expression expression)
  : GrammarElement("term"),
    _expression(expression), _unaryOp(UnaryOp::NONE), _term(nullptr) { }

// unaryOp <term>
Term::Term(const Term &term, UnaryOp op)
  : GrammarElement("term"), _unaryOp(op) {
  _term = new Term(term);
}

Term::~Term() {
  // TODO: can't do this :( because we pass around a term as
  // values on stack and the destructor gets called over and
  // over again, and the _term pointer gets copied over to
  // other temporary values on stack, so the destructor would
  // get called several times. To do this properly we'd need
  // to use move semantics and move the _term over to the new
  // Term instead.. So this is a small mem leak as is.
  // delete _term;
}

Term::operator bool() const {
  return (
    _unaryOp != UnaryOp::NONE ||
    _type ||
    _expression ||
    _subroutineCall ||
    _term
  );
}

std::vector<std::string> Term::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string Term::toXML() const {
  std::ostringstream out;

  // <unaryOp> <term>
  if (_unaryOp != UnaryOp::NONE && _term) {
    out << unaryOpToXML(_unaryOp) << '\n';
    out << _term->toXML();
  // constants | varName | varName[expression]
  } else if (_type) {
    // intConstant | stringConstant | keywordConstant | varName
    out << _type.toXML() << '\n';

    // varName[expression]
    if (_type.isIdentifier() && _expression) {
      out << "<symbol>[</symbol>\n";
      out << _expression.toXML();
      out << "<symbol>]</symbol>\n";
    } else if (_expression) {
      throw_and_debug("Cannot have an expression w/o a varname[].");
    }
  // ( expression )
  } else if (_expression) {
    out << "<symbol>(</symbol>\n";
    out << _expression.toXML();
    out << "<symbol>)</symbol>\n";
  // <subroutineCall> is the only remaining one
  } else {
    out << _subroutineCall.toXML();
  }

  return wrapXMLWithType(out.str());
}

std::string Term::unaryOpToXML(UnaryOp op) const {
  // this also escapes the value.
  return Token::fromString(unaryOpStr[op]).toXML();
}

// Expression

Expression::Expression()
  : GrammarElement("expression"), _terms({}), _ops({}) { }

Expression::Expression(std::vector<Term> terms, std::vector<Op> ops)
  : GrammarElement("expression"), _terms(terms), _ops(ops) { }

bool Expression::operator!() const { return _terms.empty(); }
Expression::operator bool() const { return !(!*this); }

std::vector<std::string> Expression::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string Expression::toXML() const {
  std::ostringstream out;
  out << _terms[0].toXML();
  for (int i = 0; i < _ops.size(); ++i) {
    out << opToXML(_ops[i]) << '\n';
    out << _terms[i+1].toXML();
  }
  return wrapXMLWithType(out.str());
}

std::string Expression::opToXML(Op op) const {
    // this also escapes the value.
    return Token::fromString(opStr[op]).toXML();
}

// Statement

// let
Statement::Statement(Token type, Token varName, std::vector<Expression> expressions)
  : GrammarElement(type.value() + "Statement"), _type(type),
    _varName(varName), _expressions(expressions) { }

// if/while/return
Statement::Statement(Token type, std::vector<Expression> expressions,
                     std::vector<Statement> statements1,
                     std::vector<Statement> statements2)
  : GrammarElement(type.value() + "Statement"), _type(type),
    _expressions(expressions), _statements1(statements1),
    _statements2(statements2) { }

// do
Statement::Statement(Token type, SubroutineCall subroutineCall)
  : GrammarElement(type.value() + "Statement"), _type(type),
    _subroutineCall(subroutineCall) { }

std::vector<std::string> Statement::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string Statement::toXML() const {
  std::ostringstream out;
  out << _type.toXML() << '\n';

  // let case 
  if (_type.value() == "let") {
    out << _varName.toXML() << '\n';
    int i = 0;
    if (_expressions.size() == 2) {
      out << "<symbol>[</symbol>\n";
      out << _expressions[i++].toXML();
      out << "<symbol>]</symbol>\n";
    }
    out << "<symbol>=</symbol>\n";
    out << _expressions[i].toXML();
    out << "<symbol>;</symbol>\n";
  } else if (_type.value() == "if" || _type.value() == "while") {
    out << "<symbol>(</symbol>\n";
    out << _expressions[0].toXML();
    out << "<symbol>)</symbol>\n";
    out << "<symbol>{</symbol>\n";
    out << "<statements>\n";
    for (const Statement &s : _statements1)
      out << s.toXML();
    out << "</statements>\n";
    out << "<symbol>}</symbol>\n";
    // else case
    if (_type.value() == "if" && !_statements2.empty()) {
      out << "<keyword>else</keyword>\n";
      out << "<symbol>{</symbol>\n";
      out << "<statements>\n";
      for (const Statement &s : _statements2)
        out << s.toXML();
      out << "</statements>\n";
      out << "<symbol>}</symbol>\n";
    }
  } else if (_type.value() == "do") {
    out << _subroutineCall.toXML();
    out << "<symbol>;</symbol>\n";
  } else if (_type.value() == "return") {
    if (!_expressions.empty())
      out << _expressions[0].toXML();
    out << "<symbol>;</symbol>\n";
  } else {
    throw_and_debug("Unknown statement type " + _type.value());
  }

  return wrapXMLWithType(out.str());
}

// VarDec

VarDec::VarDec(Token type, std::vector<Token> varNames)
  : GrammarElement("varDec"), _type(type), _varNames(varNames) { }

std::string VarDec::getType() const { return _type.value(); }

std::vector<std::string> VarDec::getNames() const {
  std::vector<std::string> out;
  std::transform(
    _varNames.begin(), _varNames.end(), std::back_inserter(out),
    [](const Token &var) { return var.value(); }
  );
  return out;
}

std::vector<std::string> VarDec::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string VarDec::toXML() const {
  std::ostringstream out;
  out << "<keyword>var</keyword>\n";
  out << _type.toXML() << '\n';
  out << _varNames[0].toXML() << '\n';
  for (int i = 1; i < _varNames.size(); ++i) {
    out << "<symbol>,</symbol>\n";
    out << _varNames[i].toXML() << '\n';
  }
  out << "<symbol>;</symbol>\n";
  return wrapXMLWithType(out.str());
}

// SubroutineBody

SubroutineBody::SubroutineBody(std::vector<VarDec> varDecs,
                               std::vector<Statement> statements)
  : GrammarElement("subroutineBody"), _varDecs(varDecs),
    _statements(statements) { }

std::vector<VarDec> SubroutineBody::getVarDecs() const {
  return _varDecs;
}

std::vector<std::string> SubroutineBody::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string SubroutineBody::toXML() const {
  std::ostringstream out;
  out << "<symbol>{</symbol>\n";
  for (const VarDec &var : _varDecs)
    out << var.toXML();
  out << "<statements>\n";
  for (const Statement &s : _statements)
    out << s.toXML();
  out << "</statements>\n";
  out << "<symbol>}</symbol>\n";
  return wrapXMLWithType(out.str());
}

// ParameterList

ParameterList::ParameterList(std::vector<std::pair<Token, Token>> parameters)
  : GrammarElement("parameterList"), _parameters(parameters) { }

std::vector<std::pair<std::string, std::string>> ParameterList::getArgs() const {
  std::vector<std::pair<std::string, std::string>> out;
  std::transform(
    _parameters.begin(), _parameters.end(), std::back_inserter(out),
    [](const auto &pair) { return std::make_pair(pair.first.value(),
                                                 pair.second.value()); }
  );
  return out;
}

std::vector<std::string> ParameterList::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string ParameterList::toXML() const {
  std::ostringstream out;
  if (!_parameters.empty()) {
    auto p = _parameters[0];
    out << p.first.toXML() << '\n';
    out << p.second.toXML() << '\n';
  }
  for (int i = 1; i < _parameters.size(); ++i) {
    auto p = _parameters[i];
    out << "<symbol>,</symbol>\n";
    out << p.first.toXML() << '\n';
    out << p.second.toXML() << '\n';
  }
  return wrapXMLWithType(out.str());
}

// SubroutineDec

SubroutineDec::SubroutineDec(
    Token kind, Token _return, Token name,
    ParameterList parameters,
    SubroutineBody body,
    std::string className)
  : GrammarElement("subroutineDec"),
    _kind(kind), _return(_return), _subroutineName(name),
    _parameters(parameters), _body(body), _className(className) { }

std::string SubroutineDec::getName() const { return _subroutineName.value(); }

std::string SubroutineDec::getKind() const { return _kind.value(); }

std::string SubroutineDec::getClassName() const { return _className; }

ParameterList SubroutineDec::getParameterList() const { return _parameters; }

SubroutineBody SubroutineDec::getBody() const { return _body; }

std::vector<std::string> SubroutineDec::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  return code;
}

std::string SubroutineDec::toXML() const {
  std::ostringstream out;
  out << _kind.toXML() << '\n';
  out << _return.toXML() << '\n';
  out << _subroutineName.toXML() << '\n';
  out << "<symbol>(</symbol>\n";
  out << _parameters.toXML();
  out << "<symbol>)</symbol>\n";
  out << _body.toXML();
  return wrapXMLWithType(out.str());
}

// ClassElement

ClassElement::ClassElement(std::string className,
                           std::vector<ClassVarDec> classVarDecs,
                           std::vector<SubroutineDec> subroutineDecs)
  : GrammarElement("class"), className(className),
    classVarDecs(classVarDecs), subroutineDecs(subroutineDecs)
{
}

std::vector<SubroutineDec> ClassElement::getSubroutineDecs() const { return subroutineDecs; }

std::vector<ClassVarDec> ClassElement::getClassVarDecs() const { return classVarDecs; }

std::string ClassElement::getName() const { return className; }

std::vector<std::string> ClassElement::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;
  for (const SubroutineDec &subroutine: getSubroutineDecs()) {
    // fill table with current class & subroutine we're on.
    table.init(*this, subroutine);
    std::vector<std::string> subroutineCode = subroutine.toVMCode(table);
    code.insert(code.end(), subroutineCode.begin(), subroutineCode.end());
  }

  return code;
}

std::string ClassElement::toXML() const {
  std::ostringstream out;
  out << "<keyword>class</keyword>\n";
  out << "<identifier>" << className << "</identifier>\n";
  out << "<symbol>{</symbol>\n";
  for (const ClassVarDec &varDec : getClassVarDecs())
    out << varDec.toXML();
  for (const SubroutineDec &subDec : subroutineDecs)
    out << subDec.toXML();
  out << "<symbol>}</symbol>\n";
  return wrapXMLWithType(out.str());
}
