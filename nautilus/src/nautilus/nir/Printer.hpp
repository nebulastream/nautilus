#pragma once

#include "nautilus/nir/Module.hpp"
#include <iosfwd>
#include <string>
#include <vector>

namespace nautilus::nir {

/// Writes the text form of 13-nir-spec.md §9. The output is a fixed function of the module, so printing, parsing and
/// printing again gives identical text. Extension types and operations are the business of M1.3a and print with
/// placeholder names until the registry exists.
std::string print(const Module& m);
void print(const Module& m, std::ostream& os);

/// Where the objects of a module lie in its text form: 1-based line numbers, 0 for an object that has no line of its
/// own. This is the side output that replaces 0.3's `IRLocationPass` (03 §2.1): a backend that points debug information
/// or a profile at the text (DWARF `FileLineColLoc`s, the line table of `JitSymbolRegistry`) prints the module once
/// with it and reads the line of an instruction from here instead of parsing the text.
struct PrintLines {
	/// Parallel to `Module::insts`. An SSA block argument is on the line of its block header.
	std::vector<uint32_t> instruction;
	/// Parallel to `Module::blocks`: the line of the `^bN` header.
	std::vector<uint32_t> block;
	/// Parallel to `Module::functions`: the line of the `func` header.
	std::vector<uint32_t> function;
};

/// The text form (identical to `print(m)`) and the lines of its objects.
std::string print(const Module& m, PrintLines& lines);

/// The spelling of a core type ("i64"), or "!ext.t<N>" for an extension type.
std::string typeName(TypeId t);

} // namespace nautilus::nir
