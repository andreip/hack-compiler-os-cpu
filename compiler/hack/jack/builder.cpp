#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <utility>

#include "../../utils.h"

#include "builder.h"
#include "grammar.h"
#include "tokenizer.h"
#include "symbol_table.h"

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
  //std::string xmlOutputFile = replaceExtension(inputFile, "xml");
  //std::cout << "Extracting parsed grammar from " << inputFile << " into " << xmlOutputFile << '\n';
  //std::ofstream xmlOut(xmlOutputFile);
  //xmlOut << classElement.toXML();

  // output vm code
  std::string outputFile = replaceExtension(inputFile, "vm");
  std::cout << "Extracting parsed grammar from " << inputFile << " into " << outputFile << '\n';
  std::ofstream out(outputFile);
  SymbolTable symbolTable;
  out << join(classElement.toVMCode(symbolTable), "\n");
}

/* class <className> { <classVarDec*> <subroutineDec*> } */
ClassElement JackCompilationEngineBuilder::buildClass(JackTokenizer &t) {
  eat(t, "class");
  Token className = eat(t, TokenType::IDENTIFIER);
  eat(t, "{");
  std::vector<ClassVarDec> classVarDecs = buildClassVarDecs(t);
  std::vector<SubroutineDec> subroutineDecs = buildSubroutineDecs(t, className.value());
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
std::vector<SubroutineDec> JackCompilationEngineBuilder::buildSubroutineDecs(JackTokenizer &t, std::string className) {
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
      SubroutineDec(kind, _return, name, parameters, body, className)
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
  eat(t, "{");

  std::vector<VarDec> varDecs;
  while (t.hasMore() && t.getCurrentToken().value() == "var") {
    varDecs.push_back(buildVarDec(t));
  }
  std::vector<Statement> statements = buildStatements(t);

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

/* statement* */
/* statement = letStatement | ifStatement | whileStatement |
 *             doStatement | returnStatement
 *
 * letStatement = let <varName>([<expression>])? = expression ;
 */
std::vector<Statement> JackCompilationEngineBuilder::buildStatements(JackTokenizer &t) {
  std::vector<Statement> statements;

  while (t.hasMore() &&
         in_array(t.getCurrentToken().value(), {"let", "if", "while", "do", "return"})) {

    std::vector<Expression> expressions;
    std::vector<Statement> statements1;
    std::vector<Statement> statements2;
    Token type = eat(t, TokenType::KEYWORD);  // let/if/while/do/return

    if (type.value() == "let") {
      Token varName = eat(t, TokenType::IDENTIFIER);  // varName
      if (t.hasMore() && t.getCurrentToken().value() == "[") {
        eat(t, "[");
        expressions.push_back(buildExpression(t));
        eat(t, "]");
      }
      eat(t, "=");
      expressions.push_back(buildExpression(t));
      eat(t, ";");

      statements.push_back(
        Statement(type, varName, expressions)
      );
    } else if (type.value() == "if") {
      eat(t, "(");
      expressions.push_back(buildExpression(t));
      eat(t, ")");
      eat(t, "{");
      statements1 = buildStatements(t);
      eat(t, "}");
      if (t.hasMore() && t.getCurrentToken().value() == "else") {
        eat(t, "else");
        eat(t, "{");
        statements2 = buildStatements(t);
        eat(t, "}");
      }

      statements.push_back(
        Statement(type, expressions, statements1, statements2)
      );
    } else if (type.value() == "while") {
      eat(t, "(");
      expressions.push_back(buildExpression(t));
      eat(t, ")");
      eat(t, "{");
      statements1 = buildStatements(t);
      eat(t, "}");

      statements.push_back(
        Statement(type, expressions, statements1, statements2)
      );
    } else if (type.value() == "do") {
      statements.push_back(
        Statement(type, buildSubroutineCall(t))
      );
      eat(t, ";");
    } else if (type.value() == "return") {
      if (t.hasMore() && t.getCurrentToken().value() != ";")
        expressions.push_back(buildExpression(t));
      eat(t, ";");
      statements.push_back(
        Statement(type, expressions, {}, {})
      );
    } else {
      throw_and_debug("Unknown statement beginning with " + type.value());
    }
  }

  return statements;
}

Expression JackCompilationEngineBuilder::buildExpression(JackTokenizer &t) {
  std::vector<Term> terms;
  std::vector<Op> ops;
  terms.push_back(buildTerm(t));

  // as long as we find an operator, keep extracting terms.
  if (t.hasMore()) {
    auto found = strOp.find(t.getCurrentToken().value());
    while (t.hasMore() && found != strOp.end()) {
      eat(t, TokenType::SYMBOL); // eat an operator
      ops.push_back(found->second);
      terms.push_back(buildTerm(t));
      found = t.hasMore() ? strOp.find(t.getCurrentToken().value()) : strOp.end();
    }
  }

  return Expression(terms, ops);
}

Term JackCompilationEngineBuilder::buildTerm(JackTokenizer &t) {
  Token tok = eat(t);

  // unaryOp <term>
  auto found = strUnaryOp.find(tok.value());
  if (found != strUnaryOp.end()) {
    UnaryOp unaryOp = found->second;
    return Term(buildTerm(t), unaryOp);
  }

  // if token is an identifier, it could be:
  // - an array access like varName[expression] or
  // - a subroutineCall
  if (tok.isIdentifier()) {
    if (t.hasMore() && t.getCurrentToken().value() == "[") {
      eat(t, "[");
      Expression expression = buildExpression(t);
      eat(t, "]");
      return Term(tok, expression);
    }

    // a subroutineCall is like
    // <obj>.<method>() | <method>() so lookahead for a "." or a "("
    if (t.hasMore() &&
        in_array(t.getCurrentToken().value(), {".", "("})) {
      return Term(buildSubroutineCall(t, tok));
    }
  }

  // since it's not a varname[] nor a subroutine call, it could be
  // intConstant | strConstant | keywordConstant | varName
  if (in_array(tok.getType(), {TokenType::INT_CONSTANT, TokenType::STR_CONSTANT, TokenType::IDENTIFIER, TokenType::KEYWORD}))
    return Term(tok);

  // ( <expr> )
  if (tok.value() == "(") {
    Expression expression = buildExpression(t);
    eat(t, ")");
    return Term(expression);
  }

  std::string msg = (
    "Unknown category for term; next tokens: " + tok.value() + " " +
    (t.hasMore() ? t.getCurrentToken().value() : "")  // next token
  );
  throw_and_debug(msg);
}

SubroutineCall JackCompilationEngineBuilder::buildSubroutineCall(JackTokenizer &t) {
  return buildSubroutineCall(t, eat(t, TokenType::IDENTIFIER));
}

SubroutineCall JackCompilationEngineBuilder::buildSubroutineCall(JackTokenizer &t, Token tok) {
  Token subroutineName = tok, classOrVarName;
  _assert(tok.isIdentifier(), "First token of a subroutine call must be an identifier");

  if (t.hasMore() && t.getCurrentToken().value() == ".") {
    classOrVarName = subroutineName;
    eat(t, ".");
    subroutineName = eat(t, TokenType::IDENTIFIER);
  }

  eat(t, "(");
  ExpressionList expressionList = buildExpressionList(t);
  eat(t, ")");

  return SubroutineCall(subroutineName, classOrVarName, expressionList);
}

ExpressionList JackCompilationEngineBuilder::buildExpressionList(JackTokenizer &t) {
  std::vector<Expression> expressions;
  // expression list only appears inside a function call, so we
  // know that if there is an expression list, then the next token
  // isn't a closing bracket ')'.
  if (t.hasMore() &&
      t.getCurrentToken().value() != ")") {
    expressions.push_back(buildExpression(t));
    while (t.hasMore() && t.getCurrentToken().value() == ",") {
      eat(t, ",");
      expressions.push_back(buildExpression(t));
    }
  }
  return ExpressionList(expressions);
}

Token JackCompilationEngineBuilder::eat(JackTokenizer &t) {
  _assert(t.hasMore(), "Tokenizer hasn't got any more tokens to give");

  Token tok = t.getCurrentToken();
  //std::cout << "eating token " << tok.value() << "\n";
  t.advance();
  return tok;
}

Token JackCompilationEngineBuilder::eat(JackTokenizer &t, std::function<bool(Token)> isValidFunc) {
  _assert(t.hasMore(), "Tokenizer hasn't got any more tokens to give");

  Token tok = t.getCurrentToken();
  if (!isValidFunc(tok))
    tok.raise();
  return eat(t);
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
