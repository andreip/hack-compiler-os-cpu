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

ClassVarDec::ClassVarDec(std::string kind,
                         std::string type,
                         std::vector<std::string> varNames)
  : GrammarElement("classVarDec"), _kind(kind), _type(type), _varNames(varNames) { }

std::string ClassVarDec::toXML() const {
  if (_varNames.empty())
    throw_and_debug("Class var declaration cannot be w/ empty variables.");

  std::ostringstream out;
  out << "<keyword>" << _kind << "</keyword>\n";
  out << "<keyword>" << _type << "</keyword>\n";
  out << "<identifier>" << _varNames[0] << "</identifier>\n";
  for (int i = 1; i < _varNames.size(); ++i) {
    out << "<symbol>,</symbol>\n";
    out << "<identifier>" << _varNames[i] << "</identifier>\n";
  }
  out << "<symbol>;</symbol>\n";
  return wrapXMLWithType(out.str());
}

// SubroutineDec

SubroutineDec::SubroutineDec(): GrammarElement("subroutineDec") { }

std::string SubroutineDec::toXML() const {
  std::ostringstream out;
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
