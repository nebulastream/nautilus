#pragma once

#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstdint>

namespace nautilus::compiler::ir {

enum LoadHintFlags : uint8_t {
	LOAD_HINT_NONE = 0,
	LOAD_HINT_INVARIANT = 1 << 0,
	LOAD_HINT_NONNULL = 1 << 1,
	LOAD_HINT_DEREFERENCEABLE = 1 << 2,
	LOAD_HINT_RANGE = 1 << 3,
};

struct LoadHints {
	uint8_t flags = LOAD_HINT_NONE;
	size_t dereferenceable_bytes = 0;
	int64_t range_lo = 0;
	int64_t range_hi = 0;

	bool hasHint(LoadHintFlags hint) const {
		return (flags & hint) != 0;
	}
};

class LoadOperation : public Operation {
public:
	explicit LoadOperation(const OperationIdentifier& identifier, Operation* address, Type stamp);
	explicit LoadOperation(const OperationIdentifier& identifier, Operation* address, Type stamp, LoadHints hints);

	~LoadOperation() override = default;

	const Operation* getAddress() const;
	const LoadHints& getLoadHints() const;

private:
	LoadHints hints_;
};
} // namespace nautilus::compiler::ir
