# xxHash (vendored)

`xxhash.h` is xxHash v0.8.2 (https://github.com/Cyan4973/xxHash/releases/tag/v0.8.2), unmodified, used header-only
with `XXH_INLINE_ALL` for the NIR structural hash (XXH3-128, docs/plans/nautilus-0.4/13-nir-spec.md §7.2). The
library and header are BSD 2-Clause licensed (`LICENSE`). Bump the version only together with a new NIR major
version: the hash is a cache key that must not change within a major version.
