
#include "nautilus/debug/DwarfVariableResolver.hpp"

// The real DWARF path lives behind two conditions that must both hold:
//   - the host platform is Linux (we read /proc/self/exe to find ourselves)
//   - the build links LLVM's DWARF/Object libraries
// The latter is signalled by NAUTILUS_DEBUG_HAS_DWARF, set from the CMake
// gate in `src/nautilus/debug/CMakeLists.txt`. Every other configuration
// gets the trivial stub that returns nullptr from getInstance().
#if defined(__linux__) && defined(NAUTILUS_DEBUG_HAS_DWARF)
#define NAUTILUS_HAS_DWARF_RESOLVER 1
#else
#define NAUTILUS_HAS_DWARF_RESOLVER 0
#endif

#if NAUTILUS_HAS_DWARF_RESOLVER

#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/DebugInfo/DIContext.h"
#include "llvm/DebugInfo/DWARF/DWARFContext.h"
#include "llvm/DebugInfo/DWARF/DWARFDie.h"
#include "llvm/DebugInfo/DWARF/DWARFUnit.h"
#include "llvm/Object/Binary.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/MemoryBuffer.h"
#include <climits>
#include <cstring>
#include <filesystem>
#include <mutex>
#include <unistd.h>
#include <unordered_map>

namespace nautilus::debug {

namespace {

/// Basename of a possibly-absolute path. Used as a last-resort key when
/// the resolver's caller-supplied file path (relative) disagrees with
/// DWARF's compile-dir-resolved absolute path, or vice versa.
std::string basenameOf(std::string_view path) {
	return std::filesystem::path(std::string(path)).filename().string();
}

struct LineCol {
	uint32_t line;
	uint32_t column;
	bool operator==(const LineCol& other) const noexcept {
		return line == other.line && column == other.column;
	}
};

struct LineColHash {
	std::size_t operator()(const LineCol& lc) const noexcept {
		// Mix line with a prime — column is usually small so a pure XOR
		// would collide heavily.
		return (static_cast<std::size_t>(lc.line) * 0x9e3779b1u) ^ lc.column;
	}
};

using FileIndex = std::unordered_map<LineCol, std::string, LineColHash>;

class RealResolver final : public DwarfVariableResolver {
public:
	std::optional<std::string> resolveVariableName(std::string_view file, uint32_t line, uint32_t column);

	/// Builds the index on first call. Returns false if the host binary
	/// has no usable DWARF; subsequent calls then short-circuit to nullopt.
	bool ensureInitialized();

private:
	void walkDie(const llvm::DWARFDie& die, llvm::DWARFUnit& unit);

	std::mutex mu_;
	bool initialized_ = false;
	bool usable_ = false;

	llvm::object::OwningBinary<llvm::object::Binary> owningBinary_;
	std::unique_ptr<llvm::DWARFContext> dwarf_;

	// Two-level index: file path -> (line, column) -> variable name.
	// Keyed by absolute path when we can resolve one via the compile-dir,
	// else by the raw filename; the lookup tries both forms.
	std::unordered_map<std::string, FileIndex> byAbsPath_;
	std::unordered_map<std::string, FileIndex> byBaseName_;
};

bool RealResolver::ensureInitialized() {
	if (initialized_) {
		return usable_;
	}
	initialized_ = true;

	// /proc/self/exe points to the currently-running binary; readlink
	// gives us the canonical path. Anything that fails here means we
	// can't find our own DWARF, so we stay in the "unusable" state.
	char buf[PATH_MAX];
	ssize_t n = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
	if (n <= 0) {
		return false;
	}
	buf[n] = '\0';

	auto binOrErr = llvm::object::createBinary(buf);
	if (!binOrErr) {
		llvm::consumeError(binOrErr.takeError());
		return false;
	}
	owningBinary_ = std::move(*binOrErr);

	auto* obj = llvm::dyn_cast<llvm::object::ObjectFile>(owningBinary_.getBinary());
	if (obj == nullptr) {
		return false;
	}

	dwarf_ = llvm::DWARFContext::create(*obj);
	if (!dwarf_ || dwarf_->getNumCompileUnits() == 0) {
		// Binary has no DWARF (likely stripped or built without -g).
		return false;
	}

	for (const auto& cu : dwarf_->compile_units()) {
		llvm::DWARFDie root = cu->getUnitDIE(/*ExtractUnitDIEOnly=*/false);
		if (root.isValid()) {
			walkDie(root, *cu);
		}
	}

	usable_ = true;
	return true;
}

void RealResolver::walkDie(const llvm::DWARFDie& die, llvm::DWARFUnit& unit) {
	for (auto child : die.children()) {
		const auto tag = child.getTag();
		const bool isVar = (tag == llvm::dwarf::DW_TAG_variable || tag == llvm::dwarf::DW_TAG_formal_parameter);
		if (isVar) {
			auto nameAttr = child.find(llvm::dwarf::DW_AT_name);
			auto fileAttr = child.find(llvm::dwarf::DW_AT_decl_file);
			auto lineAttr = child.find(llvm::dwarf::DW_AT_decl_line);
			auto colAttr = child.find(llvm::dwarf::DW_AT_decl_column);
			if (nameAttr && fileAttr && lineAttr) {
				auto nameStrExpected = nameAttr->getAsCString();
				const uint64_t fileIdx = fileAttr->getAsUnsignedConstant().value_or(0);
				const uint32_t lineNum = static_cast<uint32_t>(lineAttr->getAsUnsignedConstant().value_or(0));
				const uint32_t colNum =
				    colAttr ? static_cast<uint32_t>(colAttr->getAsUnsignedConstant().value_or(0)) : 0;

				if (nameStrExpected && lineNum != 0) {
					const char* cName = *nameStrExpected;
					if (cName != nullptr && cName[0] != '\0') {
						std::string name(cName);
						std::string absPath;
						const auto* lt = dwarf_->getLineTableForUnit(&unit);
						if (lt != nullptr) {
							lt->getFileNameByIndex(fileIdx, unit.getCompilationDir(),
							                       llvm::DILineInfoSpecifier::FileLineInfoKind::AbsoluteFilePath,
							                       absPath);
						}
						const LineCol key {lineNum, colNum};
						if (!absPath.empty()) {
							byAbsPath_[absPath][key] = name;
							byBaseName_[basenameOf(absPath)][key] = name;
						}
					}
				}
			}
		}
		// Recurse: local variables live inside DW_TAG_lexical_block /
		// DW_TAG_inlined_subroutine too, and template/inline expansions
		// nest arbitrarily deep.
		walkDie(child, unit);
	}
}

std::optional<std::string> RealResolver::resolveVariableName(std::string_view file, uint32_t line, uint32_t column) {
	std::lock_guard<std::mutex> lock(mu_);
	if (!ensureInitialized()) {
		return std::nullopt;
	}

	auto lookup = [&](const std::unordered_map<std::string, FileIndex>& table,
	                  const std::string& key) -> std::optional<std::string> {
		auto it = table.find(key);
		if (it == table.end()) {
			return std::nullopt;
		}
		const auto& sub = it->second;
		// Exact (line, column) match first, when DWARF 5 column info is
		// present.
		if (column != 0) {
			if (auto hit = sub.find({line, column}); hit != sub.end()) {
				return hit->second;
			}
		}
		// Fall back to line-only match (column 0).
		if (auto hit = sub.find({line, 0}); hit != sub.end()) {
			return hit->second;
		}
		// Last resort: pick the unique name declared on this line, if any.
		std::optional<std::string> unique;
		for (const auto& [k, v] : sub) {
			if (k.line == line) {
				if (unique && *unique != v) {
					return std::nullopt;
				}
				unique = v;
			}
		}
		return unique;
	};

	const std::string fileStr(file);

	// Try the absolute-path index first, then retry with the basename to
	// paper over mismatches between the supplied path's form and the
	// path DWARF reports.
	if (auto hit = lookup(byAbsPath_, fileStr)) {
		return hit;
	}
	if (auto hit = lookup(byBaseName_, basenameOf(file))) {
		return hit;
	}
	return std::nullopt;
}

} // namespace

DwarfVariableResolver* DwarfVariableResolver::getInstance() {
	// Meyers singleton. We hand out a non-null resolver even when DWARF
	// init turns out to fail at runtime — every subsequent lookup just
	// returns nullopt — so callers don't need a second branch for that
	// case.
	static RealResolver instance;
	return &instance;
}

std::optional<std::string> DwarfVariableResolver::resolveVariableName(std::string_view file, uint32_t line,
                                                                      uint32_t column) {
	return static_cast<RealResolver*>(this)->resolveVariableName(file, line, column);
}

} // namespace nautilus::debug

#else // NAUTILUS_HAS_DWARF_RESOLVER

namespace nautilus::debug {

DwarfVariableResolver* DwarfVariableResolver::getInstance() {
	// Non-Linux or non-MLIR builds: return nullptr so call sites skip
	// resolution cleanly. This matches the contract established by
	// SourceLocationResolver, whose @c resolveStack returns an empty
	// vector when ENABLE_STACKTRACE is off.
	return nullptr;
}

std::optional<std::string> DwarfVariableResolver::resolveVariableName(std::string_view, uint32_t, uint32_t) {
	return std::nullopt;
}

} // namespace nautilus::debug

#endif // NAUTILUS_HAS_DWARF_RESOLVER
