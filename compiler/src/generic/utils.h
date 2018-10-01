#ifndef __UTILS__H__
#define __UTILS__H__

#define UTILS_DEBUG

#include <algorithm>
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

// array

template <typename T>
bool in_array(const T val, const std::vector<T> &array) {
  return std::find(array.begin(), array.end(), val) != array.end();
}

template <typename T>
std::vector<T>& concat(std::vector<T> &v1, const std::initializer_list<std::vector<T>> &&vectors) {
  for (const auto &v2: vectors)
    v1.insert(v1.end(), v2.begin(), v2.end());
  return v1;  // for chaining
}
template <typename T>
std::vector<T>& concat(std::vector<T> &v1, const std::vector<T> &v2) {
  return concat(v1, { v2 });
}

// breaks the recursion of variadic arguments call. This will
// get chosen over defining a template w/ no arguments.
void debug();

template <typename T, typename ... Args>
void debug(T first, Args ... args) {
#ifdef DEBUG
  std::cout << first << ' ';
  debug(args ...);
#endif
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
    out << it << ", ";
  out << "]";
  return out;
}

// https://stackoverflow.com/questions/18837857/cant-use-enum-class-as-unordered-map-key
struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

#endif
