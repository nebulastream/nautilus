#pragma once

// Internals shared by the C API translation units. Not installed.

#include "nautilus/Engine.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/c/common.h"
#include "nautilus/c/engine.h"
#include "nautilus/c/ir.h"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/options.hpp"
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <memory>
#include <new>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/// One function under construction. The FunctionOperation is created only on
/// finish, because it takes its block list by value; until then the blocks
/// live in the graph's arena and are tracked here.
struct NautilusIROpaqueFunctionBuilder {
	NautilusIROpaqueGraph* graph;
	std::string name;
	nautilus::Type returnType;
	/// Interned on first use, so a builder abandoned before anyone asked for
	/// its id leaves nothing behind in the function table.
	nautilus::compiler::ir::FunctionId calleeId = nautilus::compiler::ir::INVALID_FUNCTION_ID;
	std::vector<nautilus::compiler::ir::BasicBlock*> blocks;
	std::unordered_set<const nautilus::compiler::ir::BasicBlock*> ownedBlocks;
	std::vector<nautilus::compiler::ir::AllocaSpec> allocaSpecs;
	std::unordered_map<std::string, std::string> attributes;
	uint32_t nextBlockId = 0;
};

struct NautilusIROpaqueGraph {
	std::shared_ptr<nautilus::compiler::ir::IRGraph> ir;
	/// SSA ids are handed out graph-wide, so they are unique per function too.
	uint32_t nextOperationId = 1;
	std::unordered_map<NautilusIROpaqueFunctionBuilder*, std::unique_ptr<NautilusIROpaqueFunctionBuilder>> builders;
	std::unordered_map<nautilus::compiler::ir::FunctionId, NautilusIROpaqueFunctionBuilder*> pendingByCallee;
	/// Set once the IR passes have run: they rewrite the graph in place, and
	/// the terminal exception-region pass must run exactly once.
	bool optimized = false;
};

struct NautilusOpaqueOptions {
	nautilus::engine::ModuleOptions options;
};

struct NautilusOpaqueExecutable {
	std::unique_ptr<nautilus::compiler::Executable> executable;
};

struct NautilusOpaqueEngine {
	explicit NautilusOpaqueEngine(const nautilus::engine::Options& options)
	    : engine(options), backendName(engine.getNameOfBackend()) {
	}
	nautilus::engine::NautilusEngine engine;
	std::string backendName;
};

namespace nautilus::capi {

/// A failure the API reports with a specific status.
struct ApiError : std::exception {
	ApiError(NautilusStatus status, std::string message) : status(status), message(std::move(message)) {
	}
	const char* what() const noexcept override {
		return message.c_str();
	}
	NautilusStatus status;
	std::string message;
};

void setError(NautilusStatus status, std::string message) noexcept;

/// The status the last failure on this thread recorded.
NautilusStatus lastErrorStatus() noexcept;

inline void check(bool condition, NautilusStatus status, const char* message) {
	if (!condition) {
		throw ApiError(status, message);
	}
}

inline void require(bool condition, const char* message) {
	check(condition, NAUTILUS_ERROR_INVALID_ARGUMENT, message);
}

/// Runs @p body and turns any exception into the thread's last error, so no
/// C++ exception ever crosses the API. Returns @p failure when one was thrown.
template <typename R, typename F>
R guarded(R failure, F&& body) noexcept {
	try {
		return body();
	} catch (const ApiError& e) {
		setError(e.status, e.message);
	} catch (const std::bad_alloc&) {
		setError(NAUTILUS_ERROR_OUT_OF_MEMORY, "out of memory");
	} catch (const std::exception& e) {
		setError(NAUTILUS_ERROR_INTERNAL, e.what());
	} catch (...) {
		setError(NAUTILUS_ERROR_INTERNAL, "unknown error");
	}
	return failure;
}

/// guarded() for a function that reports only a status: NAUTILUS_OK, or the
/// status of the failure it recorded.
template <typename F>
NautilusStatus status(F&& body) noexcept {
	const bool ok = guarded(false, [&] {
		body();
		return true;
	});
	return ok ? NautilusStatus {NAUTILUS_OK} : lastErrorStatus();
}

/// Runs a pass or backend step, reporting what it throws as a compilation
/// failure rather than as an internal error.
template <typename F>
auto compiling(F&& body) {
	try {
		return body();
	} catch (const ApiError&) {
		throw;
	} catch (const std::bad_alloc&) {
		throw;
	} catch (const std::exception& e) {
		throw ApiError(NAUTILUS_ERROR_COMPILATION_FAILED, e.what());
	}
}

template <typename T>
T* outParam(T* out) {
	require(out != nullptr, "out-parameter is NULL");
	return out;
}

inline std::string_view view(NautilusStringRef str) {
	require(str.data != nullptr || str.length == 0, "string data is NULL");
	return str.length == 0 ? std::string_view {} : std::string_view {str.data, str.length};
}

inline std::string toString(NautilusStringRef str) {
	return std::string(view(str));
}

/// Borrows @p str, which must outlive every use of the result.
inline NautilusStringRef borrow(const std::string& str) noexcept {
	return NautilusStringRef {str.c_str(), str.size()};
}

inline NautilusString own(const std::string& str) {
	auto* data = static_cast<char*>(std::malloc(str.size() + 1));
	if (data == nullptr) {
		throw std::bad_alloc();
	}
	std::memcpy(data, str.c_str(), str.size() + 1);
	return NautilusString {data, str.size()};
}

/// The copy-out accessor protocol: copies up to @p capacity converted
/// elements of @p range into @p out and returns the range's size.
template <typename Out, typename Range, typename Convert>
size_t copyOut(const Range& range, Out* out, size_t capacity, Convert&& convert) noexcept {
	const size_t count = std::size(range);
	if (out != nullptr) {
		const size_t n = std::min(count, capacity);
		auto it = std::begin(range);
		for (size_t i = 0; i < n; ++i, ++it) {
			out[i] = convert(*it);
		}
	}
	return count;
}

inline const engine::ModuleOptions& optionsOf(NautilusOptionsRef options) noexcept {
	static const engine::ModuleOptions defaults;
	return options != nullptr ? options->options : defaults;
}

/// Readies a graph for the pass pipeline: every builder done with, the CFG
/// bookkeeping rebuilt, and the graph verified so a malformed graph is
/// reported instead of reaching a pass or backend that assumes it is not.
void prepareForPasses(NautilusIROpaqueGraph* graph);

} // namespace nautilus::capi
