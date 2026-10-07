#include "nautilus/Engine.hpp"
#include "nautilus/common/TypedAllocation.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/artifact/ArtifactPreflight.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/region.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/val_std.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <iostream>
#include <limits>

namespace nautilus::engine {

TypedAllocation allocationFromOtherTranslationUnit();

namespace {

struct alignas(64) AllocationLayout {
	int64_t value;
};

struct SameAllocationLayout {
	int64_t value;
};

using compiler::CompilableFunction;
using compiler::artifact::hasOnlyTypedAllocations;
using compiler::artifact::validateArtifactPreflight;

std::shared_ptr<compiler::ir::IRGraph> allocationIR(std::shared_ptr<tracing::ExecutionTrace> trace,
                                                    common::ArenaPool* pool = nullptr) {
	tracing::SSACreationPhase ssa;
	trace = ssa.apply(std::move(trace));
	tracing::TraceToIRConversionPhase convert;
	return pool ? convert.apply(trace, *pool) : convert.apply(trace);
}

} // namespace

TEST_CASE("Typed allocations transport real introduction evidence through regions clones and conversion",
          "[artifact][allocation-origin]") {
	for (const auto tracking : {ConstantOriginTracking::Enabled, ConstantOriginTracking::Disabled}) {
		for (const bool pooled : {false, true}) {
			CAPTURE(tracking, pooled);
			common::Arena arena, clones;
			common::ArenaPool irPool;
			auto wrapper = details::createFunctionWrapper([](val<bool> condition) {
				val<AllocationLayout*> pointer;
				region("outer allocation", [&] {
					region("inner allocation",
					       [&] { pointer = nautilus::details::nautilus_alloca<AllocationLayout>(); });
				});
				if (condition) {
					return pointer;
				}
				return pointer;
			});
			std::shared_ptr<tracing::ExecutionTrace> trace = tracing::TraceContext::trace(wrapper, {}, arena, tracking);
			REQUIRE(trace->allocaSpecs.size() == 1);
			const auto& spec = trace->allocaSpecs.front();
			REQUIRE(spec.size == sizeof(AllocationLayout));
			REQUIRE(spec.align == alignof(AllocationLayout));
			const bool enabled = tracking == ConstantOriginTracking::Enabled;
			REQUIRE(spec.origin.has_value() == enabled);
			std::size_t allocations = 0;
			for (const auto* block : trace->getBlocks()) {
				for (const auto* operation : block->operations) {
					if (operation->op == tracing::Op::ALLOCA) {
						++allocations;
						REQUIRE(operation->regionIndex != tracing::NO_REGION);
						auto* clone = tracing::cloneTraceOp(clones, *operation);
						REQUIRE(clone->input.data() != operation->input.data());
						REQUIRE(clone->regionIndex == operation->regionIndex);
						REQUIRE(std::get<tracing::AllocaIndex>(clone->input[0]) == 0);
					}
				}
			}
			REQUIRE(allocations == 1);
			auto ir = allocationIR(trace, pooled ? &irPool : nullptr);
			const auto& copied = ir->getFunctionOperation("execute")->getAllocaSpecs().front();
			REQUIRE(copied.origin == spec.origin);
			REQUIRE(hasOnlyTypedAllocations(*ir) == enabled);
			if (enabled) {
				REQUIRE(copied.origin == TypedAllocation::forType<AllocationLayout>());
				std::cout << "allocation origin=" << copied.origin->getType() << " size=" << copied.size
				          << " align=" << copied.align << '\n';
			}
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("Typed allocation replay disagreement is sticky and raw evidence never upgrades",
          "[artifact][allocation-origin][replay]") {
	for (const bool initiallyTyped : {false, true}) {
		for (const bool replayTyped : {false, true}) {
			for (const bool changeLayout : {false, true}) {
				CAPTURE(initiallyTyped, replayTyped, changeLayout);
				int iterations = 0;
				auto wrapper = details::createFunctionWrapper([&](val<bool> condition) {
					++iterations;
					const bool typed = iterations == 1 ? initiallyTyped : replayTyped;
					const auto layout = changeLayout && iterations != 1 ? TypedAllocation::forType<int32_t>()
					                                                    : TypedAllocation::forType<int64_t>();
					auto& ref = typed ? tracing::traceTypedAlloca(layout)
					                  : tracing::traceAlloca(layout.getSize(), layout.getAlignment());
					val<void*> pointer(ref);
					if (condition) {
						return pointer;
					}
					return pointer;
				});
				common::Arena arena;
				std::shared_ptr<tracing::ExecutionTrace> trace =
				    tracing::TraceContext::trace(wrapper, {}, arena, ConstantOriginTracking::Enabled);
				REQUIRE(iterations >= 2);
				REQUIRE(trace->allocaSpecs.size() == 1);
				const bool valid = initiallyTyped && replayTyped && !changeLayout;
				REQUIRE(trace->allocaSpecs.front().origin.has_value() == valid);
				auto ir = allocationIR(trace);
				REQUIRE(hasOnlyTypedAllocations(*ir) == valid);
				REQUIRE_FALSE(tracing::inTracer());
			}
		}
	}
}

TEST_CASE("Allocation origin disagreement at a repeated RECORD tag cannot survive a merge",
          "[artifact][allocation-origin][replay]") {
	for (const bool disagree : {false, true}) {
		CAPTURE(disagree);
		common::Arena arena;
		tracing::ExecutionTrace trace(arena, ConstantOriginTracking::Enabled);
		tracing::SymbolicExecutionContext symbolic;
		symbolic.next();
		REQUIRE(symbolic.getCurrentMode() == tracing::SymbolicExecutionContext::MODE::RECORD);
		auto recorder = tracing::TagRecorder::createTagRecorder(arena);
		Options options;
		{
			tracing::ActiveTracerGuard guard;
			auto* context = tracing::TraceContext::initialize(recorder, trace, symbolic, options);
			context->resume();
			volatile int iteration = 0;
			while (iteration < 2) {
				const int current = iteration;
				const auto origin = disagree && current == 1 ? TypedAllocation::forType<SameAllocationLayout>()
				                                             : TypedAllocation::forType<int64_t>();
				tracing::traceTypedAlloca(origin);
				if (current == 0) {
					trace.setCurrentBlock(trace.createBlock());
				}
				iteration = current + 1;
			}
		}
		REQUIRE(trace.getBlocks().size() == 3);
		REQUIRE(trace.allocaSpecs.size() == 1);
		REQUIRE(trace.allocaSpecs.front().origin.has_value() == !disagree);
		REQUIRE_FALSE(tracing::inTracer());
	}
}

TEST_CASE("Allocation type evidence distinguishes same-name types with identical layouts",
          "[artifact][allocation-origin][replay]") {
	const auto first = TypedAllocation::forType<SameAllocationLayout>();
	const auto other = allocationFromOtherTranslationUnit();
	REQUIRE(first.getSize() == other.getSize());
	REQUIRE(first.getAlignment() == other.getAlignment());
	REQUIRE(first.getType() == other.getType());
	REQUIRE_FALSE(first == other);
	common::Arena arena;
	tracing::ExecutionTrace trace(arena, ConstantOriginTracking::Enabled);
	trace.addAllocaSpec(first.getSize(), first.getAlignment(), first);
	trace.reconcileAllocaSpec(0, other.getSize(), other.getAlignment(), other);
	REQUIRE_FALSE(trace.allocaSpecs.front().origin);
}

TEST_CASE("Allocation reconciliation checks type identity size alignment and cloned slots",
          "[artifact][allocation-origin][replay]") {
	for (const std::string kind : {"agreement", "type", "size", "align", "raw"}) {
		CAPTURE(kind);
		common::Arena arena, clones;
		tracing::ExecutionTrace trace(arena, ConstantOriginTracking::Enabled);
		const auto origin = TypedAllocation::forType<int64_t>();
		const auto index = trace.addAllocaSpec(origin.getSize(), origin.getAlignment(), origin);
		tracing::Snapshot tag;
		auto op = tracing::Op::ALLOCA;
		auto type = Type::ptr;
		trace.addOperationWithResult(tag, op, type, {index});
		auto* clone = tracing::cloneTraceOp(clones, *trace.getBlocks().front()->operations.front());
		const auto observed = kind == "raw"    ? std::optional<TypedAllocation> {}
		                      : kind == "type" ? TypedAllocation::forType<SameAllocationLayout>()
		                                       : origin;
		trace.reconcileAllocaSpec(std::get<tracing::AllocaIndex>(clone->input[0]),
		                          origin.getSize() + (kind == "size" ? 1 : 0),
		                          origin.getAlignment() * (kind == "align" ? 2 : 1), observed);
		trace.reconcileAllocaSpec(index, origin.getSize(), origin.getAlignment(), origin);
		REQUIRE(trace.allocaSpecs.front().origin.has_value() == (kind == "agreement"));
	}
}

TEST_CASE("Typed allocation tables remain complete for nested functions and regions before optimization",
          "[artifact][allocation-origin][preflight]") {
	for (const bool passes : {false, true}) {
		CAPTURE(passes);
		Options options;
		options.setOption("ir.runPasses", passes);
		common::ArenaPool tracePool, irPool;
		compiler::CompilationPipeline pipeline(options, tracePool, irPool);
		NautilusFunction helper("allocated helper", [](val<bool> condition) {
			val<AllocationLayout*> pointer;
			region("helper allocation", [&] { pointer = nautilus::details::nautilus_alloca<AllocationLayout>(); });
			if (condition) {
				return pointer;
			}
			return pointer;
		});
		std::list<CompilableFunction> roots;
		roots.emplace_back("execute", details::createFunctionWrapper([&](val<bool> flag) { return helper(flag); }),
		                   std::unordered_map<std::string, std::string> {},
		                   CompilableFunction::Signature {Type::ptr, {Type::b}});
		std::size_t checks = 0;
		auto ir = pipeline.compileToIR(
		    roots, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
		    [&](compiler::ir::IRGraph& graph) {
			    ++checks;
			    REQUIRE(graph.getFunctionOperations().size() == 2);
			    REQUIRE(hasOnlyTypedAllocations(graph));
			    REQUIRE_NOTHROW(validateArtifactPreflight(graph, roots));
		    },
		    ConstantOriginTracking::Enabled);
		REQUIRE(ir != nullptr);
		REQUIRE(checks == 1);
	}
}

TEST_CASE("Allocation preflight rejects malformed unavailable and unused metadata before dead code removal",
          "[artifact][allocation-origin][preflight]") {
	using namespace compiler::ir;
	for (const std::string kind : {"raw", "size", "align", "zero", "oversize", "untracked", "unused raw"}) {
		CAPTURE(kind);
		IRGraph graph("allocation-origin-metadata");
		auto& arena = graph.getArena();
		auto* block = arena.create<BasicBlock>(arena, 0, std::vector<BasicBlockArgument*> {});
		block->addOperation<ReturnOperation>();
		const auto origin = TypedAllocation::forType<int64_t>();
		std::vector<AllocaSpec> specs {{sizeof(int64_t), alignof(int64_t), origin}};
		if (kind == "raw")
			specs[0].origin.reset();
		if (kind == "size")
			++specs[0].size;
		if (kind == "align")
			specs[0].align *= 2;
		if (kind == "zero")
			specs[0].size = 0;
		if (kind == "oversize")
			specs[0].size = std::numeric_limits<size_t>::max();
		if (kind == "untracked")
			graph.invalidateConstantOrigins();
		if (kind == "unused raw")
			specs.push_back({sizeof(int64_t), alignof(int64_t)});
		auto* function = arena.create<FunctionOperation>("execute", std::vector {block}, std::vector<Type> {},
		                                                 std::vector<std::string> {}, Type::v, specs);
		graph.addFunctionOperation(function);
		CalleeDescriptor descriptor;
		descriptor.kind = CalleeDescriptor::Kind::Internal;
		descriptor.customName = "execute";
		graph.defineFunction(graph.internCallee(descriptor), function);
		std::list<CompilableFunction> roots;
		roots.emplace_back(
		    "execute", [] {}, std::unordered_map<std::string, std::string> {},
		    CompilableFunction::Signature {Type::v, {}});
		std::string reason;
		REQUIRE_FALSE(hasOnlyTypedAllocations(graph, &reason));
		REQUIRE(reason.starts_with("allocation_metadata_"));
		REQUIRE_THROWS_WITH(validateArtifactPreflight(graph, roots), Catch::Matchers::ContainsSubstring(reason));
		REQUIRE(function->getAllocaSpecs().size() == specs.size());
	}
}

TEST_CASE("Dead allocation tables are rejected by the mandatory preoptimization callback",
          "[artifact][allocation-origin][preflight]") {
	for (const bool passes : {false, true}) {
		CAPTURE(passes);
		Options options;
		options.setOption("ir.runPasses", passes);
		common::ArenaPool tracePool, irPool;
		compiler::CompilationPipeline pipeline(options, tracePool, irPool);
		std::list<CompilableFunction> roots;
		roots.emplace_back("execute", details::createFunctionWrapper([] {
			                   nautilus::details::nautilus_alloca<int64_t>();
			                   tracing::traceAlloca(sizeof(int64_t), alignof(int64_t));
			                   return cacheLiteral<int64_t {0}>();
		                   }),
		                   std::unordered_map<std::string, std::string> {},
		                   CompilableFunction::Signature {Type::i64, {}});
		std::size_t checks = 0;
		REQUIRE_THROWS_WITH(pipeline.compileToIR(
		                        roots, options.deriveModuleOptions(), nullptr, compiler::IROptimizationLevel::Full,
		                        [&](compiler::ir::IRGraph& graph) {
			                        ++checks;
			                        REQUIRE(graph.getFunctionOperation("execute")->getAllocaSpecs().size() == 2);
			                        validateArtifactPreflight(graph, roots);
		                        },
		                        ConstantOriginTracking::Enabled),
		                    Catch::Matchers::ContainsSubstring("allocation_metadata_origins_unavailable"));
		REQUIRE(checks == 1);
	}
}

TEST_CASE("Ordinary typed allocation tracking remains disabled and raw table APIs default unavailable",
          "[artifact][allocation-origin]") {
	common::Arena arena;
	tracing::ExecutionTrace ordinary(arena);
	const auto type = TypedAllocation::forType<AllocationLayout>();
	ordinary.addAllocaSpec(type.getSize(), type.getAlignment(), type);
	REQUIRE_FALSE(ordinary.allocaSpecs.front().origin);
	ordinary.reconcileAllocaSpec(std::numeric_limits<tracing::AllocaIndex>::max(), 0, 0, type);
	tracing::ExecutionTrace recorded(arena, ConstantOriginTracking::Enabled);
	recorded.addAllocaSpec(type.getSize(), type.getAlignment());
	recorded.reconcileAllocaSpec(0, type.getSize(), type.getAlignment(), type);
	REQUIRE_FALSE(recorded.allocaSpecs.front().origin);
}

} // namespace nautilus::engine
