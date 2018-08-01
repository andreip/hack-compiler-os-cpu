#include <algorithm>
#include <string>

#include <boost/algorithm/string/trim.hpp>

#include "./utils.h"

std::string trimComment(const std::string &line, const std::string &comment) {
  // Removes comment out of a line with actual code information.
  std::string trimmed = line;  // clone it
  auto it = std::search(trimmed.begin(), trimmed.end(), comment.begin(), comment.end());
  trimmed.erase(it, trimmed.end());  // up to the end.
  return trimmed;
}

std::string getComment(const std::string &line, const std::string &comment) {
  return comment + " " + line;
}

