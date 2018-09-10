#ifndef __JACK_SYMBOL_TABLE__H__
#define __JACK_SYMBOL_TABLE__H__

#include <string>
#include <unordered_map>

enum class SymbolKind {
  STATIC,
  FIELD,
  VAR,  // local
  ARG,
};
const SymbolKind SymbolKindAll[] = {
  SymbolKind::STATIC, SymbolKind::FIELD,
  SymbolKind::VAR, SymbolKind::ARG
};

class ClassElement;

// holds class level and function level field information;
// sort of like a wrapper over two hash tables, one for each level
class SymbolTable {
public:
  SymbolTable(ClassElement&);  // built from a parsed class
  void startSubroutine();
  void define(std::string name,
              std::string type,
              SymbolKind kind);
  // query methods
  int varCount(SymbolKind);
  SymbolKind kindOf(std::string name);
  std::string typeOf(std::string name);
  int indexOf(std::string name);
private:
};

#endif
