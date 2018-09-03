#ifndef __JACK_BUILDER_H__
#define __JACK_BUILDER_H__

#include <string>

class JackTokenizer;

class JackBuilder {
public:
  virtual void build(const std::string &inputFile) = 0;
  virtual ~JackBuilder();
protected:
  JackBuilder();
};

//class JackGrammarBuilder {
//public:
//  JackGrammarBuilder(JackTokenizer&);
//};

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
