
// Micro-benchmark comparing the per-branch snapshot primitives of the once-per-path
// tracers: an in-process stack copy + context capture (stackCopyTracing) versus a
// process snapshot via fork (forkTracing), with and without a state handoff over a
// socket. The numbers quantify why stackCopyTracing is the recommended mode for
// tracing throughput while forkTracing trades speed for full process isolation.

#if defined(__linux__) || defined(__APPLE__)

#include "nautilus/tracing/fiber/ContextSwitch.hpp"
#include "nautilus/tracing/fiber/FiberStack.hpp"
#include <catch2/catch_all.hpp>
#include <cstring>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#ifdef NAUTILUS_FIBER_SUPPORTED

namespace nautilus::tracing {

TEST_CASE("Snapshot Mechanism Benchmark") {
	constexpr size_t stackBytes[] = {4 * 1024, 16 * 1024, 64 * 1024};

	// (a) stack-copy snapshot: capture the register context and memcpy the live
	// stack region, as StackCopyTraceContext does per recorded branch.
	for (auto liveBytes : stackBytes) {
		detail::FiberStack stack {1024 * 1024};
		std::vector<std::byte> snapshot(liveBytes);
		Catch::Benchmark::Benchmark("stack_copy_snapshot_" + std::to_string(liveBytes / 1024) + "k")
		    .operator=([&](Catch::Benchmark::Chronometer meter) {
			    meter.measure([&] {
				    detail::CapturedContext context;
				    (void) detail::nautilus_captureContext(&context);
				    std::memcpy(snapshot.data(), stack.top() - liveBytes, liveBytes);
				    return snapshot[0];
			    });
		    });
	}

	// (b) fork snapshot: fork + immediate child exit + waitpid. Lower bound for any
	// fork-per-branch scheme, without trace-state transfer.
	Catch::Benchmark::Benchmark("fork_snapshot").operator=([&](Catch::Benchmark::Chronometer meter) {
		meter.measure([&] {
			auto pid = fork();
			if (pid == 0) {
				_exit(0);
			}
			int status = 0;
			waitpid(pid, &status, 0);
			return status;
		});
	});

	// (c) fork + handoff: fork, transfer a trace-state-sized payload to the child
	// over a socketpair and wait for it - the full per-branch cost of forkTracing.
	for (auto payloadBytes : stackBytes) {
		std::vector<std::byte> payload(payloadBytes);
		Catch::Benchmark::Benchmark("fork_handoff_" + std::to_string(payloadBytes / 1024) + "k")
		    .operator=([&](Catch::Benchmark::Chronometer meter) {
			    meter.measure([&] {
				    int pair[2];
				    REQUIRE(socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == 0);
				    auto pid = fork();
				    if (pid == 0) {
					    ::close(pair[1]);
					    std::vector<std::byte> received(payload.size());
					    size_t consumed = 0;
					    while (consumed < received.size()) {
						    auto r = read(pair[0], received.data() + consumed, received.size() - consumed);
						    if (r <= 0) {
							    _exit(1);
						    }
						    consumed += static_cast<size_t>(r);
					    }
					    _exit(0);
				    }
				    ::close(pair[0]);
				    size_t written = 0;
				    while (written < payload.size()) {
					    auto w = write(pair[1], payload.data() + written, payload.size() - written);
					    if (w <= 0) {
						    break;
					    }
					    written += static_cast<size_t>(w);
				    }
				    ::close(pair[1]);
				    int status = 0;
				    waitpid(pid, &status, 0);
				    return status;
			    });
		    });
	}
}

} // namespace nautilus::tracing

#endif // NAUTILUS_FIBER_SUPPORTED
#endif // __linux__ || __APPLE__
