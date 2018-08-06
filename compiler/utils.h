#ifndef __UTILS__H__
#define __UTILS__H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

template <typename A, typename B>
std::ostream& operator<<(std::ostream &out, const std::unordered_map<A, B> &map) {
  out << "{";
  for (const auto &it : map)
    out << it.first << ": " << it.second << ", ";
  out << "}";
  return out;
}

bool isNumber(const std::string &s);
int getNumber(const std::string &s);
std::string toString(int n);

// filesystem stuff

std::string getExtension(const std::string &path);
std::string replaceExtension(const std::string &path, const std::string &newExt);
std::string getFilename(const std::string &path);
std::string getStem(const std::string &path);

// string manipulations

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);
std::string ltrim_copy(std::string s);
std::string rtrim_copy(std::string s);
std::string trim_copy(std::string s);

std::string join(const std::vector<std::string> &parts, const std::string &delim);
void split(std::vector<std::string> &parts, const std::string &line, const std::string &delim);

#endif
