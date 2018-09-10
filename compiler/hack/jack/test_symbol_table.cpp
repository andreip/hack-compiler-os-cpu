#include <sstream>
#include <stdexcept>
#include <vector>

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "builder.h"
#include "tokenizer.h"
#include "grammar.h"
#include "symbol_table.h"

using namespace std;

struct fixture {
  SymbolTable symbol_table;

  void populate(std::istream &stream, std::string subroutineName="") {
    JackTokenizer tokenizer(stream);
    JackCompilationEngineBuilder builder;
    ClassElement classElement = builder.buildClass(tokenizer);
    symbol_table.populateFromClass(classElement);
    if (!subroutineName.empty())
      for (auto &subroutine: classElement.getSubroutineDecs())
        if (subroutine.getName() == subroutineName) {
          symbol_table.populateFromSubroutine(subroutine);
          break;
        }
  }

  void check(const std::vector<Symbol> &expected) {
    for (const Symbol &expected_symbol: expected)
      BOOST_CHECK_EQUAL(
        symbol_table.get(expected_symbol.name),
        expected_symbol
      );
  }
};

BOOST_FIXTURE_TEST_CASE(test_empty, fixture) {
  istringstream stream("class Test {}");
  populate(stream);

  for (SymbolKind kind: SymbolKindHelpers::ALL)
    BOOST_TEST(symbol_table.varCount(kind) == 0);
  BOOST_CHECK(bool(symbol_table.get("invalid")) == false);
}

BOOST_FIXTURE_TEST_CASE(test_class_var_decs, fixture) {
  istringstream stream(
  "class Test {\n"
    "field int x, y;\n"
    "static Point p1;\n"
    "field String z;\n"
  "}"
  );
  populate(stream);

  const std::vector<Symbol> expected {
    {.name="x", .type="int", .kind=SymbolKind::FIELD, .index=0},
    {.name="y", .type="int", .kind=SymbolKind::FIELD, .index=1},
    {.name="p1", .type="Point", .kind=SymbolKind::STATIC, .index=0},
    {.name="z", .type="String", .kind=SymbolKind::FIELD, .index=2},
  };

  check(expected);
}

BOOST_FIXTURE_TEST_CASE(test_class_var_decs_error, fixture) {
  istringstream stream(
  "class Test {\n"
    "local int x, y;\n"
  "}"
  );

  // can't have local at class level.
  BOOST_CHECK_THROW(populate(stream), std::runtime_error);
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
  populate(stream, "test");

  const std::vector<Symbol> expected {
    {.name="x", .type="char", .kind=SymbolKind::ARG, .index=0},
    {.name="y", .type="boolean", .kind=SymbolKind::ARG, .index=1},
    {.name="p1", .type="Point", .kind=SymbolKind::VAR, .index=0},
    {.name="p2", .type="Point", .kind=SymbolKind::VAR, .index=1},
  };

  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::ARG), 2);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::VAR), 2);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::STATIC), 0);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::FIELD), 0);
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
  populate(stream, "new");

  const std::vector<Symbol> expected {
    {.name="name", .type="char", .kind=SymbolKind::STATIC, .index=0},
    {.name="x", .type="int", .kind=SymbolKind::FIELD, .index=0},
    {.name="y", .type="int", .kind=SymbolKind::FIELD, .index=1},
    {.name="this", .type="Point", .kind=SymbolKind::ARG, .index=0},
    {.name="_x", .type="int", .kind=SymbolKind::ARG, .index=1},
    {.name="_y", .type="int", .kind=SymbolKind::ARG, .index=2},
  };

  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::ARG), 3);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::VAR), 0);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::STATIC), 1);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::FIELD), 2);
  check(expected);

  // check that clearing works
  symbol_table.clearSubroutineSymbols();
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::ARG), 0);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::VAR), 0);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::STATIC), 1);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::FIELD), 2);
  symbol_table.clear();
  for (auto &kind : SymbolKindHelpers::ALL)
    BOOST_CHECK_EQUAL(symbol_table.varCount(kind), 0);
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
  populate(stream, "equals");

  const std::vector<Symbol> expected {
    {.name="name", .type="char", .kind=SymbolKind::STATIC, .index=0},
    {.name="x", .type="int", .kind=SymbolKind::FIELD, .index=0},
    {.name="y", .type="int", .kind=SymbolKind::FIELD, .index=1},
    {.name="this", .type="Point", .kind=SymbolKind::ARG, .index=0},
    {.name="other", .type="Point", .kind=SymbolKind::ARG, .index=1},
    {.name="res", .type="boolean", .kind=SymbolKind::VAR, .index=0},
  };

  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::ARG), 2);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::VAR), 1);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::STATIC), 1);
  BOOST_CHECK_EQUAL(symbol_table.varCount(SymbolKind::FIELD), 2);
  check(expected);
}
