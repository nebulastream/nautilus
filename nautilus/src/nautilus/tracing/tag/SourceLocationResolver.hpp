
#pragma once

#include "SourceLocation.hpp"
#include "Tag.hpp"
#include <unordered_map>

namespace nautilus::tracing {

/**
 * @brief Resolves tag address chains to source location hierarchies using debug info.
 *
 * Uses backward-cpp to resolve raw addresses to file/line/function info.
 * Filters out nautilus-internal frames so only user code is visible.
 */
class SourceLocationResolver {
public:
	/**
	 * @brief Resolve a Tag* into a full call hierarchy of source locations.
	 * Walks parent pointers (via getAddressChain()), resolves each address,
	 * and filters out nautilus-internal frames.
	 * @param tag The tag to resolve
	 * @return SourceLocationChain chain from innermost to outermost user frame
	 */
	SourceLocationChain resolve(Tag* tag) const;

	/**
	 * @brief Resolve a single TagAddress to a source location.
	 * @param address The address to resolve
	 * @return SourceLocation the resolved location (may be invalid if resolution fails)
	 */
	SourceLocation resolve(TagAddress address) const;

private:
	bool isUserFrame(const SourceLocation& loc) const;

	mutable std::unordered_map<TagAddress, SourceLocation> cache_;
};

} // namespace nautilus::tracing
