#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::compiler {

/// Handle for the compile a code range came from, interned by
/// JitSymbolRegistry::intern().
///
/// An opaque index rather than the `CompilationUnitID` string itself: a
/// long-running engine accumulates a range per region per compile, and a ~40
/// character string on every one of them costs far more than the table of
/// distinct ids it would repeat.
using ModuleIndex = uint32_t;

/// The handle of a range published without a module, and the value a
/// default-constructed JitSymbol carries. Removing it is a no-op, so code that
/// never sets a module needs no special case.
inline constexpr ModuleIndex NO_MODULE = 0;

/// One contiguous range of JIT-compiled machine code, and the name it should
/// appear under in a profile.
struct JitSymbol {
	uintptr_t start = 0;
	/// Exclusive.
	uintptr_t end = 0;
	/// Region-qualified where the compile had the debug info to derive it,
	/// e.g. `execute::query::aggregate::mix`; otherwise the plain function name.
	///
	/// Not unique on its own: every single-function registration lowers to a
	/// function named literally `execute`, so two modules compiled in one
	/// process produce identically named ranges at different addresses. Pair it
	/// with `moduleIndex` to get an identity.
	std::string name;
	/// The compile this range came from, or NO_MODULE.
	ModuleIndex moduleIndex = NO_MODULE;

	[[nodiscard]] bool contains(uintptr_t address) const noexcept {
		return address >= start && address < end;
	}

	[[nodiscard]] uintptr_t size() const noexcept {
		return end - start;
	}
};

/// One row of a compile's line table: the source line a JIT address came from.
///
/// "Source" here is the Nautilus-IR dump the backend writes for this purpose,
/// not the C++ the kernel was written in -- the same file `perf annotate` reads
/// after a jitdump run (docs/profiling.md). Attributing samples to it is what
/// turns "this region is hot" into "this IR operation is hot".
struct JitLine {
	uintptr_t address = 0;
	uint32_t line = 0;
	ModuleIndex moduleIndex = NO_MODULE;
};

/// Process-wide map from JIT-compiled code addresses to symbol names.
///
/// A sampling profiler that runs inside the process cannot resolve nautilus's
/// generated code on its own: it lives in anonymous memory that no ELF file on
/// disk describes, so a symbolizer reading `/proc/self/maps` finds nothing to
/// name it by. Out of process, `perf inject --jit` solves that by synthesizing
/// an ELF per code range from the jitdump the `perf` option writes -- but that
/// costs an external `perf record` run, a `-k mono` clock, and a `perf inject`
/// pass before anything can be read.
///
/// This registry is the in-process equivalent. The MLIR backend publishes the
/// same address ranges and region-qualified names it would have written to a
/// jitdump, and an in-process profiler resolves against it directly.
///
/// Populated only when the compile was asked for it (`perf.sample`), because
/// deriving the names needs DWARF that is otherwise not emitted.
///
/// Thread-safe in both directions, which is not incidental: tiered compilation
/// promotes to MLIR on a background thread, so publication genuinely races with
/// a profiler resolving samples, and several compiles may publish at once.
class JitSymbolRegistry {
public:
	static JitSymbolRegistry& instance();

	/// Interns a `CompilationUnitID`, returning a stable handle for it. Calling
	/// it twice with the same id returns the same handle; an empty id returns
	/// NO_MODULE.
	ModuleIndex intern(const std::string& compilationUnitId);

	/// The `CompilationUnitID` behind a handle, or empty for NO_MODULE and for
	/// handles this registry never issued.
	[[nodiscard]] std::string moduleId(ModuleIndex moduleIndex) const;

	/// Publishes one code range. Ranges are expected to be disjoint; an
	/// overlapping one is still stored, and `resolve` then returns whichever
	/// covers the address first in address order.
	void add(JitSymbol symbol);

	/// Publishes several ranges at once, taking the lock once.
	void addAll(std::vector<JitSymbol> symbols);

	/// Publishes a compile's line table, and the path of the Nautilus-IR dump
	/// its lines refer to.
	///
	/// Separate from the ranges because it is optional and far larger: one row
	/// per IR operation rather than one per region. A profiler that only wants
	/// symbol names never pays for it.
	void addLines(std::vector<JitLine> lines, std::string sourceFile, ModuleIndex moduleIndex);

	/// The line @p address was generated from, or false when no line table
	/// covers it. Only addresses inside a published range resolve, so a host
	/// address never picks up a neighbouring module's line by accident.
	[[nodiscard]] bool resolveLine(uintptr_t address, JitLine& out) const;

	/// The Nautilus-IR dump a module's lines refer to, or empty.
	[[nodiscard]] std::string sourceFile(ModuleIndex moduleIndex) const;

	/// The symbol covering @p address, or false if none does -- which for a
	/// sampled instruction pointer usually just means it landed in host code.
	///
	/// Fills @p out by value rather than returning a pointer: another thread
	/// may publish (and so reallocate the table) at any time, and a profiler
	/// resolving a sample has no lock to hold a reference under.
	[[nodiscard]] bool resolve(uintptr_t address, JitSymbol& out) const;

	/// Every published range, sorted by start address.
	[[nodiscard]] std::vector<JitSymbol> snapshot() const;

	[[nodiscard]] size_t size() const;

	/// Drops every range and line published for @p moduleIndex, returning how
	/// many ranges went.
	///
	/// Called when the compiled code itself is freed, so that a later compile
	/// reusing those addresses is not resolved to the dead module's names --
	/// and so that a long-running engine compiling query after query does not
	/// accumulate ranges for code that no longer exists. The interned id stays,
	/// which costs one string and keeps handles from ever being reused.
	size_t remove(ModuleIndex moduleIndex);

	/// Drops every published range and every interned id. Exists for tests; a
	/// running process has no reason to forget its own code.
	void clear();

private:
	JitSymbolRegistry() = default;
};

} // namespace nautilus::compiler
