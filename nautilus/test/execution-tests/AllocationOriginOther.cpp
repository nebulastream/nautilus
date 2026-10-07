#include "nautilus/common/TypedAllocation.hpp"
#include <cstdint>

namespace nautilus::engine {
namespace {

struct SameAllocationLayout {
	int64_t value;
};

} // namespace

TypedAllocation allocationFromOtherTranslationUnit() {
	return TypedAllocation::forType<SameAllocationLayout>();
}

} // namespace nautilus::engine
