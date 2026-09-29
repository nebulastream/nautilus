#include "nautilus/compiler/cache/PersistentModuleCache.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/passes/CacheSafetyAnalysis.hpp"
#include "nautilus/options.hpp"
#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cerrno>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <llvm/Config/llvm-config.h>
#include <llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/SHA256.h>
#include <llvm/Target/TargetMachine.h>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#ifdef __linux__
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace nautilus::compiler {
namespace {

constexpr std::string_view CACHE_MAGIC = "NMCACHE";
constexpr uint64_t MAX_MANIFEST_SIZE = uint64_t {64} << 20U;
constexpr uint64_t MAX_ARTIFACT_SIZE = uint64_t {1} << 30U;

class CacheFailure final : public std::runtime_error {
public:
	explicit CacheFailure(std::string message) : std::runtime_error(std::move(message)) {
	}
};

class Writer {
public:
	void u8(uint8_t value) {
		data_.push_back(static_cast<char>(value));
	}

	void u32(uint32_t value) {
		for (uint32_t shift = 0; shift < 32; shift += 8) {
			u8(static_cast<uint8_t>(value >> shift));
		}
	}

	void u64(uint64_t value) {
		for (uint32_t shift = 0; shift < 64; shift += 8) {
			u8(static_cast<uint8_t>(value >> shift));
		}
	}

	void bytes(std::span<const uint8_t> value) {
		data_.append(reinterpret_cast<const char*>(value.data()), value.size());
	}

	void string(std::string_view value) {
		u64(value.size());
		data_.append(value.data(), value.size());
	}

	std::string take() {
		return std::move(data_);
	}

private:
	std::string data_;
};

class Reader {
public:
	explicit Reader(std::string_view data) : data_(data) {
	}

	uint8_t u8() {
		require(1);
		return static_cast<uint8_t>(data_[position_++]);
	}

	uint32_t u32() {
		uint32_t value = 0;
		for (uint32_t shift = 0; shift < 32; shift += 8) {
			value |= static_cast<uint32_t>(u8()) << shift;
		}
		return value;
	}

	uint64_t u64() {
		uint64_t value = 0;
		for (uint32_t shift = 0; shift < 64; shift += 8) {
			value |= static_cast<uint64_t>(u8()) << shift;
		}
		return value;
	}

	std::string string() {
		const auto length = u64();
		if (length > MAX_ARTIFACT_SIZE || length > remaining()) {
			throw CacheFailure("invalid cache string length");
		}
		std::string value(data_.substr(position_, static_cast<std::size_t>(length)));
		position_ += static_cast<std::size_t>(length);
		return value;
	}

	std::span<const uint8_t> bytes(std::size_t length) {
		require(length);
		const auto* begin = reinterpret_cast<const uint8_t*>(data_.data() + position_);
		position_ += length;
		return {begin, length};
	}

	void finish() const {
		if (position_ != data_.size()) {
			throw CacheFailure("trailing cache bytes");
		}
	}

private:
	std::size_t remaining() const {
		return data_.size() - position_;
	}

	void require(std::size_t length) const {
		if (length > remaining()) {
			throw CacheFailure("truncated cache record");
		}
	}

	std::string_view data_;
	std::size_t position_ = 0;
};

std::array<uint8_t, 32> digestBytes(std::string_view value) {
	llvm::SHA256 sha;
	sha.update(llvm::StringRef(value.data(), value.size()));
	return sha.final();
}

std::string digest(std::string_view value) {
	const auto bytes = digestBytes(value);
	static constexpr char digits[] = "0123456789abcdef";
	std::string result;
	result.reserve(bytes.size() * 2U);
	for (const auto byte : bytes) {
		result.push_back(digits[byte >> 4U]);
		result.push_back(digits[byte & 0x0fU]);
	}
	return result;
}

bool isDigest(std::string_view value) {
	return value.size() == 64 && std::ranges::all_of(value, [](char character) {
		       return (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f');
	       });
}

void writeOptionValue(Writer& writer, const engine::OptionValue& value) {
	std::visit(
	    [&](const auto& typedValue) {
		    using T = std::decay_t<decltype(typedValue)>;
		    if constexpr (std::is_same_v<T, int>) {
			    writer.u8(1);
			    writer.u64(static_cast<uint64_t>(static_cast<int64_t>(typedValue)));
		    } else if constexpr (std::is_same_v<T, double>) {
			    writer.u8(2);
			    writer.u64(std::bit_cast<uint64_t>(typedValue));
		    } else if constexpr (std::is_same_v<T, std::string>) {
			    writer.u8(3);
			    writer.string(typedValue);
		    } else if constexpr (std::is_same_v<T, bool>) {
			    writer.u8(4);
			    writer.u8(typedValue ? 1 : 0);
		    }
	    },
	    value);
}

std::optional<std::string> createKeyManifest(const std::list<CompilableFunction>& functions,
                                             const engine::ModuleOptions& options, std::string_view explicitKey,
                                             std::string_view compilerIdentity, std::string_view intrinsicFingerprint) {
	if (functions.empty() || explicitKey.empty() || options.getOptionOrDefault("mlir.inline_invoke_calls", false)) {
		return std::nullopt;
	}

	std::vector<const CompilableFunction*> canonicalFunctions;
	canonicalFunctions.reserve(functions.size());
	for (const auto& function : functions) {
		if (function.getName().empty() || !function.getSignature()) {
			return std::nullopt;
		}
		canonicalFunctions.push_back(&function);
	}
	std::ranges::sort(canonicalFunctions, {}, [](const auto* function) { return function->getName(); });
	for (std::size_t index = 1; index < canonicalFunctions.size(); ++index) {
		if (canonicalFunctions[index - 1]->getName() == canonicalFunctions[index]->getName()) {
			return std::nullopt;
		}
	}

	auto targetBuilder = llvm::orc::JITTargetMachineBuilder::detectHost();
	if (!targetBuilder) {
		(void) llvm::toString(targetBuilder.takeError());
		return std::nullopt;
	}
	auto targetMachine = targetBuilder->createTargetMachine();
	if (!targetMachine) {
		(void) llvm::toString(targetMachine.takeError());
		return std::nullopt;
	}

	Writer writer;
	writer.string("nautilus.persistent.module-cache.jitlink.2");
	writer.string(compilerIdentity);
	writer.string(intrinsicFingerprint);
	writer.string(options.getRuntimeBindings().schema());
	writer.string("jit-target-machine-defaults");
	writer.string(explicitKey);
	writer.string("mlir");
	writer.string(LLVM_VERSION_STRING);
	writer.string(targetBuilder->getTargetTriple().str());
	writer.string(targetBuilder->getCPU());
	writer.string(targetBuilder->getFeatures().getString());
	writer.string((*targetMachine)->createDataLayout().getStringRepresentation());
	writer.string(options.getOptionOrDefault("debug", false) ? "codegen-less" : "codegen-aggressive");
	writer.u64(sizeof(void*));
	writer.u8(std::endian::native == std::endian::little ? 1 : 2);
	writer.u64(__cplusplus);
#ifdef __clang_version__
	writer.string(__clang_version__);
#elif defined(__GNUC__)
	writer.string(__VERSION__);
#else
	writer.string("unknown-cxx-compiler");
#endif
#ifdef NDEBUG
	writer.string("release");
#else
	writer.string("debug");
#endif

	writer.u32(static_cast<uint32_t>(canonicalFunctions.size()));
	for (const auto* function : canonicalFunctions) {
		writer.string(function->getName());
		writer.u8(static_cast<uint8_t>(function->getSignature()->returnType));
		writer.u32(static_cast<uint32_t>(function->getSignature()->argumentTypes.size()));
		for (const auto type : function->getSignature()->argumentTypes) {
			writer.u8(static_cast<uint8_t>(type));
		}
		std::vector<std::pair<std::string, std::string>> attributes(function->getAttributes().begin(),
		                                                            function->getAttributes().end());
		std::ranges::sort(attributes);
		writer.u32(static_cast<uint32_t>(attributes.size()));
		for (const auto& [name, value] : attributes) {
			writer.string(name);
			writer.string(value);
		}
	}

	std::vector<std::pair<std::string, engine::OptionValue>> optionValues;
	for (const auto& [name, value] : options.getOptionValues()) {
		if (name == "engine.Blob.CacheDir" || name == "engine.Blob.CacheKey") {
			continue;
		}
		optionValues.emplace_back(name, value);
	}
	std::ranges::sort(optionValues, {}, [](const auto& entry) { return entry.first; });
	writer.u32(static_cast<uint32_t>(optionValues.size()));
	for (const auto& [name, value] : optionValues) {
		writer.string(name);
		writeOptionValue(writer, value);
	}
	return writer.take();
}

std::vector<std::string> exportNames(const std::list<CompilableFunction>& functions) {
	std::vector<std::string> names;
	names.reserve(functions.size());
	for (const auto& function : functions) {
		names.push_back(function.getName());
	}
	std::ranges::sort(names);
	return names;
}

struct ImportRecord {
	std::string symbol;
	common::ExecutableImageLocation image;

	bool operator==(const ImportRecord&) const = default;
};

struct Manifest {
	std::string bindingSchema;
	std::string keyManifest;
	std::string moduleManifest;
	std::string objectDigest;
	std::string bytecodeDigest;
	std::vector<ImportRecord> imports;
};

std::vector<ImportRecord> createImportRecords(const mlir::MLIRCacheArtifacts& artifacts,
                                              const RuntimeBindings& bindings) {
	std::unordered_map<std::string, void*> bindingAddresses;
	for (const auto& [identity, entry] : bindings.entries()) {
		bindingAddresses.emplace(entry->symbol, entry->address);
	}
	if (artifacts.externalSymbols.size() != artifacts.externalAddresses.size()) {
		throw CacheFailure("external symbol vector mismatch");
	}
	std::vector<ImportRecord> imports;
	imports.reserve(artifacts.externalSymbols.size());
	for (std::size_t index = 0; index < artifacts.externalSymbols.size(); ++index) {
		if (artifacts.externalSymbols[index].empty() || artifacts.externalAddresses[index] == nullptr) {
			throw CacheFailure("invalid external symbol");
		}
		if (auto binding = bindingAddresses.find(artifacts.externalSymbols[index]); binding != bindingAddresses.end()) {
			if (binding->second != artifacts.externalAddresses[index]) {
				throw CacheFailure("runtime binding address mismatch");
			}
			continue;
		}
		auto image = common::locateExecutableAddress(artifacts.externalAddresses[index]);
		if (!image) {
			throw CacheFailure("external symbol is not in a relocatable executable image: " +
			                   artifacts.externalSymbols[index]);
		}
		imports.push_back(ImportRecord {artifacts.externalSymbols[index], std::move(*image)});
	}
	std::ranges::sort(imports, {}, [](const auto& import) { return import.symbol; });
	for (std::size_t index = 1; index < imports.size(); ++index) {
		if (imports[index - 1].symbol == imports[index].symbol) {
			if (imports[index - 1].image != imports[index].image) {
				throw CacheFailure("external symbol has conflicting addresses");
			}
			imports.erase(imports.begin() + static_cast<std::ptrdiff_t>(index));
			--index;
		}
	}
	return imports;
}

std::string encodeManifest(const Manifest& manifest) {
	Writer payload;
	payload.string(manifest.bindingSchema);
	payload.string(manifest.keyManifest);
	payload.string(manifest.moduleManifest);
	payload.string(manifest.objectDigest);
	payload.string(manifest.bytecodeDigest);
	payload.u8(1);
	payload.u32(static_cast<uint32_t>(manifest.imports.size()));
	for (const auto& import : manifest.imports) {
		payload.string(import.symbol);
		payload.string(import.image.buildId);
		payload.u64(import.image.loadOffset);
	}
	auto payloadBytes = payload.take();

	Writer envelope;
	envelope.bytes({reinterpret_cast<const uint8_t*>(CACHE_MAGIC.data()), CACHE_MAGIC.size()});
	envelope.u64(payloadBytes.size());
	const auto checksum = digestBytes(payloadBytes);
	envelope.bytes(checksum);
	envelope.bytes({reinterpret_cast<const uint8_t*>(payloadBytes.data()), payloadBytes.size()});
	return envelope.take();
}

Manifest decodeManifest(std::string_view envelopeBytes, std::string_view expectedKeyManifest,
                        std::string_view expectedBindingSchema) {
	Reader envelope(envelopeBytes);
	const auto magic = envelope.bytes(CACHE_MAGIC.size());
	if (!std::equal(magic.begin(), magic.end(), reinterpret_cast<const uint8_t*>(CACHE_MAGIC.data()))) {
		throw CacheFailure("invalid cache manifest magic");
	}
	const auto payloadLength = envelope.u64();
	if (payloadLength > MAX_MANIFEST_SIZE) {
		throw CacheFailure("invalid cache manifest size");
	}
	const auto checksum = envelope.bytes(32);
	const auto payload = envelope.bytes(static_cast<std::size_t>(payloadLength));
	envelope.finish();
	const std::string_view payloadView(reinterpret_cast<const char*>(payload.data()), payload.size());
	const auto calculatedChecksum = digestBytes(payloadView);
	if (!std::equal(checksum.begin(), checksum.end(), calculatedChecksum.begin())) {
		throw CacheFailure("cache manifest checksum mismatch");
	}

	Reader reader(payloadView);
	Manifest manifest;
	manifest.bindingSchema = reader.string();
	if (manifest.bindingSchema != expectedBindingSchema) {
		throw CacheFailure("runtime binding schema mismatch");
	}
	manifest.keyManifest = reader.string();
	manifest.moduleManifest = reader.string();
	manifest.objectDigest = reader.string();
	manifest.bytecodeDigest = reader.string();
	if (reader.u8() != 1 || manifest.keyManifest != expectedKeyManifest || manifest.moduleManifest.empty() ||
	    (!manifest.objectDigest.empty() && !isDigest(manifest.objectDigest)) || !isDigest(manifest.bytecodeDigest)) {
		throw CacheFailure("cache manifest is incompatible");
	}
	const auto importCount = reader.u32();
	if (importCount > 100000) {
		throw CacheFailure("invalid cache import count");
	}
	manifest.imports.reserve(importCount);
	std::unordered_set<std::string> symbols;
	for (uint32_t index = 0; index < importCount; ++index) {
		ImportRecord import;
		import.symbol = reader.string();
		import.image.buildId = reader.string();
		import.image.loadOffset = reader.u64();
		if (import.symbol.empty() || import.image.buildId.empty() || !symbols.insert(import.symbol).second) {
			throw CacheFailure("invalid cache import");
		}
		manifest.imports.push_back(std::move(import));
	}
	reader.finish();
	return manifest;
}

std::optional<std::string> readFile(const std::filesystem::path& path, uint64_t maximumSize) {
	std::ifstream input(path, std::ios::binary | std::ios::ate);
	if (!input) {
		std::error_code error;
		if (!std::filesystem::exists(path, error) && !error) {
			return std::nullopt;
		}
		throw CacheFailure("cache artifact read failed");
	}
	const auto end = input.tellg();
	if (end < 0 || static_cast<uint64_t>(end) > maximumSize) {
		throw CacheFailure("invalid cache artifact size");
	}
	std::string bytes(static_cast<std::size_t>(end), '\0');
	input.seekg(0);
	if (!bytes.empty()) {
		input.read(bytes.data(), static_cast<std::streamsize>(bytes.size()));
	}
	if (!input) {
		throw CacheFailure("cache artifact read failed");
	}
	return bytes;
}

#ifdef __linux__
void writeAll(int descriptor, std::string_view data) {
	std::size_t written = 0;
	while (written < data.size()) {
		const auto result = ::write(descriptor, data.data() + written, data.size() - written);
		if (result < 0 && errno == EINTR) {
			continue;
		}
		if (result <= 0) {
			throw CacheFailure("cache artifact write failed");
		}
		written += static_cast<std::size_t>(result);
	}
}

class OwnedDescriptor {
public:
	explicit OwnedDescriptor(int descriptor) : descriptor_(descriptor) {
	}
	~OwnedDescriptor() {
		if (descriptor_ >= 0) {
			::close(descriptor_);
		}
	}
	OwnedDescriptor(const OwnedDescriptor&) = delete;
	OwnedDescriptor& operator=(const OwnedDescriptor&) = delete;
	int get() const {
		return descriptor_;
	}
	int release() {
		return std::exchange(descriptor_, -1);
	}

private:
	int descriptor_;
};

void syncDirectory(const std::filesystem::path& directory) {
	int flags = O_RDONLY | O_CLOEXEC;
#ifdef O_DIRECTORY
	flags |= O_DIRECTORY;
#endif
	OwnedDescriptor descriptor(::open(directory.c_str(), flags));
	if (descriptor.get() < 0 || ::fsync(descriptor.get()) != 0) {
		throw CacheFailure("cache directory sync failed");
	}
}
#endif

void atomicWrite(const std::filesystem::path& target, std::string_view bytes) {
	std::error_code error;
	std::filesystem::create_directories(target.parent_path(), error);
	if (error) {
		throw CacheFailure("cache directory creation failed");
	}
#ifdef __linux__
	auto temporaryTemplate = target.string() + ".tmp.XXXXXX";
	std::vector<char> temporaryBuffer(temporaryTemplate.begin(), temporaryTemplate.end());
	temporaryBuffer.push_back('\0');
	OwnedDescriptor descriptor(::mkstemp(temporaryBuffer.data()));
	const std::filesystem::path temporary(temporaryBuffer.data());
	if (descriptor.get() < 0 || ::fcntl(descriptor.get(), F_SETFD, FD_CLOEXEC) != 0 ||
	    ::fchmod(descriptor.get(), S_IRUSR | S_IWUSR) != 0) {
		::unlink(temporary.c_str());
		throw CacheFailure("temporary cache artifact creation failed");
	}
	try {
		writeAll(descriptor.get(), bytes);
		if (::fsync(descriptor.get()) != 0) {
			throw CacheFailure("cache artifact sync failed");
		}
		const int rawDescriptor = descriptor.release();
		if (::close(rawDescriptor) != 0) {
			throw CacheFailure("cache artifact close failed");
		}
		if (::rename(temporary.c_str(), target.c_str()) != 0) {
			throw CacheFailure("cache artifact publication failed");
		}
		syncDirectory(target.parent_path());
	} catch (...) {
		::unlink(temporary.c_str());
		throw;
	}
#else
	const auto suffix = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + "." +
	                    std::to_string(std::hash<std::thread::id> {}(std::this_thread::get_id()));
	const auto temporary = std::filesystem::path(target.string() + ".tmp." + suffix);
	try {
		std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
		if (!output) {
			throw CacheFailure("temporary cache artifact creation failed");
		}
		output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
		output.close();
		if (!output) {
			throw CacheFailure("cache artifact write failed");
		}
		std::filesystem::rename(temporary, target, error);
		if (error) {
			throw CacheFailure("cache artifact publication failed");
		}
	} catch (...) {
		std::filesystem::remove(temporary, error);
		throw;
	}
#endif
}

std::shared_ptr<std::mutex> mutexForKey(const std::string& key) {
	static std::mutex mapMutex;
	static std::unordered_map<std::string, std::weak_ptr<std::mutex>> mutexes;
	std::lock_guard guard(mapMutex);
	if (auto existing = mutexes[key].lock()) {
		return existing;
	}
	auto created = std::make_shared<std::mutex>();
	mutexes[key] = created;
	return created;
}

struct CachePaths {
	std::filesystem::path object;
	std::filesystem::path bytecode;
	std::filesystem::path manifest;
};

CachePaths getPaths(const std::filesystem::path& directory, std::string_view keyDigest) {
	const auto stem = directory / std::string(keyDigest);
	return CachePaths {stem.string() + ".o", stem.string() + ".mlirbc", stem.string() + ".manifest"};
}

std::optional<Manifest> readManifest(const CachePaths& paths, std::string_view keyManifest,
                                     std::string_view bindingSchema) {
	auto bytes = readFile(paths.manifest, MAX_MANIFEST_SIZE);
	if (!bytes) {
		return std::nullopt;
	}
	return decodeManifest(*bytes, keyManifest, bindingSchema);
}

std::string readCheckedArtifact(const std::filesystem::path& path, std::string_view expectedDigest) {
	auto bytes = readFile(path, MAX_ARTIFACT_SIZE);
	if (!bytes || digest(*bytes) != expectedDigest) {
		throw CacheFailure("cache artifact checksum mismatch");
	}
	return std::move(*bytes);
}

bool resolveImports(const std::vector<ImportRecord>& imports, const RuntimeBindings& bindings,
                    std::vector<std::string>& symbols, std::vector<void*>& addresses) {
	symbols.clear();
	addresses.clear();
	symbols.reserve(imports.size());
	addresses.reserve(imports.size());
	for (const auto& import : imports) {
		auto* address = common::resolveExecutableAddress(import.image);
		if (address == nullptr) {
			return false;
		}
		symbols.push_back(import.symbol);
		addresses.push_back(address);
	}
	for (const auto& [identity, entry] : bindings.entries()) {
		if (std::ranges::find(symbols, entry->symbol) != symbols.end()) {
			return false;
		}
		symbols.push_back(entry->symbol);
		addresses.push_back(entry->address);
	}
	return true;
}

const mlir::MLIRCompilationBackend& getMLIRBackend() {
	const auto* backend = CompilationBackendRegistry::getInstance()->getBackend("mlir");
	const auto* typedBackend = dynamic_cast<const mlir::MLIRCompilationBackend*>(backend);
	if (typedBackend == nullptr) {
		throw CacheFailure("MLIR backend is unavailable");
	}
	return *typedBackend;
}

void publish(const CachePaths& paths, const std::string& keyManifest, const std::string& bindingSchema,
             const mlir::MLIRCacheArtifacts& artifacts, const std::vector<ImportRecord>& imports) {
	if (artifacts.bytecode.empty() || artifacts.moduleManifest.empty()) {
		throw CacheFailure("cache compilation produced no MLIR bytecode");
	}
	Manifest manifest {bindingSchema,
	                   keyManifest,
	                   artifacts.moduleManifest,
	                   artifacts.object.empty() ? std::string {} : digest(artifacts.object),
	                   digest(artifacts.bytecode),
	                   imports};
	atomicWrite(paths.bytecode, artifacts.bytecode);
	if (!artifacts.object.empty()) {
		atomicWrite(paths.object, artifacts.object);
	}
	// The manifest is the commit record. Publishing it last prevents readers
	// from accepting a partially-written generation.
	atomicWrite(paths.manifest, encodeManifest(manifest));
}

void markCacheState(CompilationStatistics* statistics, std::string objectState, std::string bytecodeState,
                    bool tracingRan, std::string fallback) {
	if (statistics == nullptr) {
		return;
	}
	statistics->set("cache.eligible", int64_t {1});
	statistics->set("cache.object", std::move(objectState));
	statistics->set("cache.mlir", std::move(bytecodeState));
	statistics->set("cache.tracingRan", int64_t {tracingRan ? 1 : 0});
	statistics->set("cache.fallback", std::move(fallback));
}

} // namespace

std::unique_ptr<Executable> compileWithPersistentModuleCache(const CompilationPipeline& compiler,
                                                             std::list<CompilableFunction>& functions,
                                                             const engine::ModuleOptions& moduleOptions,
                                                             CompilationStatistics* statistics) {
	const auto cacheDirectory = moduleOptions.getOptionOrDefault<std::string>("engine.Blob.CacheDir", std::string {});
	const auto explicitKey = moduleOptions.getOptionOrDefault<std::string>("engine.Blob.CacheKey", std::string {});
	if (cacheDirectory.empty() || explicitKey.empty()) {
		return nullptr;
	}

	const auto decline = [&](const std::string& reason) -> std::unique_ptr<Executable> {
		if (statistics != nullptr) {
			statistics->set("cache.eligible", int64_t {0});
			statistics->set("cache.object", std::string {"not_used"});
			statistics->set("cache.mlir", std::string {"not_used"});
			statistics->set("cache.tracingRan", int64_t {1});
			statistics->set("cache.fallback", reason);
		}
		return nullptr;
	};
	if (moduleOptions.getOptionOrDefault("debug", false) || moduleOptions.getOptionOrDefault("perf", false) ||
	    moduleOptions.getOptionOrDefault("perf.sample", false)) {
		return decline("debug_metadata_unsupported");
	}
	const auto compilerImage =
	    common::locateExecutableAddress(reinterpret_cast<const void*>(&compileWithPersistentModuleCache));
	if (!compilerImage) {
		return decline("missing_compiler_identity");
	}
	const auto& backend = getMLIRBackend();
	const auto intrinsicFingerprint = mlir::MLIRIntrinsicPluginRegistry::instance().cacheFingerprint();
	if (!intrinsicFingerprint) {
		return decline("unidentified_intrinsic_plugin");
	}
	const auto intrinsicsUnchanged = [&] {
		return mlir::MLIRIntrinsicPluginRegistry::instance().cacheFingerprint() == intrinsicFingerprint;
	};
	auto keyManifest =
	    createKeyManifest(functions, moduleOptions, explicitKey, compilerImage->buildId, *intrinsicFingerprint);
	if (!keyManifest) {
		return nullptr;
	}
	const auto& bindings = moduleOptions.getRuntimeBindings();
	const auto bindingSchema = bindings.schema();
	const auto keyDigest = digest(*keyManifest);
	if (statistics != nullptr) {
		statistics->set("cache.key", keyDigest);
	}
	const auto paths = getPaths(cacheDirectory, keyDigest);
	std::error_code directoryError;
	std::filesystem::create_directories(std::filesystem::path(cacheDirectory), directoryError);
	if (directoryError) {
		return nullptr;
	}

	auto absoluteManifest = std::filesystem::absolute(paths.manifest, directoryError);
	if (directoryError) {
		return nullptr;
	}
	auto keyMutex = mutexForKey(absoluteManifest.lexically_normal().string());
	std::unique_lock keyLock(*keyMutex);
	const auto exports = exportNames(functions);
	if (!intrinsicsUnchanged()) {
		return decline("intrinsic_registry_changed");
	}
	static std::atomic<uint64_t> loadSequence {0};
	const auto compilationId = "cache-" + keyDigest.substr(0, 12) + "-" + std::to_string(loadSequence.fetch_add(1));
	std::string objectState = "miss";
	std::string bytecodeState = "miss";
	std::string fallback = "none";

	std::optional<Manifest> manifest;
	try {
		manifest = readManifest(paths, *keyManifest, bindingSchema);
	} catch (const std::exception&) {
		fallback = "invalid_manifest";
	}

	if (manifest) {
		std::vector<std::string> symbols;
		std::vector<void*> addresses;
		if (resolveImports(manifest->imports, bindings, symbols, addresses)) {
			if (!manifest->objectDigest.empty()) {
				try {
					auto object = readCheckedArtifact(paths.object, manifest->objectDigest);
					auto executable = backend.compileCachedObject(object, symbols, addresses, exports, moduleOptions,
					                                              statistics, compilationId);
					if (!intrinsicsUnchanged()) {
						return decline("intrinsic_registry_changed");
					}
					if (statistics != nullptr) {
						statistics->set("compilation.unitId", compilationId);
						statistics->set("backend.name", "mlir");
					}
					objectState = "hit";
					markCacheState(statistics, objectState, "not_checked", false, fallback);
					return executable;
				} catch (const std::exception&) {
					objectState = "invalid";
					fallback = "invalid_object";
				}
			}

			try {
				auto bytecode = readCheckedArtifact(paths.bytecode, manifest->bytecodeDigest);
				auto dumpHandler = DumpHandler(moduleOptions, compilationId);
				mlir::MLIRCacheArtifacts regenerated;
				auto executable =
				    backend.compileCachedBytecode(bytecode, manifest->moduleManifest, symbols, addresses, exports,
				                                  dumpHandler, moduleOptions, statistics, &regenerated, compilationId);
				if (!intrinsicsUnchanged()) {
					return decline("intrinsic_registry_changed");
				}
				executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
				if (statistics != nullptr) {
					statistics->set("compilation.unitId", compilationId);
					statistics->set("backend.name", "mlir");
				}
				bytecodeState = "hit";
				if (!regenerated.object.empty()) {
					try {
						publish(paths, *keyManifest, bindingSchema, regenerated, manifest->imports);
						objectState = objectState == "invalid" ? "invalid_rewritten" : "miss_written";
					} catch (const std::exception&) {
						fallback = "object_repair_failed";
					}
				}
				markCacheState(statistics, objectState, bytecodeState, false, fallback);
				return executable;
			} catch (const std::exception&) {
				bytecodeState = "invalid";
				if (fallback == "none") {
					fallback = "invalid_mlir";
				}
			}
		} else {
			fallback = "unresolved_import";
		}
	}

	ir::CacheScalarValidationPass scalarValidation;
	auto ir = compiler.compileToIR(
	    functions, moduleOptions, statistics, compiler.irOptimizationLevel({"mlir"}),
	    [&](ir::IRGraph& graph) { scalarValidation.apply(graph); }, ConstantOriginTracking::Enabled);
	const auto& scalarResult = scalarValidation.getResult();
	if (statistics != nullptr) {
		statistics->set("cache.scalarCertificate", int64_t {scalarResult.certified ? 1 : 0});
		statistics->set("cache.scalarRejection", scalarResult.rejection);
	}
	if (!scalarResult.certified) {
		ir::PointerRelocatabilityPass pointerRelocatability(exports);
		pointerRelocatability.apply(*ir);
		if (!pointerRelocatability.getResult().relocatable) {
			if (statistics != nullptr) {
				statistics->set("cache.rejection", pointerRelocatability.getResult().rejection);
			}
			markCacheState(statistics, objectState, bytecodeState, true, "non_relocatable_pointer");
			return compiler.compileIR(ir, "mlir", moduleOptions, statistics);
		}
	}

	auto dumpHandler = DumpHandler(moduleOptions, ir->getId());
	if (statistics != nullptr) {
		statistics->set("backend.name", "mlir");
	}
	mlir::MLIRCacheArtifacts artifacts;
	std::unique_ptr<Executable> executable;
	try {
		executable = backend.compileWithCacheArtifacts(ir, exports, dumpHandler, moduleOptions, statistics, artifacts);
		executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
	} catch (const std::exception& exception) {
		markCacheState(statistics, objectState, bytecodeState, true,
		               "artifact_generation_failed:" + std::string(exception.what()));
		return compiler.compileIR(ir, "mlir", moduleOptions, statistics);
	}

	if (!intrinsicsUnchanged()) {
		markCacheState(statistics, objectState, bytecodeState, true, "intrinsic_registry_changed");
		return executable;
	}
	try {
		auto imports = createImportRecords(artifacts, bindings);
		publish(paths, *keyManifest, bindingSchema, artifacts, imports);
		objectState = artifacts.object.empty() ? "not_emitted" : "written";
		bytecodeState = "written";
	} catch (const std::exception& exception) {
		if (fallback == "none") {
			fallback = "artifact_publication_failed:" + std::string(exception.what());
		}
	}
	markCacheState(statistics, objectState, bytecodeState, true, fallback);
	return executable;
}

} // namespace nautilus::compiler
