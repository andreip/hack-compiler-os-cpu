#include <fstream>
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

void JackCompilationEngineBuilder::buildClass(JackTokenizer &t, std::ostream &out) {
  out << "<class>" << '\n';
  eat("class", t, out);

  Token className = eat("", t, out);
  _assert(className.getType() == TokenType::IDENTIFIER,
          "Invalid identifier name: " + className.value());

  eat("{", t, out);
  buildClassVarDec(t, out);
  buildSubroutineDec(t, out);
  eat("}", t, out);
  out << "</class>" << '\n';
}

void JackCompilationEngineBuilder::buildClassVarDec(JackTokenizer &t, std::ostream &out) {
}
void JackCompilationEngineBuilder::buildSubroutineDec(JackTokenizer &t, std::ostream &out) {
}

Token JackCompilationEngineBuilder::eat(const std::string &tokenStr, JackTokenizer &t, std::ostream &out) {
  _assert(t.hasMore(), "Tokenizer hasn't got any more tokens to give; expected token " + tokenStr);

  Token tok = t.getCurrentToken();
  // tokenStr == "" allows any value for the current token.
  if (tokenStr == "" || tokenStr == tok.value()) {
    t.advance();
    out << tok.toXML() << '\n';
  } else {
    std::string msg = "Expected to find " + tokenStr + "; actually found " + tok.value();
    throw_and_debug(msg);
  }
  return tok;
}

void JackCompilationEngineBuilder::_assert(bool truth, const std::string &msg) {
  if (!truth)
    throw_and_debug(msg);
}
