#include <cstdlib>
#include <iostream>
#include <string>

#include "assembler.h"
#include "parser.h"

/* Whole architecture:
 * - class Assembler(inputFile="sth.asm", outputExt="", parserList)
 *   - List<Parser> parsers;
 *   - void assemble()
 * - abstract class Parser
 *   - void init(string)
 *   - string getResult()
 *   - Instruction* parseLine()
 *   |
 *   |- class HackParser
 *      - parseAInstruction()
 *      - parseCInstruction()
 *      - parseLabel()
 *      - parseComment()
 *      |
 *      |- class HackErrorChecker
 *      |- class HackSymbolTranslator
 *      |- class HackBinaryTranslator
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
  std::cout << filename << '\n';

  std::list<Parser> parsers = {};

  Assembler hack(filename, ".hack", &parsers);
  hack.assemble();

  return 0;
}
