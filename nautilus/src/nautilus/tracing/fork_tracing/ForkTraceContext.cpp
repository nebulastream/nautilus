
#include "nautilus/tracing/fork_tracing/ForkTraceContext.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include <cassert>
#include <cpptrace/cpptrace.hpp>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <ranges>
#include <sstream>
#include <sys/wait.h>

// #define TRACER_LOG(msg, ...) fmt::println(stderr, "[TRACER {}] " msg, getpid() __VA_OPT__(, ) __VA_ARGS__)
#define TRACER_LOG(msg, ...)

enum class ForkRole { CHILD, PARENT };
static thread_local std::vector<std::pair<pid_t, int>> pendingForks;
static thread_local size_t currentForkDepth = 0;
static thread_local uintptr_t traceFunctionAddress = 0;
static thread_local cpptrace::stacktrace forkedAt {};
static thread_local nautilus::tracing::Snapshot forkedTag {};
static thread_local bool invertNextFork = false;

[[noreturn]] void exitChild(std::optional<std::pair<nautilus::tracing::Snapshot, cpptrace::stacktrace>> mergedAt) {
	if (currentForkDepth > 0) {
		auto framesRange =
		    forkedAt.frames |
		    std::views::take_while([](const auto& frame) { return frame.raw_address != traceFunctionAddress; }) |
		    std::views::filter([](const auto& frame) { return !frame.filename.starts_with("/usr/"); }) |
		    std::views::transform([](const auto& frame) {
			    return fmt::format("{} at {}:{}", frame.symbol, frame.filename, frame.line.value_or(-1));
		    });
		std::vector<std::string> result;
		std::ranges::copy(framesRange, std::back_inserter(result));
		std::span<std::string> relevant {result.data(), result.size() - 2};

		fmt::println(stderr, "{:\t>{}} Forked at Tag {} [{}]", "", currentForkDepth, forkedTag,
		             fmt::join(relevant, " <- "));
	}
	for (auto [pid, pipe] : pendingForks) {
		write(pipe, "A", 1);
		int status;
		waitpid(pid, &status, 0);
		assert(status == 0);
	}

	if (mergedAt) {
		auto& [tag, mergedAtTrace] = *mergedAt;
		auto framesRange =
		    mergedAtTrace.frames |
		    std::views::take_while([](const auto& frame) { return frame.raw_address != traceFunctionAddress; }) |
		    std::views::filter([](const auto& frame) { return !frame.filename.starts_with("/usr/"); }) |
		    std::views::transform([](const auto& frame) {
			    return fmt::format("{} at {}:{}", frame.symbol, frame.filename, frame.line.value_or(-1));
		    });
		std::vector<std::string> result;
		std::ranges::copy(framesRange, std::back_inserter(result));
		std::span<std::string> relevant {result.data(), result.size() - 2};

		fmt::println(stderr, "{:\t>{}} Merged at Tag {} [{}]", "", currentForkDepth, tag, fmt::join(relevant, " <- "));
	}

	TRACER_LOG("EXIT");
	// Shared memory is automatically visible to parent - no need to serialize
	_exit(0);
}

ForkRole forkExecution(nautilus::tracing::Snapshot tag, nautilus::tracing::Trace& trace) {
	int sync_pipe[2];
	pipe(sync_pipe);

	int pid = fork();

	if (pid == 0) {
		assert(trace.globalTagMap.header->magic == 0xDEADBEEF);
		close(sync_pipe[1]);
		char buf;
		read(sync_pipe[0], &buf, 1);
		pendingForks.clear();
		forkedTag = tag;
		forkedAt = cpptrace::generate_trace(4);

		// Increment fork depth in child process
		currentForkDepth++;

		// Update max fork depth in shared trace header (atomic update to handle races)
		if (currentForkDepth > trace.view->maxForkDepth) {
			trace.view->maxForkDepth = currentForkDepth;
		}

		TRACER_LOG("Child at depth {}", currentForkDepth);
		return ForkRole::CHILD;
	} else {
		TRACER_LOG("WAITING");
		close(sync_pipe[0]);
		pendingForks.emplace_back(pid, sync_pipe[1]);
		return ForkRole::PARENT;
	}
}

namespace nautilus::tracing {

TypedValueRef ForkTraceContext::registerFunctionArgument(Type type, size_t) {
	return state->executionTrace.addArgument(type);
}

TypedValueRef ForkTraceContext::traceOperation(
    [[maybe_unused]] Op op,
    const std::function<std::pair<TypedValueRef, Trace::TracerOperationIdentifier>(Snapshot&)>& onCreation) {
	auto tag = recordSnapshot();
	if (state->executionTrace.checkTag(tag)) {
		auto [result, oi] = onCreation(tag);
		state->executionTrace.globalTagMap.insert(tag, oi);
		return result;
	} else {
		exitChild({{tag, cpptrace::stacktrace::current(5)}});
	}
}

void ForkTraceContext::traceReturnOperation([[maybe_unused]] Type resultType, const TypedValueRef& ref) {
	assert(resultType == ref.type && "I hope this is true");
	TRACER_LOG("RETURN");
	auto tag = recordSnapshot();
	state->executionTrace.addReturn(tag, ref);
	exitChild({});
}

bool ForkTraceContext::traceCmp(const TypedValueRef& targetRef, const double probability) {

	log::debug("Trace CMP");
	auto tag = recordSnapshot();
	if (!state->executionTrace.checkTag(tag)) {
		exitChild({{tag, cpptrace::stacktrace::current(5)}});
	}
	auto [_, oi] = state->executionTrace.appendCmp(tag, targetRef, probability);
	TRACER_LOG("CMP at position: Block: {} Operation: {}", oi.blockIndex.offset, oi.operationOffset.offset);
	state->executionTrace.globalTagMap.insert(tag, oi);
	state->executionTrace.view->forkCount++;

	bool exploreBranchFirst = !invertNextFork;
	invertNextFork = false;
	if (forkExecution(tag, state->executionTrace) == ForkRole::PARENT) {
		if (exploreBranchFirst) {
			state->executionTrace.getOperation(oi).arguments<CmpInput>().trueBlock =
			    state->executionTrace.createNewBlock().getBlockId();
			state->executionTrace.getOperation(oi).arguments<CmpInput>().falseBlock =
			    std::numeric_limits<std::uint16_t>::max();
		} else {
			state->executionTrace.getOperation(oi).arguments<CmpInput>().falseBlock =
			    state->executionTrace.createNewBlock().getBlockId();
			state->executionTrace.getOperation(oi).arguments<CmpInput>().trueBlock =
			    std::numeric_limits<std::uint16_t>::max();
		}
		return exploreBranchFirst;
	} else {
		auto oi = *state->executionTrace.globalTagMap.get(tag);
		if (exploreBranchFirst) {
			state->executionTrace.getOperation(oi).arguments<CmpInput>().falseBlock =
			    state->executionTrace.createNewBlock().getBlockId();
		} else {
			state->executionTrace.getOperation(oi).arguments<CmpInput>().trueBlock =
			    state->executionTrace.createNewBlock().getBlockId();
		}
		return !exploreBranchFirst;
	}
}

std::unique_ptr<ExecutionTrace> ForkTraceContext::trace(std::function<void()>& traceFunction,
                                                        const engine::Options& options) {
	log::debug("Initialize Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	// Allocate Trace (shared memory, no heap allocs) on the stack
	Trace tracerTrace;

	// Initialize TraceContext with references to our stack objects
	initialize(tr, tracerTrace, options);

	// Reset fork depth for new trace
	currentForkDepth = 0;

	auto pid = fork();
	if (pid == 0) {
		assert(tracerTrace.globalTagMap.header->magic == 0xDEADBEEF);
		tracerTrace.createNewBlock();
		try {
			traceFunctionAddress = cpptrace::object_trace::current(1).frames[0].raw_address;
			traceFunction();
		} catch (...) {
			fmt::println(stderr, "Trace Function threw an exception!. Terminating Trace");
			std::cerr << std::flush;
			_exit(1);
		}
		TRACER_LOG("Actually I am not sure how we got here!");
	} else {
		int status;
		waitpid(pid, &status, 0);
		if (status != 0) {
			throw RuntimeException("Tracing failed");
		}
		TRACER_LOG("Tracing done");
	}
	resetState();

	// Convert Trace to ExecutionTrace for SSA phase and return
	auto executionTrace = toExecutionTrace(tracerTrace);
	log::trace("Final trace: {}", executionTrace);
	log::info("{} Forks, Max Depth: {}", tracerTrace.view->forkCount, tracerTrace.view->maxForkDepth);

	return std::make_unique<ExecutionTrace>(std::move(executionTrace));
}

void ForkTraceContext::suggestInvertedBranch() {
	invertNextFork = true;
}

} // namespace nautilus::tracing
