#include <nautilus/logging.hpp>

namespace nautilus::log::options {
static bool logAddresses = true;
bool getLogAddresses() {
	return logAddresses;
}
void setLogAddresses(bool value) {
	logAddresses = value;
}

static bool logStaticVars = false;
bool getLogStaticVars() {
	return logStaticVars;
}
void setLogStaticVars(bool value) {
	logStaticVars = value;
}
} // namespace nautilus::log::options
