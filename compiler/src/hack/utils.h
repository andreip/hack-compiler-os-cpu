#ifndef __HACK__UTILS__H__
#define __HACK__UTILS__H__

#include <string>

std::string trimComment(const std::string &line, const std::string &comment="//");
std::string getComment(const std::string&, const std::string &comment="//");

#endif
