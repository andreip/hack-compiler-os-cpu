#ifndef __JACK__TOKENIZER__H__
#define __JACK__TOKENIZER__H__

#include <deque>
#include <iostream>
#include <istream>
#include <string>
#include <unordered_set>
#include <vector>

enum class TokenType {
  KEYWORD,
  SYMBOL,
  INT_CONSTANT,
  STR_CONSTANT,
  IDENTIFIER,
};

// holds a token that the tokenizer outputs.
class Token {
public:
  static Token fromString(const std::string&);
  Token(TokenType type, const std::string &rawValue);
  std::string toXML() const;
  std::string getTypeStr() const;
  std::string value() const;
  std::string getRawValue() const;
  friend std::ostream& operator<<(std::ostream&, const Token&);
private:
  TokenType type;
  std::string rawValue;
};

// like an iterator, but outputs tokens from the
// Jack language.
class JackTokenizer {
public:
  JackTokenizer(std::istream &in);
  virtual ~JackTokenizer();
  virtual bool hasMore();
  virtual void advance();
  virtual Token getCurrentToken();
  virtual void rewind();

  static std::string SYMBOLS;
  static std::unordered_set<std::string> KEYWORDS;
private:
  // fills buffer
  template <typename ContainerT>
  void tokenizeLine(std::string line, ContainerT &out);
  template <typename ContainerT>
  void tokenizeString(std::string str, ContainerT &out);
  void stripComments(std::string& line);
private:
  std::istream& _istream;
  std::deque<Token> _crt_buffer;
  bool _hasMore;
  bool _inMultiLineComment;

  static std::string IGNORE_CHARS;
  static std::string SINGLE_LINE_COMMENT;
  static std::string MULTILINE_COMMENT_BEGIN;
  static std::string MULTILINE_COMMENT_END;
};

#endif
