#include "nautilus/common/ExceptionTransport.hpp"
#include <cassert>

namespace nautilus::compiler {
namespace {

thread_local ExceptionFrame* tlsExceptionFrame = nullptr;

} // namespace

ExceptionFrame* currentExceptionFrame() {
	return tlsExceptionFrame;
}

void pushExceptionFrame(ExceptionFrame* frame) {
	frame->parent = tlsExceptionFrame;
	tlsExceptionFrame = frame;
}

void popExceptionFrame() {
	if (tlsExceptionFrame) {
		tlsExceptionFrame = tlsExceptionFrame->parent;
	}
}

void popExceptionFrame(ExceptionFrame* frame) {
	// Normally `frame` is the top of the stack. If it is not, an inner frame
	// leaked (its owner was skipped by an exception); those frames are dead
	// stack objects, so unwind past them rather than leaving the stack pointing
	// at destroyed memory.
	assert(tlsExceptionFrame == frame && "unbalanced ExceptionFrame push/pop");
	tlsExceptionFrame = frame->parent;
}

bool captureCurrentException() noexcept {
	auto* frame = currentExceptionFrame();
	if (frame == nullptr) {
		return false;
	}
	if (!frame->pending) {
		frame->pending = std::current_exception();
	}
	return true;
}

bool hasPendingException() {
	auto* frame = currentExceptionFrame();
	return frame != nullptr && frame->pending != nullptr;
}

} // namespace nautilus::compiler
