#include <stdexcept>
#include <string>

#include "../../utils.h"

#include "./builder.h"
#include "./translator.h"

VMHackTranslator::VMHackTranslator(const std::string &path)
  : HackTranslator(path) {
  _builders.push_back(new HackBuilderVMTranslator(path));
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
