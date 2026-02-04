
#include "SymbolicExecutionState.hpp"
#include "TraceTerminationException.hpp"
#include <cassert>

namespace nautilus::tracing {

bool SymbolicExecutionState::record(const Snapshot& tag) {
	// special case if we are currently in the follow mode, we switch to record
	// and change the last decision in this execution path.
	if (currentMode == SymbolicExecutionState::MODE::FOLLOW) {
		currentMode = SymbolicExecutionState::MODE::RECORD;
		currentExecutionPath.getPath().pop_back();
	}

	auto foundTag = tagMap.find(tag);
	if (foundTag == tagMap.end()) {
		// If was not visited yet -> store the execution trace and return true.
		tagMap.emplace(tag, SymbolicExecutionState::TagState::FirstVisit);
		currentExecutionPath.append(true);
		currentExecutionPath.setFinalTag(tag);
		inflightExecutionPaths.emplace_back(currentExecutionPath);
		return true;
	}
	// The tag already exists in the tag map.
	// Thus, the if was visited at least once.
	switch (foundTag->second) {
	case SymbolicExecutionState::TagState::FirstVisit: {
		// Tag is in FirstVisit state. Thus, it was visited one time -> so we visit
		// the false case.
		foundTag->second = SymbolicExecutionState::TagState::SecondVisit;
		currentExecutionPath.append(false);
		return false;
	};
	case SymbolicExecutionState::TagState::SecondVisit: {
		// The tag is in SecondVisit state -> terminate execution.
		// NES_DEBUG("Trace: early terminate via exception.");
		// TraceContext::get()->pause();
		throw TraceTerminationException();
	};
	}
	throw TraceTerminationException();
}

SymbolicExecutionState::MODE SymbolicExecutionState::getCurrentMode() const {
	return this->currentMode;
}

bool SymbolicExecutionState::follow() {
	assert(getCurrentMode() == MODE::FOLLOW);
	if (currentOperation >= currentExecutionPath.getSize() - 1) {
		// we have the last operation
		auto finalTag = currentExecutionPath.getFinalTag();
		return record(finalTag);
	}
	auto operation = currentExecutionPath[currentOperation];
	currentOperation++;
	return get<0>(operation);
}

bool SymbolicExecutionState::shouldFollow() {
	return currentMode == SymbolicExecutionState::MODE::FOLLOW && currentOperation < currentExecutionPath.getSize() - 1;
}

bool SymbolicExecutionState::shouldContinue() {
	if (iterations == 0) {
		return true;
	}
	while (!inflightExecutionPaths.empty()) {
		auto& trace = inflightExecutionPaths.front();
		auto& nextTracePath = trace.getPath();
		if (nextTracePath.empty()) {
			return true;
		}
		auto element = tagMap.find(trace.getFinalTag());
		// NES_ASSERT(element != tagMap.end(), "the tag should exists at this
		// point");
		if (element->second == SymbolicExecutionState::TagState::FirstVisit) {
			return true;
		} else if (element->second == SymbolicExecutionState::TagState::SecondVisit) {
			// the target tag of this path was already visited two times, so tracing
			// can skip it. NES_DEBUG("Skip tag " << element->first);
			inflightExecutionPaths.pop_front();
		}
	};
	return false;
}

void SymbolicExecutionState::next() {
	if (iterations >= MAX_ITERATIONS) {
		// NES_THROW_RUNTIME_ERROR("Tracing got lost and reached the max number of
		// iterations.");
	}
	// if this is the first iteration the execution context is already initialized
	if (iterations > 0) {
		auto& trace = inflightExecutionPaths.front();
		currentMode = SymbolicExecutionState::MODE::FOLLOW;
		currentExecutionPath = std::move(trace);
		inflightExecutionPaths.pop_front();
		currentOperation = 0;
	}
	iterations++;
}

uint64_t SymbolicExecutionState::getIterations() const {
	return iterations;
}

} // namespace nautilus::tracing
