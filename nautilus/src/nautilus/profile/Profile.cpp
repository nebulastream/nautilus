#include "nautilus/profile/profile.hpp"
#include "nautilus/function.hpp"
#include "nautilus/val_ptr.hpp"
namespace nautilus {

bool ProfilingContext::shouldCollectProfile() {
	return true;
}

void ConditionProfile::profile(ConditionProfile* p, bool condition) {
	if (condition) {
		p->taken++;
	} else {
		p->notTaken++;
	}
}

bool ConditionProfile::isValid() {
	return taken != INITIAL_COUNT_VALUE || notTaken != INITIAL_COUNT_VALUE;
}

// Evaluate the condition with profiling information
val<bool> ConditionProfile::eval(val<bool> condition) {
	// if we have valid profile data, set the probability and use it during compilation
	// if no profing data was collected, we invoke the profiling function to collect data at runtime
	if (this->isValid()) {
		// set probability based on profile
		condition.setIsTrueProbability(static_cast<double>(taken) / (taken + notTaken));
	} else if (ProfilingContext::shouldCollectProfile()) {
		invoke(profile, val<ConditionProfile*>(this), condition);
	}
	return condition;
}
} // namespace nautilus
