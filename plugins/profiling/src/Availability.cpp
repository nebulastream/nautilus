#include "nautilus/profiling/availability.hpp"
#include <asm/unistd.h>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <linux/perf_event.h>
#include <string>
#include <unistd.h>

namespace nautilus::profiling {

namespace {

/// The `perf_event_paranoid` sysctl, or nullopt when it cannot be read. Used
/// only to explain a denial: the levels are 4 (deny everything, some distro
/// kernels), 3, 2 (no kernel/tracepoint measurement), 1, 0 and -1 (allow all).
std::string readParanoidLevel() {
	std::ifstream file("/proc/sys/kernel/perf_event_paranoid");
	std::string level;
	if (file && (file >> level)) {
		return level;
	}
	return {};
}

Support probe() {
	perf_event_attr attr {};
	attr.type = PERF_TYPE_HARDWARE;
	attr.size = sizeof(perf_event_attr);
	attr.config = PERF_COUNT_HW_INSTRUCTIONS;
	// Count this thread on any CPU, excluding the kernel and the hypervisor: the
	// narrowest request there is, so a denial means counters are unavailable
	// outright rather than merely restricted. Measuring kernel time needs
	// paranoid <= 1, which is a stricter requirement than the plugin imposes.
	attr.disabled = 1;
	attr.exclude_kernel = 1;
	attr.exclude_hv = 1;

	const long fd = syscall(__NR_perf_event_open, &attr, /*pid=*/0, /*cpu=*/-1, /*group_fd=*/-1, /*flags=*/0);
	if (fd >= 0) {
		close(static_cast<int>(fd));
		return {Availability::Available, {}};
	}

	const int err = errno;
	const std::string detail = std::string(" (") + std::strerror(err) + ")";
	switch (err) {
	case EACCES:
	case EPERM: {
		std::string reason = "perf_event_open denied" + detail;
		if (const auto level = readParanoidLevel(); !level.empty()) {
			reason += "; /proc/sys/kernel/perf_event_paranoid is " + level +
			          ", needs to be 2 or lower for user-space counters";
		} else {
			reason += "; the syscall is most likely filtered by a container or seccomp policy";
		}
		return {Availability::PermissionDenied, std::move(reason)};
	}
	case ENODEV:
	case ENOENT:
	case ENOSYS:
	case EOPNOTSUPP:
		return {Availability::Unsupported, "the kernel or the hardware does not provide a cycle/instruction PMU" +
		                                       detail + "; virtual machines without PMU passthrough behave this way"};
	default:
		return {Availability::Unknown, "perf_event_open failed" + detail};
	}
}

} // namespace

const Support& support() noexcept {
	// Probed once: the verdict cannot change within a process, and a failing
	// probe on every construction would be a syscall per CounterSet.
	static const Support cached = probe();
	return cached;
}

} // namespace nautilus::profiling
