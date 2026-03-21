#include "nautilus/TieredCompilation.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/logging.hpp"

#ifdef ENABLE_COMPILER

#include "nautilus/CompilableFunction.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include <thread>

namespace nautilus::compiler {

static std::string createPromotionUnitID() {
	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);
	std::tm tm {};
	localtime_r(&time_t, &tm);
	char buf[64];
	std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d_%02d-%02d-%02d_tier1", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
	              tm.tm_hour, tm.tm_min, tm.tm_sec);
	return std::string(buf);
}

// --- DelegatingInvocable ---

DelegatingInvocable::DelegatingInvocable(std::unique_ptr<Executable>& delegate, std::shared_mutex& mutex,
                                         std::string member)
    : delegate_(delegate), mutex_(mutex), member_(std::move(member)) {
}

std::any DelegatingInvocable::invokeGeneric(const std::vector<std::any>& args) {
	std::shared_lock<std::shared_mutex> lock(mutex_);
	auto invocable = delegate_->getGenericInvocable(member_);
	if (invocable) {
		return invocable->invokeGeneric(args);
	}
	// Fallback: delegate has function pointers but no generic invocable.
	// This shouldn't happen since we check hasInvocableFunctionPtr in getInvocableMember.
	return {};
}

// --- TieredExecutable ---

TieredExecutable::TieredExecutable(std::unique_ptr<Executable> tier0, std::shared_ptr<ir::IRGraph> ir,
                                   const engine::TieredCompilationConfig& config, const engine::Options& options)
    : delegate_(std::move(tier0)), cachedIR_(std::move(ir)), config_(config), options_(options) {
	startBackgroundPromotion();
}

TieredExecutable::~TieredExecutable() {
	if (promotionThread_.joinable()) {
		promotionThread_.join();
	}
}

void TieredExecutable::setPromotionCallback(PromotionCallback callback) {
	onPromoted_ = std::move(callback);
}

bool TieredExecutable::hasInvocableFunctionPtr() {
	std::shared_lock<std::shared_mutex> lock(mutex_);
	return delegate_->hasInvocableFunctionPtr();
}

void* TieredExecutable::getInvocableFunctionPtr(const std::string& member) {
	std::shared_lock<std::shared_mutex> lock(mutex_);
	return delegate_->getInvocableFunctionPtr(member);
}

std::unique_ptr<Executable::GenericInvocable> TieredExecutable::getGenericInvocable(const std::string& member) {
	std::shared_lock<std::shared_mutex> lock(mutex_);
	return delegate_->getGenericInvocable(member);
}

bool TieredExecutable::isPromoted() const {
	return promoted_.load(std::memory_order_acquire);
}

void TieredExecutable::waitForPromotion() {
	if (promotionThread_.joinable()) {
		promotionThread_.join();
	}
}

void TieredExecutable::startBackgroundPromotion() {
	promotionThread_ = std::thread([this]() {
		try {
			auto* registry = CompilationBackendRegistry::getInstance();
			auto* backend = registry->getBackend(config_.tier1.backend);

			auto compilationId = createPromotionUnitID();
			auto dumpHandler = DumpHandler(options_, compilationId);

			auto tier1Executable = backend->compile(cachedIR_, dumpHandler, options_);
			tier1Executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());

			{
				std::unique_lock<std::shared_mutex> lock(mutex_);
				delegate_ = std::move(tier1Executable);
			}
			promoted_.store(true, std::memory_order_release);

			// Notify the module to re-resolve function handles
			if (onPromoted_) {
				onPromoted_();
			}

			cachedIR_.reset();
		} catch (const std::exception& e) {
			log::error("Tier 1 promotion failed: {}", e.what());
			promoted_.store(true, std::memory_order_release);
		}
	});
}

// --- TieredJITCompiler ---

TieredJITCompiler::TieredJITCompiler(engine::Options options, engine::TieredCompilationConfig config)
    : baseCompiler_(options), config_(std::move(config)) {
}

TieredJITCompiler::~TieredJITCompiler() = default;

std::unique_ptr<Executable> TieredJITCompiler::compile(wrapper_function function) const {
	static constexpr auto ROOT_FUNCTION_NAME = "execute";
	auto rootFunction = CompilableFunction(ROOT_FUNCTION_NAME, function);
	std::list<CompilableFunction> functionsToTrace;
	functionsToTrace.push_back(rootFunction);
	return compile(functionsToTrace);
}

std::unique_ptr<Executable> TieredJITCompiler::compile(std::list<CompilableFunction>& functions) const {
	auto ir = baseCompiler_.compileToIR(functions);
	auto tier0Executable = baseCompiler_.compileIR(ir, config_.tier0.backend);
	return std::make_unique<TieredExecutable>(std::move(tier0Executable), std::move(ir), config_,
	                                          baseCompiler_.getOptions());
}

std::string TieredJITCompiler::getName() const {
	return "tiered(" + config_.tier0.backend + "," + config_.tier1.backend + ")";
}

const engine::Options& TieredJITCompiler::getOptions() const {
	return baseCompiler_.getOptions();
}

} // namespace nautilus::compiler

#else

#include "nautilus/exceptions/RuntimeException.hpp"

namespace nautilus::compiler {

DelegatingInvocable::DelegatingInvocable(std::unique_ptr<Executable>&, std::shared_mutex&, std::string) {
}

std::any DelegatingInvocable::invokeGeneric(const std::vector<std::any>&) {
	return {};
}

TieredExecutable::TieredExecutable(std::unique_ptr<Executable>, std::shared_ptr<ir::IRGraph>,
                                   const engine::TieredCompilationConfig&, const engine::Options&) {
}
TieredExecutable::~TieredExecutable() = default;
bool TieredExecutable::hasInvocableFunctionPtr() {
	return false;
}
void* TieredExecutable::getInvocableFunctionPtr(const std::string&) {
	return nullptr;
}
std::unique_ptr<Executable::GenericInvocable> TieredExecutable::getGenericInvocable(const std::string&) {
	return nullptr;
}
bool TieredExecutable::isPromoted() const {
	return false;
}
void TieredExecutable::waitForPromotion() {
}
void TieredExecutable::startBackgroundPromotion() {
}

TieredJITCompiler::TieredJITCompiler(engine::Options, engine::TieredCompilationConfig) : baseCompiler_() {
}
TieredJITCompiler::~TieredJITCompiler() = default;
std::unique_ptr<Executable> TieredJITCompiler::compile(wrapper_function) const {
	throw RuntimeException("Jit not initialised");
}
std::unique_ptr<Executable> TieredJITCompiler::compile(std::list<CompilableFunction>&) const {
	throw RuntimeException("Jit not initialised");
}
std::string TieredJITCompiler::getName() const {
	return "";
}
const engine::Options& TieredJITCompiler::getOptions() const {
	return baseCompiler_.getOptions();
}

} // namespace nautilus::compiler

#endif
