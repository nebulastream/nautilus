
#include "nautilus/val.hpp"
#include <cstdint>
namespace nautilus {

// Profiling context to manage profiling settings
class ProfilingContext {
public:
	static bool shouldCollectProfile();
};

// Condition profile to collect branch taken/not taken counts
class ConditionProfile {
	// Count of times the condition was taken
	std::int64_t taken;
	// Count of times the condition was not taken
	std::int64_t notTaken;

public:
	// Profile the condition by updating taken/not taken counts
	static void profile(ConditionProfile* p, bool condition);
	// Check if the profile has valid data
	bool isValid();
	// Evaluate the condition with profiling information
	val<bool> eval(val<bool> condition);
};
} // namespace nautilus
