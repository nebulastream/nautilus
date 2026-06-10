#pragma once

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus {

/**
 * @brief Thrown by ReplayTraceContext when a re-execution of the traced
 * function diverges from the recorded replay log, i.e. the function is not
 * deterministic across re-executions.
 *
 * The exception is thrown by the exploration driver loop after the traced
 * function has returned - never through user stack frames.
 */
class TraceDivergenceException final : public RuntimeException {
public:
	explicit TraceDivergenceException(const std::string& message);

	~TraceDivergenceException() noexcept override;
};
} // namespace nautilus
