#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::compiler {

/// One row of the line table of a JIT_CODE_DEBUG_INFO record.
struct JitDumpLine {
	uint64_t address = 0;
	uint32_t line = 0;
	std::string file;
};

/// A writer of the perf jitdump format (`jit-<pid>.dump`, the file `perf inject --jit` reads) for the backends that
/// generate code without LLVM, which has a writer of its own (D14, 05 §7). `mlir` goes through
/// `PerfJitDumpPlugin`; `asmjit` and `tbc` write their symbols here, so that `perf record` names their frames too.
///
/// One file per process, opened by the first record under `$JITDUMPDIR/.debug/jit/<unique directory>/` (`$HOME` if
/// that is not set), and closed with a JIT_CODE_CLOSE record when the process exits. Linux only: elsewhere nothing is
/// written.
class PerfJitDump {
public:
	static PerfJitDump& instance();

	/// Appends the code range [address, address + size) as `name`: a JIT_CODE_DEBUG_INFO record with `lines` if there
	/// are any, immediately followed by the JIT_CODE_LOAD record it describes (`perf inject` pairs them by position).
	/// The bytes at the range are copied into the record. Thread safe.
	void writeCode(const std::string& name, uintptr_t address, size_t size, const std::vector<JitDumpLine>& lines);

private:
	PerfJitDump() = default;
};

} // namespace nautilus::compiler
