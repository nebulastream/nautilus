
#include "nautilus/val.hpp"
#include <cstdint>
namespace nautilus {

// Profiling context to manage profiling settings
class ProfilingContext {
public:
	static bool shouldCollectProfile();
};

// Condition profile to collect branch taken/not taken counts.
// This can be used to collect profiling information for conditional branches.
// The collection might cause some overhead, so it should ony be enabled in a first compilation pass.
class ConditionProfile {
	static constexpr std::uint64_t INITIAL_COUNT_VALUE = 0;
	// Number of times the condition was taken
	std::uint64_t taken = INITIAL_COUNT_VALUE;
	// Number of times the condition was not taken
	std::uint64_t notTaken = INITIAL_COUNT_VALUE;

public:
	// Profile the condition by updating taken/not taken counts
	static void profile(ConditionProfile* p, bool condition);
	// Check if the profile has valid data
	bool isValid();
	// Evaluate the condition with profiling information
	val<bool> eval(val<bool> condition);
};
} // namespace nautilus
