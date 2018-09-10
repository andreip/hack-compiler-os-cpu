#include <string>
#include <stdexcept>

#include "symbol_table.h"
#include "grammar.h"

SymbolTable::SymbolTable(ClassElement &classElement) {
}

void SymbolTable::startSubroutine() { }
void SymbolTable::define(std::string name,
                         std::string type,
                         SymbolKind kind) {
}
int SymbolTable::varCount(SymbolKind) { return 0; }

SymbolKind SymbolTable::kindOf(std::string name) {
  throw std::runtime_error("not found");
}

std::string SymbolTable::typeOf(std::string name) {
  throw std::runtime_error("not found");
}

int SymbolTable::indexOf(std::string name) {
  throw std::runtime_error("not found");
}
