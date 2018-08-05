#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>

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

// filesystem stuff

std::string getExtension(const std::string &path) {
  // "filename.asm" => ".asm"
  // "filename.asm.ast" => ".ast" (who uses two extensions anyway?)
  // "dir/" => "dir/"
  auto it = std::find(path.crbegin(), path.crend(), '.').base();
  std::string ext = std::string(it, path.cend());
  return ext;
}

std::string replaceExtension(const std::string &path, const std::string &newExt) {
  auto it = std::prev(std::find(path.crbegin(), path.crend(), '.').base());
  if (it == path.cend())
    throw std::runtime_error("Path " + path + " has no extension to replace.\n");

  std::string newExtWithDot = newExt;
  if (std::find(newExt.cbegin(), newExt.cend(), '.') == newExt.cend())
    newExtWithDot = "." + newExt;

  std::string pathWithoutExt = std::string(path.cbegin(), it);
  return pathWithoutExt + newExtWithDot;
}

std::string getFilename(const std::string &path) {
  std::string s(
    // find right-most separator char using a reverse iterator,
    // then get back forward iterator for std::string constructor.
    std::find_if(path.rbegin(), path.rend(),
                 [](char ch) { return ch == '\\' || ch == '/'; }
                ).base(),
    path.cend()
  );

  std::cout << "Got filename " << s << " from path " << path << "\n";
  return s;
}

std::string getStem(const std::string &path) {
  return replaceExtension(getFilename(path), "");
}

// string trimming
