#include "nautilus/compiler/JitSymbolRegistry.hpp"
#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <utility>

namespace nautilus::compiler {

namespace {

/// The registry's state, kept out of the header so the public type stays free
/// of a mutex member (and of <shared_mutex>, which every consumer would
/// otherwise pull in).
struct RegistryState {
	mutable std::shared_mutex mutex;
	/// Sorted by `start`, so a lookup is a binary search. Publication happens
	/// once per compile and lookup once per sample, which is the ratio this
	/// trades for: an insert is O(n) to keep the order, a resolve is O(log n).
	std::vector<JitSymbol> symbols;
	/// Interned `CompilationUnitID`s, indexed by ModuleIndex. Slot 0 is the
	/// empty id, so NO_MODULE needs no special case on lookup.
	std::vector<std::string> moduleIds {std::string {}};
	std::unordered_map<std::string, ModuleIndex> moduleIndices;
	/// Sorted by address, like `symbols`. One row per IR operation, so far
	/// larger than the range table and published only when asked for.
	std::vector<JitLine> lines;
	/// The Nautilus-IR dump each module's lines refer to, indexed by
	/// ModuleIndex. Per module rather than per row: a compile writes one.
	std::vector<std::string> sourceFiles {std::string {}};
};

RegistryState& state() {
	static RegistryState instance;
	return instance;
}

/// Inserts @p symbol keeping `symbols` sorted by start address. The caller
/// holds the write lock.
void insertSorted(std::vector<JitSymbol>& symbols, JitSymbol symbol) {
	const auto position = std::lower_bound(symbols.begin(), symbols.end(), symbol.start,
	                                       [](const JitSymbol& entry, uintptr_t start) { return entry.start < start; });
	symbols.insert(position, std::move(symbol));
}

} // namespace

JitSymbolRegistry& JitSymbolRegistry::instance() {
	static JitSymbolRegistry registry;
	return registry;
}

ModuleIndex JitSymbolRegistry::intern(const std::string& compilationUnitId) {
	if (compilationUnitId.empty()) {
		return NO_MODULE;
	}
	auto& registry = state();
	const std::unique_lock lock(registry.mutex);
	const auto existing = registry.moduleIndices.find(compilationUnitId);
	if (existing != registry.moduleIndices.end()) {
		return existing->second;
	}
	const auto index = static_cast<ModuleIndex>(registry.moduleIds.size());
	registry.moduleIds.push_back(compilationUnitId);
	registry.sourceFiles.emplace_back();
	registry.moduleIndices.emplace(compilationUnitId, index);
	return index;
}

std::string JitSymbolRegistry::moduleId(ModuleIndex moduleIndex) const {
	auto& registry = state();
	const std::shared_lock lock(registry.mutex);
	if (moduleIndex >= registry.moduleIds.size()) {
		return {};
	}
	return registry.moduleIds[moduleIndex];
}

void JitSymbolRegistry::add(JitSymbol symbol) {
	if (symbol.end <= symbol.start) {
		return;
	}
	auto& registry = state();
	const std::unique_lock lock(registry.mutex);
	insertSorted(registry.symbols, std::move(symbol));
}

void JitSymbolRegistry::addAll(std::vector<JitSymbol> symbols) {
	auto& registry = state();
	const std::unique_lock lock(registry.mutex);
	for (auto& symbol : symbols) {
		if (symbol.end <= symbol.start) {
			continue;
		}
		insertSorted(registry.symbols, std::move(symbol));
	}
}

void JitSymbolRegistry::addLines(std::vector<JitLine> lines, std::string sourceFile, ModuleIndex moduleIndex) {
	if (lines.empty()) {
		return;
	}
	auto& registry = state();
	const std::unique_lock lock(registry.mutex);
	if (moduleIndex < registry.sourceFiles.size() && !sourceFile.empty()) {
		registry.sourceFiles[moduleIndex] = std::move(sourceFile);
	}
	// Merged rather than appended: lookup is a binary search, so the whole table
	// has to stay ordered across modules.
	const auto middle = registry.lines.size();
	registry.lines.insert(registry.lines.end(), lines.begin(), lines.end());
	std::inplace_merge(registry.lines.begin(), registry.lines.begin() + static_cast<std::ptrdiff_t>(middle),
	                   registry.lines.end(),
	                   [](const JitLine& left, const JitLine& right) { return left.address < right.address; });
}

bool JitSymbolRegistry::resolveLine(uintptr_t address, JitLine& out) const {
	auto& registry = state();
	const std::shared_lock lock(registry.mutex);
	if (registry.lines.empty()) {
		return false;
	}

	// A line row has an address but no extent, so on its own the last row at or
	// before an address would claim every address after the module's code as
	// well. Requiring the address to sit inside a published range of the *same*
	// module is what bounds it.
	const auto symbolPosition =
	    std::upper_bound(registry.symbols.begin(), registry.symbols.end(), address,
	                     [](uintptr_t value, const JitSymbol& entry) { return value < entry.start; });
	if (symbolPosition == registry.symbols.begin() || !std::prev(symbolPosition)->contains(address)) {
		return false;
	}
	const auto moduleIndex = std::prev(symbolPosition)->moduleIndex;

	const auto position = std::upper_bound(registry.lines.begin(), registry.lines.end(), address,
	                                       [](uintptr_t value, const JitLine& entry) { return value < entry.address; });
	if (position == registry.lines.begin()) {
		return false;
	}
	const auto& candidate = *std::prev(position);
	if (candidate.moduleIndex != moduleIndex) {
		return false;
	}
	out = candidate;
	return true;
}

std::string JitSymbolRegistry::sourceFile(ModuleIndex moduleIndex) const {
	auto& registry = state();
	const std::shared_lock lock(registry.mutex);
	if (moduleIndex >= registry.sourceFiles.size()) {
		return {};
	}
	return registry.sourceFiles[moduleIndex];
}

bool JitSymbolRegistry::resolve(uintptr_t address, JitSymbol& out) const {
	auto& registry = state();
	const std::shared_lock lock(registry.mutex);

	// The last range whose start is <= address; anything earlier cannot cover
	// it, and anything later starts past it.
	const auto position = std::upper_bound(registry.symbols.begin(), registry.symbols.end(), address,
	                                       [](uintptr_t value, const JitSymbol& entry) { return value < entry.start; });
	if (position == registry.symbols.begin()) {
		return false;
	}
	const auto& candidate = *std::prev(position);
	if (!candidate.contains(address)) {
		// Between two ranges: the address is in host code, or in a gap the
		// backend published nothing for.
		return false;
	}
	out = candidate;
	return true;
}

std::vector<JitSymbol> JitSymbolRegistry::snapshot() const {
	auto& registry = state();
	const std::shared_lock lock(registry.mutex);
	return registry.symbols;
}

size_t JitSymbolRegistry::size() const {
	auto& registry = state();
	const std::shared_lock lock(registry.mutex);
	return registry.symbols.size();
}

size_t JitSymbolRegistry::remove(ModuleIndex moduleIndex) {
	if (moduleIndex == NO_MODULE) {
		return 0;
	}
	auto& registry = state();
	const std::unique_lock lock(registry.mutex);
	const auto removed =
	    std::remove_if(registry.symbols.begin(), registry.symbols.end(),
	                   [moduleIndex](const JitSymbol& symbol) { return symbol.moduleIndex == moduleIndex; });
	const auto count = static_cast<size_t>(std::distance(removed, registry.symbols.end()));
	registry.symbols.erase(removed, registry.symbols.end());
	const auto removedLines =
	    std::remove_if(registry.lines.begin(), registry.lines.end(),
	                   [moduleIndex](const JitLine& line) { return line.moduleIndex == moduleIndex; });
	registry.lines.erase(removedLines, registry.lines.end());
	// The interned id is deliberately kept. It costs one string, and retiring a
	// handle would let a later intern() hand the same index to a different
	// module -- which a profile still holding the old one would silently
	// misattribute.
	return count;
}

void JitSymbolRegistry::clear() {
	auto& registry = state();
	const std::unique_lock lock(registry.mutex);
	registry.symbols.clear();
	registry.lines.clear();
	registry.moduleIds.assign(1, std::string {});
	registry.sourceFiles.assign(1, std::string {});
	registry.moduleIndices.clear();
}

} // namespace nautilus::compiler
