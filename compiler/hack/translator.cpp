#include <list>
#include <string>
#include <stdexcept>
#include <vector>

#include "../translator.h"
#include "../utils.h"

#include "./translator.h"
#include "./utils.h"

HackTranslator::HackTranslator(const std::string &path)
  : Translator(path) {}

void HackTranslator::translate() {
  std::list<std::string> allLines;
  const std::list<std::string> *crtLines;
  for (const std::string &inputFile: getInputFiles()) {
    std::cout << "Translating single file " << inputFile << "\n";
    crtLines = translateFile(inputFile);
    debug("Got back", crtLines->size(), "lines");
    std::copy(
      crtLines->cbegin(), crtLines->cend(),
      std::back_inserter(allLines)
    );
    // insert empty line between them.
    allLines.push_back("");
  }
  beforeWriteToFile(allLines);
  writeToFile(allLines);
}

std::list<std::string> HackTranslator::getInputFiles() {
  std::list<std::string> output;
  PathType pathType = getPathType(_path);

  if (pathType == PathType::REG_FILE_TYPE) {
    if (getExtension(_path) != extension())
      throw std::runtime_error("Something went wrong, file given doesn't match translator's extension: " + _path);
    output.push_back(_path);
  } else if (pathType == PathType::DIR_TYPE) {
    std::vector<std::string> files;
    getDirFiles(files, _path);
    for (const std::string &f : files)
      if (getExtension(f) == extension())
        output.push_back(joinPaths(_path, f));
  } else {
    throw std::runtime_error("Not implemented logic for other file types");
  }

  return output;
}
