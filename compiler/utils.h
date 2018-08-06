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
template <typename T> std::string toString(T n);

// filesystem stuff

#ifdef _WIN32
constexpr char PATHSEP = '\\';
#else
constexpr char PATHSEP = '/';
#endif

std::string getExtension(const std::string &path);
std::string replaceExtension(const std::string &path, const std::string &newExt);
std::string getFilename(const std::string &path);
std::string getStem(const std::string &path);
std::string joinPaths(const std::string &path, const std::string &filename);

// string manipulations

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);
std::string ltrim_copy(std::string s);
std::string rtrim_copy(std::string s);
std::string trim_copy(std::string s);

void rstrip(std::string &s, const std::string &chars);
void lstrip(std::string &s, const std::string &chars);
void strip(std::string &s, const std::string &chars);
std::string rstrip_copy(std::string s, const std::string &chars);
std::string lstrip_copy(std::string s, const std::string &chars);
std::string strip_copy(std::string s, const std::string &chars);

template <class ContainerT>
std::string join(const ContainerT &parts, const std::string &delim);
template <class ContainerT>
void split(ContainerT &parts, const std::string &line, const std::string &delim);

#endif
