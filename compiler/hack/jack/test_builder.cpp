#include <iostream>
#include <sstream>
#include <string>

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "builder.h"
#include "tokenizer.h"

using namespace std;

// testing JackCompilationEngineBuilder

BOOST_AUTO_TEST_CASE(test_class_parser) {
  istringstream stream("class Test   {  }");
  string expected = (
    "<class>\n"
      "<keyword> class </keyword>\n"
      "<identifier> Test </identifier>\n"
      "<symbol> { </symbol>\n"
      "<symbol> } </symbol>\n"
    "</class>\n"
  );

  JackTokenizer tokenizer(stream);
  JackCompilationEngineBuilder builder;
  ostringstream out;
  builder.build(tokenizer, out);
  std::string actual = out.str();

  BOOST_CHECK_EQUAL(
    expected,
    actual
  );
}
