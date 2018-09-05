#ifndef __JACK_BUILDER_H__
#define __JACK_BUILDER_H__

#include <string>
#include <iostream>

class Token;
class JackTokenizer;

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
private:
  void buildClass(JackTokenizer &t, std::ostream &out);
  void buildClassVarDec(JackTokenizer &t, std::ostream &out);
  void buildSubroutineDec(JackTokenizer &t, std::ostream &out);

  Token eat(const std::string&, JackTokenizer&, std::ostream&);
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
