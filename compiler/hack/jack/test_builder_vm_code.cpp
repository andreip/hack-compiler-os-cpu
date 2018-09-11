#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "builder.h"
#include "grammar.h"
#include "tokenizer.h"
#include "symbol_table.h"
#include "vm_commands.h"

using namespace std;

struct fixture {
  std::vector<std::string> expected;
  std::vector<std::string> actual;

  void compute_vmcode(std::istream &stream) {
    JackTokenizer tokenizer(stream);
    JackCompilationEngineBuilder builder;
    SymbolTable symbol_table;
    actual = builder.buildClass(tokenizer).toVMCode(symbol_table);
  }

  ~fixture() {
    std::cout << "Expected: " << expected << '\n';
    std::cout << "Actual: " << actual << '\n';
    BOOST_CHECK_EQUAL_COLLECTIONS(
      expected.begin(), expected.end(),
      actual.begin(), actual.end()
    );
  }
};

BOOST_FIXTURE_TEST_CASE(test_empty_class, fixture) {
  istringstream stream("class Test { }");
  expected = {};

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_let_statement, fixture) {
  istringstream stream(
  "class Test {\n"
    "function int test(int x) {\n"
      "let x = x + 1;\n"
      "return x;\n"
    "}\n"
  "}"
  );
  expected = {
    // function
    VMCommands::Function("Test.test", 0),
    // let x = x + 1;
    VMCommands::Push("argument", "0"),
    VMCommands::Push("constant", "1"),
    VMCommands::ArithmeticLogic("add"),
    VMCommands::Pop("argument", "0"),
    // return x;
    VMCommands::Push("argument", "0"),
    VMCommands::Return(),
  };

  compute_vmcode(stream);
}
