#include <iostream>
#include <string>
#include <stdexcept>

#include "symbol_table.h"
#include "grammar.h"

// Symbol

bool Symbol::operator==(const Symbol &other) const {
  return (
    name == other.name &&
    type == other.type &&
    kind == other.kind &&
    index == other.index
  );
}

std::ostream& operator<<(std::ostream &out, const Symbol &s) {
  out << "Symbol(name=" << s.name
      << ", type= " << s.type
      << ", kind= " << static_cast<int>(s.kind)
      << ", index= " << s.index;
  return out;
}

// SymbolTable

SymbolTable::SymbolTable() { }

void SymbolTable::populateFromClass(ClassElement&) { }
void SymbolTable::populateFromSubroutine(SubroutineDec&) { }
void SymbolTable::clearSubroutineSymbols() { }
void SymbolTable::clear() { }

Symbol SymbolTable::get(std::string name) {
  Symbol s;
  return s;
  throw std::runtime_error("not found");
}

int SymbolTable::varCount(SymbolKind kind) { return 0; }

SymbolKind SymbolTable::kindOf(std::string name) {
  throw std::runtime_error("not found");
}

std::string SymbolTable::typeOf(std::string name) {
  throw std::runtime_error("not found");
}

int SymbolTable::indexOf(std::string name) {
  throw std::runtime_error("not found");
}
