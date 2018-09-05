#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "../../utils.h"

#include "builder.h"
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
  std::string outputFile = replaceExtension(inputFile, "xml");
  std::cout << "Extracting parsed grammar from " << inputFile << " into " << outputFile << '\n';

  std::ifstream in(inputFile);

  JackTokenizer tokenizer(in);
  std::ofstream out(outputFile);
  build(tokenizer, out);
}

void JackCompilationEngineBuilder::build(JackTokenizer &t, std::ostream &out) {
  buildClass(t, out);
  out.flush();
}

/* class <className> { <classVarDec*> <subroutineDec*> } */
void JackCompilationEngineBuilder::buildClass(JackTokenizer &t, std::ostream &out) {
  out << "<class>" << '\n';
  eat(t, out, "class");
  eat(t, out, TokenType::IDENTIFIER);  // className
  eat(t, out, "{");
  buildClassVarDec(t, out);
  buildSubroutineDec(t, out);
  eat(t, out, "}");
  out << "</class>" << '\n';
}

/* (static|field) <type> <varName> (, <varName>)* ; */
void JackCompilationEngineBuilder::buildClassVarDec(JackTokenizer &t, std::ostream &out) {
  if (!t.hasMore())
    return;

  while (in_array(t.getCurrentToken().value(), {"static", "field"})) {
    out << "<classVarDec>" << '\n';
    // static|field
    eat(t, out, [](Token tok) { return in_array(tok.value(), {"static", "field"}); });
    // type = int|char|boolean|className
    eat(t, out, [](Token tok) {
      return (
        in_array(tok.value(), {"int", "char", "boolean"}) ||
        tok.getType() == TokenType::IDENTIFIER  // className type
      );
    });

    // <varName> (, <varName>)*
    eat(t, out, TokenType::IDENTIFIER);  // varName
    while (t.getCurrentToken().value() == ",") {
      eat(t, out, ",");
      eat(t, out, TokenType::IDENTIFIER);  // varName
    }

    eat(t, out, ";");
    out << "</classVarDec>" << '\n';
  }
}

void JackCompilationEngineBuilder::buildSubroutineDec(JackTokenizer &t, std::ostream &out) {
  if (!t.hasMore())
    return;
}

Token JackCompilationEngineBuilder::eat(JackTokenizer &t, std::ostream &out, std::function<bool(Token)> isValidFunc) {
  _assert(t.hasMore(), "Tokenizer hasn't got any more tokens to give");

  Token tok = t.getCurrentToken();
  std::cout << "eating token " << tok.value() << "\n";
  if (isValidFunc(tok)) {
    t.advance();
    out << tok.toXML() << '\n';
  } else {
    std::string msg = "Token " + tok.value() + " isn't valid.";
    throw_and_debug(msg);
  }
  return tok;
}

Token JackCompilationEngineBuilder::eat(JackTokenizer &t, std::ostream &out, const std::string &tokenStr) {
  return eat(
    t, out,
    [&tokenStr](Token tok) { return tok.value() == tokenStr; }
  );
}

Token JackCompilationEngineBuilder::eat(JackTokenizer &t, std::ostream &out, TokenType tokenType) {
  return eat(
    t, out,
    [&tokenType](Token tok) { return tok.getType() == tokenType; }
  );
}

void JackCompilationEngineBuilder::_assert(bool truth, const std::string &msg) {
  if (!truth)
    throw_and_debug(msg);
}
