#include "nautilus/logging.hpp"
#include <nautilus/static.hpp>

namespace nautilus {

void WarnStaticValIterationLimit(int64_t limit) {
	log::warn("static_val exceeded {} iterations. This may cause excessive trace sizes and long compilation times. "
	          "Consider using a regular val<> loop instead.",
	          limit);
}

} // namespace nautilus
