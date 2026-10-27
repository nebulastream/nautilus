#pragma once

#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/nir/Module.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/AliveVariableHash.hpp"
#include "nautilus/tracing/Snapshot.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "nautilus/tracing/TracingStatistics.hpp"
#include "nautilus/tracing/nir/FlatTrace.hpp"
#include "nautilus/tracing/symbolic_execution/SymbolicExecutionContext.hpp"
#include "nautilus/tracing/tag/TagRecorder.hpp"
#include <chrono>
#include <deque>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus::tracing::nirtrace {

/// What tracing a set of functions produced: a pre-SSA NIR module, and what the session counted.
struct NirTraceResult {
	::nautilus::nir::Module module;
	TracingStatistics statistics;
};

/**
 * @brief The NIR tracer (docs/plans/nautilus-0.4/01-tracing-frontend.md §6): a second implementation of
 * `TracingInterface` next to `TraceContext`, which records the pre-SSA NIR of a function directly.
 *
 * It keeps the algorithm of `TraceContext` -- one pass of the traced body per explored path, a replay that follows
 * the recorded decisions, a recording that ends at an operation whose snapshot exists (a control-flow merge), then a
 * paused tail -- and changes what a recording is: 16-byte NIR instruction records in a `FlatTrace`, an open-addressing
 * tag map from snapshot to record id, and a replay cursor over the block's list of record ids. A `val<T>` holds the
 * id of the record that defined it.
 */
class NirTracer final : public TracingInterface {
public:
	TypedValueRef& registerFunctionArgument(Type type, size_t index) override;
	TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) override;
	TypedValueRef& traceAlloca(size_t size, size_t align) override;
	TypedValueRef& traceCopy(const TypedValueRef& ref) override;
	void noteExplicitCopy() override {
		nextCopyExplicit_ = true;
	}
	TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
	                             const TypedValueRef& right) override;
	TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) override;
	TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
	                              const TypedValueRef& third) override;
	void traceReturnOperation(Type type, const TypedValueRef& ref) override;
	void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) override;
	TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<TypedValueRef>& arguments,
	                         FunctionAttributes fnAttrs) override;
	TypedValueRef& traceCallWithExceptionHandling(void* fptn, Type resultType,
	                                              const std::vector<TypedValueRef>& arguments,
	                                              FunctionAttributes fnAttrs, void* captureFunc = nullptr) override;
	TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                 const std::vector<TypedValueRef>& arguments, FunctionAttributes fnAttrs,
	                                 void* captureFunc = nullptr) override;
	TypedValueRef& traceIndirectCallWithExceptionHandling(const TypedValueRef& fnPtrRef, Type resultType,
	                                                      const std::vector<TypedValueRef>& arguments,
	                                                      FunctionAttributes fnAttrs,
	                                                      void* captureFunc = nullptr) override;
	TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                                 Type resultType, const std::vector<TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusCallWithExceptionHandling(const NautilusFunctionDefinition* definition,
	                                                      std::function<void()> fwrapper, Type resultType,
	                                                      const std::vector<TypedValueRef>& arguments,
	                                                      FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
	                                        std::function<void()> fwrapper) override;
	bool traceBool(const TypedValueRef& value, double probability) override;
	void traceRegion(std::function<void()>& regionFunction, const RegionAttributes& attributes) override;
	TypedValueRef& traceExtOp(uint16_t opcode, uint8_t resultType, const std::vector<TypedValueRef>& inputs) override;
	TypedValueRef& traceData(std::span<const std::byte> bytes, size_t align) override;
	TypedValueRef& traceBinding(std::string_view name, size_t size, size_t align) override;
	void allocateValRef(ValueRef ref) override;
	void freeValRef(ValueRef ref) override;
	void pushStaticVal(void* ptr, size_t size) override;
	void popStaticVal() override;
	void registerDestructor(const TypedValueRef& address, void* destructor) override;
	void unregisterDestructor(const TypedValueRef& address) override;
	void transferDestructor(const TypedValueRef& from, const TypedValueRef& to) override;

	~NirTracer() override;
	NirTracer();

	/// Traces all functions of the list, and the Nautilus functions they invoke, into one pre-SSA module: the first
	/// function is the exported entry, the others internal, in the order they were first seen. @p arena backs the tag
	/// trie and must outlive the call only.
	static NirTraceResult Trace(std::list<compiler::CompilableFunction>& functions, const engine::Options& options,
	                            common::Arena& arena);

private:
	struct SessionState;
	struct ScopeState;

	NirTraceResult traceSession(std::list<compiler::CompilableFunction>& functions, const engine::Options& options,
	                            common::Arena& arena);

	bool isFollowing();
	TypedValueRef& follow();
	template <typename OnCreation>
	TypedValueRef& traceOperation(OnCreation&& onCreation);
	Snapshot recordSnapshot();
	void runScope(std::function<void()>& body);
	void traceScopeExit();
	void initRegionScope(NirTracer& parent, uint32_t entry, uint32_t exit, TagRecorder& recorder,
	                     const RegionAttributes& attributes);
	NirTracer& acquireChildScope();
	std::string describeCurrentFunction() const;

	/// Records the operations of the traced operations; each is a group of records handed to `FlatTrace`.
	TypedValueRef& finish(uint32_t first, const Snapshot& tag, TypedValueRef result);
	TypedValueRef& finish(uint32_t first, const Snapshot& tag, uint32_t resultRecord, Type resultType);
	::nautilus::nir::Ref emit(::nautilus::nir::Op op, ::nautilus::nir::TypeId type, uint8_t flags, uint32_t a = 0,
	                          uint32_t b = 0, uint32_t c = 0);
	::nautilus::nir::Ref emitCast(::nautilus::nir::Ref value, uint8_t kind, ::nautilus::nir::TypeId to);
	::nautilus::nir::Ref emitConversion(const TypedValueRef& source, Type target);
	::nautilus::nir::Ref emitBinary(::nautilus::nir::Op op, Type resultType, const TypedValueRef& a,
	                                const TypedValueRef& b, uint8_t flags = 0, uint32_t c = 0);
	TypedValueRef& recordCallRecord(uint32_t first, const Snapshot& tag, uint32_t callee, bool exceptionHandling,
	                                Type resultType, const std::vector<TypedValueRef>& arguments,
	                                FunctionAttributes fnAttrs, bool external, void* captureFunc);
	TypedValueRef& recordIndirectCall(const Snapshot& tag, bool exceptionHandling, const TypedValueRef& fnPtrRef,
	                                  Type resultType, const std::vector<TypedValueRef>& arguments,
	                                  FunctionAttributes fnAttrs, void* captureFunc);
	uint32_t externalCallee(void* fptn, Type resultType, const std::vector<TypedValueRef>& arguments,
	                        FunctionAttributes fnAttrs);
	uint32_t internalCallee(uint32_t functionIndex);
	uint32_t pointerEntry(::nautilus::nir::PointerKind kind, const void* address, uint8_t provenance);
	uint32_t signatureOf(Type result, const std::vector<TypedValueRef>& arguments);
	uint32_t cleanupList();
	uint32_t nautilusFunctionIndex(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                               FunctionAttributes fnAttrs, bool takesAddress);
	const TypedValueRef& valueOf(const TypedValueRef& ref) const {
		return ref;
	}

	/// The state of the scope being traced: the function's (or region's) recording and the objects it explores with.
	struct TraceScope {
		TagRecorder& tagRecorder;
		FlatTrace& trace;
		SymbolicExecutionContext& symbolicExecutionContext;
		const engine::Options& options;
		/// `dump.copySites` is not supported by this tracer; the option is read by the pipeline only
	};
	std::optional<TraceScope> state;

	struct Destructor {
		TypedValueRef address;
		void* ptr;
	};
	std::vector<Destructor> activeDestructors;

	uint32_t entryBlock_ = 0;
	NirTracer* parent_ = nullptr;
	NirTracer* session_ = this;

	struct RegionRecord {
		uint32_t entryBlock;
		uint32_t exitBlock;
	};
	struct RegionScopeState {
		uint32_t exitBlock = 0;
		RegionAttributes attributes;
		std::optional<Snapshot> exitSnapshot;
		std::unordered_map<Snapshot, RegionRecord> regionMemo;
		SymbolicExecutionContext symbolicExecutionContext {8};
		std::deque<TagRecorder> tagRecorders;
		std::unique_ptr<NirTracer> childScope;
	};
	RegionScopeState& regionState();
	std::unique_ptr<RegionScopeState> regionState_;

	enum class Phase : uint8_t { None, Follow, Record, Paused };
	TracingStatistics stats_;
	Phase phase_ = Phase::None;
	bool rerunPass_ = false;
	bool nextCopyExplicit_ = false;
	std::chrono::steady_clock::time_point phaseStart_, passStart_;
	void beginPass(bool rerun);
	void enterPhase(Phase phase);
	void endPass();
	void accountPhase(std::chrono::steady_clock::time_point now);
	void pause() {
		paused_ = true;
		enterPhase(Phase::Paused);
	}
	void resume();

	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars;
	bool paused_ = false;
	/// The arena of the trie nodes; set on the session's tracer and read through `session_`.
	common::Arena* arena_ = nullptr;
	TypedValueRef dummyRef_ = {0, Type::v};
	/// What a traced operation hands back; the `val<T>` copies it before the next call.
	TypedValueRef result_ = {0, Type::v};

	std::unique_ptr<SessionState> sessionState_;
};

} // namespace nautilus::tracing::nirtrace
