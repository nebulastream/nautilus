#pragma once

#include "nautilus/tracing/tag/Tag.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nautilus::tracing {

/// A source-level frame for one return address. An empty `file` and empty
/// `function` together mean "unresolved"; resolved frames always have at
/// least one non-empty string.
struct SourceFrame {
	std::string file;
	std::string function;
	uint32_t line = 0;
	uint32_t column = 0;
};

/// Symbolises return addresses captured by the tag recorder into
/// `(file, line, function)` triples and drops Nautilus-internal frames so
/// callers see only the user's code-generation hierarchy.  The first call
/// to a known PC parses DWARF; results are cached for the resolver's lifetime.
class SourceLocationResolver {
public:
	SourceLocationResolver();
	~SourceLocationResolver();

	SourceLocationResolver(const SourceLocationResolver&) = delete;
	SourceLocationResolver& operator=(const SourceLocationResolver&) = delete;

	const SourceFrame& resolve(TagAddress pc);

	/// Order is outer-to-inner: element 0 is the caller furthest from the
	/// traced operation, the last element is the innermost user frame.
	std::vector<SourceFrame> resolveStack(const Tag* leaf);

	void addInternalPathPrefix(std::string prefix);

private:
	bool isInternalFrame(const SourceFrame& frame) const;

	struct Impl;
	std::unique_ptr<Impl> impl_;
	std::unordered_map<TagAddress, SourceFrame> cache_;
	std::vector<std::string> internalPrefixes_;
};

} // namespace nautilus::tracing
