#ifndef __JACK_BUILDER_H__
#define __JACK_BUILDER_H__

#include <functional>
#include <iostream>
#include <string>
#include <vector>

class Token;
class JackTokenizer;
enum class TokenType;

class ClassVarDec;
class SubroutineDec;
class ClassElement;

class JackBuilder {
public:
  virtual void build(const std::string &inputFile) = 0;
  virtual ~JackBuilder();
protected:
  JackBuilder();
};

class JackCompilationEngineBuilder: public JackBuilder {
public:
  JackCompilationEngineBuilder();
  virtual void build(const std::string &inputFile) override;
  void build(JackTokenizer&, std::ostream&);
  ClassElement buildClass(JackTokenizer&);
  std::vector<ClassVarDec> buildClassVarDecs(JackTokenizer&);
  std::vector<SubroutineDec> buildSubroutineDecs(JackTokenizer&);
  //void buildParameterList(JackTokenizer &t, std::ostream &out);
  //void buildSubroutineBody(JackTokenizer &t, std::ostream &out);
private:
  Token eat(JackTokenizer&, std::function<bool(Token)>);
  Token eat(JackTokenizer&, const std::string&);
  Token eat(JackTokenizer&, TokenType);
  void _assert(bool truth, const std::string &msg);
private:
  std::string _outputFile;
};

/* Intermediary step that writes tokens to a file, for testing
 * purposes.
 */
class JackTokenizerBuilder: public JackBuilder {
public:
  JackTokenizerBuilder();
  virtual void build(const std::string &inputFile) override;
private:
  std::string _outputFile;
};

#endif
