#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <limits>

/**
 * @file
 * @brief Narrow integer arguments of native calls must arrive with their own
 * signedness: a uint8_t 255 is 255 in the callee, not -1.
 *
 * The values are loaded from memory or passed in as function arguments and then
 * handed to a native function that widens them to 64 bit, so the callee
 * observes exactly what the backend put into the argument register.
 */
namespace nautilus::engine {

namespace {

template <typename T>
auto widen(T value) {
	if constexpr (std::is_signed_v<T>) {
		return static_cast<int64_t>(value);
	} else {
		return static_cast<uint64_t>(value);
	}
}

template <typename T>
auto passLoaded(val<T*> ptr) {
	val<T> value = *ptr;
	return invoke(widen<T>, value);
}

template <typename T>
auto passArgument(val<T> value) {
	return invoke(widen<T>, value);
}

template <typename T>
void checkNarrowArgument(NautilusEngine& engine, const std::string& name, std::initializer_list<T> inputs) {
	DYNAMIC_SECTION(name) {
		auto loaded = engine.registerFunction(passLoaded<T>);
		auto argument = engine.registerFunction(passArgument<T>);
		for (T in : inputs) {
			T memory = in;
			INFO("input " << +in);
			REQUIRE(loaded(&memory) == widen<T>(in));
			REQUIRE(argument(in) == widen<T>(in));
		}
	}
}

void narrowCallArgumentTest(NautilusEngine& engine) {
	const auto backend = engine.getNameOfBackend();
	// bc passes narrow unsigned arguments through dyncall's signed types, and so
	// does tbc on Apple arm64, where dyncall packs stack arguments by size.
	bool narrowUnsignedSupported = backend != "bc";
#if defined(__APPLE__) && defined(__aarch64__)
	narrowUnsignedSupported = narrowUnsignedSupported && backend != "tbc";
#endif
	if (narrowUnsignedSupported) {
		checkNarrowArgument<uint8_t>(engine, "ui8", {0, 1, 127, 128, 255});
		checkNarrowArgument<uint16_t>(engine, "ui16", {0, 1, 32767, 32768, 65535});
	}
	checkNarrowArgument<uint32_t>(engine, "ui32", {0, 1, 2147483647u, 2147483648u, 4294967295u});
	checkNarrowArgument<int8_t>(engine, "i8", {0, 1, -1, 127, -128});
	checkNarrowArgument<int16_t>(engine, "i16", {0, 1, -1, 32767, -32768});
	checkNarrowArgument<int32_t>(engine, "i32", {0, 1, -1, INT32_MAX, INT32_MIN});
}

} // namespace

TEST_CASE("Narrow Call Argument Interpreter Test") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	narrowCallArgumentTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Narrow Call Argument Compiler Test") {
	nautilus::testing::forEachBackend([](NautilusEngine& engine) { narrowCallArgumentTest(engine); }, true);
}
#endif

} // namespace nautilus::engine
