#ifndef __JACK_SYMBOL_TABLE__H__
#define __JACK_SYMBOL_TABLE__H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

enum class SymbolKind {
  STATIC,
  FIELD,
  VAR,  // local
  ARG,
};

namespace SymbolKindHelpers {
  const SymbolKind ALL[] = {
    SymbolKind::STATIC, SymbolKind::FIELD, SymbolKind::VAR, SymbolKind::ARG
  };
  const std::pair<SymbolKind, std::string> MAPPING[] = {
    std::make_pair(SymbolKind::STATIC, "static"),
    std::make_pair(SymbolKind::FIELD, "field"),
    std::make_pair(SymbolKind::VAR, "local"),
    std::make_pair(SymbolKind::ARG, "argument"),
  };

  SymbolKind toKind(std::string);
  std::string toString(SymbolKind);
};

struct Symbol {
  std::string name;
  std::string type;
  SymbolKind kind;
  int index;

  bool operator==(const Symbol &other) const;
};
std::ostream& operator<<(std::ostream &out, const Symbol &s);

class ClassElement;
class SubroutineDec;

// holds class level and function level field information;
// sort of like a wrapper over two hash tables, one for each level
class SymbolTable {
public:
  SymbolTable();
  void populateFromClass(const ClassElement&);
  void populateFromSubroutine(const SubroutineDec&);
  void clearSubroutineSymbols();
  void clear();  // clears everything
  void defineClassVar(std::string name, std::string type, SymbolKind kind);
  // query methods
  Symbol get(std::string name);
  int varCount(SymbolKind);
  SymbolKind kindOf(std::string name);
  std::string typeOf(std::string name);
  int indexOf(std::string name);
private:
  std::unordered_map<std::string, Symbol> _classSymbols;
  std::unordered_map<std::string, Symbol> _subroutineSymbols;
  std::string _className;
  std::unordered_map<SymbolKind, int, EnumClassHash> _indices;
};

#endif
