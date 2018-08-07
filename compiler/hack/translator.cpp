#include <list>
#include <string>

#include "./translator.h"
#include "./utils.h"

#include "../translator.h"
#include "../utils.h"

HackTranslator::HackTranslator(std::string inputFile,
                               std::string outputFile,
                               std::list<Builder*> *builders)
  : Translator(inputFile, outputFile, builders) {}

HackTranslator::HackTranslator(const std::vector<std::string> &inputFiles,
                               std::string outputFile,
                               std::list<Builder*> *builders)
  : Translator(inputFiles, outputFile, builders) {}

std::list<std::string>* HackTranslator::translateFile(const std::string &inputFile) {
  std::list<std::string> *out = Translator::translateFile(inputFile);
  std::string fileId = "file: " + getFilename(inputFile);
  out->push_front(getComment(fileId));
  return out;
}
