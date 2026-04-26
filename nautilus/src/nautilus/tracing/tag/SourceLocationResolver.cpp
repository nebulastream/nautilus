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
	backward::Trace rawTrace(reinterpret_cast<void*>(pc), 0);
	backward::ResolvedTrace resolved = impl_->resolver.resolve(backward::ResolvedTrace(rawTrace));

	// Prefer the deepest inlined source location over the object-level one:
	// for inlined calls backward exposes the chain through `inliners`, with
	// `source` being the innermost entry.
	if (!resolved.source.filename.empty() || !resolved.source.function.empty()) {
		frame.file = resolved.source.filename;
		frame.function = resolved.source.function;
		frame.line = resolved.source.line;
		frame.column = resolved.source.col;
	} else if (!resolved.object_function.empty()) {
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
	std::vector<SourceFrame> result;
	for (const Tag* cur = leaf; cur != nullptr && cur->getParent() != nullptr; cur = cur->getParent()) {
		const SourceFrame& resolved = resolve(cur->getContent());
		if (resolved.file.empty() || isInternalFrame(resolved)) {
			continue;
		}
		result.push_back(resolved);
	}
	std::reverse(result.begin(), result.end());
	return result;
}

} // namespace nautilus::tracing
