#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "builder.h"
#include "tokenizer.h"

using namespace std;
using namespace boost;

// testing JackCompilationEngineBuilder

using Builder = JackCompilationEngineBuilder;

struct fixture {
  vector<string> expected;
  vector<string> actual;
  Builder builder;

  void buildClass(istringstream &stream) {
    auto fp = std::bind(&Builder::buildClass, builder,
                        std::placeholders::_1, std::placeholders::_2);
    _build(stream, fp);
  }

  void buildClassVarDec(istringstream &stream) {
    auto fp = std::bind(&Builder::buildClassVarDec, builder,
                        std::placeholders::_1, std::placeholders::_2);
    _build(stream, fp);
  }

  void _build(istringstream &stream,
             std::function<void(JackTokenizer&, std::ostream&)> buildMethod) {
    JackTokenizer tokenizer(stream);
    ostringstream out;
    buildMethod(tokenizer, out);

    // split lines, trim them and filter out empty ones.
    std::string s = out.str();
    split(actual, s, is_any_of("\n"));
    actual.erase(
      remove_if(
        actual.begin(), actual.end(),
        [](std::string s) { return trim_copy(s).empty(); }
      ),
      actual.end()
    );
  }

  virtual ~fixture() {
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
      std::begin(expected), std::end(expected),
      std::begin(actual), std::end(actual)
    );
  }
};

BOOST_FIXTURE_TEST_CASE(test_empty_class_parser, fixture) {
  istringstream stream("class Test   {  }");
  expected = {
    "<class>",
      "<keyword> class </keyword>",
      "<identifier> Test </identifier>",
      "<symbol> { </symbol>",
      "<symbol> } </symbol>",
    "</class>",
  };

  buildClass(stream);
}

BOOST_FIXTURE_TEST_CASE(test_class_var_dec_parser, fixture) {
  istringstream stream("static int x, y;\nfield boolean z;");
  expected = {
    "<classVarDec>",
      "<keyword> static </keyword>",
      "<keyword> int </keyword>",
      "<identifier> x </identifier>",
      "<symbol> , </symbol>",
      "<identifier> y </identifier>",
      "<symbol> ; </symbol>",
    "</classVarDec>",
    "<classVarDec>",
      "<keyword> field </keyword>",
      "<keyword> boolean </keyword>",
      "<identifier> z </identifier>",
      "<symbol> ; </symbol>",
    "</classVarDec>",
  };

  buildClassVarDec(stream);
}
