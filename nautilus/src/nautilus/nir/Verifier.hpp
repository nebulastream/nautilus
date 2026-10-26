#pragma once

#include "nautilus/nir/Diagnostics.hpp"
#include "nautilus/nir/Module.hpp"
#include <functional>
#include <vector>

namespace nautilus::nir {

struct VerifyOptions {
	/// The module comes from an image: it must have no imports (NIR-V038).
	bool deserialized = false;
	/// Whether an extension type has a C ABI type (`TypeInfo.cabi`, NIR-V035). Without a registry none has.
	std::function<bool(TypeId)> extensionHasCabi;
	/// Whether an opcode of 256 or more is a registered extension operation (NIR-V003).
	std::function<bool(uint16_t)> isExtensionOp;
	/// Stop collecting after this many diagnostics.
	size_t maxDiagnostics = 100;
};

/// Checks a module against 13-nir-spec.md §3-§6 and returns the diagnostics, empty if the module is valid.
std::vector<Diagnostic> verify(const Module& m, const VerifyOptions& options = {});

/// The function-level rules of §3, §5 and §6.1 (the module tables must have been checked first).
void verifyFunctions(const Module& m, const VerifyOptions& options, std::vector<Diagnostic>& out);

/// The module-level rules only (tables and records, §6.1 items 5, 8-13).
void verifyModuleTables(const Module& m, const VerifyOptions& options, std::vector<Diagnostic>& out);

} // namespace nautilus::nir
