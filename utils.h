#ifndef __UTILS__H__
#define __UTILS__H__

#include <iostream>
#include <string>
#include <unordered_map>

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

#endif
