#include <cctype>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include <boost/filesystem.hpp>

#include "utils.h"

bool isNumber(const std::string &s) {
  int i = 0;
  if (s[i] == '-' || s[i] == '+')
    i++;
  for (; i < s.size(); ++i)
    if (!std::isdigit(s[i]))
      return false;
  return true;
}

int getNumber(const std::string &s) {
  std::istringstream iss(s);
  int res;
  iss >> res;
  if (iss.fail()) {
    std::cout << "Couldn't convert " << s << " to number.\n";
    throw "Couldn't convert.";
  }
  return res;
}

std::string toString(int n) {
  std::ostringstream oss;
  oss << n;
  return oss.str();
}

std::string pathExtension(const std::string &filename) {
  boost::filesystem::path path(filename);
  std::string ext = path.extension().string();
  return ext;
}
