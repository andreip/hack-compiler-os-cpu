#ifndef __UTILS__H__
#define __UTILS__H__

#define UTILS_DEBUG

#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

bool isNumber(const std::string &s);
int getNumber(const std::string &s);
template <typename T> std::string toString(T n);

// filesystem stuff

#ifdef _WIN32
constexpr char PATHSEP = '\\';
#else
constexpr char PATHSEP = '/';
#endif

enum class PathType {
  DIR_TYPE,
  REG_FILE_TYPE,
};

std::string getExtension(const std::string &path);
std::string replaceExtension(const std::string &path, const std::string &newExt);
std::string getFilename(const std::string &path);
std::string getStem(const std::string &path);
std::string joinPaths(const std::string &path, const std::string &filename);
template <class ContainerT>
void getDirFiles(ContainerT &files, const std::string &dirPath);
PathType getPathType(const std::string &path);

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

bool startsWith(const std::string &str, const std::string &prefix);

template <class ContainerT>
std::string join(const ContainerT &parts, const std::string &delim);
template <class ContainerT>
void split(ContainerT &parts, const std::string &line, const std::string &substr);

template <class ContainerT>
void split_by_any_char(ContainerT &parts,
                       const std::string &line,
                       const std::string &delims,
                       // if to keep the elements we split by in output
                       bool keepSplitElements = false);

template <typename T>
bool in_array(const T val, const std::vector<T> &array) {
  return std::find(array.begin(), array.end(), val) != array.end();
}

// prints before throwing
#ifdef UTILS_DEBUG
#define throw_and_debug(msg) \
  do {\
    std::cerr << "Exception: " << msg << '\n'; \
    throw std::runtime_error(msg); \
  } while(0)
#else
#define throw_and_debug(msg) \
  do {\
    throw std::runtime_error(msg); \
  } while(0)
#endif

template <typename A, typename B>
std::ostream& operator<<(std::ostream &out, const std::unordered_map<A, B> &map) {
  out << "{";
  for (const auto &it : map)
    out << it.first << ": " << it.second << ", ";
  out << "}";
  return out;
}
template <class T>
std::ostream& operator<<(std::ostream &out, const std::list<T> &c) {
  out << "[";
  for (const auto &it : c)
    out << it << " ";
  out << "]";
  return out;
}
template <class T>
std::ostream& operator<<(std::ostream &out, const std::vector<T> &c) {
  out << "[";
  for (const auto &it : c)
    out << it << " ";
  out << "]";
  return out;
}

#endif
