#include "catch2/catch_test_macros.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/cache/plugin.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/function.hpp"
#include "nautilus/val.hpp"
#include <array>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#ifdef __linux__
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace nautilus::compiler::mlir {
namespace {

int32_t arithmeticValue(int32_t value) {
	return value + 2;
}

bool lowerArithmeticValue(std::unique_ptr<::mlir::OpBuilder>& builder, const ir::CallOperation* call,
                          MLIRLoweringProvider::ValueFrame& frame, bool multiply) {
	auto input = frame.getValue(call->getInputArguments()[0]->getIdentifier());
	auto two = ::mlir::arith::ConstantIntOp::create(*builder, builder->getUnknownLoc(), 2, 32);
	if (multiply) {
		auto result = ::mlir::arith::MulIOp::create(*builder, builder->getUnknownLoc(), input, two);
		frame.setValue(call->getIdentifier(), result);
	} else {
		auto result = ::mlir::arith::AddIOp::create(*builder, builder->getUnknownLoc(), input, two);
		frame.setValue(call->getIdentifier(), result);
	}
	return true;
}

class ArithmeticIntrinsicPlugin : public MLIRIntrinsicPlugin {
public:
	explicit ArithmeticIntrinsicPlugin(bool multiply) : multiply_(std::make_shared<std::atomic<bool>>(multiply)) {
	}

	void registerIntrinsics(MLIRIntrinsicManager& manager) override {
		++registrations;
		manager.addIntrinsic(reinterpret_cast<void*>(&arithmeticValue),
		                     [multiply = multiply_](std::unique_ptr<::mlir::OpBuilder>& builder,
		                                            const ir::CallOperation* call,
		                                            MLIRLoweringProvider::ValueFrame& frame) {
			                     return lowerArithmeticValue(builder, call, frame, multiply->load());
		                     });
	}

	void setMultiply(bool multiply) {
		multiply_->store(multiply);
	}

	int registrations = 0;

protected:
	std::shared_ptr<std::atomic<bool>> multiply_;
};

class IdentifiedArithmeticIntrinsicPlugin final : public ArithmeticIntrinsicPlugin {
public:
	using ArithmeticIntrinsicPlugin::ArithmeticIntrinsicPlugin;

	std::optional<std::string> cacheFingerprint() const override {
		auto image = cacheFingerprintForAddress(reinterpret_cast<const void*>(&lowerArithmeticValue));
		if (!image) {
			return std::nullopt;
		}
		return *image + (multiply_->load() ? ":multiply" : ":add");
	}

	bool supportsArtifacts() const override {
		return true;
	}
};

class UnidentifiedArtifactIntrinsicPlugin final : public ArithmeticIntrinsicPlugin {
public:
	using ArithmeticIntrinsicPlugin::ArithmeticIntrinsicPlugin;

	bool supportsArtifacts() const override {
		return true;
	}
};

class FailedRegistrationPlugin final : public ArithmeticIntrinsicPlugin {
public:
	using ArithmeticIntrinsicPlugin::ArithmeticIntrinsicPlugin;

	void registerIntrinsics(MLIRIntrinsicManager& manager) override {
		ArithmeticIntrinsicPlugin::registerIntrinsics(manager);
		throw std::runtime_error("intrinsic registration failed");
	}
};

class TemporaryCacheDirectory {
public:
	TemporaryCacheDirectory() {
		static std::atomic<uint64_t> sequence {0};
		path_ = std::filesystem::temp_directory_path() /
		        ("nautilus-intrinsic-plugin-cache-test-" +
		         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
		         std::to_string(sequence.fetch_add(1)));
		std::filesystem::create_directories(path_);
		std::filesystem::permissions(path_, std::filesystem::perms::owner_all);
	}

	~TemporaryCacheDirectory() {
		std::error_code ignored;
		std::filesystem::remove_all(path_, ignored);
	}

	const std::filesystem::path& path() const {
		return path_;
	}

private:
	std::filesystem::path path_;
};

template <typename T>
T cacheStat(const engine::CompiledModule& module, const std::string& name) {
	const auto statistics = module.getStatistics();
	REQUIRE(statistics != nullptr);
	INFO(statistics->toString());
	const auto* value = statistics->find(name);
	REQUIRE(value != nullptr);
	REQUIRE(std::holds_alternative<T>(*value));
	return std::get<T>(*value);
}

engine::CompiledModule compileArithmeticModule(const std::filesystem::path& directory, int& wrappers,
                                               std::function<void()> onTrace = {}) {
	engine::Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.cache.directory", directory.string());
	options.setOption("engine.cache.key", std::string("intrinsic-fingerprint"));
	options.setOption("mlir.enableMultithreading", false);
	engine::NautilusEngine engine(cache::createCompiler(options), options);
	auto module = engine.createModule();
	module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&wrappers, onTrace](val<int32_t> value) {
		++wrappers;
		if (onTrace) {
			onTrace();
		}
		return invoke(arithmeticValue, value);
	});
	return module.compile();
}

void requireArithmeticResult(engine::CompiledModule& module, bool multiply) {
	const auto execute = module.getFunction<int32_t(int32_t)>("execute");
	for (const int32_t input : {0, 7, -3}) {
		CAPTURE(input, multiply);
		REQUIRE(execute(input) == (multiply ? input * 2 : input + 2));
	}
}

std::map<std::string, std::string> readArtifacts(const std::filesystem::path& directory) {
	std::map<std::string, std::string> artifacts;
	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		const auto extension = entry.path().extension();
		if (!entry.is_regular_file() || (extension != ".o" && extension != ".mlirbc" && extension != ".manifest")) {
			continue;
		}
		std::ifstream input(entry.path(), std::ios::binary);
		REQUIRE(input.good());
		artifacts.emplace(entry.path().filename().string(), std::string(std::istreambuf_iterator<char> {input}, {}));
		REQUIRE_FALSE(input.bad());
	}
	return artifacts;
}

void requireNativeHit(const engine::CompiledModule& module) {
	REQUIRE(cacheStat<std::string>(module, "cache.object") == "hit");
	REQUIRE(cacheStat<std::string>(module, "cache.mlir") == "not_checked");
	REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == 0);
	REQUIRE(cacheStat<std::string>(module, "cache.fallback") == "none");
	const auto statistics = module.getStatistics();
	for (const auto& [name, value] : *statistics) {
		CAPTURE(name);
		REQUIRE_FALSE(name.starts_with("ir."));
		REQUIRE_FALSE(name.starts_with("irPasses."));
		REQUIRE_FALSE(name.starts_with("mlir."));
		REQUIRE_FALSE(name.starts_with("llvm."));
	}
	REQUIRE_FALSE(statistics->contains("tracing.ms"));
	REQUIRE_FALSE(statistics->contains("ssaCreation.ms"));
	REQUIRE_FALSE(statistics->contains("irGeneration.ms"));
	REQUIRE_FALSE(statistics->contains("frontend.totalMs"));
	REQUIRE_FALSE(statistics->contains("jit.compile.ms"));
	REQUIRE(statistics->contains("jit.objectLoad.ms"));
}

void requireFallback(const engine::CompiledModule& module) {
	REQUIRE(cacheStat<std::string>(module, "cache.object") != "hit");
	REQUIRE(cacheStat<std::string>(module, "cache.mlir") != "hit");
	REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == 1);
	const auto reason = cacheStat<std::string>(module, "cache.fallback");
	REQUIRE_FALSE(reason.empty());
	REQUIRE(reason != "none");
	REQUIRE(module.getStatistics()->contains("frontend.totalMs"));
}

bool runIsolated(const char* testName) {
#if defined(__linux__) && defined(__x86_64__)
	constexpr auto CHILD_TEST = "NAUTILUS_CACHE_INTRINSIC_CHILD";
	if (const auto* childTest = std::getenv(CHILD_TEST); childTest && std::string_view(childTest) == testName) {
		return true;
	}
	const auto executable = std::filesystem::read_symlink("/proc/self/exe");
	const auto child = ::fork();
	REQUIRE(child >= 0);
	if (child == 0) {
		if (::setenv(CHILD_TEST, testName, 1) != 0) {
			::_exit(125);
		}
		::execl(executable.c_str(), executable.c_str(), testName, "--reporter", "compact", static_cast<char*>(nullptr));
		::_exit(126);
	}
	int status = 0;
	pid_t waited;
	do {
		waited = ::waitpid(child, &status, 0);
	} while (waited < 0 && errno == EINTR);
	REQUIRE(waited == child);
	REQUIRE(WIFEXITED(status));
	REQUIRE(WEXITSTATUS(status) == 0);
#else
	(void) testName;
	SKIP("Persistent native caching requires the supported Linux x86-64 ELF producer");
#endif
	return false;
}

struct HooksScope {
	LLVMBackendHooks saved = getLLVMBackendHooks();
	~HooksScope() {
		getLLVMBackendHooks() = std::move(saved);
	}
};

void requireChangedPublication(engine::CompiledModule& changed, const std::filesystem::path& directory, int wrappers) {
	requireArithmeticResult(changed, true);
	REQUIRE(wrappers > 0);
	requireFallback(changed);
	REQUIRE(readArtifacts(directory).empty());
	const auto oldKey = cacheStat<std::string>(changed, "cache.key");
	REQUIRE_FALSE(oldKey.empty());
	int coldWrappers = 0;
	auto cold = compileArithmeticModule(directory, coldWrappers);
	requireArithmeticResult(cold, true);
	REQUIRE(coldWrappers > 0);
	REQUIRE(cacheStat<std::string>(cold, "cache.object") == "written");
	const auto newKey = cacheStat<std::string>(cold, "cache.key");
	REQUIRE(newKey != oldKey);
	int warmWrappers = 0;
	auto warm = compileArithmeticModule(directory, warmWrappers);
	requireArithmeticResult(warm, true);
	requireNativeHit(warm);
	REQUIRE(warmWrappers == 0);
	REQUIRE(cacheStat<std::string>(warm, "cache.key") == newKey);
	for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
		REQUIRE_FALSE(std::filesystem::exists(directory / (oldKey + extension)));
		REQUIRE(std::filesystem::is_regular_file(directory / (newKey + extension)));
	}
}

} // namespace

TEST_CASE("Cache keys distinguish intrinsic lowering states without tracing native hits", "[cache][intrinsics]") {
	if (!runIsolated("Cache keys distinguish intrinsic lowering states without tracing native hits")) {
		return;
	}
	TemporaryCacheDirectory directory;
	auto plugin = std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(false);
	MLIRIntrinsicPluginRegistry::instance().addPlugin(plugin);
	auto compile = [&](bool multiply, bool warm) {
		plugin->setMultiply(multiply);
		int wrappers = 0;
		auto module = compileArithmeticModule(directory.path(), wrappers);
		requireArithmeticResult(module, multiply);
		if (warm) {
			requireNativeHit(module);
			REQUIRE(wrappers == 0);
		} else {
			REQUIRE(wrappers > 0);
			REQUIRE(cacheStat<std::string>(module, "cache.object") == "written");
			REQUIRE(cacheStat<std::string>(module, "cache.mlir") == "written");
			REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == 1);
			REQUIRE(cacheStat<std::string>(module, "cache.fallback") == "none");
		}
		const auto key = cacheStat<std::string>(module, "cache.key");
		REQUIRE_FALSE(key.empty());
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			REQUIRE(std::filesystem::is_regular_file(directory.path() / (key + extension)));
		}
		return key;
	};
	const auto addKey = compile(false, false);
	REQUIRE(compile(false, true) == addKey);
	const auto multiplyKey = compile(true, false);
	REQUIRE(multiplyKey != addKey);
	REQUIRE(compile(true, true) == multiplyKey);
	REQUIRE(compile(false, true) == addKey);
	REQUIRE(compile(true, true) == multiplyKey);
	for (const bool multiply : {false, true}) {
		const auto& key = multiply ? multiplyKey : addKey;
		REQUIRE(std::filesystem::remove(directory.path() / (key + ".o")));
		plugin->setMultiply(multiply);
		int wrappers = 0;
		auto repaired = compileArithmeticModule(directory.path(), wrappers);
		requireArithmeticResult(repaired, multiply);
		REQUIRE(cacheStat<std::string>(repaired, "cache.key") == key);
		REQUIRE(cacheStat<std::string>(repaired, "cache.mlir") == "hit");
		REQUIRE(cacheStat<std::string>(repaired, "cache.object") != "hit");
		REQUIRE(cacheStat<int64_t>(repaired, "cache.tracingRan") == 0);
		REQUIRE_FALSE(repaired.getStatistics()->contains("tracing.ms"));
		REQUIRE(repaired.getStatistics()->contains("jit.compile.ms"));
		REQUIRE(wrappers == 0);
		REQUIRE(compile(multiply, true) == key);
	}
	REQUIRE(readArtifacts(directory.path()).size() == 6);
	REQUIRE(plugin->registrations == 1);
}

TEST_CASE("Unknown intrinsic fingerprints bypass populated and empty caches", "[cache][intrinsics]") {
	if (!runIsolated("Unknown intrinsic fingerprints bypass populated and empty caches")) {
		return;
	}
	TemporaryCacheDirectory populated;
	TemporaryCacheDirectory empty;
	auto& registry = MLIRIntrinsicPluginRegistry::instance();
	registry.addPlugin(std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(false));
	int coldWrappers = 0;
	auto cold = compileArithmeticModule(populated.path(), coldWrappers);
	requireArithmeticResult(cold, false);
	REQUIRE(cacheStat<std::string>(cold, "cache.object") == "written");
	const auto original = readArtifacts(populated.path());
	REQUIRE(original.size() == 3);
	auto unidentified = std::make_shared<UnidentifiedArtifactIntrinsicPlugin>(true);
	REQUIRE(unidentified->supportsArtifacts());
	REQUIRE_FALSE(unidentified->cacheFingerprint().has_value());
	registry.addPlugin(unidentified);
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
	REQUIRE_FALSE(registry.artifactFingerprint().has_value());
	for (const auto& directory : {populated.path(), empty.path()}) {
		std::array<int, 2> wrappers {};
		for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
			auto module = compileArithmeticModule(directory, wrappers[iteration]);
			requireArithmeticResult(module, true);
			REQUIRE(wrappers[iteration] > 0);
			requireFallback(module);
			REQUIRE_FALSE(module.getStatistics()->contains("cache.key"));
			REQUIRE(readArtifacts(populated.path()) == original);
			REQUIRE(std::filesystem::is_empty(empty.path()));
		}
	}
	REQUIRE(unidentified->registrations == 1);
}

TEST_CASE("Partial intrinsic registration cannot authorize cache publication", "[cache][intrinsics]") {
	if (!runIsolated("Partial intrinsic registration cannot authorize cache publication")) {
		return;
	}
	TemporaryCacheDirectory directory;
	auto& registry = MLIRIntrinsicPluginRegistry::instance();
	REQUIRE_THROWS_AS(registry.addPlugin(std::make_shared<FailedRegistrationPlugin>(true)), std::runtime_error);
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
	for (int iteration = 0; iteration < 2; ++iteration) {
		int wrappers = 0;
		auto module = compileArithmeticModule(directory.path(), wrappers);
		requireArithmeticResult(module, true);
		requireFallback(module);
		REQUIRE(wrappers > 0);
		REQUIRE(readArtifacts(directory.path()).empty());
	}
}

TEST_CASE("Intrinsic registration during tracing withholds old-key publication", "[cache][intrinsics]") {
	if (!runIsolated("Intrinsic registration during tracing withholds old-key publication")) {
		return;
	}
	TemporaryCacheDirectory directory;
	auto& registry = MLIRIntrinsicPluginRegistry::instance();
	registry.addPlugin(std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(false));
	auto replacement = std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(true);
	bool registered = false;
	int wrappers = 0;
	auto changed = compileArithmeticModule(directory.path(), wrappers, [&] {
		if (!registered) {
			registry.addPlugin(replacement);
			registered = true;
		}
	});
	REQUIRE(registered);
	REQUIRE(replacement->registrations == 1);
	requireChangedPublication(changed, directory.path(), wrappers);
}

TEST_CASE("Intrinsic fingerprint mutation during tracing withholds old-key publication", "[cache][intrinsics]") {
	if (!runIsolated("Intrinsic fingerprint mutation during tracing withholds old-key publication")) {
		return;
	}
	TemporaryCacheDirectory directory;
	auto plugin = std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(false);
	MLIRIntrinsicPluginRegistry::instance().addPlugin(plugin);
	const auto before = plugin->cacheFingerprint();
	REQUIRE(before.has_value());
	int wrappers = 0;
	auto changed = compileArithmeticModule(directory.path(), wrappers, [&] { plugin->setMultiply(true); });
	REQUIRE(plugin->cacheFingerprint() != before);
	requireChangedPublication(changed, directory.path(), wrappers);
	REQUIRE(plugin->registrations == 1);
}

TEST_CASE("Unfingerprinted LLVM hooks bypass cached code while ordinary compilation still runs",
          "[cache][intrinsics][hooks]") {
	if (!runIsolated("Unfingerprinted LLVM hooks bypass cached code while ordinary compilation still runs")) {
		return;
	}
	TemporaryCacheDirectory directory;
	int coldWrappers = 0;
	auto cold = compileArithmeticModule(directory.path(), coldWrappers);
	requireArithmeticResult(cold, false);
	REQUIRE(cacheStat<std::string>(cold, "cache.object") == "written");
	const auto original = readArtifacts(directory.path());
	REQUIRE(original.size() == 3);
	for (const auto* kind : {"optimizer", "symbol contributor", "call naming"}) {
		CAPTURE(kind);
		int hookCalls = 0;
		{
			HooksScope restore;
			auto& hooks = getLLVMBackendHooks();
			if (std::string_view(kind) == "optimizer") {
				hooks.preOptModuleTransform = [&](llvm::Module&) {
					++hookCalls;
				};
			} else if (std::string_view(kind) == "symbol contributor") {
				hooks.jitSymbolContributor = [&](const SymbolContributor&) {
					++hookCalls;
				};
			} else {
				hooks.callNameOverride = [&](void*) -> std::optional<std::string> {
					++hookCalls;
					return std::nullopt;
				};
			}
			std::array<int, 2> wrappers {};
			for (std::size_t iteration = 0; iteration < wrappers.size(); ++iteration) {
				auto module = compileArithmeticModule(directory.path(), wrappers[iteration]);
				requireArithmeticResult(module, false);
				REQUIRE(wrappers[iteration] > 0);
				requireFallback(module);
				REQUIRE(readArtifacts(directory.path()) == original);
			}
			if (std::string_view(kind) == "symbol contributor") {
				REQUIRE(hookCalls > 0);
			} else {
				REQUIRE(hookCalls == 0);
			}
		}
		int warmWrappers = 0;
		auto warm = compileArithmeticModule(directory.path(), warmWrappers);
		requireArithmeticResult(warm, false);
		requireNativeHit(warm);
		REQUIRE(warmWrappers == 0);
		REQUIRE(readArtifacts(directory.path()) == original);
	}
}

} // namespace nautilus::compiler::mlir
