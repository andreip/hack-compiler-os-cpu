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

template <typename T>
std::string toString(T n) {
  std::ostringstream oss;
  oss << n;
  return oss.str();
}

template std::string toString<int>(int);
template std::string toString<char>(char);

// filesystem stuff

/*
 * "filename.asm" => ".asm"
 * "filename.asm.ast" => ".ast" (who uses two extensions anyway?)
 * "dir/" => ""
 */
std::string getExtension(const std::string &path) {
  // best to first get filename, since it could be a path that has
  // dot in it, like ./dir/ which might confuse the extension finder.
  std::string filename = getFilename(path);
  auto it = std::find(filename.crbegin(), filename.crend(), '.').base();
  // no extension
  if (it == filename.cbegin())
    return "";

  std::string ext = std::string(it, filename.cend());
  return ext;
}

std::string replaceExtension(const std::string &path, const std::string &newExt) {
  if (getExtension(path).empty())
    throw std::runtime_error("Path " + path + " has no extension to replace.\n");

  auto it = std::prev(std::find(path.crbegin(), path.crend(), '.').base());
  std::string pathWithoutExt = std::string(path.cbegin(), it);

  std::string newExtWithDot = newExt;
  if (std::find(newExt.cbegin(), newExt.cend(), '.') == newExt.cend())
    newExtWithDot = "." + newExt;

  return pathWithoutExt + newExtWithDot;
}

std::string getFilename(const std::string &path) {
  std::string p = rstrip_copy(path, toString(PATHSEP));

  std::string filename(
    // find right-most separator char using a reverse iterator,
    // then get back forward iterator for std::string constructor.
    std::find_if(p.rbegin(), p.rend(),
                 [](char ch) { return ch == PATHSEP; }
                ).base(),
    p.end()
  );

  return filename;
}

std::string getStem(const std::string &path) {
  return replaceExtension(getFilename(path), "");
}

std::string joinPaths(const std::string &path, const std::string &filename) {
  std::string sep = toString(PATHSEP);
  return (
    rstrip_copy(path, sep) +
    sep +
    filename
  );
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

void lstrip(std::string &s, const std::string &chars) {
  bool stripped = true;
  while (stripped) {
    stripped = false;
    for (const int &c : chars) {
      if (s.front() == c) {
        s.erase(s.front());
        stripped = true;
      }
    }
  }
}

void rstrip(std::string &s, const std::string &chars) {
  bool stripped = true;
  while (stripped) {
    stripped = false;
    for (const int &c : chars) {
      if (s.back() == c) {
        s.erase(std::prev(s.end()));
        stripped = true;
      }
    }
  }
}

void strip(std::string &s, const std::string &chars) {
  lstrip(s, chars);
  rstrip(s, chars);
}

std::string lstrip_copy(std::string s, const std::string &chars) {
  lstrip(s, chars);
  return s;
}

std::string rstrip_copy(std::string s, const std::string &chars) {
  rstrip(s, chars);
  return s;
}

std::string strip_copy(std::string s, const std::string &chars) {
  strip(s, chars);
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
