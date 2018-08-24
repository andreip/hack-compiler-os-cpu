#ifndef __JACK__TOKENIZER__H__
#define __JACK__TOKENIZER__H__

#include <deque>
#include <istream>
#include <string>
#include <unordered_set>
#include <vector>

// like an iterator, but outputs tokens from the
// Jack language.
class JackTokenizer {
public:
  JackTokenizer(std::istream &in);
  virtual ~JackTokenizer();
  virtual bool hasMore();
  virtual void advance();
  virtual std::string getCurrentToken();
private:
  // fills buffer
  template <typename ContainerT>
  static void tokenizeLine(std::string line, ContainerT &out);
private:
  std::istream& _istream;
  std::deque<std::string> _crt_buffer;
  bool _hasMore;

  static std::string SYMBOLS;
  static std::unordered_set<std::string> KEYWORDS;
  static std::string IGNORE_CHARS;
  static std::string SINGLE_LINE_COMMENT;
  static std::string MULTILINE_COMMENT_BEGIN;
  static std::string MULTILINE_COMMENT_END;
};

#endif
