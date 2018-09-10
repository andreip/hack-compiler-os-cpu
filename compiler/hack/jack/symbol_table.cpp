#include <algorithm>
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

Symbol::operator bool() const { return !name.empty(); }
bool Symbol::operator!() const { return name.empty(); }

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

void SymbolTable::populateFromSubroutine(const SubroutineDec &sub) {
  clearSubroutineSymbols();  // can't have two subroutine's symbols at same time
  // populate args
  // TODO: if method/constructor, first put "this"
  // (type, name)
  for (const auto &pair: sub.getParameterList().getArgs())
    defineSubroutineVar(pair.second, pair.first, SymbolKind::ARG);
  // populate vars
  for (const VarDec &dec : sub.getBody().getVarDecs())
    for (const std::string &name: dec.getNames())
      defineSubroutineVar(name, dec.getType(), SymbolKind::VAR);
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
  define(name, type, kind, _classSymbols);
}

void SymbolTable::defineSubroutineVar(std::string name, std::string type, SymbolKind kind) {
  define(name, type, kind, _subroutineSymbols);
}

template <typename MapT>
void SymbolTable::define(std::string name, std::string type, SymbolKind kind, MapT &map) {
  if (map.find(name) != map.end())
    throw_and_debug("Redefinition of symbol '" + name + "' in class " + _className);

  int index = _indices[kind]++;
  map[name] = {
    .name=name,
    .type=type,
    .kind=kind,
    .index=index
  };
}

Symbol SymbolTable::get(std::string name) const {
  if (_subroutineSymbols.find(name) != _subroutineSymbols.end())
    return _subroutineSymbols.at(name);
  if (_classSymbols.find(name) != _classSymbols.end())
    return _classSymbols.at(name);
  throw std::runtime_error("SymbolTable::get couldn't find symbol " + name);
}

int SymbolTable::varCount(SymbolKind kind) const {
  int count = 0;
  std::for_each(
    _classSymbols.begin(), _classSymbols.end(),
    [&count, kind](auto &e) { if (e.second.kind == kind) ++count; }
  );
  std::for_each(
    _subroutineSymbols.begin(), _subroutineSymbols.end(),
    [&count, kind](auto &e) { if (e.second.kind == kind) ++count; }
  );
  return count;
}

SymbolKind SymbolTable::kindOf(std::string name) const { return get(name).kind; }

std::string SymbolTable::typeOf(std::string name) const { return get(name).type; }

int SymbolTable::indexOf(std::string name) const { return get(name).index; }
