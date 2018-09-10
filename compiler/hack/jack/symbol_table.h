#ifndef __JACK_SYMBOL_TABLE__H__
#define __JACK_SYMBOL_TABLE__H__

#include <iostream>
#include <string>
#include <unordered_map>

enum class SymbolKind {
  STATIC,
  FIELD,
  VAR,  // local
  ARG,
};
const SymbolKind SymbolKindAll[] = {
  SymbolKind::STATIC, SymbolKind::FIELD, SymbolKind::VAR, SymbolKind::ARG
};

struct Symbol {
  std::string name;
  std::string type;
  SymbolKind kind;
  int index;

  bool operator==(const Symbol &other) const;
};
std::ostream& operator<<(std::ostream &out, const Symbol &s);

//struct SymbolHash {
//  std::size_t operator()(const Symbol &s) const noexcept;
//};

class ClassElement;
class SubroutineDec;

// holds class level and function level field information;
// sort of like a wrapper over two hash tables, one for each level
class SymbolTable {
public:
  SymbolTable();
  void populateFromClass(ClassElement&);
  void populateFromSubroutine(SubroutineDec&);
  void clearSubroutineSymbols();
  void clear();  // clears everything
  void define(std::string name,
              std::string type,
              SymbolKind kind);
  // query methods
  Symbol get(std::string name);
  int varCount(SymbolKind);
  SymbolKind kindOf(std::string name);
  std::string typeOf(std::string name);
  int indexOf(std::string name);
private:
};

#endif
