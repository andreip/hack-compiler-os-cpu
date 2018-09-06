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
  NONE,
};

enum class KeywordTokenType {
  CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT,
  BOOLEAN, CHAR, VOID, VAR, STATIC, FIELD, LET,
  DO, IF, ELSE, WHILE, RETURN, TRUE, FALSE, _NULL, THIS
};

// holds a token that the tokenizer outputs.
class Token {
public:
  static Token fromString(const std::string&);
  Token(TokenType type=TokenType::NONE, const std::string &rawValue="");
  bool operator!() const;
  operator bool() const;
  std::string toXML() const;
  TokenType getType() const;
  std::string getTypeStr() const;
  std::string value() const;
  std::string escapedValue() const;

  bool isAType() const;
  bool isIdentifier() const;

  // TODO: implement these
  //KeywordTokenType getKeywordType() const;
  //char getSymbol() const;
  //std::string getIdentifier() const;
  //int getIntConstant() const;
  //std::string getStrConstant() const;  // returns w/o double quotes

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
