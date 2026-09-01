#include "nautilus/common/ExceptionTransport.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>
#include <string>
#include <thread>

using nautilus::compiler::captureCurrentException;
using nautilus::compiler::captureThrowingCall;
using nautilus::compiler::currentExceptionFrame;
using nautilus::compiler::ExceptionFrame;
using nautilus::compiler::ExceptionFrameScope;
using nautilus::compiler::hasPendingException;
using nautilus::compiler::popExceptionFrame;
using nautilus::compiler::pushExceptionFrame;

namespace {

int doubleIt(int x) {
	return x * 2;
}

void throwVoid() {
	throw std::runtime_error("void");
}

} // namespace

TEST_CASE("ExceptionFrame TLS push/pop", "[ExceptionTransport]") {
	ExceptionFrame a;
	ExceptionFrame b;
	pushExceptionFrame(&a);
	REQUIRE(currentExceptionFrame() == &a);
	pushExceptionFrame(&b);
	REQUIRE(currentExceptionFrame() == &b);
	REQUIRE(currentExceptionFrame()->parent == &a);
	popExceptionFrame();
	REQUIRE(currentExceptionFrame() == &a);
	popExceptionFrame();
	REQUIRE(currentExceptionFrame() == nullptr);
}

TEST_CASE("ExceptionFrame nested frames isolate", "[ExceptionTransport]") {
	ExceptionFrame a;
	ExceptionFrame b;
	pushExceptionFrame(&a);
	pushExceptionFrame(&b);
	b.pending = std::make_exception_ptr(std::runtime_error("b"));
	REQUIRE(hasPendingException()); // uses b
	popExceptionFrame();
	REQUIRE(!hasPendingException()); // a has no pending
	popExceptionFrame();
}

TEST_CASE("captureThrowingCall returns on success", "[ExceptionTransport]") {
	ExceptionFrame frame;
	pushExceptionFrame(&frame);
	auto result = captureThrowingCall(doubleIt, 21);
	REQUIRE(result == 42);
	REQUIRE(!hasPendingException());
	popExceptionFrame();
}

TEST_CASE("captureThrowingCall stores exception", "[ExceptionTransport]") {
	ExceptionFrame frame;
	pushExceptionFrame(&frame);
	captureThrowingCall(+[](int) -> int { throw std::runtime_error("test"); }, 0);
	REQUIRE(hasPendingException());
	REQUIRE_THROWS_AS(std::rethrow_exception(frame.pending), std::runtime_error);
	popExceptionFrame();
}

TEST_CASE("captureThrowingCall void return", "[ExceptionTransport]") {
	ExceptionFrame frame;
	pushExceptionFrame(&frame);
	captureThrowingCall(throwVoid);
	REQUIRE(hasPendingException());
	popExceptionFrame();
}

TEST_CASE("captureThrowingCall does not overwrite pending", "[ExceptionTransport]") {
	ExceptionFrame frame;
	pushExceptionFrame(&frame);
	frame.pending = std::make_exception_ptr(std::runtime_error("first"));
	captureThrowingCall(+[]() { throw std::runtime_error("second"); });
	REQUIRE(hasPendingException());
	std::string msg;
	try {
		std::rethrow_exception(frame.pending);
	} catch (const std::runtime_error& e) {
		msg = e.what();
	}
	REQUIRE(msg == "first"); // does NOT overwrite
	popExceptionFrame();
}

TEST_CASE("concurrent invocations isolate frames", "[ExceptionTransport]") {
	ExceptionFrame t1;
	ExceptionFrame t2;
	std::thread th1([&] {
		pushExceptionFrame(&t1);
		captureThrowingCall(+[]() { throw std::runtime_error("t1"); });
		REQUIRE(hasPendingException());
		popExceptionFrame();
	});
	std::thread th2([&] {
		pushExceptionFrame(&t2);
		REQUIRE(!hasPendingException());
		popExceptionFrame();
	});
	th1.join();
	th2.join();
}

TEST_CASE("ExceptionFrameScope pops when the guarded call throws", "[ExceptionTransport]") {
	// The hand-rolled push/call/pop this replaced skipped the pop whenever the
	// call threw, leaving the TLS stack pointing at a destroyed stack frame.
	REQUIRE(currentExceptionFrame() == nullptr);
	REQUIRE_THROWS_AS(
	    [] {
		    ExceptionFrameScope scope(true);
		    REQUIRE(currentExceptionFrame() != nullptr);
		    throw std::runtime_error("escapes past the guard");
	    }(),
	    std::runtime_error);
	REQUIRE(currentExceptionFrame() == nullptr);
}

TEST_CASE("ExceptionFrameScope nests and restores the parent", "[ExceptionTransport]") {
	ExceptionFrameScope outer(true);
	auto* outerFrame = currentExceptionFrame();
	REQUIRE(outerFrame != nullptr);
	{
		ExceptionFrameScope inner(true);
		REQUIRE(currentExceptionFrame() != outerFrame);
		REQUIRE(currentExceptionFrame()->parent == outerFrame);
	}
	REQUIRE(currentExceptionFrame() == outerFrame);
}

TEST_CASE("disabled ExceptionFrameScope pushes nothing", "[ExceptionTransport]") {
	REQUIRE(currentExceptionFrame() == nullptr);
	{
		ExceptionFrameScope scope(false);
		REQUIRE(currentExceptionFrame() == nullptr);
		REQUIRE_FALSE(scope.pending());
	}
	REQUIRE(currentExceptionFrame() == nullptr);
}

TEST_CASE("a frame is reusable after it captured an exception", "[ExceptionTransport]") {
	// One invocation throwing must not poison the next one on the same thread.
	{
		ExceptionFrameScope first(true);
		captureThrowingCall(+[]() { throw std::runtime_error("boom"); });
		REQUIRE(first.pending());
	}
	REQUIRE(currentExceptionFrame() == nullptr);
	{
		ExceptionFrameScope second(true);
		REQUIRE_FALSE(second.pending());
		REQUIRE(captureThrowingCall(+[](int x) { return x + 1; }, 41) == 42);
		REQUIRE_FALSE(second.pending());
	}
}

TEST_CASE("captureCurrentException reports a missing frame", "[ExceptionTransport]") {
	REQUIRE(currentExceptionFrame() == nullptr);
	bool captured = true;
	try {
		throw std::runtime_error("nowhere to put this");
	} catch (...) {
		captured = captureCurrentException();
	}
	// False tells the caller to rethrow rather than silently drop it, which is
	// what the BC dyncall handlers now do when driven without a frame.
	REQUIRE_FALSE(captured);

	ExceptionFrameScope scope(true);
	try {
		throw std::runtime_error("this one lands");
	} catch (...) {
		REQUIRE(captureCurrentException());
	}
	REQUIRE(scope.pending());
}
