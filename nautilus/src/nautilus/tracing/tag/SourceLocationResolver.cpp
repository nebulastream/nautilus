#include "nautilus/tracing/tag/SourceLocationResolver.hpp"
#include "nautilus/config.hpp"

#ifdef ENABLE_STACKTRACE
#include <backward.hpp>
#endif

#include <algorithm>
#include <utility>

namespace nautilus::tracing {

struct SourceLocationResolver::Impl {
#ifdef ENABLE_STACKTRACE
	backward::TraceResolver resolver;
#endif
};

SourceLocationResolver::SourceLocationResolver() : impl_(std::make_unique<Impl>()) {
	// Default filter: drop frames inside the Nautilus source / include trees
	// so callers see only the user's code-generation hierarchy.  Paths match
	// as substrings for portability (backward reports absolute paths whose
	// leading segments depend on the build location).
	internalPrefixes_.emplace_back("/nautilus/src/");
	internalPrefixes_.emplace_back("/nautilus/include/");
	internalPrefixes_.emplace_back("/nautilus/nautilus/src/");
	internalPrefixes_.emplace_back("/nautilus/nautilus/include/");
}

SourceLocationResolver::~SourceLocationResolver() = default;

void SourceLocationResolver::addInternalPathPrefix(std::string prefix) {
	internalPrefixes_.emplace_back(std::move(prefix));
}

bool SourceLocationResolver::isInternalFrame(const SourceFrame& frame) const {
	if (frame.file.empty()) {
		return false;
	}
	return std::any_of(internalPrefixes_.begin(), internalPrefixes_.end(),
	                   [&](const std::string& prefix) { return frame.file.find(prefix) != std::string::npos; });
}

const SourceFrame& SourceLocationResolver::resolve(TagAddress pc) {
	if (auto it = cache_.find(pc); it != cache_.end()) {
		return it->second;
	}

	SourceFrame frame;
#ifdef ENABLE_STACKTRACE
	// `load_addresses` is mandatory before `resolve` for the
	// backtrace_symbol fallback path: that impl indexes into a
	// `_symbols` array populated by `load_addresses`, and skipping
	// the load step makes `resolve` dereference a null pointer.  The
	// libdw / libbfd / libdwarf paths happen to tolerate the skip,
	// which is why this only surfaces on systems missing the dwarf
	// backends — but driving the API correctly here makes the
	// resolver portable across all backward-cpp configurations.
	void* address = reinterpret_cast<void*>(pc);
	impl_->resolver.load_addresses(&address, 1);
	backward::Trace rawTrace(address, 0);
	backward::ResolvedTrace resolved = impl_->resolver.resolve(backward::ResolvedTrace(rawTrace));

	// Prefer the deepest inlined source location (DWARF) over the object-level
	// fallback: for inlined calls backward exposes the chain through
	// `inliners`, with `source` being the innermost entry.  Gate the
	// preference on `source.filename`, not `source.function` — the
	// `backtrace_symbol` fallback path mirrors `object_function` into
	// `source.function` even when it has no DWARF source info, so keying
	// on the function alone would silently swallow a non-empty
	// `object_filename` and leave us with an empty `frame.file`.
	if (!resolved.source.filename.empty()) {
		frame.file = resolved.source.filename;
		frame.function = resolved.source.function;
		frame.line = resolved.source.line;
		frame.column = resolved.source.col;
	} else if (!resolved.object_function.empty() || !resolved.object_filename.empty()) {
		frame.function = resolved.object_function;
		frame.file = resolved.object_filename;
	}
#else
	(void) pc;
#endif
	auto [it, _] = cache_.emplace(pc, std::move(frame));
	return it->second;
}

std::vector<SourceFrame> SourceLocationResolver::resolveStack(const Tag* leaf) {
	// Walk parent links leaf-to-root, push frames as we go (skipping
	// unresolved and internal ones), then reverse so the result reads
	// outer-to-inner. Folding the walk and the resolution into one pass
	// avoids materialising the full address vector.
	//
	// "Uninformative" frames — no file, or only a binary path with
	// `line == 0` and no function name (the shape backward-cpp's
	// `backtrace_symbol` fallback returns when the binary wasn't
	// linked with `-rdynamic` and DWARF backends are absent) — get
	// dropped here.  They would otherwise inflate every MLIR op's
	// `CallSiteLoc` chain by 10–15 entries that the DWARF emitter
	// would silently strip downstream anyway.
	std::vector<SourceFrame> result;
	for (const Tag* cur = leaf; cur != nullptr && cur->getParent() != nullptr; cur = cur->getParent()) {
		const SourceFrame& resolved = resolve(cur->getContent());
		if (resolved.file.empty() || isInternalFrame(resolved)) {
			continue;
		}
		if (resolved.line == 0 && resolved.function.empty()) {
			continue;
		}
		result.push_back(resolved);
	}
	std::reverse(result.begin(), result.end());
	return result;
}

} // namespace nautilus::tracing
