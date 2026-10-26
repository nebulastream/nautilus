#pragma once

#include "nautilus/nir/Module.hpp"
#include <functional>

namespace nautilus::nir {

struct CanonOptions {
	/// Canonicalize as if the module had no name (`ModuleRecord.nameId` = NONE): the name stays in `names` only if
	/// something else uses it. The structural hash does this (13-nir-spec.md §7.2).
	bool anonymous = false;
	/// Alignment of a constant wider than 8 bytes by type, at most 16 (§7, step 5). Without a registry: 1.
	std::function<uint32_t(TypeId)> constAlign;
};

/// The canonical form of 13-nir-spec.md §7: a deterministic, idempotent function of an SSA-level module; structurally
/// equal modules give sections that compare equal byte for byte. Debug sections are dropped.
/// Throws std::invalid_argument if a function is at the pre-SSA level. The module must verify.
Module canonicalize(const Module& m, const CanonOptions& options = {});

/// Whether the canonical sections of two modules are identical (what the image writer would serialize).
bool sectionsEqual(const Module& a, const Module& b);

} // namespace nautilus::nir
