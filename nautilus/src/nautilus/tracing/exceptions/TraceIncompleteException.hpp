#pragma once

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus {

/**
 * @brief Thrown by ReplayTraceContext when passive replay cannot drive the
 * traced function to its natural exit within the configured step limit
 * (option "engine.tracePassiveStepLimit").
 *
 * This typically indicates a loop that only exits via break on a runtime
 * condition (e.g. `for (;;) { if (c) break; }`), which passive mode - always
 * answering false - can never leave. Unlike the other replay-tracing errors,
 * this exception unwinds through the traced function's stack frames; this is
 * a deliberate, unrecoverable abort.
 */
class TraceIncompleteException final : public RuntimeException {
public:
	explicit TraceIncompleteException(const std::string& message);

	~TraceIncompleteException() noexcept override;
};
} // namespace nautilus
