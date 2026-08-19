#include "nautilus/common/ExceptionTransport.hpp"

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

bool hasPendingException() {
	auto* frame = currentExceptionFrame();
	return frame != nullptr && frame->pending != nullptr;
}

} // namespace nautilus::compiler
