#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

#include "../../utils.h"

#include "builder.h"
#include "grammar.h"
#include "tokenizer.h"

// JackBuilder

JackBuilder::JackBuilder() { }

JackBuilder::~JackBuilder() { }

// JackTokenizerBuilder

JackTokenizerBuilder::JackTokenizerBuilder() : JackBuilder() { }

void JackTokenizerBuilder::build(const std::string &inputFile) {
  std::string outputFile = replaceExtension(inputFile, "T.xml");
  std::cout << "Extracting tokens from " << inputFile << " into " << outputFile << '\n';

  std::ifstream in(inputFile);
  JackTokenizer tokenizer(in);

  std::ofstream out(outputFile);
  out << "<tokens>\n";
  while (tokenizer.hasMore()) {
    const Token token = tokenizer.getCurrentToken();
    out << token.toXML() << "\n";
    tokenizer.advance();
  }
  out << "</tokens>\n";
}

// JackCompilationEngineBuilder

JackCompilationEngineBuilder::JackCompilationEngineBuilder(): JackBuilder() { }

void JackCompilationEngineBuilder::build(const std::string &inputFile) {
  std::ifstream in(inputFile);
  JackTokenizer tokenizer(in);
  ClassElement classElement = buildClass(tokenizer);

  // output parsed tree to xml format.
  std::string outputFile = replaceExtension(inputFile, "xml");
  std::cout << "Extracting parsed grammar from " << inputFile << " into " << outputFile << '\n';
  std::ofstream out(outputFile);
  out << classElement.toXML();
}

/* class <className> { <classVarDec*> <subroutineDec*> } */
ClassElement JackCompilationEngineBuilder::buildClass(JackTokenizer &t) {
  eat(t, "class");
  Token className = eat(t, TokenType::IDENTIFIER);
  eat(t, "{");
  std::vector<ClassVarDec> classVarDecs = buildClassVarDecs(t);
  std::vector<SubroutineDec> subroutineDecs = buildSubroutineDecs(t);
  eat(t, "}");
  return ClassElement(className.value(), classVarDecs, subroutineDecs);
}

/* (static|field) <type> <varName> (, <varName>)* ; */
std::vector<ClassVarDec> JackCompilationEngineBuilder::buildClassVarDecs(JackTokenizer &t) {
  std::vector<ClassVarDec> classVarDecs;

  while (t.hasMore() && in_array(t.getCurrentToken().value(), {"static", "field"})) {
    // static|field
    Token kind = eat(
      t,
      [](Token tok) { return in_array(tok.value(), {"static", "field"}); }
    );

    // type = int|char|boolean|className
    Token type = eat(t, [](Token tok) { return tok.isAType(); });

    // <varName> (, <varName>)*
    std::vector<Token> varNames;
    Token var = eat(t, TokenType::IDENTIFIER);  // varName
    varNames.push_back(var);
    while (t.hasMore() && t.getCurrentToken().value() == ",") {
      eat(t, ",");
      var = eat(t, TokenType::IDENTIFIER);  // varName
      varNames.push_back(var);
    }

    eat(t, ";");
    classVarDecs.push_back(ClassVarDec(kind, type, varNames));
  }

  return classVarDecs;
}

/* (constructor|function|method) (void|<type>) <subroutineName> (<parameterList>) <subroutineBody> */
std::vector<SubroutineDec> JackCompilationEngineBuilder::buildSubroutineDecs(JackTokenizer &t) {
  std::vector<SubroutineDec> subroutineDecs;

  while (t.hasMore() &&
         in_array(t.getCurrentToken().value(), {"constructor", "function", "method"})) {
    // constructor|function|method
    Token kind = eat(t, [](Token tok) {
      return in_array(tok.value(), {"constructor", "function", "method"});
    });
    // void|<type>
    Token _return = eat(t, [](Token tok) {
      return (tok.value() == "void" || tok.isAType());
    });

    // <subroutineName>
    Token name = eat(t, [](Token tok) { return tok.isIdentifier(); });
    eat(t, "(");
    ParameterList parameters = buildParameterList(t);
    eat(t, ")");
    SubroutineBody body = buildSubroutineBody(t);

    subroutineDecs.push_back(
      SubroutineDec(kind, _return, name, parameters, body)
    );
  }

  return subroutineDecs;
}

/* ( (<type> <varName>) (, <type> <varName>)* )? */
ParameterList JackCompilationEngineBuilder::buildParameterList(JackTokenizer &t) {
  std::vector<std::pair<Token, Token>> parameters;

  t.hasMore();
  if (t.hasMore() && t.getCurrentToken().isAType()) {
    Token type = eat(t, [](Token tok) { return tok.isAType(); });
    Token varName = eat(t, TokenType::IDENTIFIER);

    parameters.push_back(std::make_pair(type, varName));

    while (t.hasMore() && t.getCurrentToken().value() == ",") {
      eat(t, ",");
      type = eat(t, [](Token tok) { return tok.isAType(); });
      varName = eat(t, TokenType::IDENTIFIER);
      parameters.push_back(std::make_pair(type, varName));
    }
  }

  return ParameterList(parameters);
}

/* '{' <varDec>* <statement>*) '}' */
SubroutineBody JackCompilationEngineBuilder::buildSubroutineBody(JackTokenizer &t) {
  std::vector<VarDec> varDecs;
  std::vector<Statement> statements;

  eat(t, "{");

  while (t.hasMore() && t.getCurrentToken().value() == "var") {
    varDecs.push_back(buildVarDec(t));
  }

  // TODO
  //while (t.hasMore() &&
  //       in_array(t.getCurrentToken().value(), {"let", "if", "while", "do", "return"})) {
  //  
  //}

  eat(t, "}");

  return SubroutineBody(varDecs, statements);
}

/* 'var' <type> <varName> (, <varName>)* ; */
VarDec JackCompilationEngineBuilder::buildVarDec(JackTokenizer &t) {
  eat(t, "var");
  Token type = eat(t, [](Token tok) { return tok.isAType(); });
  std::vector<Token> varNames;
  varNames.push_back(eat(t, [](Token tok) { return tok.isIdentifier(); }));
  while (t.hasMore() && t.getCurrentToken().value() == ",") {
    eat(t, ",");
    varNames.push_back(
      eat(t, [](Token tok) { return tok.isIdentifier(); })
    );
  }
  eat(t, ";");

  return VarDec(type, varNames);
}

Token JackCompilationEngineBuilder::eat(JackTokenizer &t, std::function<bool(Token)> isValidFunc) {
  _assert(t.hasMore(), "Tokenizer hasn't got any more tokens to give");

  Token tok = t.getCurrentToken();
  std::cout << "eating token " << tok.value() << "\n";
  if (isValidFunc(tok)) {
    t.advance();
  } else {
    std::string msg = "Token " + tok.value() + " isn't valid.";
    throw_and_debug(msg);
  }
  return tok;
}

Token JackCompilationEngineBuilder::eat(JackTokenizer &t, const std::string &tokenStr) {
  return eat(
    t,
    [&tokenStr](Token tok) { return tok.value() == tokenStr; }
  );
}

Token JackCompilationEngineBuilder::eat(JackTokenizer &t, TokenType tokenType) {
  return eat(
    t,
    [&tokenType](Token tok) { return tok.getType() == tokenType; }
  );
}

void JackCompilationEngineBuilder::_assert(bool truth, const std::string &msg) {
  if (!truth)
    throw_and_debug(msg);
}
