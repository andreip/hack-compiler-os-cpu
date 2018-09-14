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
    VMCommands::Push("argument", 0),
    VMCommands::Push("constant", 1),
    VMCommands::ArithmeticLogic("add"),
    VMCommands::Pop("argument", 0),
    // return x;
    VMCommands::Push("argument", 0),
    VMCommands::Return(),
  };

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_let_statement_str_constant, fixture) {
  istringstream stream(
  "class Test {\n"
    "function int test(String x) {\n"
      "let x = \"abc\";\n"
      "return x;\n"
    "}\n"
  "}"
  );
  expected = {
    // function
    VMCommands::Function("Test.test", 0),
    // let x = "abc"
    VMCommands::Push("constant", 3),
    VMCommands::Call("String.new", 1),
    VMCommands::Push("constant", 'a'),
    VMCommands::Call("String.appendChar", 2),
    VMCommands::Push("constant", 'b'),
    VMCommands::Call("String.appendChar", 2),
    VMCommands::Push("constant", 'c'),
    VMCommands::Call("String.appendChar", 2),
    VMCommands::Pop("argument", 0),
    // return x;
    VMCommands::Push("argument", 0),
    VMCommands::Return(),
  };

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_do_statement, fixture) {
  istringstream stream(
  "class Test {\n"
    "function void print(String msg) {\n"
      "do Output.printString(msg);\n"
      "return;\n"
    "}\n"
  "}"
  );
  expected = {
    // function
    VMCommands::Function("Test.print", 0),
    // do Output.printString(msg);
    VMCommands::Push("argument", 0),
    VMCommands::Call("Output.printString", 1),
    VMCommands::Pop("temp", 0),
    // return x;
    VMCommands::Push("constant", 0),
    VMCommands::Return(),
  };

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_function_cannot_access_fields, fixture) {
  istringstream stream(
  "class Point {\n"
    "field int x, y;"
    "field int getX() { return x; }\n"
  "}"
  );

  BOOST_CHECK_THROW(compute_vmcode(stream), std::runtime_error);
}

BOOST_FIXTURE_TEST_CASE(test_method_this_setup, fixture) {
  istringstream stream(
  "class Point {\n"
    "field int x, y;"
    "method int getX() { return x; }\n"
  "}"
  );
  expected = {
    VMCommands::Function("Point.getX", 0),
    VMCommands::Push("argument", 0),
    VMCommands::Pop("pointer", 0),      // sets base ptr THIS
    VMCommands::Push("this", 0),
    VMCommands::Return(),               // return x;
  };

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_constructor_this_setup, fixture) {
  istringstream stream(
  "class Point {\n"
    "field int x, y;"
    "constructor Point new(int _x, int _y) {\n"
      "let x = _x;\n"
      "let y = _y;\n"
      "return this;\n"
    "}\n"
  "}"
  );
  expected = {
  // constructor Point new
    VMCommands::Function("Point.new", 0),
    VMCommands::Push("constant", 2),
    VMCommands::Call("Memory.alloc", 1),
    VMCommands::Pop("pointer", 0),          // sets base ptr THIS = Memory.alloc(2)
    VMCommands::Push("argument", 0),
    VMCommands::Pop("this", 0),     // let x = _x;
    VMCommands::Push("argument", 1),
    VMCommands::Pop("this", 1),     // let y = _y;
    VMCommands::Push("pointer", 0),
    VMCommands::Return(),           // return this;
  };

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_do_statement_methods_and_functions, fixture) {
  istringstream stream(
  "class Test {\n"
    "constructor Test new() {\n"
      "do init(42);\n"
      "return this;\n"
    "}\n"
    "method void init(int x) {\n"
      "do Test.print(\"init\");\n"
      "return;\n"
    "}\n"
    "function void print(String x) {\n"
      "do Output.printString(x);\n"
      "return;\n"
    "}\n"
  "}"
  );
  expected = {
  // constructor Test new()
    VMCommands::Function("Test.new", 0),
    VMCommands::Push("constant", 1),
    VMCommands::Call("Memory.alloc", 1),
    VMCommands::Pop("pointer", 0),
    VMCommands::Push("pointer", 0),     // push this
    VMCommands::Push("constant", 42),
    VMCommands::Call("Test.init", 2),   // Test.init(this, 42)
    VMCommands::Pop("temp", 0),
    VMCommands::Push("pointer", 0),
    VMCommands::Return(),               // return this

  // method void init
    VMCommands::Function("Test.init", 0),
    VMCommands::Push("argument", 0),
    VMCommands::Pop("pointer", 0),            // setup THIS from 1st arg
    VMCommands::Push("constant", 4),
    VMCommands::Call("String.new", 1),
    VMCommands::Push("constant", 'i'),
    VMCommands::Call("String.appendChar", 2),
    VMCommands::Push("constant", 'n'),
    VMCommands::Call("String.appendChar", 2),
    VMCommands::Push("constant", 'i'),
    VMCommands::Call("String.appendChar", 2),
    VMCommands::Push("constant", 't'),
    VMCommands::Call("String.appendChar", 2),  // "init" base addr on stack
    VMCommands::Call("Test.print", 1),
    VMCommands::Pop("temp", 0),
    VMCommands::Push("constant", 0),
    VMCommands::Return(),                     // return;

  // function void print
    VMCommands::Function("Test.print", 0),
    VMCommands::Push("argument", 0),
    VMCommands::Call("Output.printString", 1),  // Output.printString(x)
    VMCommands::Pop("temp", 0),                 // do ...
    VMCommands::Push("constant", 0),
    VMCommands::Return(),                       // return;
  };

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_call_methods_on_objects, fixture) {
  istringstream stream(
  "class Point {\n"
    "field int x, y;"
    "constructor Point new(int _x, int _y) {\n"
      "let x = _x;\n"
      "let y = _y;\n"
      "return this;\n"
    "}\n"
    "method Point add(Point other) {\n"
      "var Point res;\n"
      "let res = Point.new(x + other.getX(), y + other.getY());\n"
      "return res;\n"
    "}\n"
    "method int getX() { return x; }\n"
    "method int getY() { return y; }\n"
  "}"
  );
  expected = {
  // constructor Point new
    VMCommands::Function("Point.new", 0),
    VMCommands::Push("constant", 2),
    VMCommands::Call("Memory.alloc", 1),
    VMCommands::Pop("pointer", 0),          // sets base ptr THIS = Memory.alloc(2)
    VMCommands::Push("argument", 0),
    VMCommands::Pop("this", 0),     // let x = _x;
    VMCommands::Push("argument", 1),
    VMCommands::Pop("this", 1),     // let y = _y;
    VMCommands::Push("pointer", 0),
    VMCommands::Return(),           // return this;

  // method Point add
    VMCommands::Function("Point.add", 1),
    VMCommands::Push("argument", 0),
    VMCommands::Pop("pointer", 0),      // sets base ptr THIS = first_arg
    VMCommands::Push("this", 0),        // push x
    VMCommands::Push("argument", 1),    // push other
    VMCommands::Call("Point.getX", 1),  // calls other.getX()
    VMCommands::ArithmeticLogic(Op::ADD), // x + other.getX()
    VMCommands::Push("this", 1),        // push y
    VMCommands::Push("argument", 1),
    VMCommands::Call("Point.getY", 1),  // calls other.getY()
    VMCommands::ArithmeticLogic(Op::ADD), // y + other.getY()
    VMCommands::Call("Point.new", 2),   // calls Point.new(...)
    VMCommands::Pop("local", 0),        // let res = ...
    VMCommands::Push("local", 0),
    VMCommands::Return(),               // return res;

  // method int getX
    VMCommands::Function("Point.getX", 0),
    VMCommands::Push("argument", 0),
    VMCommands::Pop("pointer", 0),      // sets base ptr THIS
    VMCommands::Push("this", 0),
    VMCommands::Return(),               // return x;

  // method int getY
    VMCommands::Function("Point.getY", 0),
    VMCommands::Push("argument", 0),
    VMCommands::Pop("pointer", 0),      // sets base ptr THIS
    VMCommands::Push("this", 1),
    VMCommands::Return(),               // return y;
  };

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_while_statement, fixture) {
  istringstream stream(
  "class Test {\n"
    "function void test(int x) {\n"
      "while (x > 0) {\n"
        "do Output.printInt(x);\n"
        "let x = x - 1;\n"
      "}\n"
      "return;\n"
    "}\n"
  "}"
  );
  std::string L0 = "WHILE_EXP0";
  std::string L1 = "WHILE_END1";
  expected = {
    VMCommands::Function("Test.test", 0),
    VMCommands::Label(L0),                      // L0
    VMCommands::Push("argument", 0),
    VMCommands::Push("constant", 0),
    VMCommands::ArithmeticLogic(Op::GT),
    VMCommands::ArithmeticLogic(UnaryOp::NOT),  // not <expr>
    VMCommands::IfGoto(L1),                     // if-goto L1
    VMCommands::Push("argument", 0),
    VMCommands::Call("Output.printInt", 1),
    VMCommands::Pop("temp", 0),                 // do Output.printInt(x);
    VMCommands::Push("argument", 0),
    VMCommands::Push("constant", 1),
    VMCommands::ArithmeticLogic(Op::SUB),
    VMCommands::Pop("argument", 0),             // let x = x - 1;
    VMCommands::Goto(L0),                       // goto L0
    VMCommands::Label(L1),                      // L1
    VMCommands::Push("constant", 0),
    VMCommands::Return(),                       // return;
  };

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_if_statement, fixture) {
  istringstream stream(
  "class Test {\n"
    "function void test(int x, int y) {\n"
      "if (x > 0) {\n"
        "do Output.printInt(x);\n"
      "} else {\n"
        "do Output.printInt(y);\n"
      "}\n"
      "return;\n"
    "}\n"
  "}"
  );
  std::string L0 = "IF_FALSE2";
  std::string L1 = "IF_TRUE3";
  expected = {
    VMCommands::Function("Test.test", 0),
    VMCommands::Push("argument", 0),
    VMCommands::Push("constant", 0),
    VMCommands::ArithmeticLogic(Op::GT),
    VMCommands::ArithmeticLogic(UnaryOp::NOT),  // not <expr>
    VMCommands::IfGoto(L0),                     // if-goto L1
    VMCommands::Push("argument", 0),
    VMCommands::Call("Output.printInt", 1),
    VMCommands::Pop("temp", 0),                 // do Output.printInt(x);
    VMCommands::Goto(L1),                       // goto L1
    VMCommands::Label(L0),                      // L0
    VMCommands::Push("argument", 1),
    VMCommands::Call("Output.printInt", 1),
    VMCommands::Pop("temp", 0),                 // do Output.printInt(y);
    VMCommands::Goto(L1),                       // goto L1
    VMCommands::Label(L1),                      // L1
    VMCommands::Push("constant", 0),
    VMCommands::Return(),                       // return;
  };

  compute_vmcode(stream);
}

BOOST_FIXTURE_TEST_CASE(test_array, fixture) {
  istringstream stream(
  "class Main {\n"
    "function void main() {\n"
      "var Array a;\n"
      "let a = Array.new(2);\n"
      "let a[0] = 1;\n"
      "let a[1] = a[0] + 2;\n"
      "do Output.printInt(a[1]);\n"
      "return;\n"
    "}\n"
  "}"
  );
  expected = {
    VMCommands::Function("Main.main", 1),
    VMCommands::Push("constant", 2),
    VMCommands::Call("Array.new", 1),
    VMCommands::Pop("local", 0),
    // let a[0] = 1;
    VMCommands::Push("constant", 1),
    VMCommands::Pop("temp", 0),         // puts rhs in temp0
    VMCommands::Push("local", 0),
    VMCommands::Push("constant", 0),
    VMCommands::ArithmeticLogic(Op::ADD),   // array + index
    VMCommands::Pop("pointer", 1),      // puts THAT on array + index
    VMCommands::Push("temp", 0),
    VMCommands::Pop("that", 0),         // *THAT = rhs
    // let a[1] = a[0] + 2;
    VMCommands::Push("local", 0),
    VMCommands::Push("constant", 0),
    VMCommands::ArithmeticLogic(Op::ADD), // a + 0 offset
    VMCommands::Pop("pointer", 1),
    VMCommands::Push("that", 0),          // a[0] push
    VMCommands::Push("constant", 2),
    VMCommands::ArithmeticLogic(Op::ADD),
    VMCommands::Pop("temp", 0),           // temp0 = a[0] + 2
    VMCommands::Push("local", 0),
    VMCommands::Push("constant", 1),
    VMCommands::ArithmeticLogic(Op::ADD), // a + 1 offset
    VMCommands::Pop("pointer", 1),        // set THAT = a + 1
    VMCommands::Push("temp", 0),
    VMCommands::Pop("that", 0),           // let a[1] = temp0
    // do Output.printInt(a[1]);
    VMCommands::Push("local", 0),
    VMCommands::Push("constant", 1),
    VMCommands::ArithmeticLogic(Op::ADD),
    VMCommands::Pop("pointer", 1),
    VMCommands::Push("that", 0),          // push a[1]
    VMCommands::Call("Output.printInt", 1),
    VMCommands::Pop("temp", 0),
    // return;
    VMCommands::Push("constant", 0),
    VMCommands::Return(),
  };

  compute_vmcode(stream);
}
