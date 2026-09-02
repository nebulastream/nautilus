#include "nautilus/compiler/ir/IntrinsicRegistry.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus::compiler::ir {

IntrinsicRegistry& IntrinsicRegistry::instance() {
	static IntrinsicRegistry registry;
	return registry;
}

IntrinsicId IntrinsicRegistry::registerIntrinsic(void* address, std::string_view name) {
	if (address == nullptr) {
		// The fallback invariant, enforced at the only place it can be: an
		// intrinsic without a callable address is one the C++, bytecode and
		// threaded-bytecode backends could never emit, and the failure would
		// surface as a call through a null pointer rather than as the
		// registration bug it is.
		throw RuntimeException("an intrinsic must be registered with the address of its native implementation");
	}

	std::lock_guard lock(mutex_);
	if (const auto it = byAddress_.find(address); it != byAddress_.end()) {
		return it->second;
	}
	names_.emplace_back(name);
	const auto id = static_cast<IntrinsicId>(names_.size());
	byAddress_.emplace(address, id);
	return id;
}

IntrinsicId IntrinsicRegistry::lookup(void* address) const {
	if (address == nullptr) {
		return IntrinsicId::None;
	}
	std::lock_guard lock(mutex_);
	const auto it = byAddress_.find(address);
	return it != byAddress_.end() ? it->second : IntrinsicId::None;
}

std::string IntrinsicRegistry::getName(IntrinsicId id) const {
	if (id == IntrinsicId::None) {
		return {};
	}
	std::lock_guard lock(mutex_);
	const auto index = static_cast<size_t>(id) - 1;
	return index < names_.size() ? names_[index] : std::string {};
}

size_t IntrinsicRegistry::size() const {
	std::lock_guard lock(mutex_);
	return names_.size();
}

} // namespace nautilus::compiler::ir
