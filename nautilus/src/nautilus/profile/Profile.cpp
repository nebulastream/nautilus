#include "nautilus/profile/profile.hpp"

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
	return (taken + notTaken) > 0;
}

// Evaluate the condition with profiling information
val<bool> ConditionProfile::eval(val<bool> condition) {
	if (this->isValid()) {
		// set probability based on profile
		condition.setProbability(static_cast<double>(taken) / (taken + notTaken));
	} else if (ProfilingContext::shouldCollectProfile()) {
		invoke(profile, val<ConditionProfile*>(this), condition);
	}
	return condition;
}
} // namespace nautilus
