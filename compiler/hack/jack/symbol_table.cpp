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

std::string Symbol::segment() const {
  if (kind == SymbolKind::FIELD) return "this";
  if (kind == SymbolKind::STATIC) return "static";
  if (kind == SymbolKind::VAR) return "local";
  if (kind == SymbolKind::ARG) return "argument";
  throw_and_debug("Symbol::segment: unknown segment for kind");
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

SymbolTable::~SymbolTable() { reset(); }

void SymbolTable::init(const ClassElement &classElement, const SubroutineDec &subroutine) {
  reset();
  _className = classElement.getName();
  _subroutine = &subroutine;
  _classElement = &classElement;

  // init class-level args
  for (const ClassVarDec &varsDec : classElement.getClassVarDecs()) {
    std::string kind = varsDec.getKind();
    std::string type = varsDec.getType();

    // if subroutine is a function, skip over "field" type
    // variables, since those aren't accessible from the subroutine.
    if (kind == "field" && subroutine.getKind() == "function")
      continue;

    for (std::string &name : varsDec.getVarNames()) {
      defineClassVar(name, type, SymbolKindHelpers::toKind(kind));
    }
  }

  // init subroutine-level args
  // populate args; if it's a method, the first argument is
  // a hidden "this" pointer
  if (subroutine.getKind() == "method")
    defineThis(subroutine.getClassName());
  for (const auto &pair: subroutine.getParameterList().getArgs())
    // (type, name)
    defineSubroutineVar(pair.second, pair.first, SymbolKind::ARG);

  // populate vars
  for (const VarDec &dec : subroutine.getBody().getVarDecs())
    for (const std::string &name: dec.getNames())
      defineSubroutineVar(name, dec.getType(), SymbolKind::VAR);
}

void SymbolTable::reset() {
  _classSymbols.clear();
  _subroutineSymbols.clear();
  _indices.clear();
  _className = "NoClassName";
  _subroutine = nullptr;
  _classElement = nullptr;
}

void SymbolTable::defineThis(std::string type) {
  define("this", type, SymbolKind::ARG, _subroutineSymbols);
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
  return Symbol();  // empty evaluates to falsy
}

int SymbolTable::count(SymbolKind kind) const {
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

std::string SymbolTable::getCurrentSubroutineKind() const {
  if (_subroutine)
    return _subroutine->getKind();

  throw_and_debug("SymbolTable::getCurrentSubroutineKind: No subroutine on table");
}

std::string SymbolTable::getCurrentClassName() const {
  return _className;
}
