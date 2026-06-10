#pragma once

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus {

/**
 * @brief Thrown by ReplayTraceContext when path exploration exceeds the
 * configured iteration limit (option "engine.traceMaxIterations").
 *
 * Thrown by the exploration driver loop, never through user stack frames.
 */
class TraceLimitException final : public RuntimeException {
public:
	explicit TraceLimitException(const std::string& message);

	~TraceLimitException() noexcept override;
};
} // namespace nautilus
