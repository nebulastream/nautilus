#pragma once

// Internals shared by the C API translation units (nautilus/c/ir.h,
// nautilus/c/engine.h). Not installed.

#include "nautilus/Engine.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/c/engine.h"
#include "nautilus/c/ir.h"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/options.hpp"
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
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
	nautilus::compiler::ir::FunctionId calleeId;
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

struct NautilusIROpaqueOptions {
	nautilus::engine::ModuleOptions options;
};

struct NautilusIROpaqueExecutable {
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

inline thread_local std::string lastError;
inline thread_local bool hasLastError = false;

inline void setError(std::string message) {
	lastError = std::move(message);
	hasLastError = true;
}

/// Runs @p body and turns any exception into the thread's last error, so no
/// C++ exception ever crosses the C boundary.
template <typename R, typename F>
R guarded(R failure, F&& body) {
	try {
		return body();
	} catch (const std::exception& e) {
		setError(e.what());
	} catch (...) {
		setError("unknown error");
	}
	return failure;
}

struct ApiError : std::exception {
	explicit ApiError(std::string message) : message(std::move(message)) {
	}
	const char* what() const noexcept override {
		return message.c_str();
	}
	std::string message;
};

inline void require(bool condition, const char* message) {
	if (!condition) {
		throw ApiError(message);
	}
}

inline void prepareForPasses(NautilusIROpaqueGraph* graph) {
	require(graph->builders.empty(), "every function builder must be finished first");
	for (const auto& target : graph->ir->getFunctionTable().getTargets()) {
		require(target.getLinkage() != compiler::ir::Linkage::Internal || target.getDefinition() != nullptr,
		        "a called function was never successfully finished");
	}
	compiler::ir::rebuildPredecessorLists(*graph->ir);
}

inline const engine::ModuleOptions& optionsOf(NautilusIROptionsRef options) {
	static const engine::ModuleOptions defaults;
	return options != nullptr ? options->options : defaults;
}

} // namespace nautilus::capi
