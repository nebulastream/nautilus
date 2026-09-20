#include "ThreadSlots.hpp"

namespace nautilus::profiling::detail {

ThreadSlots::ThreadSlots(uint16_t capacity) : capacity_(capacity), owners_(capacity) {
}

uint16_t ThreadSlots::acquire() {
	const auto self = std::this_thread::get_id();
	const std::lock_guard lock(mutex_);
	for (uint16_t slot = 0; slot < assignedCount_; ++slot) {
		if (owners_[slot] == self) {
			return slot;
		}
	}
	if (assignedCount_ >= capacity_) {
		return INVALID;
	}
	owners_[assignedCount_] = self;
	return assignedCount_++;
}

uint16_t ThreadSlots::current() const {
	const auto self = std::this_thread::get_id();
	const std::lock_guard lock(mutex_);
	for (uint16_t slot = 0; slot < assignedCount_; ++slot) {
		if (owners_[slot] == self) {
			return slot;
		}
	}
	return INVALID;
}

std::vector<uint16_t> ThreadSlots::assigned() const {
	const std::lock_guard lock(mutex_);
	std::vector<uint16_t> slots;
	slots.reserve(assignedCount_);
	for (uint16_t slot = 0; slot < assignedCount_; ++slot) {
		slots.push_back(slot);
	}
	return slots;
}

void ThreadSlots::clear() {
	const std::lock_guard lock(mutex_);
	for (auto& owner : owners_) {
		owner = std::thread::id {};
	}
	assignedCount_ = 0;
}

} // namespace nautilus::profiling::detail
