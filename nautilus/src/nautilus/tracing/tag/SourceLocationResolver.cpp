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

	backward::ResolvedTrace resolve(TagAddress pc) {
		// The backtrace_symbols backend (used when neither libdw nor libbfd is available) resolves an index into
		// the addresses loaded last, so the address has to be loaded first; the DWARF backends ignore this.
		void* address = reinterpret_cast<void*>(pc);
		resolver.load_addresses(&address, 1);
		backward::Trace rawTrace(address, 0);
		return resolver.resolve(backward::ResolvedTrace(rawTrace));
	}
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
	backward::ResolvedTrace resolved = impl_->resolve(pc);

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

const std::vector<SourceFrame>& SourceLocationResolver::resolveInlined(TagAddress pc) {
	if (auto it = inlinedCache_.find(pc); it != inlinedCache_.end()) {
		return it->second;
	}

	std::vector<SourceFrame> frames;
#ifdef ENABLE_STACKTRACE
	backward::ResolvedTrace resolved = impl_->resolve(pc);
	auto toFrame = [](const backward::ResolvedTrace::SourceLoc& loc) {
		return SourceFrame {.file = loc.filename, .function = loc.function, .line = loc.line, .column = loc.col};
	};
	if (!resolved.source.filename.empty() || !resolved.source.function.empty()) {
		frames.push_back(toFrame(resolved.source));
		// backward lists the frames `source` is inlined into from the innermost one outwards.
		for (const auto& inliner : resolved.inliners) {
			frames.push_back(toFrame(inliner));
		}
	} else if (!resolved.object_function.empty()) {
		frames.push_back(SourceFrame {.file = resolved.object_filename, .function = resolved.object_function});
	}
#endif
	if (frames.empty()) {
		frames.emplace_back();
	}
	auto [it, _] = inlinedCache_.emplace(pc, std::move(frames));
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
