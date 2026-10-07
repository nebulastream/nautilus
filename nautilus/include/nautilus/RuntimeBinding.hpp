#pragma once

#include "nautilus/RuntimeBindingInfo.hpp"
#include "nautilus/val.hpp"
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>

namespace nautilus {

namespace runtime_binding {

template <typename T>
constexpr std::string_view typeName() {
#if defined(__clang__) || defined(__GNUC__)
	return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
	return __FUNCSIG__;
#else
#error "Runtime bindings require deterministic compiler type names"
#endif
}

template <typename T>
std::string typeSchema() {
	auto schema = std::string(typeName<T>());
	if constexpr (std::is_void_v<T>) {
		return schema + ":0:1";
	} else {
		return schema + ":" + std::to_string(sizeof(T)) + ":" + std::to_string(alignof(T));
	}
}

} // namespace runtime_binding

template <typename T>
class RuntimeBinding {
	static_assert(std::is_object_v<T> || std::is_void_v<T>);

public:
	RuntimeBinding() = default;

	bool isBound() const noexcept {
		return entry_ != nullptr;
	}

	val<T*> get() const {
		if (!entry_) {
			throw std::logic_error("Runtime binding handle is unbound");
		}
#ifdef ENABLE_TRACING
		if (tracing::inTracer()) {
			return val<T*>(tracing::traceRuntimeBinding(*entry_));
		}
#endif
		return val<T*>(static_cast<T*>(entry_->address));
	}

private:
	friend class RuntimeBindings;
	explicit RuntimeBinding(std::shared_ptr<const runtime_binding::Entry> entry) : entry_(std::move(entry)) {
	}
	std::shared_ptr<const runtime_binding::Entry> entry_;
};

template <typename T>
RuntimeBinding<T> RuntimeBindings::bind(std::string identity, T* address) {
	static_assert(std::is_object_v<T> || std::is_void_v<T>);
	return RuntimeBinding<T>(add(std::move(identity), runtime_binding::typeSchema<T>(),
	                             const_cast<void*>(static_cast<const volatile void*>(address))));
}

} // namespace nautilus
