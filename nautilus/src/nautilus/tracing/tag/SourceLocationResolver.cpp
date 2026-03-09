
#include "SourceLocationResolver.hpp"
#include "nautilus/config.hpp"

#ifdef ENABLE_STACKTRACE
#include <backward.hpp>
#endif

namespace nautilus::tracing {

SourceLocation SourceLocationResolver::resolve(TagAddress address) const {
	if (auto it = cache_.find(address); it != cache_.end()) {
		return it->second;
	}

	SourceLocation loc;

#ifdef ENABLE_STACKTRACE
	backward::ResolvedTrace trace;
	trace.addr = reinterpret_cast<void*>(address);
	trace.idx = 0;

	backward::TraceResolver resolver;
	void* addrPtr = trace.addr;
	resolver.load_addresses(&addrPtr, 1);
	auto resolved = resolver.resolve(trace);

	if (!resolved.source.filename.empty()) {
		loc.file = resolved.source.filename;
		loc.line = resolved.source.line;
		loc.column = resolved.source.col;
		loc.function = resolved.source.function;
	}
#endif

	cache_[address] = loc;
	return loc;
}

SourceLocationChain SourceLocationResolver::resolve(Tag* tag) const {
	if (tag == nullptr) {
		return {};
	}
	auto addresses = tag->getAddressChain();
	SourceLocationChain chain;
	for (auto addr : addresses) {
		auto loc = resolve(addr);
		if (loc.isValid() && isUserFrame(loc)) {
			chain.push_back(std::move(loc));
		}
	}
	return chain;
}

bool SourceLocationResolver::isUserFrame(const SourceLocation& loc) const {
	// Filter out nautilus-internal frames (val<T> operators, tracing infra, etc.)
	return loc.file.find("nautilus/src/nautilus/") == std::string::npos &&
	       loc.file.find("nautilus/include/nautilus/") == std::string::npos;
}

} // namespace nautilus::tracing
