#pragma once

#include <nautilus/Engine.hpp>

namespace nautilus::engine {

// 1024 textually distinct statements: every one is its own call site, so every operation gets a tag of its own and
// the tag trie sees 1024 children below one node (docs/plans/nautilus-0.4/p0-baseline.md, `distinctSites1024`).
#define NAUTILUS_SITE r = r * 3 + 1;
#define NAUTILUS_SITES_X4(x) x x x x
#define NAUTILUS_SITES_X16(x) NAUTILUS_SITES_X4(NAUTILUS_SITES_X4(x))
#define NAUTILUS_SITES_X64(x) NAUTILUS_SITES_X4(NAUTILUS_SITES_X16(x))
#define NAUTILUS_SITES_X256(x) NAUTILUS_SITES_X4(NAUTILUS_SITES_X64(x))
#define NAUTILUS_SITES_X1024(x) NAUTILUS_SITES_X4(NAUTILUS_SITES_X256(x))

inline val<int64_t> distinctSites1024(val<int64_t> r) {
	NAUTILUS_SITES_X1024(NAUTILUS_SITE)
	return r;
}

} // namespace nautilus::engine
