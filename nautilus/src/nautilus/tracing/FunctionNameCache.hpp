#pragma once

#include <string>

namespace nautilus::tracing {

/// The symbol a native function pointer resolves to, and that symbol demangled.
///
/// Both are the pointer itself, stringified, when dladdr cannot name it (e.g. a
/// `static` function absent from the dynamic symbol table).
struct ResolvedFunctionName {
	std::string mangled;
	std::string demangled;
};

/// Resolves a native function pointer to its symbol names.
///
/// dladdr scans the containing object's symbol table and is expensive in large
/// binaries (about 0.75 ms per call in a ~700 MB executable), and __cxa_demangle
/// allocates on every call. A function pointer's name cannot change while the
/// process is alive, so the result is cached process-wide: every trace, region
/// scope and engine shares it and each callee is resolved at most once per
/// process. Safe to call concurrently from several tracing threads.
///
/// The returned reference stays valid for the lifetime of the process.
const ResolvedFunctionName& resolveFunctionName(void* fnptr);

} // namespace nautilus::tracing
