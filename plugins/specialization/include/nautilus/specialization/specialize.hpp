#pragma once

#include "nautilus/specialization/profile.hpp"
#include "nautilus/val.hpp"
#include <cstdint>

namespace nautilus {

/// Runtime value profile for a single function argument.
///
/// `ValueProfile<T>` is the value-specialization counterpart of
/// `ConditionProfile`: instead of tracking how often a boolean is taken,
/// it tracks the most-recently-seen value of an integral argument and the
/// number of times that value reoccurred. The profile is *read* at trace
/// time by `SpecializedNautilusFunction` to decide which dispatcher
/// variant to emit, and *written* at runtime via the `profile()` proxy
/// call that the dispatcher injects into the generic path.
///
/// The implementation is intentionally tiny (last value + hit count + total
/// count) so that the per-call profiling overhead is two stores and one
/// compare. A "stable" profile is one where the dominant value's hit ratio
/// exceeds `threshold` after at least `min_samples` total samples.
template <class T>
class ValueProfile {
	static_assert(std::is_integral_v<T>, "ValueProfile only supports integral types in v1");

public:
	/// Runtime profiling hook. Registered as a proxy call by
	/// `SpecializedNautilusFunction`. Mirrors `ConditionProfile::profile`.
	static void profile(ValueProfile<T>* p, T value) {
		if (p->total_ == 0 || p->last_ != value) {
			p->last_ = value;
			p->hits_ = 1;
		} else {
			p->hits_++;
		}
		p->total_++;
	}

	/// True if the dominant value has been observed often enough to be
	/// worth specializing on. Read at trace time only.
	bool isStable(double threshold = 0.95, std::uint64_t min_samples = 1024) const {
		if (total_ < min_samples) {
			return false;
		}
		return static_cast<double>(hits_) / static_cast<double>(total_) >= threshold;
	}

	/// The current dominant value. Only meaningful when `isStable()` is true.
	T dominant() const {
		return last_;
	}

private:
	T last_ = T {};
	std::uint64_t hits_ = 0;
	std::uint64_t total_ = 0;
};

} // namespace nautilus
