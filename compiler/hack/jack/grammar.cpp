#include <cstdio>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include "../../utils.h"

#include "grammar.h"

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

// Statement

// let
Statement::Statement(std::string type, std::string varName, std::vector<Expression> expressions)
  : GrammarElement(type + "Statement"), _type(type),
    _varName(varName), _expressions(expressions) { }

// if/while/return
Statement::Statement(std::string type, std::vector<Expression> expressions,
                     std::vector<Statement> statements1,
                     std::vector<Statement> statements2)
  : GrammarElement(type + "Statement"), _type(type),
    _expressions(expressions), _statements1(statements1),
    _statements2(statements2) { }

// do
Statement::Statement(std::string type, SubroutineCall subroutineCall)
  : GrammarElement(type + "Statement"), _type(type),
    _subroutineCall(subroutineCall) { }

std::string Statement::toXML() const {
  std::ostringstream out;
  return wrapXMLWithType(out.str());
}

// VarDec

VarDec::VarDec(Token type, std::vector<Token> varNames)
  : GrammarElement("varDec"), _type(type), _varNames(varNames) { }

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
    SubroutineBody body)
  : GrammarElement("subroutineDec"),
    _kind(kind), _return(_return), _subroutineName(name),
    _parameters(parameters), _body(body) { }

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

std::string ClassElement::toXML() const {
  std::ostringstream out;
  out << "<keyword>class</keyword>\n";
  out << "<identifier>" << className << "</identifier>\n";
  out << "<symbol>{</symbol>\n";
  for (const ClassVarDec &varDec : classVarDecs)
    out << varDec.toXML();
  for (const SubroutineDec &subDec : subroutineDecs)
    out << subDec.toXML();
  out << "<symbol>}</symbol>\n";
  return wrapXMLWithType(out.str());
}
