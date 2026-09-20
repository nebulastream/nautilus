#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace nautilus::profiling {

/// Why this process can or cannot read hardware performance counters.
///
/// Counters are a privileged, environment-dependent resource: a container, a
/// hardened `perf_event_paranoid` setting, a VM without PMU passthrough or a
/// non-Linux host all make them unavailable through no fault of the caller. The
/// plugin therefore never throws on an unavailable subsystem -- every entry
/// point degrades to a no-op and reports why here, so a test can skip and an
/// application can carry on unmeasured.
enum class Availability : uint8_t {
	/// perf_event_open(2) succeeded on a probe event.
	Available,
	/// Not Linux; there is no perf subsystem to talk to.
	UnsupportedPlatform,
	/// The kernel refused the probe (EACCES/EPERM), almost always
	/// `perf_event_paranoid` or a seccomp filter.
	PermissionDenied,
	/// The kernel or the hardware does not offer the event (ENODEV/ENOENT/ENOSYS),
	/// typical inside a VM with no virtualized PMU.
	Unsupported,
	/// The probe failed for a reason none of the above covers.
	Unknown,
};

/// The result of probing the perf subsystem, with a human-readable explanation
/// suitable for a log line or a test's skip message.
struct Support {
	Availability status = Availability::Unknown;
	/// Empty when `status == Availability::Available`.
	std::string reason;

	[[nodiscard]] explicit operator bool() const noexcept {
		return status == Availability::Available;
	}
};

/// Probes the perf subsystem, once per process, and caches the verdict.
///
/// The probe is a real `perf_event_open` for a hardware instruction counter,
/// immediately closed again: reading `/proc/sys/kernel/perf_event_paranoid`
/// alone is not conclusive, because seccomp filters and container policies deny
/// the syscall at a level the sysctl does not describe. The paranoid level is
/// still read, to explain a denial rather than to predict it.
[[nodiscard]] const Support& support() noexcept;

/// Convenience for `bool(support())`.
[[nodiscard]] inline bool available() noexcept {
	return static_cast<bool>(support());
}

/// Convenience for `support().reason`.
[[nodiscard]] inline std::string_view unavailableReason() noexcept {
	return support().reason;
}

} // namespace nautilus::profiling
