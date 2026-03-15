#include "nautilus/MultiTierExecutable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/backends/bc/Dyncall.hpp"
#include "nautilus/logging.hpp"
#include <fmt/core.h>

#ifdef ENABLE_COMPILER

#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include "nautilus/tracing/LazyTraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"

#endif

namespace nautilus::compiler {

#ifdef ENABLE_COMPILER
extern std::string createCompilationUnitID();
#endif

/// Wraps a raw function pointer into the GenericInvocable interface using dyncall.
/// Used when tier 2 backends (MLIR, C++) only provide function pointers.
class FunctionPointerInvocable : public Executable::GenericInvocable {
public:
	FunctionPointerInvocable(void* fptr, Type return_type, std::vector<Type> arg_types)
	    : fptr_(fptr), return_type_(return_type), arg_types_(std::move(arg_types)) {
	}

	std::any invokeGeneric(const std::vector<std::any>& args) override {
		auto& dyncall = bc::Dyncall::getVM();
		dyncall.reset();

		for (size_t i = 0; i < args.size(); i++) {
			addArg(dyncall, args[i]);
		}

		return callWithReturnType(dyncall);
	}

private:
	void addArg(bc::Dyncall& dyncall, const std::any& arg) {
		if (auto* v = std::any_cast<int8_t>(&arg)) {
			dyncall.addArgI8(*v);
		} else if (auto* v = std::any_cast<int16_t>(&arg)) {
			dyncall.addArgI16(*v);
		} else if (auto* v = std::any_cast<int32_t>(&arg)) {
			dyncall.addArgI32(*v);
		} else if (auto* v = std::any_cast<int64_t>(&arg)) {
			dyncall.addArgI64(*v);
		} else if (auto* v = std::any_cast<uint8_t>(&arg)) {
			dyncall.addArgI8(static_cast<int8_t>(*v));
		} else if (auto* v = std::any_cast<uint16_t>(&arg)) {
			dyncall.addArgI16(static_cast<int16_t>(*v));
		} else if (auto* v = std::any_cast<uint32_t>(&arg)) {
			dyncall.addArgI32(static_cast<int32_t>(*v));
		} else if (auto* v = std::any_cast<uint64_t>(&arg)) {
			dyncall.addArgI64(static_cast<int64_t>(*v));
		} else if (auto* v = std::any_cast<float>(&arg)) {
			dyncall.addArgF(*v);
		} else if (auto* v = std::any_cast<double>(&arg)) {
			dyncall.addArgD(*v);
		} else if (auto* v = std::any_cast<bool>(&arg)) {
			dyncall.addArgB(*v);
		} else if (auto* v = std::any_cast<void*>(&arg)) {
			dyncall.addArgPtr(*v);
		} else if (auto* v = std::any_cast<const void*>(&arg)) {
			dyncall.addArgPtr(const_cast<void*>(*v));
		} else {
			throw std::runtime_error(
			    fmt::format("FunctionPointerInvocable: unsupported argument type: {}", arg.type().name()));
		}
	}

	std::any callWithReturnType(bc::Dyncall& dyncall) {
		switch (return_type_) {
		case Type::v:
			dyncall.callVoid(fptr_);
			return nullptr;
		case Type::i8:
			return dyncall.callI8(fptr_);
		case Type::i16:
			return dyncall.callI16(fptr_);
		case Type::i32:
			return dyncall.callI32(fptr_);
		case Type::i64:
			return dyncall.callI64(fptr_);
		case Type::f32:
			return dyncall.callF(fptr_);
		case Type::f64:
			return dyncall.callD(fptr_);
		case Type::ptr:
			return dyncall.callPtr(fptr_);
		case Type::b:
			return dyncall.callB(fptr_);
		default:
			return dyncall.callI64(fptr_);
		}
	}

	void* fptr_;
	Type return_type_;
	std::vector<Type> arg_types_;
};

class MultiTierExecutable::MultiTierInvocable : public Executable::GenericInvocable {
public:
	explicit MultiTierInvocable(MultiTierExecutable& owner, const std::string& member)
	    : owner_(owner), member_(member) {
	}

	std::any invokeGeneric(const std::vector<std::any>& args) override {
		owner_.onInvocation();
		auto* active = owner_.getActiveExecutable();
		auto invocable = active->getGenericInvocable(member_);
		if (invocable) {
			return invocable->invokeGeneric(args);
		}
		// Active executable only supports function pointers (MLIR, C++).
		// Use dyncall to bridge from GenericInvocable to function pointer.
		if (active->hasInvocableFunctionPtr()) {
			auto fptr = active->getInvocableFunctionPtr(member_);
			if (fptr) {
				auto wrapper = owner_.getFunctionPointerInvocable(member_, fptr);
				return wrapper->invokeGeneric(args);
			}
		}
		throw std::runtime_error("MultiTierExecutable: no invocable available for active tier");
	}

private:
	MultiTierExecutable& owner_;
	std::string member_;
};

MultiTierExecutable::MultiTierExecutable(std::unique_ptr<Executable> tier1Executable,
                                         MultiTierJitCompiler::wrapper_function wrapperFunction,
                                         engine::Options options, const CompilationBackendRegistry* backends,
                                         uint64_t tier2Threshold, std::string tier1BackendName,
                                         std::string tier2BackendName)
    : tier1_executable_(std::move(tier1Executable)), tier2_executable_(nullptr),
      wrapper_function_(std::move(wrapperFunction)), options_(std::move(options)), backends_(backends),
      tier2_threshold_(tier2Threshold), tier1_backend_name_(std::move(tier1BackendName)),
      tier2_backend_name_(std::move(tier2BackendName)), current_tier_(1), invocation_count_(0), tier2_compiling_(false),
      tier2_failed_(false) {
}

MultiTierExecutable::~MultiTierExecutable() {
	if (tier2_compilation_future_.valid()) {
		tier2_compilation_future_.wait();
	}
}

void* MultiTierExecutable::getInvocableFunctionPtr(const std::string& member) {
	// Forward to the active tier. During tier 1 (bc) this typically returns nullptr
	// since bc uses GenericInvocable. After tier 2 (MLIR) is active, callers get
	// the optimized function pointer directly — no tier 3 exists, so bypassing
	// onInvocation() is safe and faster.
	auto* active = getActiveExecutable();
	if (active->hasInvocableFunctionPtr()) {
		return active->getInvocableFunctionPtr(member);
	}
	return nullptr;
}

bool MultiTierExecutable::hasInvocableFunctionPtr() {
	return getActiveExecutable()->hasInvocableFunctionPtr();
}

std::unique_ptr<Executable::GenericInvocable> MultiTierExecutable::getGenericInvocable(const std::string& member) {
	return std::make_unique<MultiTierInvocable>(*this, member);
}

Executable::GenericInvocable* MultiTierExecutable::getFunctionPointerInvocable(const std::string& member, void* fptr) {
	std::lock_guard<std::mutex> lock(tier_transition_mutex_);
	auto it = fptr_invocable_cache_.find(member);
	if (it != fptr_invocable_cache_.end() && it->second.first == fptr) {
		return it->second.second.get();
	}
	auto invocable = std::make_unique<FunctionPointerInvocable>(fptr, return_type_, arg_types_);
	auto* raw = invocable.get();
	fptr_invocable_cache_[member] = std::make_pair(fptr, std::move(invocable));
	return raw;
}

void MultiTierExecutable::onInvocation() {
	uint64_t count = invocation_count_.fetch_add(1, std::memory_order_acq_rel);

	// Trigger tier 2 after tier2_threshold_ invocations (threshold=0 means first call)
	bool at_threshold = (tier2_threshold_ == 0) ? (count == 0) : (count + 1 == tier2_threshold_);
	if (at_threshold && !tier2_compiling_.load(std::memory_order_acquire) &&
	    !tier2_failed_.load(std::memory_order_acquire)) {
		bool expected = false;
		if (tier2_compiling_.compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
			tier2_compilation_future_ = std::async(std::launch::async, [this]() { this->compileTier2(); });
		}
	}
}

Executable* MultiTierExecutable::getActiveExecutable() {
	uint8_t tier = current_tier_.load(std::memory_order_acquire);
	if (tier == 2 && tier2_executable_) {
		return tier2_executable_.get();
	}
	return tier1_executable_.get();
}

void MultiTierExecutable::compileTier2() {
#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)
	try {
		const CompilationUnitID compilationId = createCompilationUnitID();
		auto dumpHandler = DumpHandler(options_, compilationId + "_tier2");

		auto traceMode = options_.getOptionOrDefault("engine.traceMode", std::string("lazyTracing"));
		auto executionTrace = (traceMode == "lazyTracing")
		                          ? tracing::LazyTraceContext::trace(wrapper_function_, options_)
		                          : tracing::ExceptionBasedTraceContext::trace(wrapper_function_, options_);
		dumpHandler.dump("after_tracing", "trace", [&]() { return executionTrace->toString(); });

		auto ssaCreationPhase = tracing::SSACreationPhase();
		auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
		dumpHandler.dump("after_ssa", "trace", [&]() { return afterSSA->toString(); });

		auto irGenerationPhase = tracing::TraceToIRConversionPhase();
		const auto ir = irGenerationPhase.apply(std::move(afterSSA), compilationId);
		dumpHandler.dump("after_ir_creation", "ir", [&]() { return ir->toString(); });

		// Extract return type and argument types from IR for dyncall dispatch
		auto& rootOp = ir->getRootOperation();
		auto return_type = rootOp.getOutputArg();
		auto arg_types = rootOp.getInputArgs();

		if (options_.getOptionOrDefault("dump.graph", false)) {
			ir::createGraphVizFromIr(ir, options_, dumpHandler);
		}

		const auto tier2Backend = backends_->getBackend(tier2_backend_name_);
		auto newExecutable = tier2Backend->compile(ir, dumpHandler, options_);
		newExecutable->setGeneratedFiles(dumpHandler.getGeneratedFiles());

		switchToTier2(std::move(newExecutable), return_type, std::move(arg_types));
	} catch (const std::exception& e) {
		log::error("Tier 2 compilation failed: {}", e.what());
		std::lock_guard<std::mutex> lock(tier_transition_mutex_);
		tier2_failed_.store(true, std::memory_order_release);
		tier2_compiling_.store(false, std::memory_order_release);
	}
#else
	std::lock_guard<std::mutex> lock(tier_transition_mutex_);
	tier2_failed_.store(true, std::memory_order_release);
	tier2_compiling_.store(false, std::memory_order_release);
#endif
}

void MultiTierExecutable::switchToTier2(std::unique_ptr<Executable> newExecutable, Type return_type,
                                        std::vector<Type> arg_types) {
	std::lock_guard<std::mutex> lock(tier_transition_mutex_);
	return_type_ = return_type;
	arg_types_ = std::move(arg_types);
	fptr_invocable_cache_.clear();
	tier2_executable_ = std::move(newExecutable);
	current_tier_.store(2, std::memory_order_release);
	tier2_compiling_.store(false, std::memory_order_release);
}

} // namespace nautilus::compiler
