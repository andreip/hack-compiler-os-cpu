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

#include "../../utils.h"

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

  void buildSubroutineDecs(istringstream &stream, std::string className="") {
    auto fp = std::bind(&Builder::buildSubroutineDecs, builder,
                        std::placeholders::_1, className);
    _buildFromVector<SubroutineDec>(stream, fp);
  }

  void buildSubroutineBody(istringstream &stream) {
    auto fp = std::bind(&Builder::buildSubroutineBody, builder,
                        std::placeholders::_1);
    _build<SubroutineBody>(stream, fp);
  }

  void buildExpression(istringstream &stream) {
    auto fp = std::bind(&Builder::buildExpression, builder,
                        std::placeholders::_1);
    _build<Expression>(stream, fp);
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
    //std::cout << "Expected: " << expected << '\n';
    //std::cout << "Actual: " << actual << '\n';
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
      "<parameterList>",
      "</parameterList>",
      "<symbol>)</symbol>",
      "<subroutineBody>",
        "<symbol>{</symbol>",
        "<statements>",
        "</statements>",
        "<symbol>}</symbol>",
      "</subroutineBody>",
    "</subroutineDec>",
  };

  buildSubroutineDecs(stream);
}

BOOST_FIXTURE_TEST_CASE(test_class_subroutine_dec, fixture) {
  istringstream stream("method void test(int x, char y) { }");
  expected = {
    "<subroutineDec>",
      "<keyword>method</keyword>",
      "<keyword>void</keyword>",
      "<identifier>test</identifier>",
      "<symbol>(</symbol>",
      "<parameterList>",
        "<keyword>int</keyword>",
        "<identifier>x</identifier>",
        "<symbol>,</symbol>",
        "<keyword>char</keyword>",
        "<identifier>y</identifier>",
      "</parameterList>",
      "<symbol>)</symbol>",
      "<subroutineBody>",
        "<symbol>{</symbol>",
          "<statements>",
          "</statements>",
        "<symbol>}</symbol>",
      "</subroutineBody>",
    "</subroutineDec>",
  };

  buildSubroutineDecs(stream);
}

BOOST_FIXTURE_TEST_CASE(test_subroutine_body_var_decls, fixture) {
  istringstream stream("{ var int x, y;\n var Test a, b; }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<varDec>",
        "<keyword>var</keyword>",
        "<keyword>int</keyword>",
        "<identifier>x</identifier>",
        "<symbol>,</symbol>",
        "<identifier>y</identifier>",
        "<symbol>;</symbol>",
      "</varDec>",
      "<varDec>",
        "<keyword>var</keyword>",
        "<identifier>Test</identifier>",
        "<identifier>a</identifier>",
        "<symbol>,</symbol>",
        "<identifier>b</identifier>",
        "<symbol>;</symbol>",
      "</varDec>",
      "<statements>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}

BOOST_FIXTURE_TEST_CASE(test_subroutine_body_let_statement, fixture) {
  istringstream stream("{ let x = 2; }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<statements>",
        "<letStatement>",
          "<keyword>let</keyword>",
          "<identifier>x</identifier>",
          "<symbol>=</symbol>",
          "<expression>",
            "<term>",
              "<integerConstant>2</integerConstant>",
            "</term>",
          "</expression>",
          "<symbol>;</symbol>",
        "</letStatement>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}

BOOST_FIXTURE_TEST_CASE(test_subroutine_body_let_statement2, fixture) {
  istringstream stream("{ let x[i] = j; }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<statements>",
        "<letStatement>",
          "<keyword>let</keyword>",
          "<identifier>x</identifier>",
          "<symbol>[</symbol>",
          "<expression>",
            "<term>",
              "<identifier>i</identifier>",
            "</term>",
          "</expression>",
          "<symbol>]</symbol>",
          "<symbol>=</symbol>",
          "<expression>",
            "<term>",
              "<identifier>j</identifier>",
            "</term>",
          "</expression>",
          "<symbol>;</symbol>",
        "</letStatement>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}

BOOST_FIXTURE_TEST_CASE(test_subroutine_body_let_statement3, fixture) {
  istringstream stream("{ let sum = sum + a[i]; }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<statements>",
        "<letStatement>",
          "<keyword>let</keyword>",
          "<identifier>sum</identifier>",
          "<symbol>=</symbol>",
          "<expression>",
            "<term>",
              "<identifier>sum</identifier>",
            "</term>",
            "<symbol>+</symbol>",
            "<term>",
              "<identifier>a</identifier>",
              "<symbol>[</symbol>",
              "<expression>",
                "<term>",
                  "<identifier>i</identifier>",
                "</term>",
              "</expression>",
              "<symbol>]</symbol>",
            "</term>",
          "</expression>",
          "<symbol>;</symbol>",
        "</letStatement>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}

BOOST_FIXTURE_TEST_CASE(test_subroutine_body_if_statement, fixture) {
  istringstream stream("{ if (i) { let s = i; let s = j; } }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<statements>",
        "<ifStatement>",
          "<keyword>if</keyword>",
          "<symbol>(</symbol>",
          "<expression>",
            "<term>",
              "<identifier>i</identifier>",
            "</term>",
          "</expression>",
          "<symbol>)</symbol>",
          "<symbol>{</symbol>",
          "<statements>",
            "<letStatement>",
              "<keyword>let</keyword>",
              "<identifier>s</identifier>",
              "<symbol>=</symbol>",
              "<expression>",
                "<term>",
                  "<identifier>i</identifier>",
                "</term>",
              "</expression>",
              "<symbol>;</symbol>",
            "</letStatement>",
            "<letStatement>",
              "<keyword>let</keyword>",
              "<identifier>s</identifier>",
              "<symbol>=</symbol>",
              "<expression>",
                "<term>",
                  "<identifier>j</identifier>",
                "</term>",
              "</expression>",
              "<symbol>;</symbol>",
            "</letStatement>",
          "</statements>",
          "<symbol>}</symbol>",
        "</ifStatement>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}

BOOST_FIXTURE_TEST_CASE(test_subroutine_body_nested_if_else_statement, fixture) {
  istringstream stream("{ if (i) { if (j) { let s = i; }}/*inner if ended*/ else { let i = i; let j = j; } }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<statements>",
        "<ifStatement>",
          "<keyword>if</keyword>",
          "<symbol>(</symbol>",
          "<expression>",
            "<term>",
              "<identifier>i</identifier>",
            "</term>",
          "</expression>",
          "<symbol>)</symbol>",
          "<symbol>{</symbol>",
          "<statements>",
            "<ifStatement>",
              "<keyword>if</keyword>",
              "<symbol>(</symbol>",
              "<expression>",
                "<term>",
                  "<identifier>j</identifier>",
                "</term>",
              "</expression>",
              "<symbol>)</symbol>",
              "<symbol>{</symbol>",
              "<statements>",
                "<letStatement>",
                  "<keyword>let</keyword>",
                  "<identifier>s</identifier>",
                  "<symbol>=</symbol>",
                  "<expression>",
                    "<term>",
                      "<identifier>i</identifier>",
                    "</term>",
                  "</expression>",
                  "<symbol>;</symbol>",
                "</letStatement>",
              "</statements>",
              "<symbol>}</symbol>",
            "</ifStatement>",
          "</statements>",
          "<symbol>}</symbol>",
          "<keyword>else</keyword>",
          "<symbol>{</symbol>",
          "<statements>",
            "<letStatement>",
              "<keyword>let</keyword>",
              "<identifier>i</identifier>",
              "<symbol>=</symbol>",
              "<expression>",
                "<term>",
                  "<identifier>i</identifier>",
                "</term>",
              "</expression>",
              "<symbol>;</symbol>",
            "</letStatement>",
            "<letStatement>",
              "<keyword>let</keyword>",
              "<identifier>j</identifier>",
              "<symbol>=</symbol>",
              "<expression>",
                "<term>",
                  "<identifier>j</identifier>",
                "</term>",
              "</expression>",
              "<symbol>;</symbol>",
            "</letStatement>",
          "</statements>",
          "<symbol>}</symbol>",
        "</ifStatement>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}


BOOST_FIXTURE_TEST_CASE(test_subroutine_body_while_statement, fixture) {
  istringstream stream("{ while (i) { let j = \"string ct\"; } }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<statements>",
        "<whileStatement>",
          "<keyword>while</keyword>",
          "<symbol>(</symbol>",
          "<expression>",
            "<term>",
              "<identifier>i</identifier>",
            "</term>",
          "</expression>",
          "<symbol>)</symbol>",
          "<symbol>{</symbol>",
          "<statements>",
            "<letStatement>",
              "<keyword>let</keyword>",
              "<identifier>j</identifier>",
              "<symbol>=</symbol>",
              "<expression>",
                "<term>",
                  "<stringConstant>string ct</stringConstant>",
                "</term>",
              "</expression>",
              "<symbol>;</symbol>",
            "</letStatement>",
          "</statements>",
          "<symbol>}</symbol>",
        "</whileStatement>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}

BOOST_FIXTURE_TEST_CASE(test_subroutine_body_subroutine_call1, fixture) {
  istringstream stream("{ do call(x, y); }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<statements>",
        "<doStatement>",
          "<keyword>do</keyword>",
          "<identifier>call</identifier>",
          "<symbol>(</symbol>",
          "<expressionList>",
            "<expression>",
              "<term>",
                "<identifier>x</identifier>",
              "</term>",
            "</expression>",
            "<symbol>,</symbol>",
            "<expression>",
              "<term>",
                "<identifier>y</identifier>",
              "</term>",
            "</expression>",
          "</expressionList>",
          "<symbol>)</symbol>",
          "<symbol>;</symbol>",
        "</doStatement>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}

BOOST_FIXTURE_TEST_CASE(test_subroutine_body_subroutine_call_nested, fixture) {
  istringstream stream("{ do call(-x, y,\n call2()); }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<statements>",
        "<doStatement>",
          "<keyword>do</keyword>",
          "<identifier>call</identifier>",
          "<symbol>(</symbol>",
          "<expressionList>",
            "<expression>",
              "<term>",
                "<symbol>-</symbol>",
                "<term>",
                  "<identifier>x</identifier>",
                "</term>",
              "</term>",
            "</expression>",
            "<symbol>,</symbol>",
            "<expression>",
              "<term>",
                "<identifier>y</identifier>",
              "</term>",
            "</expression>",
            "<symbol>,</symbol>",
            "<expression>",
              "<term>",
                "<identifier>call2</identifier>",
                "<symbol>(</symbol>",
                "<expressionList>",
                "</expressionList>",
                "<symbol>)</symbol>",
              "</term>",
            "</expression>",
          "</expressionList>",
          "<symbol>)</symbol>",
          "<symbol>;</symbol>",
        "</doStatement>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}

BOOST_FIXTURE_TEST_CASE(test_subroutine_body_subroutine_return_statement, fixture) {
  istringstream stream("{ return x; }");
  expected = {
    "<subroutineBody>",
      "<symbol>{</symbol>",
      "<statements>",
        "<returnStatement>",
          "<keyword>return</keyword>",
          "<expression>",
            "<term>",
              "<identifier>x</identifier>",
            "</term>",
          "</expression>",
          "<symbol>;</symbol>",
        "</returnStatement>",
      "</statements>",
      "<symbol>}</symbol>",
    "</subroutineBody>",
  };
  buildSubroutineBody(stream);
}

BOOST_FIXTURE_TEST_CASE(test_expression_add, fixture) {
  istringstream stream("x + 1");
  expected = {
    "<expression>",
      "<term>",
        "<identifier>x</identifier>",
      "</term>",
      "<symbol>+</symbol>",
      "<term>",
        "<integerConstant>1</integerConstant>",
      "</term>",
    "</expression>",
  };
  buildExpression(stream);
}

BOOST_FIXTURE_TEST_CASE(test_expression_escape, fixture) {
  istringstream stream("x < 2");
  expected = {
    "<expression>",
      "<term>",
        "<identifier>x</identifier>",
      "</term>",
      "<symbol>&lt;</symbol>",
      "<term>",
        "<integerConstant>2</integerConstant>",
      "</term>",
    "</expression>",
  };
  buildExpression(stream);
}

BOOST_FIXTURE_TEST_CASE(test_expression_nested_parenthesis, fixture) {
  istringstream stream("( ( x + 2 ) )");
  expected = {
    "<expression>",
      "<term>",
        "<symbol>(</symbol>",
        "<expression>",
          "<term>",
            "<symbol>(</symbol>",
            "<expression>",
              "<term>",
                "<identifier>x</identifier>",
              "</term>",
              "<symbol>+</symbol>",
              "<term>",
                "<integerConstant>2</integerConstant>",
              "</term>",
            "</expression>",
            "<symbol>)</symbol>",
          "</term>",
        "</expression>",
        "<symbol>)</symbol>",
      "</term>",
    "</expression>",
  };
  buildExpression(stream);
}

BOOST_FIXTURE_TEST_CASE(test_expression_complicated, fixture) {
  istringstream stream("x & ( (y * 2) | ~(z - 4) )");
  expected = {
    "<expression>",
      "<term>",
        "<identifier>x</identifier>",
      "</term>",
      "<symbol>&amp;</symbol>",
      "<term>",
        "<symbol>(</symbol>",
        "<expression>",
          "<term>",
            "<symbol>(</symbol>",
            "<expression>",
              "<term>",
                "<identifier>y</identifier>",
              "</term>",
              "<symbol>*</symbol>",
              "<term>",
                "<integerConstant>2</integerConstant>",
              "</term>",
            "</expression>",
            "<symbol>)</symbol>",
          "</term>",
          "<symbol>|</symbol>",
          "<term>",
            "<symbol>~</symbol>",
            "<term>",
              "<symbol>(</symbol>",
              "<expression>",
                "<term>",
                  "<identifier>z</identifier>",
                "</term>",
                "<symbol>-</symbol>",
                "<term>",
                  "<integerConstant>4</integerConstant>",
                "</term>",
              "</expression>",
              "<symbol>)</symbol>",
            "</term>",
          "</term>",
        "</expression>",
        "<symbol>)</symbol>",
      "</term>",
    "</expression>",
  };
  buildExpression(stream);
}
