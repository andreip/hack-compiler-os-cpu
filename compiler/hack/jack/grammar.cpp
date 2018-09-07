#include <cstdio>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

#include "../../utils.h"

#include "grammar.h"

std::unordered_map<Op, std::string> opStr {
  {Op::ADD, "+"}, {Op::SUB, "-"}, {Op::MUL, "*"}, {Op::DIV, "/"},
  {Op::AND, "&"}, {Op::OR, "|"}, {Op::LT, "<"}, {Op::GT, ">"}, {Op::EQ, "="},
};
std::unordered_map<std::string, Op> strOp {
  {"+", Op::ADD}, {"-", Op::SUB}, {"*", Op::MUL}, {"/", Op::DIV},
  {"&", Op::AND}, {"|", Op::OR}, {"<", Op::LT}, {">", Op::GT}, {"=", Op::EQ},
};
std::unordered_map<UnaryOp, std::string> unaryOpStr {
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

// Term

// int/str/keyword constant or varname and unary operation
Term::Term(Token type, UnaryOp op)
  : GrammarElement("term"), _type(type), _unaryOp(op) { }

// subroutine call and unary op
Term::Term(SubroutineCall subroutineCall, UnaryOp op)
  : GrammarElement("term"), _subroutineCall(subroutineCall), _unaryOp(op) { }

// varname[expr] and unary op
Term::Term(Token type, Expression expression, UnaryOp op)
  : GrammarElement("term"),
    _type(type), _expression(expression), _unaryOp(op) { }

// (expr) and unary operation
Term::Term(Expression expression, UnaryOp op)
  : GrammarElement("term"),
    _expression(expression), _unaryOp(op) { }

std::string Term::toXML() const {
  std::ostringstream out;

  std::cout << "outputting xml for term of type " << _type.value() << '\n';

  if (_unaryOp != UnaryOp::NONE)
    out << unaryOpToXML(_unaryOp) << '\n';

  if (_type) {
    // intConstant | stringConstant | keywordConstant | varName
    out << _type.toXML() << '\n';

    // varName[expression]
    if (_type.isIdentifier() && _expression) {
      out << "<symbol>[</symbol>\n";
      out << _expression.toXML() << '\n';
      out << "<symbol>]</symbol>\n";
    } else if (_expression) {
      throw_and_debug("Cannot have an expression w/o a varname[].");
    }
  // ( expression )
  } else if (_expression) {
    out << "<symbol>(</symbol>\n";
    out << _expression.toXML();
    out << "<symbol>)</symbol>\n";
  // might be subroutine call
  } else {
    // TODO:
  }
  return wrapXMLWithType(out.str());
}

std::string Term::unaryOpToXML(UnaryOp op) const {
  return "<symbol>" + unaryOpStr[op] + "</symbol>";
}

// Expression

Expression::Expression()
  : GrammarElement("expression"), _terms({}), _ops({}) { }

Expression::Expression(std::vector<Term> terms, std::vector<Op> ops)
  : GrammarElement("expression"), _terms(terms), _ops(ops) { }

bool Expression::operator!() const { return _terms.empty(); }
Expression::operator bool() const { return !(!*this); }

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
    return "<symbol>" + opStr[op] + "</symbol>";
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
    // TODO enable this
    //out << _subroutineCall.toXML();
    out << "<symbol>;</symbol>\n";
  } else if (_type.value() == "return") {
    if (!_expressions.empty())
      out << _expressions[0];
    out << "<symbol>;</symbol>\n";
  } else {
    throw_and_debug("Unknown statement type " + _type.value());
  }

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
