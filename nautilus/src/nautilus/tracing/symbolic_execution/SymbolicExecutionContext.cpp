
#include "SymbolicExecutionContext.hpp"
#include <cassert>

namespace nautilus::tracing {

SymbolicExecutionContext::SymbolicExecutionContext(size_t expectedTags) {
	tagMap.reserve(expectedTags);
}

void SymbolicExecutionContext::reset() {
	// clear() keeps the bucket array, which is the point of pooling scopes.
	tagMap.clear();
	inflightExecutionPaths.clear();
	currentExecutionPath = SymbolicExecutionPath();
	currentMode = MODE::RECORD;
	currentOperation = 0;
	iterations = 0;
}

RecordResult SymbolicExecutionContext::record(const Snapshot& tag) {
	// special case if we are currently in the follow mode, we switch to record
	// and change the last decision in this execution path.
	if (currentMode == SymbolicExecutionContext::MODE::FOLLOW) {
		currentMode = SymbolicExecutionContext::MODE::RECORD;
		currentExecutionPath.getPath().pop_back();
	}

	auto foundTag = tagMap.find(tag);
	if (foundTag == tagMap.end()) {
		// If was not visited yet -> store the execution trace and return true.
		tagMap.emplace(tag, SymbolicExecutionContext::TagState::FirstVisit);
		currentExecutionPath.append(true);
		currentExecutionPath.setFinalTag(tag);
		inflightExecutionPaths.emplace_back(currentExecutionPath);
		return {true, false};
	}
	// The tag already exists in the tag map.
	switch (foundTag->second) {
	case SymbolicExecutionContext::TagState::FirstVisit: {
		foundTag->second = SymbolicExecutionContext::TagState::SecondVisit;
		currentExecutionPath.append(false);
		return {false, false};
	};
	case SymbolicExecutionContext::TagState::SecondVisit: {
		// The tag is in SecondVisit state -> signal termination.
		return {false, true};
	};
	}
	return {false, true};
}

RecordResult SymbolicExecutionContext::follow() {
	assert(getCurrentMode() == MODE::FOLLOW);
	if (currentOperation >= currentExecutionPath.getSize() - 1) {
		// we have the last operation
		auto finalTag = currentExecutionPath.getFinalTag();
		return record(finalTag);
	}
	auto operation = currentExecutionPath[currentOperation];
	currentOperation++;
	return {get<0>(operation), false};
}

SymbolicExecutionContext::MODE SymbolicExecutionContext::getCurrentMode() const {
	return this->currentMode;
}

bool SymbolicExecutionContext::shouldContinue() {
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
		if (element->second == SymbolicExecutionContext::TagState::FirstVisit) {
			return true;
		} else if (element->second == SymbolicExecutionContext::TagState::SecondVisit) {
			// the target tag of this path was already visited two times, so tracing
			// can skip it. NES_DEBUG("Skip tag " << element->first);
			inflightExecutionPaths.pop_front();
		}
	};
	return false;
}

void SymbolicExecutionContext::next() {
	if (iterations >= MAX_ITERATIONS) {
		// NES_THROW_RUNTIME_ERROR("Tracing got lost and reached the max number of
		// iterations.");
	}
	// if this is the first iteration the execution context is already initialized
	if (iterations > 0) {
		auto& trace = inflightExecutionPaths.front();
		currentMode = SymbolicExecutionContext::MODE::FOLLOW;
		currentExecutionPath = std::move(trace);
		inflightExecutionPaths.pop_front();
		currentOperation = 0;
	}
	iterations++;
}

uint64_t SymbolicExecutionContext::getIterations() const {
	return iterations;
}

} // namespace nautilus::tracing
