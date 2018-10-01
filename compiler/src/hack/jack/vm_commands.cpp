#include <string>

#include "generic/utils.h"

#include "./vm_commands.h"
#include "./grammar.h"

const std::vector<std::string> VMCommands::Segments = {
  "constant", "argument", "local", "this",
  "that", "static", "temp", "pointer"
};

std::string VMCommands::UniqueLabel(std::string prefix) {
  static int n = 0;
  return prefix + toString(n++);
}

std::string VMCommands::Function(std::string name, int nLocals) {
  return "function " + name + " " + toString(nLocals);
}

std::string VMCommands::ArithmeticLogic(std::string name) { return name; }

std::string VMCommands::ArithmeticLogic(Op op) {
  if (op == Op::ADD) return ArithmeticLogic("add");
  if (op == Op::SUB) return ArithmeticLogic("sub");
  if (op == Op::MUL) return Call("Math.multiply", 2);
  if (op == Op::DIV) return Call("Math.divide", 2);
  if (op == Op::AND) return ArithmeticLogic("and");
  if (op == Op::OR) return ArithmeticLogic("or");
  if (op == Op::LT) return ArithmeticLogic("lt");
  if (op == Op::GT) return ArithmeticLogic("gt");
  if (op == Op::EQ) return ArithmeticLogic("eq");
  throw_and_debug("VMCommands::ArithmeticLogic: Unknown operation");
}

std::string VMCommands::ArithmeticLogic(UnaryOp op) {
  if (op == UnaryOp::NEG) return ArithmeticLogic("neg");
  if (op == UnaryOp::NOT) return ArithmeticLogic("not");
  throw_and_debug("VMCommands::ArithmeticLogic: Unknown unary operation");
}

std::string VMCommands::Goto(std::string label) { return "goto " + label; }

std::string VMCommands::IfGoto(std::string label) { return "if-goto " + label; }

std::string VMCommands::Label(std::string label) { return "label " + label; }

std::string VMCommands::Push(std::string segment, std::string val) {
  if (!in_array(segment, Segments))
    throw_and_debug("Invalid segment " + segment);
  return "push " + segment + " " + val;
}

std::string VMCommands::Push(std::string segment, int val) {
  return Push(segment, toString(val));
}

std::string VMCommands::Pop(std::string segment, std::string val) {
  if (!in_array(segment, Segments))
    throw_and_debug("Invalid segment " + segment);
  return "pop " + segment + " " + val;
}

std::string VMCommands::Pop(std::string segment, int val) {
  return Pop(segment, toString(val));
}

std::string VMCommands::Return() { return "return"; }

std::string VMCommands::Call(std::string name, int nArgs) {
  return "call " + name + " " + toString(nArgs);
}
