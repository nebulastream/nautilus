// Regression test for https://github.com/nebulastream/nautilus/issues/474:
// invoke() used to require its return type R to be default-constructible even
// when the invoked function pointer was genuinely `noexcept`, because the
// runtime `fnAttrs.noUnwind ? ... : ...` dispatch in
// CallableRuntimeFunction::operator() odr-used
// compiler::captureThrowingCall<R, Args...> (whose `catch` block needs
// `R{}`) unconditionally, even on the branch that could never reach it. This
// made it impossible to invoke() a noexcept function returning a
// deliberately-non-default-constructible strong-id/newtype wrapper.
//
// This mirrors the self-contained repro from the issue: a strong-id type
// `Id` with no default constructor, wired up as a val<Id> the same way
// NebulaStream's strong-id wrappers are, and a noexcept proxy function
// returning it.

#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/Types.hpp"
#include "nautilus/val_ptr.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus {

// A "strong id" type, deliberately NOT default-constructible.
class Id {
public:
	explicit Id(unsigned long value) : value(value) {
	}
	[[nodiscard]] unsigned long getRawValue() const {
		return value;
	}

private:
	unsigned long value;
};

namespace tracing {
template <>
struct TypeResolver<Id> {
	static constexpr Type to_type() {
		return TypeResolver<unsigned long>::to_type();
	}
};
} // namespace tracing

template <>
class val<Id> {
public:
	val(unsigned long value) : value(value) { // NOLINT: intentionally non-explicit
	}
	val(Id id) : value(id.getRawValue()) { // NOLINT: needed for the non-tracing (interpreted) call path
	}
#ifdef ENABLE_TRACING
	// Needed so CallableRuntimeFunction::operator() can wrap the traced result
	// reference in a val<Id> on the (unexercised by this test, but still
	// compiled) tracing path.
	explicit val(tracing::TypedValueRef ref) : value(ref) {
	}
#endif
	val<unsigned long> value;
};

// noexcept: this call can never actually take the exceptional path.
Id nonDefaultConstructibleProxy(const int* p) noexcept {
	return Id(static_cast<unsigned long>(*p));
}

val<Id> invokeNonDefaultConstructibleReturn(val<int*> p) {
	return invoke(nonDefaultConstructibleProxy, p);
}

TEST_CASE("invoke() accepts a noexcept function returning a non-default-constructible type") {
	int value = 42;
	val<int*> p = &value;
	val<Id> result = invokeNonDefaultConstructibleReturn(p);
	REQUIRE(result.value == 42UL);
}

} // namespace nautilus
