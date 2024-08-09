
#include "nautilus/exceptions/Stacktrace.hpp"
#include "nautilus/config.hpp"

#ifdef ENABLE_STACKTRACE

#include <backward.hpp>

static backward::SignalHandling sh;
namespace nautilus {
std::string printAndCollectCurrentStacktrace() {
	using namespace backward;
	backward::StackTrace stackTrace;
	backward::Printer printer;
	stackTrace.load_here(32);
	printer.object = true;
	printer.color_mode = backward::ColorMode::always;
	std::stringbuf buffer;
	std::ostream os(&buffer);
	printer.print(stackTrace, os);
	return buffer.str();
}
} // namespace nautilus
#else
namespace nautilus {
std::string printAndCollectCurrentStacktrace() {
	// nop if stacktrace is disabled.
	return {};
}
} // namespace nautilus
#endif
