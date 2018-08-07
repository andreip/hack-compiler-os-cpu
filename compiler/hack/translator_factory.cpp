#include <list>
#include <stdexcept>

#include "../utils.h"
#include "./builder.h"
#include "./builder_vm.h"
#include "./translator_factory.h"
#include "./translator.h"

HACKTranslatorFactory::HACKTranslatorFactory(): TranslatorFactory() {}

HACKTranslatorFactory::~HACKTranslatorFactory() {
  for (Builder *b: *_builders)
    delete b;
  delete _builders;
}

Translator* HACKTranslatorFactory::getTranslator(const std::string &path) {
  std::string extension = getExtension(path);
  if (extension == "asm")
    return getAssembler(path);
  else if (extension == "vm")
    return getVMTranslator(path);
  else if (extension == "")
    return getVMTranslatorForDir(path);

  throw std::runtime_error("Unknown extension type " + extension);
}

Translator* HACKTranslatorFactory::getAssembler(const std::string &path) {
  _builders = new std::list<Builder*> {new HackSymbolTranslator(path), new HackBinaryTranslator(path)};
  // outputs in same directory as the input.
  std::string newPath = replaceExtension(path, ".hack");
  return new HackTranslator(path, newPath, _builders);
}

Translator* HACKTranslatorFactory::getVMTranslator(const std::string &path) {
  _builders = new std::list<Builder*> {new HackBuilderVMTranslator(path)};
  // outputs in same directory as the input.
  std::string newPath = replaceExtension(path, ".asm");
  return new HackTranslator(path, newPath, _builders);
}

Translator* HACKTranslatorFactory::getVMTranslatorForDir(const std::string &path) {
  _builders = new std::list<Builder*> {new HackBuilderVMTranslator};

  // gather all .vm files from given directory path/
  std::vector<std::string> files, vmfiles;
  getDirFiles(files, path);
  for (const std::string &f : files)
    if (getExtension(f) == "vm")
      vmfiles.push_back(joinPaths(path, f));

  // form name of output file where we'll output all .asm code of the
  // .vm files.
  std::string filename = getFilename(path) + ".asm";
  std::string outputPath = joinPaths(path, filename);
  std::cout << "Output file " << outputPath << '\n';

  return new HackTranslator(vmfiles, outputPath, _builders);
}
