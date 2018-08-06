#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>

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

// string manipulations

void ltrim(std::string &s) {
  s.erase(
    s.cbegin(),
    std::find_if(
      s.begin(), s.end(),
      [](char ch) { return !std::isspace(ch); }
    )
  );
}

void rtrim(std::string &s) {
  s.erase(
    std::find_if(
      s.rbegin(), s.rend(),
      [](char ch) { return !std::isspace(ch); }
    ).base(),
    s.end()
  );
}

void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

std::string ltrim_copy(std::string s) {
  ltrim(s);
  return s;
}

std::string rtrim_copy(std::string s) {
  rtrim(s);
  return s;
}

std::string trim_copy(std::string s) {
  trim(s);
  return s;
}

std::string join(const std::vector<std::string> &parts, const std::string &delim) {
  if (parts.size() == 0)
    return "";

  std::ostringstream oss;
  for (auto it = parts.cbegin(); it != parts.cend(); ++it) {
    oss << *it;
    if (std::next(it) != parts.cend())
      oss << delim;
  }

  return oss.str();
}

void split(std::vector<std::string> &parts, const std::string &line, const std::string &delim) {
  size_t last = 0;
  size_t next = 0;
  while ((next = line.find(delim, last)) != std::string::npos) {
    parts.push_back(line.substr(last, next - last));
    last = next + delim.size();
  }
  parts.push_back(line.substr(last, next - last));
}
