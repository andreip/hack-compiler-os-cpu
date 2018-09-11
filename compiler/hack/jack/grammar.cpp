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
#include "vm_commands.h"

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
  for (const Expression &e: _expressions)
    concat(code, { e.toVMCode(table) });
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

int ExpressionList::size() const {
  return _expressions.size();
}

// SubroutineCall

SubroutineCall::SubroutineCall(): GrammarElement("subroutineCall") { }

SubroutineCall::SubroutineCall(Token subroutineName, Token classOrVarName, ExpressionList expressionList)
  : GrammarElement("subroutineCall"),
    _subroutineName(subroutineName), _classOrVarName(classOrVarName),
    _expressionList(expressionList) { }

std::vector<std::string> SubroutineCall::toVMCode(SymbolTable &table) const {
  std::vector<std::string> code;

  int nArgs = _expressionList.size();
  std::string subroutineFullName;

  if (_classOrVarName) {
    Symbol s = table.get(_classOrVarName.value());
    // it's a class variable
    if (!s) {
      subroutineFullName = (
        _classOrVarName.value() + "." + _subroutineName.value()
      );
    // it's a method of an object
    } else {
      // push method's "this" on the stack as 1st arg.
      concat(code, { VMCommands::Push(s.segment(), s.index) });
      nArgs++;
      subroutineFullName = s.type + "." + _subroutineName.value();
    }
  // it's a this->subroutine() call, from inside another method.
  } else {
    if (table.getCurrentSubroutineKind() == "function")
      _subroutineName.raise("Cannot call method from function");

    // push "this" to the new method as 1st arg
    concat(code, { VMCommands::Push("pointer", 0) });
    nArgs++;
    subroutineFullName = (
      table.getCurrentClassName() + "." + _subroutineName.value()
    );
  }

  concat(code, {
    _expressionList.toVMCode(table),
    { VMCommands::Call(subroutineFullName, nArgs) },
  });

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

  if (_unaryOp != UnaryOp::NONE && _term) {
    concat(code, {
      _term->toVMCode(table),
      { VMCommands::ArithmeticLogic(_unaryOp) }
    });
  // constants | varName | varName[expression]
  } else if (_type) {
    // varName
    if (_type.isIdentifier()) {
      Symbol s = table.get(_type.value());
      if (!s)
        _type.raise("Variable not defined");

      concat(code, { VMCommands::Push(s.segment(), s.index) });

      // TODO arrays
      // varName[expression]
      if (_expression) { }
    } else if (_type.isIntConstant()) {
      concat(code, { VMCommands::Push("constant", _type.value()) });
    } else if (_type.isStringConstant()) {
      std::string strCt = _type.escapedValue();
      concat(code, {
        VMCommands::Push("constant", strCt.length()),
        VMCommands::Call("String.new", 1),
      });

      for (int  i = 0; i < strCt.length(); ++i) {
        std::cout << "ascii code of " << char(strCt[i]) << ": " << int(strCt[i]) << '\n';
        concat(code, {
          VMCommands::Push("constant", strCt[i]),
          VMCommands::Call("String.appendChar", 2),
        });
      }
    } else if (_type.isKeyword()) {
      if (_type.value() == "true") {
        concat(code, { VMCommands::Push("constant", -1) });
      } else if (in_array(_type.value(), {"false", "null"})) {
        concat(code, { VMCommands::Push("constant", 0) });
      } else if (_type.value() == "this") {
        if (table.getCurrentSubroutineKind() == "function")
          _type.raise("Can't access this from function");
        concat(code, { VMCommands::Push("pointer", 0) });
      } else {
        throw_and_debug("Term::toVMCode: cannot use keyword " + _type.value() + " in an expression.");
      }
    } else {
      throw_and_debug("Term::toVMCode: invalid type " + _type.value());
    }
  } else if (_expression) {
    concat(code, _expression.toVMCode(table));
  } else if (_subroutineCall) {
    concat(code, _subroutineCall.toVMCode(table));
  } else {
    throw_and_debug("Term::toVMCode: Some error in if/else logic");
  }

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
  if (_terms.size() > 2)
    throw_and_debug("Expression has >2 terms in it? It should have been recursive and not have >2 terms in it.");

  std::vector<std::string> code;
  concat(code, _terms[0].toVMCode(table));
  if (_terms.size() == 2) {
    concat(code, {
      _terms[1].toVMCode(table),
      { VMCommands::ArithmeticLogic(_ops[0]) },
    });
  }
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

  std::vector<std::string> codeSt1;
  for (const Statement &s: _statements1)
    concat(codeSt1, s.toVMCode(table));
  std::vector<std::string> codeSt2;
  for (const Statement &s: _statements2)
    concat(codeSt2, s.toVMCode(table));

  if (_type.value() == "let") {
    const Symbol &s = table.get(_varName.value());
    if (!s)
      _varName.raise("Variable not defined");

    // TODO: also treat varname[x]
    std::vector<std::string> rhsCode = _expressions[0].toVMCode(table);

    concat(code, {
      std::move(rhsCode),
      { VMCommands::Pop(s.segment(), s.index) }
    });
  } else if (_type.value() == "if") {
    std::string L1 = VMCommands::UniqueLabel();
    std::string L2 = VMCommands::UniqueLabel();
    std::cout << "Generated unique numbers " << L1 << " " << L2 << '\n';

    concat(code, {
      _expressions[0].toVMCode(table),
      { VMCommands::ArithmeticLogic("not") },
      { VMCommands::IfGoto(L1) },
      std::move(codeSt1),
      { VMCommands::Goto(L2) },
      { VMCommands::Label(L1) },
      std::move(codeSt2),
      { VMCommands::Goto(L2) },
      { VMCommands::Label(L2) },
    });
  } else if (_type.value() == "while") {
    std::string L1 = VMCommands::UniqueLabel();
    std::string L2 = VMCommands::UniqueLabel();
    std::cout << "Generated unique numbers " << L1 << " " << L2 << '\n';

    concat(code, {
      { VMCommands::Label(L1) },
      _expressions[0].toVMCode(table),
      { VMCommands::ArithmeticLogic("not") },
      { VMCommands::IfGoto(L2) },
      std::move(codeSt1),
      { VMCommands::Goto(L1) },
      { VMCommands::Label(L2) },
    });
  } else if (_type.value() == "do") {
    concat(code, {
      _subroutineCall.toVMCode(table),
      { VMCommands::Pop("temp", "0") },  // pop unused return value
    });
  } else if (_type.value() == "return") {
    std::vector<std::string> returnValCode;
    if (!_expressions.empty())
      returnValCode = _expressions[0].toVMCode(table);
    else
      returnValCode = { VMCommands::Push("constant", "0") };

    concat(code, {
      std::move(returnValCode),
      { VMCommands::Return() },
    });
  } else {
    throw_and_debug("Unknown statement starting with " + _type.value());
  }

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
  // does nothing, symbol table was generated already in subroutine.
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
  for (const Statement &s: _statements)
    concat(code, s.toVMCode(table));
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
  // does nothing, symbol table was generated already in subroutine.
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
  std::string funcName = getClassName() + "." + getName();
  int nLocals = table.count(SymbolKind::VAR);
  std::vector<std::string> code = {
    VMCommands::Function(funcName, nLocals),
  };

  // have to setup THIS base ptr from 1st argument which points to "this"
  if (getKind() == "method") {
    Symbol s = table.get("this");
    concat(code, {
      VMCommands::Push(s.segment(), s.index),
      VMCommands::Pop("pointer", 0),
    });
  // have to allocate space for an object and then set THIS
  // base ptr to point to that newly allocated space.
  } else if (getKind() == "constructor") {
    // Allocate at least 1 in size if we've got a constructor,
    // to have some address to pass around.
    // https://www.coursera.org/learn/nand2tetris2/discussions/weeks/5/threads/qm11ULYCEeieGhKKx3bnrg
    int size = std::max(1, table.count(SymbolKind::FIELD));
    concat(code, {
      VMCommands::Push("constant", size),
      VMCommands::Call("Memory.alloc", 1),
      VMCommands::Pop("pointer", 0),
    });
  }

  return concat(code, getBody().toVMCode(table));
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
