#pragma once

#include "nautilus/nir/Module.hpp"
#include <iosfwd>
#include <string>

namespace nautilus::nir {

/// Writes the text form of 13-nir-spec.md §9. The output is a fixed function of the module, so printing, parsing and
/// printing again gives identical text. Extension types and operations are the business of M1.3a and print with
/// placeholder names until the registry exists.
std::string print(const Module& m);
void print(const Module& m, std::ostream& os);

/// The spelling of a core type ("i64"), or "!ext.t<N>" for an extension type.
std::string typeName(TypeId t);

} // namespace nautilus::nir
