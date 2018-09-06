#include <cstdio>
#include <string>
#include <sstream>
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

// SubroutineBody

SubroutineBody::SubroutineBody(std::vector<VarDec> varDecs,
                               std::vector<Statement> statements)
  : GrammarElement("subroutineBody"), _varDecs(varDecs),
    _statements(statements) { }

std::string SubroutineBody::toXML() const {
  std::ostringstream out;
  out << "<symbol>{</symbol>\n";
  out << "<symbol>}</symbol>\n";
  return wrapXMLWithType(out.str());
}

// SubroutineDec

SubroutineDec::SubroutineDec(
    Token kind, Token _return, Token name,
    std::vector<Parameter> parameters,
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
  //for (const Parameter &p : _parameters)
  //  out << p.toXML();
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
