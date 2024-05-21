
#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace nautilus {

/**
 * @brief Exception to be used to report errors and stacktraces
 * This is meant to be used for NES-related errors, wrap std exceptions with their own stacktrace, etc..
 */
class RuntimeException : virtual public std::exception {

protected:
	std::string errorMessage; ///< Error message

public:
	/** Constructor
	 *  @param msg The error message
	 *  @param stacktrace Error stacktrace
	 */
	explicit RuntimeException(std::string msg);

	/** Destructor.
	 *  Virtual to allow for subclassing.
	 */
	virtual ~RuntimeException() noexcept override;

	/** Returns a pointer to the (constant) error description.
	 *  @return A pointer to a const char*. The underlying memory
	 *  is in possession of the Except object. Callers must
	 *  not attempt to free the memory.
	 */
	[[nodiscard]] const char* what() const noexcept override;
};

} // namespace nautilus
