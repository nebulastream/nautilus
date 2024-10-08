#include <nautilus/logging.hpp>

namespace nautilus::log::options {
static bool logAddresses = true;
bool getLogAddresses() {
	return logAddresses;
}
void setLogAddresses(bool value) {
	logAddresses = value;
}
} // namespace nautilus::log::options
