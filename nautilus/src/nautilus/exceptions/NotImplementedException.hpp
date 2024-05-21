
#pragma once

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus {

class NotImplementedException : public RuntimeException {
public:
	NotImplementedException(std::string msg);

	~NotImplementedException() noexcept override;
};

} // namespace nautilus
