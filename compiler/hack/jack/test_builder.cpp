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
#include "grammar.h"
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
                        std::placeholders::_1);
    _build<ClassElement>(stream, fp);
  }

  void buildClassVarDecs(istringstream &stream) {
    auto fp = std::bind(&Builder::buildClassVarDecs, builder,
                        std::placeholders::_1);
    _buildFromVector<ClassVarDec>(stream, fp);
  }

  void buildSubroutineDecs(istringstream &stream) {
    auto fp = std::bind(&Builder::buildSubroutineDecs, builder,
                        std::placeholders::_1);
    _buildFromVector<SubroutineDec>(stream, fp);
  }

  template <typename GrammarElement>
  void _build(istringstream &stream,
             std::function<GrammarElement(JackTokenizer&)> buildMethod) {
    JackTokenizer tokenizer(stream);
    populateActualfromXML(buildMethod(tokenizer).toXML());
  }

  template <typename GrammarElement>
  void _buildFromVector(
    istringstream &stream,
    std::function<vector<GrammarElement>(JackTokenizer&)> buildMethod) {
    JackTokenizer tokenizer(stream);
    std::ostringstream out;
    for (const GrammarElement &el : buildMethod(tokenizer))
      out << el.toXML();
    populateActualfromXML(out.str());
  }

  void populateActualfromXML(std::string xml) {
    // split lines, trim them and filter out empty ones.
    split(actual, xml, is_any_of("\n"));
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
      "<keyword>class</keyword>",
      "<identifier>Test</identifier>",
      "<symbol>{</symbol>",
      "<symbol>}</symbol>",
    "</class>",
  };

  buildClass(stream);
}

BOOST_FIXTURE_TEST_CASE(test_class_var_dec_parser, fixture) {
  istringstream stream("static int x, y;\nfield boolean z;");
  expected = {
    "<classVarDec>",
      "<keyword>static</keyword>",
      "<keyword>int</keyword>",
      "<identifier>x</identifier>",
      "<symbol>,</symbol>",
      "<identifier>y</identifier>",
      "<symbol>;</symbol>",
    "</classVarDec>",
    "<classVarDec>",
      "<keyword>field</keyword>",
      "<keyword>boolean</keyword>",
      "<identifier>z</identifier>",
      "<symbol>;</symbol>",
    "</classVarDec>",
  };

  buildClassVarDecs(stream);
}

BOOST_FIXTURE_TEST_CASE(test_class_subroutine_dec_empty_params, fixture) {
  istringstream stream("constructor Square new() { }");
  expected = {
    "<subroutineDec>",
      "<keyword>constructor</keyword>",
      "<identifier>Square</identifier>",
      "<identifier>new</identifier>",
      "<symbol>(</symbol>",
      "<symbol>)</symbol>",
      "<subroutineBody>",
        "<symbol>{</symbol>",
        "<symbol>}</symbol>",
      "</subroutineBody>",
    "</subroutineDec>",
  };

  buildSubroutineDecs(stream);
}
