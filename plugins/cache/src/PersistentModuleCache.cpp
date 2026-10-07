#include "PersistentModuleCache.hpp"
#include "CacheOptions.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include <utility>

#if defined(ENABLE_MLIR_BACKEND) && defined(__linux__) && defined(__x86_64__)
#include "CacheSafetyAnalysis.hpp"
#include "nautilus/Artifact.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/artifact/ArtifactCodec.hpp"
#include "nautilus/compiler/artifact/ArtifactPreflight.hpp"
#include "nautilus/compiler/artifact/ArtifactSupport.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/compiler/backends/mlir/MLIRArtifactValidation.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/passes/ExceptionRegionPreparationPass.hpp"
#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <sys/file.h>
#include <sys/stat.h>
#include <typeinfo>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#endif

namespace nautilus::cache::detail {
namespace {

void markCacheState(compiler::CompilationStatistics* statistics, std::string objectState, std::string bytecodeState,
                    bool tracingRan, std::string fallback, bool eligible = true) {
	if (statistics == nullptr) {
		return;
	}
	statistics->set("cache.eligible", int64_t {eligible ? 1 : 0});
	statistics->set("cache.object", std::move(objectState));
	statistics->set("cache.mlir", std::move(bytecodeState));
	statistics->set("cache.tracingRan", int64_t {tracingRan ? 1 : 0});
	statistics->set("cache.fallback", std::move(fallback));
}

#if defined(ENABLE_MLIR_BACKEND) && defined(__linux__) && defined(__x86_64__)
namespace transport = ::nautilus::artifact::detail;
namespace artifact = ::nautilus::artifact;

constexpr std::string_view CACHE_MAGIC = "NAUTILUS-MODULE-CACHE-1";
constexpr uint64_t MAX_MANIFEST_SIZE = 2 * transport::MAX_DESCRIPTOR_SIZE + 1024;

class CacheFailure final : public std::runtime_error {
public:
	explicit CacheFailure(std::string message) : std::runtime_error(std::move(message)) {
	}
};

bool cacheOption(std::string_view name) {
	return name == "engine.cache.directory" || name == "engine.cache.key" || name == "engine.Blob.CacheDir" ||
	       name == "engine.Blob.CacheKey";
}

engine::Options artifactOptions(const engine::Options& options) {
	engine::Options effective;
	for (const auto& [name, value] : options.getOptionValues()) {
		if (!cacheOption(name)) {
			effective.setOption(name, value);
		}
	}
	return effective;
}

std::string effectiveOptionsDigest(const engine::Options& options) {
	engine::Options identity;
	for (const auto& [name, value] : options.getOptionValues()) {
		if (!cacheOption(name)) {
			identity.setOption("option." + name, value);
		}
	}
	return transport::optionsDigest(identity);
}

std::vector<artifact::ExportDescriptor> createExports(const std::list<compiler::CompilableFunction>& functions) {
	if (functions.empty() || functions.size() > transport::MAX_RECORD_COUNT) {
		throw CacheFailure("invalid export count");
	}
	std::vector<artifact::ExportDescriptor> exports;
	std::unordered_set<std::string> names;
	for (const auto& function : functions) {
		if (function.getName().empty() || function.getName().find('\0') != std::string::npos ||
		    !names.insert(function.getName()).second || !function.getSignature()) {
			throw CacheFailure("exports require unique names and declared signatures");
		}
		const auto& signature = *function.getSignature();
		if (static_cast<uint8_t>(signature.returnType) > static_cast<uint8_t>(Type::ptr) ||
		    signature.argumentTypes.size() > transport::MAX_RECORD_COUNT) {
			throw CacheFailure("unsupported export signature");
		}
		for (const auto type : signature.argumentTypes) {
			if (type == Type::v || static_cast<uint8_t>(type) > static_cast<uint8_t>(Type::ptr)) {
				throw CacheFailure("unsupported export argument type");
			}
		}
		if (!function.getAttributes().empty()) {
			throw CacheFailure("user-supplied export attributes are unsupported");
		}
		artifact::ExportDescriptor entry;
		entry.name = function.getName();
		entry.returnType = signature.returnType;
		entry.argumentTypes = signature.argumentTypes;
		entry.attributes.assign(function.getAttributes().begin(), function.getAttributes().end());
		std::ranges::sort(entry.attributes);
		exports.push_back(std::move(entry));
	}
	std::ranges::sort(exports, {}, &artifact::ExportDescriptor::name);
	return exports;
}

std::string createKeyManifest(std::string_view semanticKey, const artifact::Descriptor& descriptor) {
	transport::Writer writer(transport::MAX_DESCRIPTOR_SIZE);
	writer.string(CACHE_MAGIC);
	writer.u32(descriptor.version);
	writer.string(semanticKey);
	const auto& identity = descriptor.compatibility;
	for (const auto* image : {&identity.compilerImage, &identity.producerImage}) {
		writer.string(image->buildId);
		writer.u64(image->loadOffset);
	}
	for (const auto* field : {&identity.llvmVersion, &identity.targetTriple, &identity.cpu, &identity.features,
	                          &identity.dataLayout, &identity.optionsDigest, &identity.extensionFingerprint}) {
		writer.string(*field);
	}
	writer.u32(identity.pointerSize);
	writer.u8(identity.littleEndian);
	writer.u64(__cplusplus);
#ifdef __clang_version__
	writer.string(__clang_version__);
#elif defined(__GNUC__)
	writer.string(__VERSION__);
#else
	throw CacheFailure("C++ compiler identity is unavailable");
#endif
#ifdef NDEBUG
	writer.u8(1);
#else
	writer.u8(0);
#endif
	writer.u32(static_cast<uint32_t>(descriptor.exports.size()));
	for (const auto& entry : descriptor.exports) {
		writer.string(entry.name);
		writer.u8(static_cast<uint8_t>(entry.returnType));
		writer.u32(static_cast<uint32_t>(entry.argumentTypes.size()));
		for (const auto type : entry.argumentTypes) {
			writer.u8(static_cast<uint8_t>(type));
		}
		writer.u32(entry.callingConvention);
		writer.u32(static_cast<uint32_t>(entry.attributes.size()));
		for (const auto& [name, value] : entry.attributes) {
			writer.string(name);
			writer.string(value);
		}
	}
	return writer.take();
}

bool matchingExports(const std::vector<artifact::ExportDescriptor>& expected,
                     const std::vector<artifact::ExportDescriptor>& actual) {
	if (expected.size() != actual.size()) {
		return false;
	}
	for (std::size_t index = 0; index < expected.size(); ++index) {
		const auto& left = expected[index];
		const auto& right = actual[index];
		if (left.name != right.name || left.returnType != right.returnType ||
		    left.argumentTypes != right.argumentTypes || left.attributes != right.attributes ||
		    left.callingConvention != right.callingConvention) {
			return false;
		}
	}
	return true;
}

std::string encodeManifest(std::string_view keyManifest, const artifact::ModuleArtifact& value) {
	transport::validateDescriptor(value);
	transport::Writer payload(MAX_MANIFEST_SIZE);
	payload.string(keyManifest);
	payload.string(transport::encodeDescriptor(value.descriptor));
	payload.string(value.descriptorDigest);
	const auto bytes = payload.take();
	transport::Writer envelope(MAX_MANIFEST_SIZE);
	envelope.string(CACHE_MAGIC);
	envelope.string(bytes);
	envelope.string(transport::digest(bytes));
	return envelope.take();
}

artifact::ModuleArtifact decodeManifest(std::string_view bytes, std::string_view expectedKey,
                                        const artifact::Descriptor& expected) {
	transport::Reader envelope(bytes);
	if (envelope.string(128) != CACHE_MAGIC) {
		throw CacheFailure("invalid cache manifest magic");
	}
	const auto payload = envelope.string(MAX_MANIFEST_SIZE);
	const auto checksum = envelope.string(64);
	envelope.finish();
	if (!transport::isDigest(checksum) || transport::digest(payload) != checksum) {
		throw CacheFailure("cache manifest integrity mismatch");
	}
	transport::Reader reader(payload);
	if (reader.string(transport::MAX_DESCRIPTOR_SIZE) != expectedKey) {
		throw CacheFailure("cache semantic or compatibility key mismatch");
	}
	artifact::ModuleArtifact value;
	value.descriptor = transport::decodeDescriptor(reader.string(transport::MAX_DESCRIPTOR_SIZE));
	value.descriptorDigest = reader.string(64);
	reader.finish();
	transport::validateDescriptor(value);
	if (value.descriptor.compatibility != expected.compatibility ||
	    !matchingExports(expected.exports, value.descriptor.exports)) {
		throw CacheFailure("cache descriptor compatibility mismatch");
	}
	return value;
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
	OwnedDescriptor(OwnedDescriptor&& other) noexcept : descriptor_(other.release()) {
	}
	OwnedDescriptor& operator=(OwnedDescriptor&& other) noexcept {
		if (this != &other) {
			if (descriptor_ >= 0) {
				::close(descriptor_);
			}
			descriptor_ = other.release();
		}
		return *this;
	}
	int get() const {
		return descriptor_;
	}
	int release() {
		return std::exchange(descriptor_, -1);
	}

private:
	int descriptor_;
};

struct stat descriptorStatus(int descriptor) {
	struct stat status {};
	if (descriptor < 0 || ::fstat(descriptor, &status) != 0) {
		throw CacheFailure("cache file status failed");
	}
	return status;
}

void validateDirectory(const struct stat& status, bool privateDirectory) {
	if (!S_ISDIR(status.st_mode) || (status.st_uid != 0 && status.st_uid != ::geteuid())) {
		throw CacheFailure("cache directory ownership is untrusted");
	}
	const bool protectedSticky = status.st_uid == 0 && (status.st_mode & S_ISVTX) != 0;
	if ((status.st_mode & (S_IWGRP | S_IWOTH)) != 0 && !protectedSticky) {
		throw CacheFailure("cache directory ancestor is writable by another user");
	}
	if (privateDirectory && (status.st_uid != ::geteuid() || (status.st_mode & (S_IRWXG | S_IRWXO)) != 0)) {
		throw CacheFailure("cache directory must be private and owned by the effective user");
	}
}

void validateOwnedFile(const struct stat& status) {
	if (!S_ISREG(status.st_mode) || status.st_uid != ::geteuid() || status.st_nlink != 1 ||
	    (status.st_mode & (S_IRWXG | S_IRWXO | S_IXUSR | S_ISUID | S_ISGID | S_ISVTX)) != 0) {
		throw CacheFailure("cache file must be private, owned, regular and singly linked");
	}
}

OwnedDescriptor openDirectory(const std::string& name) {
	if (name.empty() || name.find('\0') != std::string::npos) {
		throw CacheFailure("invalid cache directory path");
	}
	const std::filesystem::path path(name);
	OwnedDescriptor directory(::open(path.is_absolute() ? "/" : ".", O_RDONLY | O_CLOEXEC | O_DIRECTORY | O_NOFOLLOW));
	validateDirectory(descriptorStatus(directory.get()), false);
	for (const auto& part : path.relative_path()) {
		if (part == "." || part.empty()) {
			continue;
		}
		if (part == "..") {
			throw CacheFailure("cache directory parent traversal is unsupported");
		}
		int next = ::openat(directory.get(), part.c_str(), O_RDONLY | O_CLOEXEC | O_DIRECTORY | O_NOFOLLOW);
		if (next < 0 && errno == ENOENT) {
			if (::mkdirat(directory.get(), part.c_str(), S_IRWXU) != 0 && errno != EEXIST) {
				throw CacheFailure("cache directory creation failed");
			}
			next = ::openat(directory.get(), part.c_str(), O_RDONLY | O_CLOEXEC | O_DIRECTORY | O_NOFOLLOW);
		}
		OwnedDescriptor child(next);
		validateDirectory(descriptorStatus(child.get()), false);
		directory = std::move(child);
	}
	validateDirectory(descriptorStatus(directory.get()), true);
	return directory;
}

void writeAll(int descriptor, std::string_view data) {
	std::size_t written = 0;
	while (written < data.size()) {
		const auto count = ::write(descriptor, data.data() + written, data.size() - written);
		if (count < 0 && errno == EINTR) {
			continue;
		}
		if (count <= 0) {
			throw CacheFailure("cache artifact write failed");
		}
		written += static_cast<std::size_t>(count);
	}
}

class TrustedDirectory {
public:
	explicit TrustedDirectory(const std::string& name) : descriptor_(openDirectory(name)) {
	}
	int get() const {
		validateDirectory(descriptorStatus(descriptor_.get()), true);
		return descriptor_.get();
	}
	std::string identity() const {
		const auto status = descriptorStatus(get());
		return std::to_string(static_cast<uint64_t>(status.st_dev)) + ":" +
		       std::to_string(static_cast<uint64_t>(status.st_ino));
	}
	std::optional<std::string> read(const std::string& name, uint64_t maximumSize) const {
		OwnedDescriptor input(::openat(get(), name.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK));
		if (input.get() < 0 && errno == ENOENT) {
			return std::nullopt;
		}
		const auto before = descriptorStatus(input.get());
		validateOwnedFile(before);
		if (before.st_size < 0 || static_cast<uint64_t>(before.st_size) > maximumSize) {
			throw CacheFailure("invalid cache artifact size");
		}
		std::string bytes(static_cast<std::size_t>(before.st_size), '\0');
		std::size_t consumed = 0;
		while (consumed < bytes.size()) {
			const auto count = ::read(input.get(), bytes.data() + consumed, bytes.size() - consumed);
			if (count < 0 && errno == EINTR) {
				continue;
			}
			if (count <= 0) {
				throw CacheFailure("cache artifact read failed");
			}
			consumed += static_cast<std::size_t>(count);
		}
		char extra;
		ssize_t count;
		do {
			count = ::read(input.get(), &extra, 1);
		} while (count < 0 && errno == EINTR);
		const auto after = descriptorStatus(input.get());
		validateOwnedFile(after);
		if (count != 0 || before.st_size != after.st_size || before.st_mtim.tv_sec != after.st_mtim.tv_sec ||
		    before.st_mtim.tv_nsec != after.st_mtim.tv_nsec || before.st_ctim.tv_sec != after.st_ctim.tv_sec ||
		    before.st_ctim.tv_nsec != after.st_ctim.tv_nsec) {
			throw CacheFailure("cache artifact changed while reading");
		}
		return bytes;
	}
	void atomicWrite(const std::string& name, std::string_view bytes) const {
		static std::atomic<uint64_t> sequence {0};
		std::string temporary;
		OwnedDescriptor output(-1);
		for (unsigned attempt = 0; attempt < 32; ++attempt) {
			temporary = name + ".tmp." + std::to_string(::getpid()) + "." + std::to_string(sequence.fetch_add(1));
			output = OwnedDescriptor(::openat(get(), temporary.c_str(),
			                                  O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC | O_NOFOLLOW, S_IRUSR | S_IWUSR));
			if (output.get() >= 0) {
				break;
			}
			if (errno != EEXIST) {
				throw CacheFailure("temporary cache artifact creation failed");
			}
		}
		if (output.get() < 0) {
			throw CacheFailure("temporary cache artifact names exhausted");
		}
		try {
			if (::fchmod(output.get(), S_IRUSR | S_IWUSR) != 0) {
				throw CacheFailure("temporary cache artifact permissions failed");
			}
			validateOwnedFile(descriptorStatus(output.get()));
			writeAll(output.get(), bytes);
			if (::fsync(output.get()) != 0) {
				throw CacheFailure("cache artifact sync failed");
			}
			if (::close(output.release()) != 0) {
				throw CacheFailure("cache artifact close failed");
			}
			struct stat target {};
			if (::fstatat(get(), name.c_str(), &target, AT_SYMLINK_NOFOLLOW) == 0) {
				validateOwnedFile(target);
			} else if (errno != ENOENT) {
				throw CacheFailure("cache publication target status failed");
			}
			if (::renameat(get(), temporary.c_str(), get(), name.c_str()) != 0) {
				throw CacheFailure("cache artifact publication failed");
			}
			if (::fsync(get()) != 0) {
				throw CacheFailure("cache directory sync failed");
			}
		} catch (...) {
			::unlinkat(get(), temporary.c_str(), 0);
			throw;
		}
	}

private:
	OwnedDescriptor descriptor_;
};

std::shared_ptr<std::mutex> mutexForKey(const std::string& key) {
	static std::mutex mapMutex;
	static std::unordered_map<std::string, std::weak_ptr<std::mutex>> mutexes;
	std::lock_guard guard(mapMutex);
	if (const auto entry = mutexes.find(key); entry != mutexes.end()) {
		if (auto existing = entry->second.lock()) {
			return existing;
		}
	}
	std::erase_if(mutexes, [](const auto& entry) { return entry.second.expired(); });
	auto created = std::make_shared<std::mutex>();
	mutexes[key] = created;
	return created;
}

class KeyFileLock {
public:
	KeyFileLock(const TrustedDirectory& directory, const std::string& key)
	    : descriptor_(::openat(directory.get(), (key + ".lock").c_str(),
	                           O_RDWR | O_CREAT | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK, S_IRUSR | S_IWUSR)) {
		const auto before = descriptorStatus(descriptor_.get());
		validateOwnedFile(before);
		int result;
		do {
			result = ::flock(descriptor_.get(), LOCK_EX);
		} while (result < 0 && errno == EINTR);
		if (result != 0) {
			throw CacheFailure("cache key lock failed");
		}
		struct stat current {};
		if (::fstatat(directory.get(), (key + ".lock").c_str(), &current, AT_SYMLINK_NOFOLLOW) != 0) {
			throw CacheFailure("cache key lock status failed");
		}
		validateOwnedFile(current);
		if (before.st_dev != current.st_dev || before.st_ino != current.st_ino) {
			throw CacheFailure("cache key lock was replaced");
		}
	}

private:
	OwnedDescriptor descriptor_;
};

struct CachePaths {
	std::string object;
	std::string bytecode;
	std::string manifest;
};

CachePaths getPaths(const std::string& keyDigest) {
	return {keyDigest + ".o", keyDigest + ".mlirbc", keyDigest + ".manifest"};
}

std::string readCheckedArtifact(const TrustedDirectory& directory, const std::string& name,
                                std::string_view expectedDigest) {
	auto bytes = directory.read(name, transport::MAX_ARTIFACT_SIZE);
	if (!bytes) {
		throw CacheFailure("cache artifact is missing");
	}
	transport::validatePayload(*bytes, expectedDigest);
	return std::move(*bytes);
}

bool implementationsUnchanged(const artifact::Compatibility& identity);

void publish(const TrustedDirectory& directory, const CachePaths& paths, std::string_view keyManifest,
             const artifact::ModuleArtifact& value) {
	transport::validatePayload(value.object, value.descriptor.objectDigest);
	transport::validatePayload(value.bytecode, value.descriptor.bytecodeDigest);
	const auto manifest = encodeManifest(keyManifest, value);
	directory.atomicWrite(paths.bytecode, value.bytecode);
	directory.atomicWrite(paths.object, value.object);
	if (!implementationsUnchanged(value.descriptor.compatibility)) {
		throw CacheFailure("artifact implementation identity changed before manifest publication");
	}
	directory.atomicWrite(paths.manifest, manifest);
}

const compiler::mlir::MLIRCompilationBackend& getMLIRBackend() {
	const auto* backend = compiler::CompilationBackendRegistry::getInstance()->getBackend("mlir");
	const auto* typed = dynamic_cast<const compiler::mlir::MLIRCompilationBackend*>(backend);
	if (typed == nullptr || typeid(*typed) != typeid(compiler::mlir::MLIRCompilationBackend)) {
		throw CacheFailure("MLIR artifact backend implementation is unsupported");
	}
	return *typed;
}

bool activeHooks() {
	const auto& hooks = compiler::mlir::getLLVMBackendHooks();
	return hooks.preOptModuleTransform || hooks.jitSymbolContributor || hooks.callNameOverride;
}

bool implementationsUnchanged(const artifact::Compatibility& identity) {
	try {
		return !activeHooks() &&
		       compiler::mlir::MLIRIntrinsicPluginRegistry::instance().artifactFingerprint() ==
		           std::optional<std::string>(identity.extensionFingerprint) &&
		       transport::imageAt(reinterpret_cast<const void*>(&compiler::CompilationBackendRegistry::getInstance)) ==
		           identity.compilerImage &&
		       transport::imageAt(reinterpret_cast<const void*>(&compileWithPersistentModuleCache)) ==
		           identity.producerImage;
	} catch (...) {
		return false;
	}
}
#endif

} // namespace

void recordCacheDecline(compiler::CompilationStatistics& statistics, const engine::ModuleOptions& options,
                        std::string reason, bool tracingRan) {
	if (options.getOptionOrDefault("engine.cache.directory", std::string()).empty()) {
		reason = "cache_disabled";
	} else if (options.getOptionOrDefault("engine.cache.key", std::string()).empty()) {
		reason = "missing_semantic_key";
	}
	markCacheState(&statistics, "not_used", "not_used", tracingRan, std::move(reason), false);
}

std::unique_ptr<compiler::Executable>
compileWithPersistentModuleCache(const compiler::CompilationPipeline& pipeline,
                                 std::list<compiler::CompilableFunction>& functions,
                                 const engine::ModuleOptions& options, compiler::CompilationStatistics* statistics) {
	const auto moduleOptions = normalizeOptions(options);
	const auto decline = [&](std::string reason) -> std::unique_ptr<compiler::Executable> {
		if (statistics != nullptr) {
			recordCacheDecline(*statistics, moduleOptions, std::move(reason));
		}
		return nullptr;
	};
#if !defined(ENABLE_MLIR_BACKEND) || !defined(__linux__) || !defined(__x86_64__)
	(void) pipeline;
	(void) functions;
	return decline("unsupported_platform");
#else
	const auto cacheDirectory = moduleOptions.getOptionOrDefault("engine.cache.directory", std::string());
	const auto semanticKey = moduleOptions.getOptionOrDefault("engine.cache.key", std::string());
	if (cacheDirectory.empty() || semanticKey.empty()) {
		return decline("missing_cache_configuration");
	}
	if (moduleOptions.getOptionOrDefault("debug", false) || moduleOptions.getOptionOrDefault("perf", false) ||
	    moduleOptions.getOptionOrDefault("perf.sample", false)) {
		return decline("debug_metadata_unsupported");
	}
	if (moduleOptions.getOptionOrDefault("mlir.inline_invoke_calls", false)) {
		return decline("inline_invoke_calls_unsupported");
	}
	if (!moduleOptions.getOptionOrDefault("mlir.targetCpu", std::string()).empty()) {
		return decline("pinned_target_cpu_unsupported");
	}
	if (activeHooks()) {
		return decline("active_backend_hooks_unsupported");
	}
	artifact::Descriptor descriptor;
	try {
		descriptor.compatibility.compilerImage =
		    transport::imageAt(reinterpret_cast<const void*>(&compiler::CompilationBackendRegistry::getInstance));
	} catch (const std::exception&) {
		return decline("missing_compiler_identity");
	}
	try {
		descriptor.compatibility.producerImage =
		    transport::imageAt(reinterpret_cast<const void*>(&compileWithPersistentModuleCache));
	} catch (const std::exception&) {
		return decline("missing_cache_provider_identity");
	}
	try {
		descriptor.exports = createExports(functions);
	} catch (const std::exception& error) {
		return decline("unsupported_export_metadata:" + std::string(error.what()));
	}
	std::string keyManifest;
	const compiler::mlir::MLIRCompilationBackend* backend = nullptr;
	try {
		backend = &getMLIRBackend();
		const auto providerImage = descriptor.compatibility.producerImage;
		descriptor.compatibility = transport::currentCompatibility(artifactOptions(moduleOptions));
		descriptor.compatibility.producerImage = providerImage;
		descriptor.compatibility.optionsDigest = effectiveOptionsDigest(moduleOptions);
		keyManifest = createKeyManifest(semanticKey, descriptor);
	} catch (const std::exception& error) {
		return decline("unsupported_artifact_configuration:" + std::string(error.what()));
	}
	const auto keyDigest = transport::digest(keyManifest);
	if (statistics != nullptr) {
		statistics->set("cache.key", keyDigest);
	}
	std::optional<TrustedDirectory> directory;
	std::shared_ptr<std::mutex> keyMutex;
	std::unique_lock<std::mutex> keyLock;
	std::optional<KeyFileLock> fileLock;
	try {
		directory.emplace(cacheDirectory);
		keyMutex = mutexForKey(directory->identity() + ":" + keyDigest);
		keyLock = std::unique_lock(*keyMutex);
		fileLock.emplace(*directory, keyDigest);
	} catch (const std::exception& error) {
		return decline("cache_storage_unavailable:" + std::string(error.what()));
	}
	if (!implementationsUnchanged(descriptor.compatibility)) {
		return decline("implementation_identity_changed");
	}
	const auto paths = getPaths(keyDigest);
	const auto exports = transport::exportNames(descriptor);
	static std::atomic<uint64_t> loadSequence {0};
	const auto compilationId = "cache-" + keyDigest.substr(0, 12) + "-" + std::to_string(loadSequence.fetch_add(1));
	std::string objectState = "miss";
	std::string bytecodeState = "miss";
	std::string fallback = "none";
	markCacheState(statistics, objectState, bytecodeState, false, fallback);

	std::optional<artifact::ModuleArtifact> cached;
	try {
		if (auto bytes = directory->read(paths.manifest, MAX_MANIFEST_SIZE)) {
			cached = decodeManifest(*bytes, keyManifest, descriptor);
		}
	} catch (const std::exception&) {
		fallback = "invalid_manifest";
	}
	if (cached) {
		std::optional<transport::ResolvedImports> nativeImports;
		try {
			nativeImports = transport::resolveImports(cached->descriptor, false);
		} catch (const std::exception&) {
			fallback = "unresolved_import";
		}
		if (nativeImports) {
			try {
				const auto object = readCheckedArtifact(*directory, paths.object, cached->descriptor.objectDigest);
				auto executable = backend->compileCachedObject(object, nativeImports->symbols, nativeImports->addresses,
				                                               exports, moduleOptions, statistics, compilationId);
				if (!implementationsUnchanged(descriptor.compatibility)) {
					return decline("implementation_identity_changed");
				}
				if (statistics != nullptr) {
					statistics->set("compilation.unitId", compilationId);
					statistics->set("backend.name", std::string("mlir"));
				}
				markCacheState(statistics, "hit", "not_checked", false, "none");
				return executable;
			} catch (const std::exception&) {
				objectState = "invalid";
				fallback = "invalid_object";
			}
			if (!implementationsUnchanged(descriptor.compatibility)) {
				return decline("implementation_identity_changed");
			}
			try {
				cached->bytecode = readCheckedArtifact(*directory, paths.bytecode, cached->descriptor.bytecodeDigest);
				const auto imports = transport::resolveImports(cached->descriptor, true);
				compiler::DumpHandler dump(moduleOptions, compilationId);
				compiler::mlir::MLIRCacheArtifacts regenerated;
				const auto objectPreflight = [&](std::string_view object) {
					compiler::mlir::validateArtifactObjectSymbols(object, exports, nativeImports->symbols,
					                                              nativeImports->addresses);
				};
				auto executable = backend->compileCachedBytecode(
				    cached->bytecode, cached->descriptor.moduleManifest, imports.symbols, imports.addresses, exports,
				    dump, moduleOptions, statistics, &regenerated, compilationId, imports.auxiliarySymbols,
				    imports.auxiliaryAddresses, &cached->descriptor.exports, objectPreflight);
				if (!implementationsUnchanged(descriptor.compatibility)) {
					return decline("implementation_identity_changed");
				}
				executable->setGeneratedFiles(dump.getGeneratedFiles());
				if (statistics != nullptr) {
					statistics->set("compilation.unitId", compilationId);
					statistics->set("backend.name", std::string("mlir"));
				}
				bytecodeState = "hit";
				if (!regenerated.object.empty()) {
					try {
						cached->object = std::move(regenerated.object);
						cached->descriptor.objectDigest = transport::digest(cached->object);
						cached->descriptorDigest = transport::digest(transport::encodeDescriptor(cached->descriptor));
						publish(*directory, paths, keyManifest, *cached);
						objectState = "invalid_rewritten";
					} catch (const std::exception&) {
						fallback = "object_repair_failed";
					}
				} else {
					fallback = "object_not_emitted";
				}
				markCacheState(statistics, objectState, bytecodeState, false, fallback);
				return executable;
			} catch (const std::exception&) {
				bytecodeState = "invalid";
				fallback = "invalid_mlir";
			}
		}
	}

	compiler::ir::CacheScalarValidationPass scalarValidation;
	compiler::ir::PointerRelocatabilityPass pointerValidation(exports);
	std::string rootRejection;
	std::string metadataRejection;
	bool typedAllocations = false;
	std::string scalarFailure;
	std::string pointerFailure;
	markCacheState(statistics, objectState, bytecodeState, true, fallback);
	auto ir = pipeline.compileToIR(
	    functions, moduleOptions, statistics, backend->irOptimizationLevel(),
	    [&](compiler::ir::IRGraph& graph) {
		    try {
			    compiler::artifact::validateArtifactRoots(graph, functions);
		    } catch (const std::exception& error) {
			    rootRejection = error.what();
		    }
		    typedAllocations = compiler::artifact::hasOnlyTypedAllocations(graph, &metadataRejection);
		    try {
			    scalarValidation.apply(graph);
		    } catch (const std::exception& error) {
			    scalarFailure = error.what();
		    }
		    if (!scalarFailure.empty() || !scalarValidation.getResult().certified || !typedAllocations) {
			    try {
				    pointerValidation.apply(graph);
			    } catch (const std::exception& error) {
				    pointerFailure = error.what();
			    }
		    }
	    },
	    ConstantOriginTracking::Enabled);
	const auto& scalarResult = scalarValidation.getResult();
	if (statistics != nullptr) {
		statistics->set("cache.scalarCertificate", int64_t {scalarResult.certified && scalarFailure.empty() ? 1 : 0});
		statistics->set("cache.scalarRejection", scalarFailure.empty() ? scalarResult.rejection : scalarFailure);
	}
	if (!moduleOptions.getOptionOrDefault("ir.runPasses", true)) {
		compiler::ir::ExceptionRegionPreparationPass preparation;
		preparation.apply(*ir);
	}
	const auto ordinaryCompile = [&](std::string reason, std::string rejection = {}) {
		if (statistics != nullptr && !rejection.empty()) {
			statistics->set("cache.rejection", std::move(rejection));
		}
		markCacheState(statistics, objectState, bytecodeState, true, std::move(reason), false);
		return pipeline.compileIR(ir, "mlir", moduleOptions, statistics);
	};
	if (!rootRejection.empty()) {
		return ordinaryCompile("invalid_export_signature", rootRejection);
	}
	if (!typedAllocations) {
		return ordinaryCompile("unsupported_allocation_metadata", metadataRejection);
	}
	if ((!scalarFailure.empty() || !scalarResult.certified) &&
	    (!pointerFailure.empty() || !pointerValidation.getResult().relocatable)) {
		return ordinaryCompile("non_relocatable_pointer",
		                       pointerFailure.empty() ? pointerValidation.getResult().rejection : pointerFailure);
	}
	if (!implementationsUnchanged(descriptor.compatibility)) {
		return ordinaryCompile("implementation_identity_changed");
	}

	compiler::DumpHandler dump(moduleOptions, ir->getId());
	compiler::mlir::MLIRCacheArtifacts raw;
	std::unique_ptr<compiler::Executable> executable;
	try {
		const auto objectPreflight = [&](std::string_view object) {
			transport::createImports(descriptor, raw, object);
			const auto imports = transport::resolveImports(descriptor, false);
			compiler::mlir::validateArtifactObjectSymbols(object, exports, imports.symbols, imports.addresses);
		};
		if (statistics != nullptr) {
			statistics->set("backend.name", std::string("mlir"));
		}
		executable = backend->compileWithCacheArtifacts(ir, exports, dump, moduleOptions, statistics, raw,
		                                                &descriptor.exports, objectPreflight);
		executable->setGeneratedFiles(dump.getGeneratedFiles());
	} catch (const std::exception& error) {
		return ordinaryCompile("artifact_generation_failed:" + std::string(error.what()));
	}
	if (!implementationsUnchanged(descriptor.compatibility)) {
		markCacheState(statistics, objectState, bytecodeState, true, "implementation_identity_changed", false);
		return executable;
	}
	try {
		if (raw.object.empty() || raw.bytecode.empty() || raw.exportABIs.size() != descriptor.exports.size()) {
			throw CacheFailure("producer did not emit one complete native object and bytecode module");
		}
		for (std::size_t index = 0; index < descriptor.exports.size(); ++index) {
			descriptor.exports[index].loweredABI = raw.exportABIs[index];
		}
		transport::createImports(descriptor, raw, raw.object);
		const auto imports = transport::resolveImports(descriptor, false);
		compiler::mlir::validateArtifactObjectSymbols(raw.object, exports, imports.symbols, imports.addresses);
		artifact::ModuleArtifact value;
		descriptor.moduleManifest = std::move(raw.moduleManifest);
		descriptor.objectDigest = transport::digest(raw.object);
		descriptor.bytecodeDigest = transport::digest(raw.bytecode);
		value.descriptor = std::move(descriptor);
		value.descriptorDigest = transport::digest(transport::encodeDescriptor(value.descriptor));
		value.object = std::move(raw.object);
		value.bytecode = std::move(raw.bytecode);
		if (!implementationsUnchanged(value.descriptor.compatibility)) {
			markCacheState(statistics, objectState, bytecodeState, true, "implementation_identity_changed", false);
			return executable;
		}
		publish(*directory, paths, keyManifest, value);
		objectState = "written";
		bytecodeState = "written";
	} catch (const std::exception& error) {
		fallback = "artifact_publication_failed:" + std::string(error.what());
	}
	markCacheState(statistics, objectState, bytecodeState, true, fallback);
	return executable;
#endif
}

} // namespace nautilus::cache::detail
