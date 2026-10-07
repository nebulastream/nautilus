#pragma once

#include <cstddef>
#include <source_location>
#include <string_view>
#include <type_traits>

namespace nautilus {

class TypedAllocation {
public:
	template <typename T>
	static constexpr TypedAllocation forType() {
		static_assert(std::is_object_v<T>);
		return TypedAllocation(sizeof(T), alignof(T), std::source_location::current().function_name(), &identity<T>);
	}

	constexpr size_t getSize() const {
		return size;
	}

	constexpr size_t getAlignment() const {
		return alignment;
	}

	constexpr std::string_view getType() const {
		return type;
	}

	bool operator==(const TypedAllocation&) const = default;

private:
	template <typename T>
	inline static const char identity = 0;

	constexpr TypedAllocation(size_t size, size_t alignment, std::string_view type, const void* typeIdentity)
	    : size(size), alignment(alignment), type(type), typeIdentity(typeIdentity) {
	}

	size_t size;
	size_t alignment;
	std::string_view type;
	const void* typeIdentity;
};

} // namespace nautilus
