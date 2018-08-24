#include <algorithm>
#include <iostream>

#include "../../utils.h"

#include "tokenizer.h"

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

std::string JackTokenizer::getCurrentToken() {
  return _crt_buffer.front();
}

template <typename ContainerT>
void JackTokenizer::tokenizeLine(std::string line, ContainerT &out) {
    // first tokenize line by double quotes, to get the string literals

    std::cout << "read line : '" << line << "'\n";
    strip(line, JackTokenizer::IGNORE_CHARS);
    if (!line.empty()) {
      // split line into individual tokens and strip each one
      split_by_any_char(
        out, line,
        JackTokenizer::IGNORE_CHARS + JackTokenizer::SYMBOLS,
        true  // preserves the chracters we split by
      );
      std::cout << "crt line buffer size " << out.size() << '\n';
      std::for_each(
        out.begin(), out.end(),
        [](std::string &tok) { return strip(tok, JackTokenizer::IGNORE_CHARS); }
      );
      // remove now empty tokens from collection.
      out.erase(
        std::remove_if(out.begin(), out.end(),
                       [](std::string s) { return s.empty(); }),
        out.end()
      );
    }
    std::cout << "crt line buffer size " << out.size() << '\n';
}
