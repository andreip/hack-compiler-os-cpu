#include <algorithm>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "../../utils.h"

#include "tokenizer.h"

// JackTokenizer

std::string JackTokenizer::SYMBOLS = "(){}[].,;+-*/&|<>=~";

std::unordered_set<std::string> JackTokenizer::KEYWORDS = {
  "class", "constructor", "function", "method", "field", "static",
  "var", "int", "char", "boolean", "void", "true", "false",
  "null", "this", "let", "do", "if", "else", "while", "return",
};

std::string JackTokenizer::IGNORE_CHARS = " \t\n";
std::string JackTokenizer::SINGLE_LINE_COMMENT = "//";
std::string JackTokenizer::MULTILINE_COMMENT_BEGIN = "/*";
std::string JackTokenizer::MULTILINE_COMMENT_END = "*/";

JackTokenizer::JackTokenizer(std::istream &in): _istream(in), _hasMore(true) {
  if (!_istream) {
    std::cerr << "Input stream in tokenizer failed.\n";
  }
}

JackTokenizer::~JackTokenizer() { }

bool JackTokenizer::hasMore() {
  if (!_crt_buffer.empty())
    return true;

  // try refilling the buffer until we reach eof
  while (_crt_buffer.empty() && _hasMore) {
    std::string line;
    std::getline(_istream, line);

    if (!line.empty())
      JackTokenizer::tokenizeLine(line, _crt_buffer);

    if (!_istream || _istream.eof()) {
      _hasMore = false;
    }
  }

  return !_crt_buffer.empty();
}

void JackTokenizer::advance() {
  // must call hasMore() first, to check
  _crt_buffer.pop_front();
}

Token JackTokenizer::getCurrentToken() {
  return _crt_buffer.front();
}

template <typename ContainerT>
void JackTokenizer::tokenizeLine(std::string line, ContainerT &out) {
  strip(line, JackTokenizer::IGNORE_CHARS);
  if (line.empty())
    return;

  if (std::count(line.begin(), line.end(), '"') % 2 != 0)
    throw_and_debug("Ill formed string constants in line: " + line);

  std::deque<std::string> temp;
  // split the line by double quote first, to get the string constants.
  // also preserve double-quotes in split array, to be able to
  // tell when we're inside a double-quoted string constant or outside.
  split_by_any_char(temp, line, "\"", true);
  std::for_each(
    temp.begin(), temp.end(),
    [&out, in_quote=false](std::string &s) mutable {
      // if we found a quote, toggle if we're in quote or not currently.
      if (s == "\"") {
        in_quote = !in_quote;
      } else {
        // if inside a quote, add the raw string including quotes
        // as a token string constant.
        if (in_quote)
          out.push_back(Token::fromString("\"" + s + "\""));
        // otherwise this string is string-constants free, so
        // we can pass it to be parsed by the other symbols of
        // the language and populate the out container.
        else
          tokenizeString(s, out);
      }
    }
  );
}

/*
 * Parses string and adds it to the end of the given output container.
 */
template <typename ContainerT>
void JackTokenizer::tokenizeString(std::string line, ContainerT &out) {

    std::cout << "read line : '" << line << "'\n";
    strip(line, JackTokenizer::IGNORE_CHARS);
    if (!line.empty()) {
      // split line into individual tokens and strip each one
      std::deque<std::string> temp;
      split_by_any_char(
        temp, line,
        JackTokenizer::IGNORE_CHARS + JackTokenizer::SYMBOLS,
        true  // preserves the chracters we split by
      );
      std::cout << "crt line buffer size " << temp.size() << '\n';

      // copy each stripped-non-empty token from temp to output buffer.
      std::for_each(
        temp.begin(), temp.end(),
        [&out](std::string s) {
          strip(s, JackTokenizer::IGNORE_CHARS);
          if (!s.empty())
            out.push_back(Token::fromString(s));
        }
      );
    }
    std::cout << "out size " << out.size() << '\n';
}

// Token class related

Token Token::fromString(const std::string &value) {
  if (JackTokenizer::KEYWORDS.find(value) != JackTokenizer::KEYWORDS.end())
    return Token(TokenType::KEYWORD, value);

  if (value.length() == 1 &&
      JackTokenizer::SYMBOLS.find(value) != std::string::npos)
        return Token(TokenType::SYMBOL, value);

  if (isNumber(value)) {
    int n = getNumber(value);
    if (n >= 0 && n <= 32767)
      return Token(TokenType::INT_CONSTANT, value);
    throw_and_debug("Number out of 16-bit range: " + value);
  }

  if (value.front() == '"' && value.back() == '"') {
    // make sure it doesn't contain double quote inside just for
    // sanity checking.
    if (value.find("\"", 1, value.size() - 2) != std::string::npos)
      throw_and_debug("String constant contains too many double quotes: " + value);

    return Token(TokenType::STR_CONSTANT, value);
  }

  if (value.front() == '_' || std::isalpha(value.front()))
    if (std::all_of(
          value.begin(), value.end(),
          [](char c) { return c == '_' || std::isalnum(c); }))
      return Token(TokenType::IDENTIFIER, value);

  throw_and_debug("Unkown token type for value '" + value + "'");
}

Token::Token(TokenType type, const std::string &rawValue)
  : type(type), rawValue(rawValue) { }

std::string Token::toXML() {
  char xml[100];
  snprintf(xml, sizeof(xml), "<%s> %s </%s>",
           getTypeStr().c_str(), value().c_str(), getTypeStr().c_str());
  return xml;
}

std::string Token::getTypeStr() {
  if (type == TokenType::KEYWORD) return "keyword";
  if (type == TokenType::SYMBOL) return "symbol";
  if (type == TokenType::INT_CONSTANT) return "integerConstant";
  if (type == TokenType::STR_CONSTANT) return "stringConstant";
  if (type == TokenType::IDENTIFIER) return "identifier";
  throw_and_debug("Unknown token type");
}

std::string Token::value() {
  if (type == TokenType::STR_CONSTANT)
    return strip_copy(rawValue, "\"");
  return rawValue;
}

std::string Token::getRawValue() {
  return rawValue;
}
