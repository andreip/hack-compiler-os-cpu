#include <sstream>
#include <stdexcept>
#include <vector>

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "../../utils.h"

#include "builder.h"
#include "tokenizer.h"
#include "grammar.h"
#include "symbol_table.h"

using namespace std;

struct fixture {
  SymbolTable symbol_table;

  void init(std::istream &stream, std::string subroutineName) {
    JackTokenizer tokenizer(stream);
    JackCompilationEngineBuilder builder;
    ClassElement classElement = builder.buildClass(tokenizer);
    bool found = false;
    for (auto &subroutine: classElement.getSubroutineDecs())
      if (subroutine.getName() == subroutineName) {
        symbol_table.init(classElement, subroutine);
        found = true;
        break;
      }
   if (!found)
    throw_and_debug("Couldn't find subroutine name " + subroutineName);
  }

  void check(const std::vector<Symbol> &expected) {
    for (const Symbol &expected_symbol: expected)
      BOOST_CHECK_EQUAL(
        symbol_table.get(expected_symbol.name),
        expected_symbol
      );
  }

  void check_false(const std::vector<Symbol> &expected) {
    for (const Symbol &expected_symbol: expected)
      BOOST_CHECK(
        symbol_table.get(expected_symbol.name) == false
      );
  }
};

BOOST_FIXTURE_TEST_CASE(test_empty, fixture) {
  istringstream stream("class Test { function void test() {} }");
  init(stream, "test");

  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::VAR), 0);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::ARG), 0);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::STATIC), 0);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::FIELD), 0);
  BOOST_CHECK(bool(symbol_table.get("invalid")) == false);
}

BOOST_FIXTURE_TEST_CASE(test_class_var_decs_error, fixture) {
  istringstream stream(
  "class Test {\n"
    "local int x, y;\n"
  "}"
  );

  // can't have local at class level.
  BOOST_CHECK_THROW(init(stream, ""), std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(test_function_var_dec, fixture) {
  istringstream stream(
  "class Test {\n"
    "function void test(char x, boolean y) {\n"
      "var Point p1, p2;\n"
      "return;\n"
    "}\n"
  "}"
  );
  init(stream, "test");

  const std::vector<Symbol> expected {
    {.name="x", .type="char", .kind=SymbolKind::ARG, .index=0},
    {.name="y", .type="boolean", .kind=SymbolKind::ARG, .index=1},
    {.name="p1", .type="Point", .kind=SymbolKind::VAR, .index=0},
    {.name="p2", .type="Point", .kind=SymbolKind::VAR, .index=1},
  };

  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::VAR), 2);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::ARG), 2);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::STATIC), 0);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::FIELD), 0);
  check(expected);
}

BOOST_FIXTURE_TEST_CASE(test_constructor_var_dec, fixture) {
  istringstream stream(
  "class Point {\n"
    "static char name;\n"
    "field int x, y;\n"
    "constructor Test new(int _x, int _y) {\n"
      "let x = _x; let y = _y;\n"
    "}\n"
  "}"
  );
  init(stream, "new");

  const std::vector<Symbol> expected {
    {.name="name", .type="char", .kind=SymbolKind::STATIC, .index=0},
    {.name="x", .type="int", .kind=SymbolKind::FIELD, .index=0},
    {.name="y", .type="int", .kind=SymbolKind::FIELD, .index=1},
    {.name="_x", .type="int", .kind=SymbolKind::ARG, .index=0},
    {.name="_y", .type="int", .kind=SymbolKind::ARG, .index=1},
  };

  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::VAR), 0);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::ARG), 2);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::STATIC), 1);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::FIELD), 2);
  check(expected);

  // check that switching works and erases previous symbols.
  istringstream empty("class Test { function void test() {} }");
  init(empty, "test");
  for (SymbolKind kind : SymbolKindHelpers::ALL)
    BOOST_CHECK_EQUAL(symbol_table.count(kind), 0);
  check_false(expected);
}

BOOST_FIXTURE_TEST_CASE(test_method_var_dec, fixture) {
  istringstream stream(
  "class Point {\n"
    "static char name;\n"
    "field int x, y;\n"
    "constructor Test new(int _x, int _y) {\n"
      "let x = _x; let y = _y;\n"
    "}\n"
    "method boolean equals(Point other) {\n"
      "var boolean res;\n"
      "let res = ((other.getX() = x) & (other.getY() = y));\n"
      "return res;\n"
    "}\n"
  "}"
  );
  init(stream, "equals");

  const std::vector<Symbol> expected {
    {.name="name", .type="char", .kind=SymbolKind::STATIC, .index=0},
    {.name="x", .type="int", .kind=SymbolKind::FIELD, .index=0},
    {.name="y", .type="int", .kind=SymbolKind::FIELD, .index=1},
    {.name="this", .type="Point", .kind=SymbolKind::ARG, .index=0},
    {.name="other", .type="Point", .kind=SymbolKind::ARG, .index=1},
    {.name="res", .type="boolean", .kind=SymbolKind::VAR, .index=0},
  };

  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::VAR), 1);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::ARG), 2); // including "this"
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::STATIC), 1);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::FIELD), 2);
  check(expected);
}

BOOST_FIXTURE_TEST_CASE(test_function_no_field_access, fixture) {
  istringstream stream(
  "class Point {\n"
    "static char name;\n"
    "field int x;\n"
    "function void test() { }\n"
  "}"
  );
  init(stream, "test");

  const std::vector<Symbol> expected {
    {.name="name", .type="char", .kind=SymbolKind::STATIC, .index=0},
  };

  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::VAR), 0);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::ARG), 0);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::STATIC), 1);
  BOOST_CHECK_EQUAL(symbol_table.count(SymbolKind::FIELD), 0);
  check(expected);
  // can't access a field from a function
  BOOST_CHECK(!symbol_table.get("x"));
}
