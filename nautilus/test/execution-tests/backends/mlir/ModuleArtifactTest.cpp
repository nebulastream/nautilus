#include "nautilus/Artifact.hpp"
#include "nautilus/Engine.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/function.hpp"
#include "nautilus/select.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val_func.hpp"
#include "nautilus/val_std.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#ifdef __linux__
#include <dlfcn.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace nautilus::engine {
namespace {

std::size_t artifactWrapperCalls = 0;

int64_t artifactProxy(int64_t value) noexcept {
	return value * 3 - 4;
}

int64_t artifactOtherProxy(int64_t value) noexcept {
	return value * 5 + 2;
}

int64_t artifactCheckedProxy(int64_t value) {
	if (value < 0) {
		throw std::runtime_error("artifact checked proxy failure");
	}
	return artifactProxy(value);
}

void artifactScalarCleanup(int64_t* value) noexcept {
	++*value;
}

uintptr_t artifactProxyAddress() noexcept {
	return reinterpret_cast<uintptr_t>(&artifactProxy);
}

int32_t artifactNarrowProxy(bool flag, int8_t signedByte, uint16_t unsignedWord, int32_t wide) noexcept {
	return flag ? wide + signedByte + unsignedWord : wide - signedByte - unsignedWord;
}

struct ArtifactFailure : std::runtime_error {
	explicit ArtifactFailure(int32_t value) : std::runtime_error("artifact cleanup failure"), value(value) {
	}
	int32_t value;
};

int32_t artifactMaybeThrow(int32_t* state, bool shouldThrow) {
	++state[2];
	if (shouldThrow) {
		throw ArtifactFailure(state[2]);
	}
	return state[2];
}

void artifactFirstCleanup(int32_t* state) noexcept {
	state[0] = state[0] * 10 + 1;
	++state[1];
}

void artifactSecondCleanup(int32_t* state) noexcept {
	state[0] = state[0] * 10 + 2;
	++state[1];
}

struct alignas(64) ArtifactTypedBuffer {
	int32_t* state = nullptr;
	int32_t id = 0;

	ArtifactTypedBuffer() noexcept = default;
	ArtifactTypedBuffer(int32_t* state, int32_t id) noexcept : state(state), id(id) {
		++state[2];
	}
	ArtifactTypedBuffer(const ArtifactTypedBuffer& other) noexcept : ArtifactTypedBuffer(other.state, other.id + 1) {
	}
	~ArtifactTypedBuffer() noexcept {
		if (state) {
			state[0] = state[0] * 10 + id;
			++state[1];
		}
	}
};

int32_t artifactUseTypedBuffer(ArtifactTypedBuffer* buffer, bool shouldThrow) {
	++buffer->state[3];
	if (shouldThrow) {
		throw ArtifactFailure(buffer->state[3]);
	}
	return buffer->id;
}

void artifactUseTypedBufferVoid(ArtifactTypedBuffer* buffer, bool shouldThrow) {
	artifactUseTypedBuffer(buffer, shouldThrow);
}

class ArtifactNativeGuard {
public:
	ArtifactNativeGuard(val<int32_t*> address, void (*cleanup)(int32_t*) noexcept)
	    : address_(std::move(address)), cleanup_(cleanup) {
		if (tracing::inTracer()) {
			tracing::registerDestructor(address_.state, reinterpret_cast<void*>(cleanup_));
		}
	}

	~ArtifactNativeGuard() noexcept {
		if (tracing::inTracer()) {
			tracing::unregisterDestructor(address_.state);
		}
		invoke(cleanup_, address_);
	}

	ArtifactNativeGuard(const ArtifactNativeGuard&) = delete;
	ArtifactNativeGuard& operator=(const ArtifactNativeGuard&) = delete;

private:
	val<int32_t*> address_;
	void (*cleanup_)(int32_t*) noexcept;
};

void requireArtifactSupport() {
#ifdef __linux__
	REQUIRE(artifact::isSupported());
#else
	if (!artifact::isSupported()) {
		SKIP("Module artifacts require Linux ELF executable identities");
	}
#endif
}

Options artifactOptions() {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	return options;
}

artifact::ModuleArtifact scalarArtifact(const Options& options = artifactOptions(), int32_t increment = 7,
                                        const std::string& name = "increment") {
	NautilusEngine engine(options);
	auto module = engine.createModule();
	module.registerFunction<val<int32_t>(val<int32_t>)>(name, [increment](val<int32_t> value) {
		++artifactWrapperCalls;
		return value + cacheInvariant(increment);
	});
	return module.createArtifact();
}

artifact::ModuleArtifact proxyArtifact(bool alternate = false) {
	NautilusEngine engine(artifactOptions());
	auto module = engine.createModule();
	module.registerFunction<val<int64_t>(val<int64_t>)>("proxy", [alternate](val<int64_t> value) {
		++artifactWrapperCalls;
		return alternate ? invoke(artifactOtherProxy, value) : invoke(artifactProxy, value);
	});
	return module.createArtifact();
}

artifact::ModuleArtifact multiExportArtifact(const Options& options = artifactOptions()) {
	NautilusEngine engine(options);
	auto module = engine.createModule();
	module.registerFunction<val<int32_t>(val<int32_t>)>("increment", [](val<int32_t> value) {
		++artifactWrapperCalls;
		return value + cacheLiteral<int32_t {7}>();
	});
	module.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>)>("sum", [](val<int64_t> a, val<int64_t> b) {
		++artifactWrapperCalls;
		return a + b;
	});
	module.registerFunction<val<double>(val<double>)>("scaled", [](val<double> value) {
		++artifactWrapperCalls;
		return value * cacheInvariant(1.5) + cacheInvariant(0.25);
	});
	module.registerFunction<val<int32_t>(val<int32_t>)>("sum_loop", [](val<int32_t> count) {
		++artifactWrapperCalls;
		auto total = cacheLiteral<int32_t {0}>();
		for (auto i = cacheLiteral<int32_t {0}>(); i < count; i = i + cacheLiteral<int32_t {1}>()) {
			total = total + i;
		}
		return total;
	});
	module.registerFunction<val<bool>(val<bool>)>("boolean", [](val<bool> value) {
		++artifactWrapperCalls;
		return value;
	});
	module.registerFunction<val<int8_t>(val<int8_t>)>("signed_byte", [](val<int8_t> value) {
		++artifactWrapperCalls;
		return value;
	});
	module.registerFunction<val<uint8_t>(val<uint8_t>)>("unsigned_byte", [](val<uint8_t> value) {
		++artifactWrapperCalls;
		return value;
	});
	module.registerFunction<val<int16_t>(val<int16_t>)>("signed_word", [](val<int16_t> value) {
		++artifactWrapperCalls;
		return value;
	});
	module.registerFunction<val<uint16_t>(val<uint16_t>)>("unsigned_word", [](val<uint16_t> value) {
		++artifactWrapperCalls;
		return value;
	});
	module.registerFunction<val<int32_t>(val<bool>, val<int8_t>, val<uint16_t>, val<int32_t>)>(
	    "narrow_proxy", [](val<bool> flag, val<int8_t> byte, val<uint16_t> word, val<int32_t> wide) {
		    ++artifactWrapperCalls;
		    return invoke(artifactNarrowProxy, flag, byte, word, wide);
	    });
	module.registerFunction<val<int64_t>(val<int64_t>)>("proxy", [](val<int64_t> value) {
		++artifactWrapperCalls;
		return invoke(artifactProxy, value);
	});
	module.registerFunction<val<uintptr_t>()>("proxy_address", [] {
		++artifactWrapperCalls;
		return invoke(artifactProxyAddress);
	});
	module.registerFunction<val<int64_t>(val<int64_t*>, val<int64_t>)>("state",
	                                                                   [](val<int64_t*> state, val<int64_t> delta) {
		                                                                   ++artifactWrapperCalls;
		                                                                   *state = val<int64_t>(*state) + delta;
		                                                                   return val<int64_t>(*state);
	                                                                   });
	module.registerFunction<void(val<int64_t*>, val<int64_t>)>("store", [](val<int64_t*> state, val<int64_t> value) {
		++artifactWrapperCalls;
		*state = value;
	});
	module.registerFunction<val<int32_t>(val<int32_t*>, val<bool>)>("guarded", [](val<int32_t*> state, val<bool> flag) {
		++artifactWrapperCalls;
		ArtifactNativeGuard first(state, artifactFirstCleanup);
		ArtifactNativeGuard second(state, artifactSecondCleanup);
		return invoke(artifactMaybeThrow, state, flag);
	});
	module.registerFunction<val<int32_t>(val<int32_t*>, val<bool>)>(
	    "typed_owned", [](val<int32_t*> state, val<bool> flag) {
		    ++artifactWrapperCalls;
		    val<ArtifactTypedBuffer> empty;
		    val<ArtifactTypedBuffer> first(state, cacheLiteral<int32_t {1}>());
		    val<ArtifactTypedBuffer> copy(first);
		    val<ArtifactTypedBuffer> moved(std::move(copy));
		    return invoke(artifactUseTypedBuffer, &moved, flag);
	    });
	module.registerFunction<val<int32_t>(val<int32_t*>, val<bool>, val<int32_t (*)(ArtifactTypedBuffer*, bool)>)>(
	    "typed_callback",
	    [](val<int32_t*> state, val<bool> flag, val<int32_t (*)(ArtifactTypedBuffer*, bool)> callback) {
		    ++artifactWrapperCalls;
		    val<ArtifactTypedBuffer> first(state, cacheLiteral<int32_t {1}>());
		    val<ArtifactTypedBuffer> copy(first);
		    return callback(&copy, flag);
	    });
	module.registerFunction<void(val<int32_t*>, val<bool>, val<void (*)(ArtifactTypedBuffer*, bool)>)>(
	    "typed_callback_void",
	    [](val<int32_t*> state, val<bool> flag, val<void (*)(ArtifactTypedBuffer*, bool)> callback) {
		    ++artifactWrapperCalls;
		    val<ArtifactTypedBuffer> first(state, cacheLiteral<int32_t {1}>());
		    val<ArtifactTypedBuffer> copy(first);
		    callback(&copy, flag);
	    });
	return module.createArtifact();
}

const artifact::ExportDescriptor& findExport(const artifact::ModuleArtifact& value, const std::string& name) {
	const auto found = std::ranges::find(value.descriptor.exports, name, &artifact::ExportDescriptor::name);
	REQUIRE(found != value.descriptor.exports.end());
	return *found;
}

void requireExport(const artifact::ModuleArtifact& value, const std::string& name, Type result,
                   std::vector<Type> arguments) {
	const auto& exported = findExport(value, name);
	CAPTURE(name);
	REQUIRE(exported.returnType == result);
	REQUIRE(exported.argumentTypes == arguments);
	REQUIRE_FALSE(exported.loweredABI.empty());
}

void requireDigest(const std::string& digest) {
	REQUIRE(digest.size() == 64);
	REQUIRE(std::ranges::all_of(digest, [](char character) {
		return (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f') ||
		       (character >= 'A' && character <= 'F');
	}));
}

void checkMultiExportModule(CompiledModule& module) {
	REQUIRE(module.getExecutable() != nullptr);
	const auto increment = module.getFunction<int32_t(int32_t)>("increment");
	const auto sum = module.getFunction<int64_t(int64_t, int64_t)>("sum");
	const auto scaled = module.getFunction<double(double)>("scaled");
	const auto loop = module.getFunction<int32_t(int32_t)>("sum_loop");
	const auto boolean = module.getFunction<bool(bool)>("boolean");
	const auto signedByte = module.getFunction<int8_t(int8_t)>("signed_byte");
	const auto unsignedByte = module.getFunction<uint8_t(uint8_t)>("unsigned_byte");
	const auto signedWord = module.getFunction<int16_t(int16_t)>("signed_word");
	const auto unsignedWord = module.getFunction<uint16_t(uint16_t)>("unsigned_word");
	const auto narrow = module.getFunction<int32_t(bool, int8_t, uint16_t, int32_t)>("narrow_proxy");
	const auto proxy = module.getFunction<int64_t(int64_t)>("proxy");
	const auto proxyAddress = module.getFunction<uintptr_t()>("proxy_address");
	const auto state = module.getFunction<int64_t(int64_t*, int64_t)>("state");
	const auto store = module.getFunction<void(int64_t*, int64_t)>("store");
	const auto guarded = module.getFunction<int32_t(int32_t*, bool)>("guarded");
	REQUIRE(increment(-8) == -1);
	REQUIRE(increment(100) == 107);
	REQUIRE(sum(int64_t {1} << 40, -(int64_t {1} << 39)) == (int64_t {1} << 39));
	REQUIRE(scaled(2.0) == 3.25);
	REQUIRE(scaled(-4.0) == -5.75);
	REQUIRE(loop(0) == 0);
	REQUIRE(loop(10) == 45);
	REQUIRE(boolean(true));
	REQUIRE_FALSE(boolean(false));
	for (const auto value : {int8_t {-128}, int8_t {-1}, int8_t {0}, int8_t {127}}) {
		REQUIRE(signedByte(value) == value);
	}
	for (const auto value : {uint8_t {0}, uint8_t {128}, uint8_t {255}}) {
		REQUIRE(unsignedByte(value) == value);
	}
	for (const auto value : {int16_t {-32768}, int16_t {-1}, int16_t {32767}}) {
		REQUIRE(signedWord(value) == value);
	}
	for (const auto value : {uint16_t {0}, uint16_t {32768}, uint16_t {65535}}) {
		REQUIRE(unsignedWord(value) == value);
	}
	for (const bool flag : {false, true}) {
		REQUIRE(narrow(flag, int8_t {-128}, uint16_t {65535}, -19) ==
		        artifactNarrowProxy(flag, int8_t {-128}, uint16_t {65535}, -19));
	}
	REQUIRE(proxy(-9) == artifactProxy(-9));
	REQUIRE(proxy(17) == artifactProxy(17));
	REQUIRE(proxyAddress() == artifactProxyAddress());
	int64_t first = 10, second = -40;
	REQUIRE(state(&first, 5) == 15);
	REQUIRE(state(&second, 7) == -33);
	REQUIRE(state(&first, -2) == 13);
	REQUIRE(first == 13);
	REQUIRE(second == -33);
	store(&second, 900);
	REQUIRE(second == 900);
	int32_t cleanups[3] = {};
	REQUIRE(guarded(cleanups, false) == 1);
	REQUIRE(cleanups[0] == 21);
	REQUIRE(cleanups[1] == 2);
	REQUIRE(cleanups[2] == 1);
	cleanups[0] = cleanups[1] = 0;
	try {
		guarded(cleanups, true);
		FAIL("The loaded artifact did not propagate the native exception");
	} catch (const ArtifactFailure& failure) {
		REQUIRE(failure.value == 2);
		REQUIRE(std::string(failure.what()) == "artifact cleanup failure");
	}
	REQUIRE(cleanups[0] == 21);
	REQUIRE(cleanups[1] == 2);
	REQUIRE(cleanups[2] == 2);
	cleanups[0] = cleanups[1] = 0;
	REQUIRE(guarded(cleanups, false) == 3);
	REQUIRE(cleanups[0] == 21);
	REQUIRE(cleanups[1] == 2);
	REQUIRE(cleanups[2] == 3);
	const auto owned = module.getFunction<int32_t(int32_t*, bool)>("typed_owned");
	const auto callback =
	    module.getFunction<int32_t(int32_t*, bool, int32_t (*)(ArtifactTypedBuffer*, bool))>("typed_callback");
	const auto callbackVoid =
	    module.getFunction<void(int32_t*, bool, void (*)(ArtifactTypedBuffer*, bool))>("typed_callback_void");
	for (const std::string call : {"direct", "callback", "void callback"}) {
		for (const bool throwing : {false, true, false}) {
			CAPTURE(call, throwing);
			int32_t counts[4] = {};
			const auto execute = [&] {
				if (call == "direct") {
					return owned(counts, throwing);
				} else if (call == "callback") {
					return callback(counts, throwing, artifactUseTypedBuffer);
				} else {
					callbackVoid(counts, throwing, artifactUseTypedBufferVoid);
					return 2;
				}
			};
			if (throwing) {
				try {
					execute();
					FAIL("The typed owned artifact did not propagate its exception");
				} catch (const ArtifactFailure& failure) {
					REQUIRE(failure.value == 1);
				}
			} else {
				REQUIRE(execute() == 2);
			}
			REQUIRE(counts[0] == 21);
			REQUIRE(counts[1] == 2);
			REQUIRE(counts[2] == 2);
			REQUIRE(counts[3] == 1);
		}
	}
	REQUIRE_THROWS(module.getFunction<int32_t(int32_t)>("not_an_export")(1));
}

void requireRejected(const artifact::ModuleArtifact& value, const Options& options = {}) {
	const auto wrappers = artifactWrapperCalls;
	REQUIRE_THROWS(artifact::loadNative(value, options));
	REQUIRE_THROWS(artifact::loadBytecode(value, options));
	REQUIRE(artifactWrapperCalls == wrappers);
	REQUIRE_FALSE(tracing::inTracer());
}

struct ArtifactHooksScope {
	compiler::mlir::LLVMBackendHooks saved = compiler::mlir::getLLVMBackendHooks();
	~ArtifactHooksScope() {
		compiler::mlir::getLLVMBackendHooks() = std::move(saved);
	}
};

#ifdef __linux__
class ArtifactTempDirectory {
public:
	ArtifactTempDirectory() {
		auto pattern = (std::filesystem::temp_directory_path() / "nautilus-module-artifact-XXXXXX").string();
		if (const auto* directory = ::mkdtemp(pattern.data())) {
			path_ = directory;
		} else {
			throw std::system_error(errno, std::generic_category(), "mkdtemp");
		}
	}
	~ArtifactTempDirectory() {
		std::error_code ignored;
		std::filesystem::remove_all(path_, ignored);
	}
	const std::filesystem::path& path() const {
		return path_;
	}

private:
	std::filesystem::path path_;
};

void writeArtifactFile(const std::filesystem::path& path, std::string_view contents) {
	std::ofstream stream(path, std::ios::binary | std::ios::trunc);
	REQUIRE(stream.is_open());
	stream.write(contents.data(), static_cast<std::streamsize>(contents.size()));
	stream.close();
	REQUIRE_FALSE(stream.fail());
}

std::string readArtifactFile(const std::filesystem::path& path) {
	std::ifstream stream(path, std::ios::binary);
	REQUIRE(stream.is_open());
	std::string contents {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
	REQUIRE_FALSE(stream.bad());
	return contents;
}

constexpr const char* ARTIFACT_EXEC_TEST = "Module artifacts round trip through a fresh ASLR exec";

void execArtifactChild(const std::filesystem::path& directory, const std::string& mode) {
	const auto path = directory.string();
	const auto child = ::fork();
	REQUIRE(child >= 0);
	if (child == 0) {
		if (::setenv("NAUTILUS_MODULE_ARTIFACT_CHILD", path.c_str(), 1) != 0 ||
		    ::setenv("NAUTILUS_MODULE_ARTIFACT_MODE", mode.c_str(), 1) != 0 ||
		    ::setenv("TMPDIR", path.c_str(), 1) != 0 || ::chdir(path.c_str()) != 0) {
			::_exit(126);
		}
		::execl("/proc/self/exe", "nautilus-module-artifact-child", ARTIFACT_EXEC_TEST, "--reporter", "console",
		        static_cast<char*>(nullptr));
		::_exit(127);
	}
	int status = 0;
	pid_t waited;
	do {
		waited = ::waitpid(child, &status, 0);
	} while (waited < 0 && errno == EINTR);
	REQUIRE(waited == child);
	INFO("child wait status: " << status);
	REQUIRE(WIFEXITED(status));
	REQUIRE(WEXITSTATUS(status) == 0);
}
#endif

} // namespace

TEST_CASE("Module artifacts expose real descriptors and reload every export without retracing",
          "[artifact][mlir][B1]") {
	requireArtifactSupport();
	const auto before = artifactWrapperCalls;
	const auto original = multiExportArtifact();
	REQUIRE(artifactWrapperCalls > before);
	const auto wrappers = artifactWrapperCalls;
	REQUIRE_FALSE(original.object.empty());
	REQUIRE_FALSE(original.bytecode.empty());
	requireDigest(original.descriptorDigest);
	requireDigest(original.descriptor.objectDigest);
	requireDigest(original.descriptor.bytecodeDigest);
	REQUIRE(original.descriptor.version == 2);
	REQUIRE(original.descriptor.exports.size() == 18);
	REQUIRE_FALSE(original.descriptor.imports.empty());
	REQUIRE_FALSE(original.descriptor.moduleManifest.empty());
	const auto& compatibility = original.descriptor.compatibility;
	REQUIRE_FALSE(compatibility.compilerImage.buildId.empty());
	REQUIRE_FALSE(compatibility.producerImage.buildId.empty());
	REQUIRE_FALSE(compatibility.llvmVersion.empty());
	REQUIRE_FALSE(compatibility.targetTriple.empty());
	REQUIRE_FALSE(compatibility.cpu.empty());
	REQUIRE_FALSE(compatibility.dataLayout.empty());
	REQUIRE(compatibility.pointerSize > 0);
	requireDigest(compatibility.optionsDigest);
	REQUIRE(compatibility.littleEndian == (std::endian::native == std::endian::little));
	for (const auto& imported : original.descriptor.imports) {
		REQUIRE_FALSE(imported.symbol.empty());
		REQUIRE_FALSE(imported.image.buildId.empty());
		REQUIRE(imported.image.loadOffset > 0);
	}
	requireExport(original, "increment", Type::i32, {Type::i32});
	requireExport(original, "sum", Type::i64, {Type::i64, Type::i64});
	requireExport(original, "scaled", Type::f64, {Type::f64});
	requireExport(original, "sum_loop", Type::i32, {Type::i32});
	requireExport(original, "boolean", Type::b, {Type::b});
	requireExport(original, "signed_byte", Type::i8, {Type::i8});
	requireExport(original, "unsigned_byte", Type::ui8, {Type::ui8});
	requireExport(original, "signed_word", Type::i16, {Type::i16});
	requireExport(original, "unsigned_word", Type::ui16, {Type::ui16});
	requireExport(original, "narrow_proxy", Type::i32, {Type::b, Type::i8, Type::ui16, Type::i32});
	requireExport(original, "proxy", Type::i64, {Type::i64});
	requireExport(original, "proxy_address", tracing::TypeResolver<uintptr_t>::to_type(), {});
	requireExport(original, "state", Type::i64, {Type::ptr, Type::i64});
	requireExport(original, "store", Type::v, {Type::ptr, Type::i64});
	requireExport(original, "guarded", Type::i32, {Type::ptr, Type::b});
	for (const auto& [name, attribute] :
	     std::array<std::pair<std::string_view, std::string_view>, 5> {{{"boolean", "zeroext"},
	                                                                    {"signed_byte", "signext"},
	                                                                    {"unsigned_byte", "zeroext"},
	                                                                    {"signed_word", "signext"},
	                                                                    {"unsigned_word", "zeroext"}}}) {
		const auto& exported = findExport(original, std::string(name));
		auto abi = exported.loweredABI;
		for (const auto& [key, value] : exported.attributes) {
			abi += key + value;
		}
		CAPTURE(name, abi);
		REQUIRE(abi.find(attribute) != std::string::npos);
	}
	const auto encoded = artifact::encode(original);
	const auto decoded = artifact::decode(encoded);
	REQUIRE(decoded.descriptor == original.descriptor);
	REQUIRE(decoded.descriptorDigest == original.descriptorDigest);
	REQUIRE(decoded.object == original.object);
	REQUIRE(decoded.bytecode == original.bytecode);
	REQUIRE(artifact::encode(decoded) == encoded);
	for (const bool bytecode : {false, true}) {
		CAPTURE(bytecode);
		auto loaded = bytecode ? artifact::loadBytecode(decoded) : artifact::loadNative(decoded);
		checkMultiExportModule(loaded);
		REQUIRE(artifactWrapperCalls == wrappers);
		REQUIRE_FALSE(tracing::inTracer());
	}
}

TEST_CASE("Module artifact descriptors distinguish bodies exports imports signatures and options",
          "[artifact][mlir][B1]") {
	requireArtifactSupport();
	const auto baseline = scalarArtifact();
	const auto differentBody = scalarArtifact(artifactOptions(), 11);
	REQUIRE(differentBody.descriptor.objectDigest != baseline.descriptor.objectDigest);
	REQUIRE(differentBody.descriptor.bytecodeDigest != baseline.descriptor.bytecodeDigest);
	REQUIRE(differentBody.descriptorDigest != baseline.descriptorDigest);
	const auto differentName = scalarArtifact(artifactOptions(), 7, "renamed");
	REQUIRE(differentName.descriptor.exports != baseline.descriptor.exports);
	REQUIRE(differentName.descriptor.moduleManifest != baseline.descriptor.moduleManifest);
	REQUIRE(differentName.descriptorDigest != baseline.descriptorDigest);
	NautilusEngine engine(artifactOptions());
	auto wide = engine.createModule();
	wide.registerFunction<val<int64_t>(val<int64_t>)>(
	    "increment", [](val<int64_t> value) { return value + cacheLiteral<int64_t {7}>(); });
	const auto differentSignature = wide.createArtifact();
	REQUIRE(differentSignature.descriptor.exports != baseline.descriptor.exports);
	REQUIRE(differentSignature.descriptorDigest != baseline.descriptorDigest);
	const auto proxy = proxyArtifact();
	const auto otherProxy = proxyArtifact(true);
	REQUIRE(proxy.descriptor.imports != otherProxy.descriptor.imports);
	REQUIRE(proxy.descriptorDigest != otherProxy.descriptorDigest);
	Options options = artifactOptions();
	options.setOption("optimizationLevel", 0);
	const auto differentOptions = scalarArtifact(options);
	REQUIRE(differentOptions.descriptor.compatibility.optionsDigest != baseline.descriptor.compatibility.optionsDigest);
	REQUIRE(differentOptions.descriptorDigest != baseline.descriptorDigest);
	requireRejected(differentOptions);
	for (const bool bytecode : {false, true}) {
		auto loaded = bytecode ? artifact::loadBytecode(differentOptions, options)
		                       : artifact::loadNative(differentOptions, options);
		REQUIRE(loaded.getFunction<int32_t(int32_t)>("increment")(5) == 12);
	}
	Options incompatible = artifactOptions();
	incompatible.setOption("optimizationLevel", 1);
	requireRejected(baseline, incompatible);
}

TEST_CASE("Module artifacts require compiled tracing and nonempty unique export names", "[artifact][mlir][B1]") {
	requireArtifactSupport();
	Options interpretedOptions = artifactOptions();
	interpretedOptions.setOption("engine.Compilation", false);
	NautilusEngine interpreted(interpretedOptions);
	auto interpretedModule = interpreted.createModule();
	interpretedModule.registerFunction<val<int32_t>(val<int32_t>)>("identity",
	                                                               [](val<int32_t> value) { return value; });
	REQUIRE_THROWS(interpretedModule.createArtifact());
	NautilusEngine engine(artifactOptions());
	auto empty = engine.createModule();
	REQUIRE_THROWS(empty.createArtifact());
	for (const auto& name : {std::string {}, std::string("invalid\0name", 12)}) {
		auto invalid = engine.createModule();
		invalid.registerFunction<val<int32_t>(val<int32_t>)>(name, [](val<int32_t> value) { return value; });
		REQUIRE_THROWS(invalid.createArtifact());
		REQUIRE_FALSE(tracing::inTracer());
	}
	auto duplicate = engine.createModule();
	for (int i = 0; i < 2; ++i) {
		duplicate.registerFunction<val<int32_t>(val<int32_t>)>("identity", [](val<int32_t> value) { return value; });
	}
	REQUIRE_THROWS(duplicate.createArtifact());
	REQUIRE_FALSE(tracing::inTracer());
}

TEST_CASE("Module artifacts reject tampered descriptors and native import manifests", "[artifact][mlir][B1]") {
	requireArtifactSupport();
	const auto original = proxyArtifact();
	REQUIRE_FALSE(original.descriptor.imports.empty());
	const std::vector<std::pair<std::string, std::function<void(artifact::ModuleArtifact&)>>> changes {
	    {"descriptor digest", [](auto& value) { value.descriptorDigest[0] ^= 1; }},
	    {"version", [](auto& value) { ++value.descriptor.version; }},
	    {"compiler identity", [](auto& value) { value.descriptor.compatibility.compilerImage.buildId += "0"; }},
	    {"compiler offset", [](auto& value) { ++value.descriptor.compatibility.compilerImage.loadOffset; }},
	    {"producer identity", [](auto& value) { value.descriptor.compatibility.producerImage.buildId += "0"; }},
	    {"producer offset", [](auto& value) { ++value.descriptor.compatibility.producerImage.loadOffset; }},
	    {"LLVM version", [](auto& value) { value.descriptor.compatibility.llvmVersion += "-different"; }},
	    {"target", [](auto& value) { value.descriptor.compatibility.targetTriple += "-different"; }},
	    {"CPU", [](auto& value) { value.descriptor.compatibility.cpu += "-different"; }},
	    {"features", [](auto& value) { value.descriptor.compatibility.features += ",+different"; }},
	    {"layout", [](auto& value) { value.descriptor.compatibility.dataLayout += "-different"; }},
	    {"pointer width", [](auto& value) { ++value.descriptor.compatibility.pointerSize; }},
	    {"endianness", [](auto& value) { value.descriptor.compatibility.littleEndian ^= true; }},
	    {"options", [](auto& value) { value.descriptor.compatibility.optionsDigest[0] ^= 1; }},
	    {"extensions", [](auto& value) { value.descriptor.compatibility.extensionFingerprint += "different"; }},
	    {"export name", [](auto& value) { value.descriptor.exports.front().name += "different"; }},
	    {"export result", [](auto& value) { value.descriptor.exports.front().returnType = Type::f64; }},
	    {"export arguments", [](auto& value) { value.descriptor.exports.front().argumentTypes.push_back(Type::ptr); }},
	    {"lowered ABI", [](auto& value) { value.descriptor.exports.front().loweredABI += "different"; }},
	    {"calling convention", [](auto& value) { ++value.descriptor.exports.front().callingConvention; }},
	    {"attributes",
	     [](auto& value) { value.descriptor.exports.front().attributes.emplace_back("different", "true"); }},
	    {"module manifest", [](auto& value) { value.descriptor.moduleManifest += "different"; }},
	    {"object digest", [](auto& value) { value.descriptor.objectDigest[0] ^= 1; }},
	    {"bytecode digest", [](auto& value) { value.descriptor.bytecodeDigest[0] ^= 1; }},
	    {"missing import", [](auto& value) { value.descriptor.imports.clear(); }},
	    {"unresolved import image",
	     [](auto& value) {
		     auto& identity = value.descriptor.imports.front().image.buildId;
		     identity.front() = identity.front() == '0' ? '1' : '0';
	     }},
	    {"wrong import offset", [](auto& value) { ++value.descriptor.imports.front().image.loadOffset; }},
	    {"unmapped import offset",
	     [](auto& value) { value.descriptor.imports.front().image.loadOffset = std::numeric_limits<uint64_t>::max(); }},
	    {"renamed import", [](auto& value) { value.descriptor.imports.front().symbol += "different"; }},
	    {"bytecode import flag", [](auto& value) { value.descriptor.imports.front().bytecodeImport ^= true; }},
	    {"duplicate import", [](auto& value) { value.descriptor.imports.push_back(value.descriptor.imports.front()); }},
	};
	for (const auto& [name, change] : changes) {
		CAPTURE(name);
		auto changed = original;
		change(changed);
		requireRejected(changed);
	}
	auto loaded = artifact::loadNative(original);
	REQUIRE(loaded.getFunction<int64_t(int64_t)>("proxy")(8) == artifactProxy(8));
}

TEST_CASE("Module artifact codec and both payloads reject corruption and malformed input", "[artifact][mlir][B1]") {
	requireArtifactSupport();
	const auto original = scalarArtifact();
	const auto encoded = artifact::encode(original);
	for (const auto size :
	     {std::size_t {0}, std::size_t {1}, std::size_t {7}, encoded.size() / 2, encoded.size() - 1}) {
		CAPTURE(size);
		REQUIRE_THROWS(artifact::decode(std::string_view(encoded).substr(0, size)));
	}
	REQUIRE_THROWS(artifact::decode(encoded + "trailing bytes"));
	for (const auto& field : {original.descriptorDigest, original.object, original.bytecode}) {
		auto changed = encoded;
		const auto position = changed.find(field);
		REQUIRE(position != std::string::npos);
		changed[position] ^= 1;
		REQUIRE_THROWS(artifact::decode(changed));
	}
	auto badMagic = encoded;
	badMagic.front() ^= 1;
	REQUIRE_THROWS(artifact::decode(badMagic));
	for (const auto& bytes : {std::string(1024, '\0'), std::string(1024, '\xff'), std::string("not an artifact")}) {
		REQUIRE_THROWS(artifact::decode(bytes));
	}
	for (const bool bytecode : {false, true}) {
		for (const bool truncate : {false, true}) {
			CAPTURE(bytecode, truncate);
			auto changed = original;
			auto& bytes = bytecode ? changed.bytecode : changed.object;
			if (truncate) {
				bytes.resize(bytes.size() / 2);
			} else {
				bytes[bytes.size() / 2] ^= 1;
			}
			if (bytecode) {
				REQUIRE_THROWS(artifact::loadBytecode(changed));
				auto independent = artifact::loadNative(changed);
				REQUIRE(independent.getFunction<int32_t(int32_t)>("increment")(5) == 12);
			} else {
				REQUIRE_THROWS(artifact::loadNative(changed));
				auto independent = artifact::loadBytecode(changed);
				REQUIRE(independent.getFunction<int32_t(int32_t)>("increment")(5) == 12);
			}
		}
	}
	requireRejected(artifact::ModuleArtifact {});
	const auto decoded = artifact::decode(encoded);
	auto native = artifact::loadNative(decoded);
	auto bytecode = artifact::loadBytecode(decoded);
	REQUIRE(native.getFunction<int32_t(int32_t)>("increment")(5) == 12);
	REQUIRE(bytecode.getFunction<int32_t(int32_t)>("increment")(5) == 12);
	for (const bool useBytecode : {false, true}) {
		const auto detached = [&] {
			const auto value = artifact::decode(encoded);
			auto module = useBytecode ? artifact::loadBytecode(value) : artifact::loadNative(value);
			return module.getFunction<int32_t(int32_t)>("increment");
		}();
		REQUIRE(detached(-8) == -1);
	}
}

TEST_CASE("Module artifact emission audits unsafe values before every IR scheduling choice", "[artifact][mlir][B1]") {
	requireArtifactSupport();
	int64_t captured = 19;
	const auto address = reinterpret_cast<uintptr_t>(&captured);
	for (const std::string_view scheduling :
	     {"default", "passes disabled", "optimization disabled", "eight iterations"}) {
		for (const bool fold : {false, true}) {
			Options options = artifactOptions();
			options.setOption("engine.foldStaticConstants", fold);
			if (scheduling == "passes disabled") {
				options.setOption("ir.runPasses", false);
			} else if (scheduling == "optimization disabled") {
				options.setOption("ir.runOptimizationPasses", false);
			} else if (scheduling == "eight iterations") {
				options.setOption("ir.maxPipelineIterations", 8);
			}
			NautilusEngine engine(options);
			for (const std::string_view unsafe :
			     {"raw scalar", "raw boolean", "raw floating", "dead scalar", "raw captured pointer", "encoded address",
			      "folded zero", "destructor-only pointer"}) {
				CAPTURE(scheduling, fold, unsafe);
				auto module = engine.createModule();
				module.registerFunction<val<int64_t>(val<int64_t>, val<int64_t*>)>(
				    "unsafe", [unsafe, &captured, address](val<int64_t> value, val<int64_t*> pointer) -> val<int64_t> {
					    ++artifactWrapperCalls;
					    if (unsafe == "raw scalar") {
						    return value + int64_t {7};
					    }
					    if (unsafe == "raw boolean") {
						    return select(val<bool>(true), value, value);
					    }
					    if (unsafe == "raw floating") {
						    return value + static_cast<val<int64_t>>(val<double>(1.25));
					    }
					    if (unsafe == "dead scalar") {
						    return select(cacheLiteral<true>(), value, val<int64_t>(7));
					    }
					    if (unsafe == "raw captured pointer") {
						    return val<int64_t>(*val<int64_t*>(&captured));
					    }
					    if (unsafe == "encoded address") {
						    return static_cast<val<int64_t>>(val<uintptr_t>(address));
					    }
					    if (unsafe == "folded zero") {
						    return val<int64_t>(*(pointer + 0));
					    }
					    val<int64_t*> cleanupOnly(&captured);
					    tracing::registerDestructor(cleanupOnly.state, reinterpret_cast<void*>(artifactScalarCleanup));
					    auto result = invoke(artifactCheckedProxy, value);
					    tracing::unregisterDestructor(cleanupOnly.state);
					    return result;
				    });
				const auto reason = unsafe == "raw captured pointer" || unsafe == "destructor-only pointer"
				                        ? "embedded_non_null_pointer"
				                        : "uncertified_scalar";
				REQUIRE_THROWS_MATCHES(module.createArtifact(), std::exception,
				                       Catch::Matchers::MessageMatches(Catch::Matchers::ContainsSubstring(reason)));
				REQUIRE_FALSE(tracing::inTracer());
			}
			auto safe = engine.createModule();
			safe.registerFunction<val<int64_t>(val<int64_t*>)>("safe", [](val<int64_t*> pointer) {
				return val<int64_t>(*(pointer + cacheLiteral<std::size_t {0}>())) + cacheLiteral<int64_t {7}>();
			});
			const auto recovered = safe.createArtifact();
			for (const bool bytecode : {false, true}) {
				auto loaded =
				    bytecode ? artifact::loadBytecode(recovered, options) : artifact::loadNative(recovered, options);
				REQUIRE(loaded.getFunction<int64_t(int64_t*)>("safe")(&captured) == 26);
				REQUIRE_FALSE(tracing::inTracer());
			}
		}
	}
}

TEST_CASE("Module artifact emission rejects uncacheable LLVM hooks without poisoning later tests",
          "[artifact][mlir][B1]") {
	requireArtifactSupport();
	const auto original = proxyArtifact();
	for (const std::string_view hook : {"pre optimization", "symbol contributor", "call naming"}) {
		CAPTURE(hook);
		{
			ArtifactHooksScope restore;
			auto& hooks = compiler::mlir::getLLVMBackendHooks();
			if (hook == "pre optimization") {
				hooks.preOptModuleTransform = [](llvm::Module&) {
					throw std::runtime_error("uncacheable optimizer hook");
				};
			} else if (hook == "symbol contributor") {
				hooks.jitSymbolContributor = [](const compiler::mlir::SymbolContributor&) {
					throw std::runtime_error("uncacheable symbol hook");
				};
			} else {
				hooks.callNameOverride = [](void*) -> std::optional<std::string> {
					throw std::runtime_error("uncacheable call naming hook");
				};
			}
			REQUIRE_THROWS(proxyArtifact());
			requireRejected(original);
		}
		REQUIRE(artifact::isSupported());
		auto loaded = artifact::loadNative(original);
		REQUIRE(loaded.getFunction<int64_t(int64_t)>("proxy")(8) == artifactProxy(8));
	}
}

TEST_CASE("Module artifact native loading never reads missing or invalid bytecode", "[artifact][mlir][B1]") {
	requireArtifactSupport();
	const auto original = scalarArtifact();
	const auto wrappers = artifactWrapperCalls;
	for (const auto& bytecode : {std::string {}, std::string("not MLIR bytecode"), std::string(1024, '\xff')}) {
		auto detached = original;
		detached.bytecode = bytecode;
		auto loaded = artifact::loadNative(detached);
		REQUIRE(loaded.getFunction<int32_t(int32_t)>("increment")(5) == 12);
		REQUIRE_THROWS(artifact::loadBytecode(detached));
		REQUIRE(artifactWrapperCalls == wrappers);
	}
}

TEST_CASE("Typed allocation evidence never certifies raw scalars callbacks or destructor-only addresses",
          "[artifact][mlir][B1][allocation-origin]") {
	requireArtifactSupport();
	int32_t captured[4] = {};
	for (const std::string kind : {"raw scalar", "encoded address", "callback", "destructor only", "raw metadata"}) {
		for (const bool passes : {false, true}) {
			CAPTURE(kind, passes);
			Options options = artifactOptions();
			options.setOption("ir.runPasses", passes);
			NautilusEngine engine(options);
			auto builder = engine.createModule();
			builder.registerFunction<val<int32_t>(val<int32_t*>)>("unsafe", [&](val<int32_t*> runtime) {
				val<ArtifactTypedBuffer> empty;
				if (kind == "raw scalar") {
					val<ArtifactTypedBuffer> object(runtime, int32_t {1});
				} else if (kind == "raw metadata") {
					tracing::traceAlloca(sizeof(ArtifactTypedBuffer), alignof(ArtifactTypedBuffer));
				} else {
					val<uintptr_t> encoded(reinterpret_cast<uintptr_t>(captured));
					auto address = static_cast<val<int32_t*>>(encoded);
					if (kind == "destructor only") {
						ArtifactNativeGuard guard(address, artifactFirstCleanup);
						return invoke(artifactMaybeThrow, runtime, cacheLiteral<bool {false}>());
					} else if (kind == "callback") {
						invoke(artifactFirstCleanup, address);
					} else {
						val<ArtifactTypedBuffer> object(address, cacheLiteral<int32_t {1}>());
					}
				}
				return cacheLiteral<int32_t {0}>();
			});
			REQUIRE_THROWS_WITH(builder.createArtifact(),
			                    Catch::Matchers::ContainsSubstring(kind == "raw metadata"
			                                                           ? "allocation_metadata_origins_unavailable"
			                                                           : "uncertified_scalar"));
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE("Module artifact compatibility probes and producers have stable identities", "[artifact][mlir][B1]") {
	requireArtifactSupport();
	const auto first = scalarArtifact();
	for (int iteration = 0; iteration < 3; ++iteration) {
		REQUIRE(artifact::isSupported());
		const auto second = scalarArtifact();
		REQUIRE(first.descriptor.compatibility == second.descriptor.compatibility);
		auto loaded = artifact::loadNative(first);
		REQUIRE(loaded.getFunction<int32_t(int32_t)>("increment")(iteration) == iteration + 7);
	}
}

TEST_CASE("Module artifact exception cleanup is mandatory when optional passes are disabled", "[artifact][mlir][B1]") {
	requireArtifactSupport();
	for (const bool passes : {false, true}) {
		Options options = artifactOptions();
		options.setOption("ir.runPasses", passes);
		const auto original = multiExportArtifact(options);
		const auto wrappers = artifactWrapperCalls;
		for (const bool bytecode : {false, true}) {
			auto loaded =
			    bytecode ? artifact::loadBytecode(original, options) : artifact::loadNative(original, options);
			checkMultiExportModule(loaded);
			REQUIRE(artifactWrapperCalls == wrappers);
		}
	}
}

#ifdef __linux__
extern "C" int32_t nautilusArtifactUnidentifiedProxy(int32_t value);
extern "C" int32_t nautilusArtifactIdentifiedProxy(int32_t value);
TEST_CASE("Module artifact imports require an identified executable image", "[artifact][mlir][B1]") {
	requireArtifactSupport();
	NautilusEngine engine(artifactOptions());
	auto module = engine.createModule();
	module.registerFunction<val<int32_t>(val<int32_t>)>(
	    "identity", [](val<int32_t> value) { return invoke(nautilusArtifactUnidentifiedProxy, value); });
	REQUIRE_THROWS(module.createArtifact());
	REQUIRE_FALSE(tracing::inTracer());
}

TEST_CASE("Module artifact imports reject ambiguous identified native images", "[artifact][mlir][B1]") {
	requireArtifactSupport();
	NautilusEngine engine(artifactOptions());
	auto builder = engine.createModule();
	builder.registerFunction<val<int32_t>(val<int32_t>)>(
	    "imported", [](val<int32_t> value) { return invoke(nautilusArtifactIdentifiedProxy, value); });
	const auto original = builder.createArtifact();
	ArtifactTempDirectory temporary;
	const auto duplicate = temporary.path() / "duplicate.so";
	std::filesystem::copy_file(ARTIFACT_IDENTIFIED_PROXY_PATH, duplicate);
	{
		const auto handle = std::shared_ptr<void>(::dlopen(duplicate.c_str(), RTLD_NOW | RTLD_LOCAL),
		                                          [](void* value) { ::dlclose(value); });
		REQUIRE(handle != nullptr);
		requireRejected(original);
		REQUIRE_THROWS(builder.createArtifact());
	}
	auto recovered = artifact::loadNative(original);
	REQUIRE(recovered.getFunction<int32_t(int32_t)>("imported")(7) == 9);
}

TEST_CASE("Module artifacts round trip through a fresh ASLR exec", "[artifact][mlir][B1][artifact-exec]") {
	requireArtifactSupport();
	if (const auto* directory = std::getenv("NAUTILUS_MODULE_ARTIFACT_CHILD")) {
		const auto* mode = std::getenv("NAUTILUS_MODULE_ARTIFACT_MODE");
		REQUIRE(mode != nullptr);
		REQUIRE(artifactWrapperCalls == 0);
		const auto original = artifact::decode(readArtifactFile(std::filesystem::path(directory) / "module.artifact"));
		auto loaded =
		    std::string_view(mode) == "native" ? artifact::loadNative(original) : artifact::loadBytecode(original);
		checkMultiExportModule(loaded);
		REQUIRE(artifactWrapperCalls == 0);
		REQUIRE_FALSE(tracing::inTracer());
		const auto report = std::to_string(artifactProxyAddress()) + "\n" + original.descriptorDigest + "\n";
		writeArtifactFile(std::filesystem::path(directory) / (std::string(mode) + ".report"), report);
		return;
	}
	ArtifactTempDirectory temporary;
	const auto original = multiExportArtifact();
	writeArtifactFile(temporary.path() / "module.artifact", artifact::encode(original));
	const auto wrappers = artifactWrapperCalls;
	for (const std::string mode : {"native", "bytecode"}) {
		CAPTURE(mode);
		execArtifactChild(temporary.path(), mode);
		const auto report = readArtifactFile(temporary.path() / (mode + ".report"));
		const auto separator = report.find('\n');
		REQUIRE(separator != std::string::npos);
		const auto childAddress = std::stoull(report.substr(0, separator));
		std::cout << "artifact ASLR " << mode << " parent=" << artifactProxyAddress() << " child=" << childAddress
		          << " wrappers=" << artifactWrapperCalls << '\n';
		REQUIRE(childAddress != 0);
		REQUIRE(childAddress != artifactProxyAddress());
		REQUIRE(report.substr(separator + 1) == original.descriptorDigest + "\n");
		REQUIRE(artifactWrapperCalls == wrappers);
	}
}
#endif

} // namespace nautilus::engine

#endif
