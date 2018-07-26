#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include "assembler.h"
#include "builder.h"

#include "hack/builder.h"

/* Whole architecture:
 * - class Assembler(inputFile="sth.asm", outputExt="", builderList)
 *   - List<Builder> builders;
 *   - void assemble()
 * - abstract class Builder
 *   - void init(string)
 *   - string getResult()
 *   - Instruction* parseLine()
 *   |
 *   |- class HackBuilder
 *      - parseAInstruction()
 *      - parseCInstruction()
 *      - parseLabel()
 *      - parseComment()
 *      |
 *      |- class HackErrorChecker
 *      |- class HackSymbolTranslator
 *      |- class HackHackBinaryTranslator
 *
 * - abstract class Instruction
 *   - string toString()
 *   - bool isValid()
 *   - string toBinary()
 *   |
 *   |- class Comment
 *   |- class Label
 *      - string name()
 *   |- class AInstruction
 *      - string value()
 *      - setValue(string)
 *   |- class CInstruction
 */

void usage() {
  std::cout << "Usage:\n\t./exec file.asm\n";
}

int main(int argc, char **argv) {
  if (argc != 2) {
    usage();
    std::exit(1);
  }

  std::string filename(argv[1]);
  std::list<Builder*> builders {new HackSymbolTranslator, new HackBinaryTranslator};

  Assembler hack(filename, ".hack", &builders);
  try {
    hack.assemble();
  } catch (std::runtime_error &e) {
    std::cout << e.what();
  }

  return 0;
}
