#ifndef __VM_COMMANDS__H__
#define __VM_COMMANDS__H__

#include <cstdio>
#include <string>

enum class Op;
enum class UnaryOp;

class VMCommands {
public:
  static std::string UniqueLabel(std::string prefix="L");
  static std::string Function(std::string name, int nLocals);
  static std::string ArithmeticLogic(std::string name);
  static std::string ArithmeticLogic(Op);
  static std::string ArithmeticLogic(UnaryOp);
  static std::string Goto(std::string label);
  static std::string IfGoto(std::string label);
  static std::string Label(std::string label);
  static std::string Push(std::string segment, std::string val);
  static std::string Push(std::string segment, int val);
  static std::string Pop(std::string segment, std::string val);
  static std::string Pop(std::string segment, int val);
  static std::string Return();
  static std::string Call(std::string name, int nArgs);

public:
  static const std::vector<std::string> Segments;
};

#endif
