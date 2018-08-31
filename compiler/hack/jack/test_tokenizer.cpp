#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "tokenizer.h"

#include <sstream>
#include <vector>

using namespace std;

struct fixture {
  vector<string> expected;
  vector<string> actual;

  void tokenize(istringstream &stream) {
    JackTokenizer tok(stream);
    while (tok.hasMore()) {
      actual.push_back(tok.getCurrentToken().getRawValue());
      tok.advance();
    }
  }

  virtual ~fixture() {
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
      begin(expected), end(expected),
      begin(actual), end(actual));
  }
};

BOOST_FIXTURE_TEST_CASE(test_empty_stream, fixture) {
  istringstream stream("");
  expected = {};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_only_spaces_stream, fixture) {
  istringstream stream("  \n   ");
  expected = {};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_delimited_by_space_line, fixture) {
  istringstream stream("a spaced       line");
  expected = {"a", "spaced", "line"};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_two_lines_by_space, fixture) {
  istringstream stream("a spaced  line\n second line ");
  expected = {"a", "spaced", "line", "second", "line"};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_split_if, fixture) {
  istringstream stream("if (x = 2) { return 42; }");
  expected = {"if", "(", "x", "=", "2", ")", "{", "return", "42", ";", "}"};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_split_while, fixture) {
  istringstream stream("while (x = 2) { let x = 3; do a.c(); }");
  expected = {"while", "(", "x", "=", "2", ")", "{", "let", "x",
              "=", "3", ";", "do", "a", ".", "c", "(", ")", ";", "}"};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_let_array_string, fixture) {
  istringstream stream("let a[i] = Keyboard.readInt(\"ENTER THE NEXT NUMBER: \");");
  expected = {"let", "a", "[", "i", "]", "=", "Keyboard", ".",
              "readInt", "(", "\"ENTER THE NEXT NUMBER: \"", ")", ";"};

  tokenize(stream);
}
