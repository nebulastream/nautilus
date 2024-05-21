
#pragma once

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus {

class TagCreationException final : public RuntimeException {
public:
	explicit TagCreationException(const std::string& message);

	~TagCreationException() noexcept override;
};
} // namespace nautilus
