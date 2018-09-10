#include <sstream>
#include <stdexcept>

#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include "builder.h"
#include "tokenizer.h"
#include "grammar.h"
#include "symbol_table.h"

using namespace std;

BOOST_AUTO_TEST_CASE(test_empty) {
  istringstream stream("class Test {}");
  JackTokenizer tokenizer(stream);
  JackCompilationEngineBuilder builder;
  ClassElement classElement = builder.buildClass(tokenizer);
  SymbolTable symbolTable(classElement);

  for (SymbolKind kind: SymbolKindAll)
    BOOST_TEST(symbolTable.varCount(kind) == 0);
  BOOST_CHECK_THROW(symbolTable.indexOf("invalid"), std::runtime_error);
}
