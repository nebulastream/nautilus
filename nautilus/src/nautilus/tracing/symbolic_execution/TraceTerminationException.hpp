#pragma once

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus::tracing {

/**
 * @brief This exceptions indicates the termination of an symbolic execution.
 * This exception should never be used, to indicate errors to the runtime.
 */
class TraceTerminationException final : public std::exception {
public:
	explicit TraceTerminationException();

	~TraceTerminationException() override;
};
} // namespace nautilus::tracing
