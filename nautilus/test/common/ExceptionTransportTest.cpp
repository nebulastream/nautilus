#include "nautilus/common/ExceptionTransport.hpp"
#include <catch2/catch_all.hpp>
#include <stdexcept>
#include <string>
#include <thread>

using nautilus::compiler::captureThrowingCall;
using nautilus::compiler::currentExceptionFrame;
using nautilus::compiler::ExceptionFrame;
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
