#pragma once

// Extension ids in memory and in an image (13-nir-spec.md §8.4). In memory an extension operation or type is named by
// the process's registry id (`Inst.op` from 256, a type from 16); in an image it is the position of its entry of kind
// operation or type in the `extensions` section, so that an image does not depend on the order in which a process
// registered its plugins. `toImageIds` and `toProcessIds` translate every place that holds such an id.

#include "nautilus/nir/Diagnostics.hpp"
#include "nautilus/nir/Module.hpp"
#include <optional>
#include <string>
#include <vector>

namespace nautilus::nir {

struct ExtensionEntry {
	std::string name; // "name@version"
	ExtensionKind kind;
};

/// The `extensions` section a module is written with: its own entries, followed by the extension operations and types
/// that its instructions, constants, bindings and signatures use and the entries lack (with the registered version),
/// in first-use order.
std::vector<ExtensionEntry> extensionEntries(const Module& m);

/// A copy of `m` with image-local extension ids and the `extensions` section of `extensionEntries`.
Module toImageIds(const Module& m);

struct IdError {
	DiagCode code;
	std::string message;
};

/// Resolves the entries of `m.extensions` against the registry and replaces image-local ids by process ids. Fails with
/// NIR-L010 (missing extension, or an id outside the section), NIR-L011 (version not accepted) or NIR-L012 (an
/// intrinsic callee's signature differs from the registered one).
std::optional<IdError> toProcessIds(Module& m);

} // namespace nautilus::nir
