#pragma once

#include "nautilus/nir/Module.hpp"
#include "nautilus/tracing/ExecutionTrace.hpp"
#include <string>
#include <string_view>

/// The NIR tracer's side of the front end (docs/plans/nautilus-0.4/01-tracing-frontend.md §6): a trace as NIR at the
/// pre-SSA level. This namespace is not `nautilus::tracing::nir` so that `nir::` keeps meaning the IR library.
namespace nautilus::tracing::nirtrace {

/// How a trace is turned into NIR.
struct Options {
	/// Emit debug sections (source tags and regions) (D14); off by default.
	bool debugInfo = false;
};

/// What `Type` means in NIR: the width of the type and, where an operation depends on it, the signedness.
struct TypeInfo {
	::nautilus::nir::TypeId id;
	bool isSigned;
	bool isFloat;
};
TypeInfo typeInfo(Type type);

/// The type word of `type` at a native boundary (13 §4.5.1): narrow integers carry the extension their signedness
/// implies.
::nautilus::nir::TypeWord typeWord(Type type);

/// The pre-SSA NIR of a whole trace module: one function per traced function, the entry functions first and the others
/// by name. Throws RuntimeException for a trace that cannot be expressed (a bug in the mapping or in the trace).
::nautilus::nir::Module toPreSSA(const TraceModule& module, const Options& options = {});

/// The pre-SSA NIR of a lone trace, as the one function `name`.
::nautilus::nir::Module toPreSSA(const ExecutionTrace& trace, std::string_view name = "execute",
                                 const Options& options = {});

} // namespace nautilus::tracing::nirtrace
