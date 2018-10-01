#include <algorithm>
#include <cctype>
#include <deque>
#include <dirent.h>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdexcept>
#include <iostream>
#include <unistd.h>
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
  // if there's a dot somewhere in the extension, we'll assume
  // it's already prepared for concatenation and caller knows what it's
  // doing. The reason to not check for dot at the beginning is that
  // sometimes you might want to change an extension from a.txt to
  // something like aXX.gov so new extension would be XX.gov here which
  // allows for a bit more functionality, changing the extension with a
  // new suffix. Probably not the cleanest, but actually used scenario.
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
  std::string filename = getFilename(path);

  std::string stem(
    filename.cbegin(),
    std::prev(
      std::find_if(
        filename.crbegin(), filename.crend(),
        [](char ch) { return ch == '.'; }
      ).base()
    )
  );

  return stem;
}

std::string joinPaths(const std::string &path, const std::string &filename) {
  std::string sep = toString(PATHSEP);
  return (
    rstrip_copy(path, sep) +
    sep +
    filename
  );
}

// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
template <class ContainerT>
void getDirFiles(ContainerT &files, const std::string &dirPath) {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(dirPath.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      files.push_back(std::string(ent->d_name));
    }
    closedir(dir);
  } else {
    throw std::runtime_error("Cannot open dir " + dirPath);
  }
}

template void getDirFiles<std::vector<std::string>>(std::vector<std::string>&, const std::string&);

// https://stackoverflow.com/questions/146924/how-can-i-tell-if-a-given-path-is-a-directory-or-a-file-c-c
PathType getPathType(const std::string &path) {
  struct stat s;
  if (stat(path.c_str(),&s) == 0) {
    if (s.st_mode & S_IFDIR)
      return PathType::DIR_TYPE;
    else if (s.st_mode & S_IFREG)
      return PathType::REG_FILE_TYPE;
    else
      throw std::runtime_error("Not implemented logic for other file types");
  } else {
    throw std::runtime_error("Path type couldn't be determined for " + path);
  }
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
      if (!s.empty() && s.front() == c) {
        s.erase(0, 1);
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
      if (!s.empty() && s.back() == c) {
        s.erase(s.size() - 1);
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

bool startsWith(const std::string &str, const std::string &prefix) {
  if (str.size() < prefix.size())
    return false;
  return prefix == str.substr(0, prefix.size());
}

template <class ContainerT>
std::string join(const ContainerT &parts, const std::string &delim) {
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

template std::string join<std::vector<std::string>>(const std::vector<std::string>&, const std::string&);

template <class ContainerT>
void split(ContainerT &parts, const std::string &line, const std::string &substr) {
  size_t last = 0;
  size_t next = 0;
  while ((next = line.find(substr, last)) != std::string::npos) {
    std::string match = line.substr(last, next - last);
    if (!match.empty())
      parts.push_back(match);
    last = next + substr.size();
  }
  parts.push_back(line.substr(last, next - last));
}

template void split<std::vector<std::string>>(std::vector<std::string>&, const std::string&, const std::string&);

template <class ContainerT>
void split_by_any_char(ContainerT &parts,
                       const std::string &line,
                       const std::string &delims,
                       bool keepSplitElements) {
  size_t last = 0;
  size_t next = 0;
  while ((next = line.find_first_of(delims, last)) != std::string::npos) {
    std::string match = line.substr(last, next - last);
    if (!match.empty())
      parts.push_back(match);
    if (keepSplitElements)
      parts.push_back(line.substr(next, 1));
    // we search for any character from delims, so we need to
    // skip by a single character.
    last = next + 1;
  }
  parts.push_back(line.substr(last, next - last));
}

template void split_by_any_char<std::deque<std::string>>(std::deque<std::string>&, const std::string&, const std::string&, bool);

void debug() {
#ifdef DEBUG
  std::cout << '\n';
#endif
}
