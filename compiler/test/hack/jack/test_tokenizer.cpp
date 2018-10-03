#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "hack/jack/tokenizer.h"

using namespace std;

struct fixture {
  vector<string> expected;
  vector<string> actual;

  void tokenize(istringstream &stream) {
    JackTokenizer tok(stream);
    while (tok.hasMore()) {
      actual.push_back(tok.getCurrentToken().value());
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

BOOST_FIXTURE_TEST_CASE(test_constant_string, fixture) {
  istringstream stream("{ let x = \" \"; }");
  expected = {"{", "let", "x", "=", "\" \"", ";", "}"};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_empty_string_as_param, fixture) {
  istringstream stream("{ do Output.printString(\" \", \"\"); }");
  expected = {"{", "do", "Output", ".", "printString", "(", "\" \"", ",", "\"\"", ")", ";", "}"};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_line_comment, fixture) {
  istringstream stream("// some comments");
  expected = {};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_line_comment_not_at_beginning, fixture) {
  istringstream stream("let x = 2; // some comments");
  expected = {"let", "x", "=", "2", ";"};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_multiline_comment, fixture) {
  istringstream stream("/* some multiline */");
  expected = {};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_multiline_comment2, fixture) {
  istringstream stream("/* some /* multiline */");
  expected = {};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_multiline_comment_on_2_lines, fixture) {
  istringstream stream("/** some multiline \n bla xx*@! comment */");
  expected = {};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_multiline_comment_before_and_after, fixture) {
  istringstream stream(
    "before /** some multiline \n bla xx*@! comment */ after");
  expected = {"before", "after"};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_two_multiline_comments, fixture) {
  istringstream stream(
    "before /* comment1*/ in_between \n/*comment2*/ after");
  expected = {"before", "in_between", "after"};

  tokenize(stream);
}

BOOST_FIXTURE_TEST_CASE(test_class_with_comments, fixture) {
  istringstream stream(
    "// filename: ...\n"
    "/** some comment */\n"
    "class Main {\n"
    "}"
  );
  expected = {"class", "Main", "{", "}"};

  tokenize(stream);
}
