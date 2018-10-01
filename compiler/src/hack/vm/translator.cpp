#include <list>
#include <stdexcept>
#include <string>
#include <vector>

#include "generic/utils.h"

#include "./builder.h"
#include "./instruction.h"
#include "./translator.h"

Translator* getTranslatorFromPath(const std::string &path) {
  return new VMHackTranslator(path);
}

VMHackTranslator::VMHackTranslator(const std::string &path)
  : HackTranslator(path) {
  _builders.push_back(new HackBuilderVMTranslator(path));
}

std::list<std::string>* VMHackTranslator::translateFile(const std::string &path) {
  std::list<std::string> *out = Translator::translateFile(path);
  // add a line at the beginning to know what file generated the output.
  out->push_front(getComment("file: " + getFilename(path)));
  return out;
}

std::string VMHackTranslator::getOutputFile() {
  PathType pathType = getPathType(_path);
  if (pathType == PathType::REG_FILE_TYPE) {
    return replaceExtension(_path, "asm");
  } else if (pathType == PathType::DIR_TYPE) {
    // all .vm files will go into a dirname/<dirname>.asm file
    std::string filename = getFilename(_path) + ".asm";
    return joinPaths(_path, filename);
  }

  throw std::runtime_error("Not implemented logic for other file types");
}

std::string VMHackTranslator::extension() { return "vm"; }

void VMHackTranslator::beforeWriteToFile(std::list<std::string> &lines) {
  HackTranslator::beforeWriteToFile(lines);
  // calls bootstrap code only if path is a directory. Convention
  // of the instructors, so individual files can be tested in isolation
  // w/o having to worry about the whole integration. But when we want
  // to compile a directory, then we need to bootstrap the code somehow
  // to call Sys.init.
  if (getPathType(_path) == PathType::DIR_TYPE) {
    std::list<std::string> code = getBootstrapCode();
    lines.insert(lines.cbegin(), code.cbegin(), code.cend());
  }
}

std::list<std::string> VMHackTranslator::getBootstrapCode() {
  // first add registers initialization
  std::list<std::string> bootstrapCode {
    "// bootstrap code",
    "@256",
    "D=A",
    "@SP",
    "M=D    // *SP = 256",
    "@LCL",
    "M=-1   // *LCL = -1, illegal value to start with",
    "@ARG",
    "M=-1   // *ARG = -1",
    "@THIS",
    "M=-1   // *THIS = -1",
    "@THAT",
    "M=-1   // *THAT = -1",
  };

  CallInstruction sys_init_call("Sys.init", 0);
  std::vector<std::string> tmp = sys_init_call.doTranslate();
  bootstrapCode.push_back(getComment(sys_init_call.toString()));
  bootstrapCode.insert(
    bootstrapCode.cend(),
    tmp.cbegin(), tmp.cend()
  );

  return bootstrapCode;
}
