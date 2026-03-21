#pragma once

#include "nautilus/Executable.hpp"
#include "nautilus/JITCompiler.hpp"
#include <atomic>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>

namespace nautilus::engine {

/**
 * @brief Configuration for a single compilation tier.
 */
struct TierConfig {
	std::string backend; // backend name ("bc", "mlir", "cpp", "asmjit")
};

/**
 * @brief Configuration for multi-tier JIT compilation.
 *
 * Tier 0 is the fast compilation tier used immediately.
 * Tier 1 is the high-performance tier compiled in the background.
 */
struct TieredCompilationConfig {
	TierConfig tier0 {"bc"};   // low-latency tier (compiled synchronously)
	TierConfig tier1 {"mlir"}; // high-performance tier (compiled in background)
};

} // namespace nautilus::engine

namespace nautilus::compiler {

/**
 * @brief A GenericInvocable that delegates to the current executable under a shared lock.
 *
 * Used by TieredExecutable to allow transparent switching between tier-0 and tier-1
 * executables. Each call re-resolves against the current delegate, so when the
 * background promotion swaps the delegate, subsequent calls go to the new executable.
 */
class DelegatingInvocable : public Executable::GenericInvocable {
public:
	DelegatingInvocable(std::unique_ptr<Executable>& delegate, std::shared_mutex& mutex, std::string member);
	std::any invokeGeneric(const std::vector<std::any>& args) override;

private:
	std::unique_ptr<Executable>& delegate_;
	std::shared_mutex& mutex_;
	std::string member_;
};

/**
 * @brief An executable wrapper that starts with a fast tier-0 executable and
 * atomically switches to a tier-1 executable when background compilation completes.
 *
 * The onPromoted callback is invoked (from the background thread) when tier-1 is ready.
 * CompiledModule uses this to bump its version, causing ModuleFunction handles to re-resolve.
 *
 * Thread-safety: all accessor methods are safe to call concurrently with the
 * background promotion thread. A shared_mutex protects the delegate swap.
 */
class TieredExecutable : public Executable {
public:
	using PromotionCallback = std::function<void()>;

	TieredExecutable(std::unique_ptr<Executable> tier0, std::shared_ptr<ir::IRGraph> ir,
	                 const engine::TieredCompilationConfig& config, const engine::Options& options);
	~TieredExecutable() override;

	/**
	 * @brief Set a callback invoked when tier-1 promotion completes.
	 * Must be called before any invocations if re-resolution is desired.
	 */
	void setPromotionCallback(PromotionCallback callback);

	bool hasInvocableFunctionPtr() override;
	void* getInvocableFunctionPtr(const std::string& member) override;
	std::unique_ptr<GenericInvocable> getGenericInvocable(const std::string& member) override;

	/**
	 * @brief Returns true if tier 1 compilation has completed and is active.
	 */
	bool isPromoted() const;

	/**
	 * @brief Blocks until tier 1 promotion completes.
	 */
	void waitForPromotion();

private:
	void startBackgroundPromotion();

	std::unique_ptr<Executable> delegate_;
	std::shared_ptr<ir::IRGraph> cachedIR_;
	engine::TieredCompilationConfig config_;
	engine::Options options_;
	mutable std::shared_mutex mutex_;
	std::atomic<bool> promoted_ {false};
	std::thread promotionThread_;
	PromotionCallback onPromoted_;
};

/**
 * @brief Multi-tier JIT compiler implementation.
 *
 * Compiles functions first with a fast backend (tier 0) for immediate execution,
 * then promotes to a high-performance backend (tier 1) in the background.
 * The returned Executable transparently switches from tier-0 to tier-1 when ready.
 */
class TieredJITCompiler : public IJITCompiler {
public:
	TieredJITCompiler(engine::Options options, engine::TieredCompilationConfig config);
	~TieredJITCompiler() override;

	[[nodiscard]] std::unique_ptr<Executable> compile(wrapper_function function) const override;
	[[nodiscard]] std::unique_ptr<Executable> compile(std::list<CompilableFunction>& functions) const override;

	std::string getName() const override;
	const engine::Options& getOptions() const override;

private:
	JITCompiler baseCompiler_;
	engine::TieredCompilationConfig config_;
};

} // namespace nautilus::compiler
