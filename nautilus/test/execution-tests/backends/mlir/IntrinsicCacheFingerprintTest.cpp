#include "catch2/catch_test_macros.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRMemoryIntrinsics.hpp"
#include "nautilus/function.hpp"
#include "nautilus/val.hpp"
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
	using MLIRIntrinsicPlugin::cacheFingerprintForAddress;

	std::optional<std::string> cacheFingerprint() const override {
		auto image = cacheFingerprintForAddress(reinterpret_cast<const void*>(&lowerArithmeticValue));
		if (!image) {
			return std::nullopt;
		}
		return *image + (multiply_->load() ? ":multiply" : ":add");
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
		        ("nautilus-intrinsic-cache-test-" +
		         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "-" +
		         std::to_string(sequence.fetch_add(1)));
		std::filesystem::create_directories(path_);
	}

	~TemporaryCacheDirectory() {
		std::error_code error;
		std::filesystem::remove_all(path_, error);
	}

	const std::filesystem::path& path() const {
		return path_;
	}

private:
	std::filesystem::path path_;
};

std::string framedIdentity(std::string_view identity) {
	return std::to_string(identity.size()) + ":" + std::string(identity);
}

template <typename T>
T cacheStat(const engine::CompiledModule& module, const std::string& name) {
	auto statistics = module.getStatistics();
	REQUIRE(statistics != nullptr);
	const auto* value = statistics->find(name);
	REQUIRE(value != nullptr);
	return std::get<T>(*value);
}

engine::CompiledModule compileArithmeticModule(const std::filesystem::path& directory, int& traces,
                                               std::function<void()> onTrace = {}) {
	engine::Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("engine.Blob.CacheDir", directory.string());
	options.setOption("engine.Blob.CacheKey", std::string("intrinsic-fingerprint-v1"));
	options.setOption("mlir.enableMultithreading", false);
	engine::NautilusEngine engine(options);
	auto module = engine.createModule();
	module.registerFunction<val<int32_t>(val<int32_t>)>("execute", [&traces, onTrace](val<int32_t> value) {
		++traces;
		if (onTrace) {
			onTrace();
		}
		return invoke(arithmeticValue, value);
	});
	return module.compile();
}

void requireArithmeticResult(engine::CompiledModule& module, bool multiply) {
	auto execute = module.getFunction<int32_t(int32_t)>("execute");
	for (const int32_t input : {0, 7, -3}) {
		CAPTURE(input, multiply);
		REQUIRE(execute(input) == (multiply ? input * 2 : input + 2));
	}
}

std::map<std::filesystem::path, std::string> readArtifacts(const std::filesystem::path& directory) {
	std::map<std::filesystem::path, std::string> artifacts;
	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		std::ifstream input(entry.path(), std::ios::binary);
		REQUIRE(input.good());
		artifacts.emplace(entry.path(), std::string(std::istreambuf_iterator<char> {input}, {}));
	}
	return artifacts;
}

bool runIsolated(const char* testName) {
#ifdef __linux__
	static constexpr auto CHILD_TEST = "NAUTILUS_INTRINSIC_CACHE_TEST_CHILD";
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
	SKIP("Executable image fingerprints require Linux ELF build IDs");
#endif
	return false;
}

} // namespace

TEST_CASE("MLIR intrinsic cache fingerprints identify built-in executable images", "[cache][intrinsics]") {
	if (!runIsolated("MLIR intrinsic cache fingerprints identify built-in executable images")) {
		return;
	}
	auto& registry = MLIRIntrinsicPluginRegistry::instance();
	const auto before = registry.cacheFingerprint();
	REQUIRE(before.has_value());
	const auto image =
	    common::locateExecutableAddress(reinterpret_cast<const void*>(&RegisterMLIRMemoryIntrinsicPlugin));
	REQUIRE(image.has_value());
	RegisterMLIRMemoryIntrinsicPlugin();
	const auto identity = image->buildId + ":" + std::to_string(image->loadOffset);
	REQUIRE(registry.cacheFingerprint() == *before + framedIdentity(identity));
	REQUIRE(registry.cacheFingerprint() == registry.cacheFingerprint());
	REQUIRE_FALSE(IdentifiedArithmeticIntrinsicPlugin::cacheFingerprintForAddress(nullptr).has_value());
	int data = 42;
	REQUIRE_FALSE(IdentifiedArithmeticIntrinsicPlugin::cacheFingerprintForAddress(&data).has_value());
	const auto withMemory = registry.cacheFingerprint();
	registry.addPlugin(nullptr);
	REQUIRE(registry.cacheFingerprint() == withMemory);
}

TEST_CASE("MLIR intrinsic cache fingerprints track plugin state and registration order", "[cache][intrinsics]") {
	if (!runIsolated("MLIR intrinsic cache fingerprints track plugin state and registration order")) {
		return;
	}
	auto& registry = MLIRIntrinsicPluginRegistry::instance();
	const auto before = registry.cacheFingerprint();
	REQUIRE(before.has_value());
	auto add = std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(false);
	auto multiply = std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(true);
	const auto addIdentity = add->cacheFingerprint();
	const auto multiplyIdentity = multiply->cacheFingerprint();
	REQUIRE(addIdentity.has_value());
	REQUIRE(multiplyIdentity.has_value());
	REQUIRE(addIdentity != multiplyIdentity);
	IdentifiedArithmeticIntrinsicPlugin equivalent(false);
	REQUIRE(equivalent.cacheFingerprint() == addIdentity);

	registry.addPlugin(add);
	const auto withAdd = registry.cacheFingerprint();
	REQUIRE(withAdd == *before + framedIdentity(*addIdentity));
	const auto intrinsicId = ir::IntrinsicRegistry::instance().lookup(reinterpret_cast<void*>(&arithmeticValue));
	REQUIRE(intrinsicId != ir::IntrinsicId::None);
	add->setMultiply(true);
	REQUIRE(registry.cacheFingerprint() == *before + framedIdentity(*multiplyIdentity));
	add->setMultiply(false);
	REQUIRE(registry.cacheFingerprint() == withAdd);

	registry.addPlugin(multiply);
	const auto ordered = registry.cacheFingerprint();
	REQUIRE(ordered == *before + framedIdentity(*addIdentity) + framedIdentity(*multiplyIdentity));
	add->setMultiply(true);
	multiply->setMultiply(false);
	REQUIRE(registry.cacheFingerprint() == *before + framedIdentity(*multiplyIdentity) + framedIdentity(*addIdentity));
	REQUIRE(registry.cacheFingerprint() != ordered);

	MLIRIntrinsicManager first;
	MLIRIntrinsicManager second;
	registry.registerAllIntrinsics(first);
	registry.registerAllIntrinsics(second);
	REQUIRE(first.getIntrinsic(intrinsicId).has_value());
	REQUIRE(second.getIntrinsic(intrinsicId).has_value());
	REQUIRE(add->registrations == 1);
	REQUIRE(multiply->registrations == 1);
	REQUIRE(ir::IntrinsicRegistry::instance().lookup(reinterpret_cast<void*>(&arithmeticValue)) == intrinsicId);
}

TEST_CASE("MLIR intrinsic plugins without explicit fingerprints disable cache eligibility", "[cache][intrinsics]") {
	if (!runIsolated("MLIR intrinsic plugins without explicit fingerprints disable cache eligibility")) {
		return;
	}
	auto& registry = MLIRIntrinsicPluginRegistry::instance();
	REQUIRE(registry.cacheFingerprint().has_value());
	auto plugin = std::make_shared<ArithmeticIntrinsicPlugin>(true);
	REQUIRE_FALSE(plugin->cacheFingerprint().has_value());
	registry.addPlugin(plugin);
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
	const auto intrinsicId = ir::IntrinsicRegistry::instance().lookup(reinterpret_cast<void*>(&arithmeticValue));
	MLIRIntrinsicManager manager;
	registry.registerAllIntrinsics(manager);
	REQUIRE(manager.getIntrinsic(intrinsicId).has_value());
	REQUIRE(plugin->registrations == 1);
}

TEST_CASE("MLIR partially registered intrinsic plugins disable cache eligibility", "[cache][intrinsics]") {
	if (!runIsolated("MLIR partially registered intrinsic plugins disable cache eligibility")) {
		return;
	}
	auto& registry = MLIRIntrinsicPluginRegistry::instance();
	REQUIRE(registry.cacheFingerprint().has_value());
	REQUIRE_THROWS_AS(registry.addPlugin(std::make_shared<FailedRegistrationPlugin>(true)), std::runtime_error);
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
	registry.addPlugin(std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(false));
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
}

TEST_CASE("MLIR persistent cache separates intrinsic plugin states without tracing warm hits", "[cache][intrinsics]") {
	if (!runIsolated("MLIR persistent cache separates intrinsic plugin states without tracing warm hits")) {
		return;
	}
	TemporaryCacheDirectory cache;
	auto plugin = std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(false);
	MLIRIntrinsicPluginRegistry::instance().addPlugin(plugin);
	int traces = 0;
	auto compile = [&](bool multiply, bool warm) {
		plugin->setMultiply(multiply);
		const auto before = traces;
		auto module = compileArithmeticModule(cache.path(), traces);
		INFO(module.getStatistics()->toString());
		requireArithmeticResult(module, multiply);
		REQUIRE((warm ? traces == before : traces > before));
		REQUIRE(cacheStat<int64_t>(module, "cache.eligible") == 1);
		REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == (warm ? 0 : 1));
		REQUIRE(cacheStat<std::string>(module, "cache.object") == (warm ? "hit" : "written"));
		REQUIRE(cacheStat<std::string>(module, "cache.mlir") == (warm ? "not_checked" : "written"));
		REQUIRE(cacheStat<std::string>(module, "cache.fallback") == "none");
		const auto key = cacheStat<std::string>(module, "cache.key");
		REQUIRE_FALSE(key.empty());
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			REQUIRE(std::filesystem::is_regular_file(cache.path() / (key + extension)));
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
		REQUIRE(std::filesystem::remove(cache.path() / (key + ".o")));
		plugin->setMultiply(multiply);
		const auto before = traces;
		auto module = compileArithmeticModule(cache.path(), traces);
		requireArithmeticResult(module, multiply);
		REQUIRE(cacheStat<std::string>(module, "cache.key") == key);
		REQUIRE(cacheStat<std::string>(module, "cache.mlir") == "hit");
		REQUIRE(cacheStat<std::string>(module, "cache.object") == "invalid_rewritten");
		REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == 0);
		REQUIRE(traces == before);
		REQUIRE(compile(multiply, true) == key);
	}
	REQUIRE(readArtifacts(cache.path()).size() == 6);
	REQUIRE(plugin->registrations == 1);
}

TEST_CASE("MLIR persistent cache bypasses artifacts for unidentified intrinsic plugins", "[cache][intrinsics]") {
	if (!runIsolated("MLIR persistent cache bypasses artifacts for unidentified intrinsic plugins")) {
		return;
	}
	TemporaryCacheDirectory populated;
	TemporaryCacheDirectory empty;
	auto& registry = MLIRIntrinsicPluginRegistry::instance();
	registry.addPlugin(std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(false));
	int traces = 0;
	{
		auto module = compileArithmeticModule(populated.path(), traces);
		requireArithmeticResult(module, false);
		REQUIRE(cacheStat<std::string>(module, "cache.object") == "written");
	}
	const auto originalArtifacts = readArtifacts(populated.path());
	REQUIRE(originalArtifacts.size() == 3);
	registry.addPlugin(std::make_shared<ArithmeticIntrinsicPlugin>(true));
	REQUIRE_FALSE(registry.cacheFingerprint().has_value());
	for (const auto& directory : {populated.path(), empty.path()}) {
		for (int iteration = 0; iteration < 2; ++iteration) {
			const auto before = traces;
			auto module = compileArithmeticModule(directory, traces);
			INFO(module.getStatistics()->toString());
			requireArithmeticResult(module, true);
			REQUIRE(traces > before);
			REQUIRE(cacheStat<int64_t>(module, "cache.eligible") == 0);
			REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == 1);
			REQUIRE(cacheStat<std::string>(module, "cache.object") == "not_used");
			REQUIRE(cacheStat<std::string>(module, "cache.mlir") == "not_used");
			REQUIRE(cacheStat<std::string>(module, "cache.fallback") == "unidentified_intrinsic_plugin");
			REQUIRE_FALSE(module.getStatistics()->contains("cache.key"));
			REQUIRE(readArtifacts(populated.path()) == originalArtifacts);
			REQUIRE(std::filesystem::is_empty(empty.path()));
		}
	}
}

TEST_CASE("MLIR persistent cache withholds publication when intrinsic plugins register during tracing",
          "[cache][intrinsics]") {
	if (!runIsolated("MLIR persistent cache withholds publication when intrinsic plugins register during tracing")) {
		return;
	}
	TemporaryCacheDirectory cache;
	auto& registry = MLIRIntrinsicPluginRegistry::instance();
	registry.addPlugin(std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(false));
	auto replacement = std::make_shared<IdentifiedArithmeticIntrinsicPlugin>(true);
	int traces = 0;
	bool registered = false;
	auto changed = compileArithmeticModule(cache.path(), traces, [&] {
		if (!registered) {
			registry.addPlugin(replacement);
			registered = true;
		}
	});
	INFO(changed.getStatistics()->toString());
	REQUIRE(registered);
	REQUIRE(replacement->registrations == 1);
	requireArithmeticResult(changed, true);
	REQUIRE(traces > 0);
	REQUIRE(cacheStat<int64_t>(changed, "cache.tracingRan") == 1);
	REQUIRE(cacheStat<std::string>(changed, "cache.object") == "miss");
	REQUIRE(cacheStat<std::string>(changed, "cache.mlir") == "miss");
	REQUIRE(cacheStat<std::string>(changed, "cache.fallback") == "intrinsic_registry_changed");
	const auto oldKey = cacheStat<std::string>(changed, "cache.key");
	REQUIRE_FALSE(oldKey.empty());
	REQUIRE(std::filesystem::is_empty(cache.path()));
	std::string newKey;
	for (int iteration = 0; iteration < 2; ++iteration) {
		const auto before = traces;
		auto module = compileArithmeticModule(cache.path(), traces);
		requireArithmeticResult(module, true);
		REQUIRE((iteration == 0 ? traces > before : traces == before));
		REQUIRE(cacheStat<int64_t>(module, "cache.tracingRan") == (iteration == 0 ? 1 : 0));
		REQUIRE(cacheStat<std::string>(module, "cache.object") == (iteration == 0 ? "written" : "hit"));
		REQUIRE(cacheStat<std::string>(module, "cache.fallback") == "none");
		const auto key = cacheStat<std::string>(module, "cache.key");
		REQUIRE(key != oldKey);
		if (iteration == 0) {
			newKey = key;
		} else {
			REQUIRE(key == newKey);
		}
		for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
			REQUIRE_FALSE(std::filesystem::exists(cache.path() / (oldKey + extension)));
			REQUIRE(std::filesystem::is_regular_file(cache.path() / (newKey + extension)));
		}
	}
}

} // namespace nautilus::compiler::mlir
