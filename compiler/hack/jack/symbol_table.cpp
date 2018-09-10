#include <iostream>
#include <string>
#include <stdexcept>
#include <utility>

#include "../../utils.h"

#include "symbol_table.h"
#include "grammar.h"

// SymbolKind

SymbolKind SymbolKindHelpers::toKind(std::string kind) {
  for (const auto &kind__str : SymbolKindHelpers::MAPPING)
    if (kind__str.second == kind)
      return kind__str.first;
  throw_and_debug("Unknown kind " + kind);
}

std::string SymbolKindHelpers::toString(SymbolKind kind) {
  for (const auto &kind__str : SymbolKindHelpers::MAPPING)
    if (kind__str.first == kind)
      return kind__str.second;
  throw_and_debug("SymbolKindHelpers::toString : unknown kind");
}

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

void SymbolTable::populateFromClass(const ClassElement &classElement) {
  _className = classElement.getName();
  for (const ClassVarDec &varsDec : classElement.getClassVarDecs()) {
    std::string kind = varsDec.getKind();
    std::string type = varsDec.getType();
    for (std::string &name : varsDec.getVarNames()) {
      defineClassVar(name, type, SymbolKindHelpers::toKind(kind));
    }
  }
}

void SymbolTable::populateFromSubroutine(const SubroutineDec&) {
}

void SymbolTable::clearSubroutineSymbols() {
  _subroutineSymbols.clear();
  _indices[SymbolKind::VAR] = _indices[SymbolKind::ARG] = 0;
}

void SymbolTable::clear() {
  _classSymbols.clear();
  _subroutineSymbols.clear();
  _indices.clear();
}

void SymbolTable::defineClassVar(std::string name, std::string type, SymbolKind kind) {
  if (_classSymbols.find(name) != _classSymbols.end())
    throw_and_debug("Redefinition of symbol '" + name + "' in class " + _className);

  int index = _indices[kind]++;
  _classSymbols[name] = {
    .name=name,
    .type=type,
    .kind=kind,
    .index=index
  };
}

Symbol SymbolTable::get(std::string name) {
  if (_subroutineSymbols.find(name) != _subroutineSymbols.end())
    return _subroutineSymbols[name];
  if (_classSymbols.find(name) != _classSymbols.end())
    return _classSymbols[name];
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
