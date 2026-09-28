#include "nautilus/compiler/cache/PersistentModuleCache.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/common/ConstantOrigin.hpp"
#include "nautilus/common/ExecutableImage.hpp"
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/passes/Dominators.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
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

struct OperationLocation {
	const ir::FunctionOperation* function = nullptr;
	const ir::BasicBlock* block = nullptr;
};
using OperationLocations = std::unordered_map<const ir::Operation*, OperationLocation>;

std::string operationTypeName(ir::Operation::OperationType type) {
	using Op = ir::Operation::OperationType;
	switch (type) {
	case Op::AddOp:
		return "AddOp";
	case Op::AndOp:
		return "AndOp";
	case Op::NotOp:
		return "NotOp";
	case Op::BasicBlockArgument:
		return "BasicBlockArgument";
	case Op::BlockInvocation:
		return "BlockInvocation";
	case Op::BranchOp:
		return "BranchOp";
	case Op::ConstIntOp:
		return "ConstIntOp";
	case Op::ConstBooleanOp:
		return "ConstBooleanOp";
	case Op::ConstPtrOp:
		return "ConstPtrOp";
	case Op::ConstFloatOp:
		return "ConstFloatOp";
	case Op::CastOp:
		return "CastOp";
	case Op::CompareOp:
		return "CompareOp";
	case Op::DivOp:
		return "DivOp";
	case Op::ModOp:
		return "ModOp";
	case Op::FunctionOp:
		return "FunctionOp";
	case Op::IfOp:
		return "IfOp";
	case Op::LoadOp:
		return "LoadOp";
	case Op::MulOp:
		return "MulOp";
	case Op::MLIR_YIELD:
		return "MLIR_YIELD";
	case Op::NegateOp:
		return "NegateOp";
	case Op::OrOp:
		return "OrOp";
	case Op::CallOp:
		return "CallOp";
	case Op::IndirectCallOp:
		return "IndirectCallOp";
	case Op::ReturnOp:
		return "ReturnOp";
	case Op::SelectOp:
		return "SelectOp";
	case Op::StoreOp:
		return "StoreOp";
	case Op::SubOp:
		return "SubOp";
	case Op::BinaryComp:
		return "BinaryComp";
	case Op::ShiftOp:
		return "ShiftOp";
	case Op::AllocaOp:
		return "AllocaOp";
	case Op::FunctionAddressOfOp:
		return "FunctionAddressOfOp";
	case Op::RuntimeBindingOp:
		return "RuntimeBindingOp";
	}
	return "Unknown(" + std::to_string(static_cast<unsigned>(type)) + ")";
}

std::string describeOperation(const ir::Operation* operation, OperationLocation location) {
	std::string description =
	    "function=" + (location.function ? location.function->getName() : std::string {"<module>"});
	if (location.block != nullptr) {
		description += " block=" + std::to_string(location.block->getIdentifier().getId());
		const auto& operations = location.block->getOperations();
		if (const auto position = std::ranges::find(operations, operation); position != operations.end()) {
			description += " index=" + std::to_string(position - operations.begin());
		}
	}
	if (operation == nullptr) {
		return description + " operation=null";
	}
	return description + " operation=" + operation->getIdentifier().toString() +
	       " type=" + operationTypeName(operation->getOperationType());
}

std::optional<bool> constantCondition(const ir::Operation* operation) {
	if (const auto* boolean = operation->dynCast<ir::ConstBooleanOperation>()) {
		return boolean->getValue();
	}
	if (operation->getOperationType() == ir::Operation::OperationType::NotOp) {
		if (const auto value = constantCondition(operation->getInputs()[0])) {
			return !*value;
		}
	}
	if (const auto* compare = operation->dynCast<ir::CompareOperation>()) {
		const auto* left = compare->getLeftInput()->dynCast<ir::ConstPtrOperation>();
		const auto* right = compare->getRightInput()->dynCast<ir::ConstPtrOperation>();
		if (left && right && left->getValue() == nullptr && right->getValue() == nullptr) {
			if (compare->getComparator() == ir::CompareOperation::EQ) {
				return true;
			}
			if (compare->getComparator() == ir::CompareOperation::NE) {
				return false;
			}
		}
	}
	return std::nullopt;
}

const ir::Operation* nonNullCondition(const ir::Operation* condition, bool value) {
	if (condition->getOperationType() == ir::Operation::OperationType::NotOp) {
		return nonNullCondition(condition->getInputs()[0], !value);
	}
	const auto* compare = condition->dynCast<ir::CompareOperation>();
	if (compare == nullptr || !((compare->getComparator() == ir::CompareOperation::NE && value) ||
	                            (compare->getComparator() == ir::CompareOperation::EQ && !value))) {
		return nullptr;
	}
	const auto* left = compare->getLeftInput();
	const auto* right = compare->getRightInput();
	if (const auto* null = left->dynCast<ir::ConstPtrOperation>(); null && null->getValue() == nullptr) {
		return right->getStamp() == Type::ptr ? right : nullptr;
	}
	if (const auto* null = right->dynCast<ir::ConstPtrOperation>(); null && null->getValue() == nullptr) {
		return left->getStamp() == Type::ptr ? left : nullptr;
	}
	return nullptr;
}

enum AddressSource : uint8_t {
	RuntimeInteger = 1,
	RuntimePointer = 2,
	Constant = 4,
	Unsupported = 8,
	NullPointer = 16,
	PointerOffset = 32
};
using AddressSources = std::unordered_map<const ir::Operation*, uint8_t>;
using AddressCallees = std::unordered_map<const ir::Operation*, std::vector<const ir::FunctionOperation*>>;
using AddressValues = std::unordered_set<const ir::Operation*>;

std::string describeAddressSources(uint8_t sources) {
	std::string result;
	for (const auto& [flag, name] : {std::pair {RuntimeInteger, "RuntimeInteger"},
	                                 {RuntimePointer, "RuntimePointer"},
	                                 {Constant, "Constant"},
	                                 {Unsupported, "Unsupported"},
	                                 {NullPointer, "NullPointer"},
	                                 {PointerOffset, "PointerOffset"}}) {
		if (sources & flag) {
			if (!result.empty()) {
				result += "|";
			}
			result += name;
		}
	}
	return result.empty() ? "none" : result;
}

std::vector<ir::BasicBlockInvocation*> feasibleSuccessors(ir::Operation& operation) {
	auto successors = ir::getSuccessorInvocations(operation);
	if (const auto* branch = operation.dynCast<ir::IfOperation>()) {
		if (const auto condition = constantCondition(branch->getValue())) {
			std::erase_if(successors, [&](const auto* invocation) {
				return (invocation == &branch->getTrueBlockInvocation()) != *condition;
			});
		}
	}
	return successors;
}

uint8_t addressSources(const ir::Operation& operation, const AddressSources& sources, const AddressCallees& callees,
                       const AddressValues& opaqueCalls, const AddressValues& nonNull, uint8_t memory) {
	using Op = ir::Operation::OperationType;
	const auto inputs = operation.getInputs();
	const auto source = [&](const ir::Operation* input) {
		return nonNull.contains(input) ? sources.at(input) & ~NullPointer : sources.at(input);
	};
	switch (operation.getOperationType()) {
	case Op::ConstIntOp:
	case Op::ConstBooleanOp:
	case Op::ConstFloatOp:
		return Constant;
	case Op::ConstPtrOp:
		return operation.dynCast<ir::ConstPtrOperation>()->getValue() == nullptr ? NullPointer : Constant;
	case Op::CallOp:
	case Op::IndirectCallOp: {
		uint8_t result = 0;
		if (const auto found = callees.find(&operation); found != callees.end()) {
			for (const auto* callee : found->second) {
				result |= source(callee);
			}
		}
		if (opaqueCalls.contains(&operation)) {
			const auto* call = operation.dynCast<ir::CallOperation>();
			const auto effect = call == nullptr ? ModRefInfo::ModRef : call->getFunctionAttributes().modRefInfo;
			const auto readMemory = effect == ModRefInfo::Ref || effect == ModRefInfo::ModRef ||
			                                (call != nullptr && !call->getDestructors().empty())
			                            ? memory
			                            : uint8_t {0};
			result |= (operation.getStamp() == Type::ptr ? RuntimePointer : RuntimeInteger) | readMemory;
			if (readMemory & PointerOffset) {
				result |= Unsupported;
			}
			for (const auto* input : inputs) {
				result |= source(input) & RuntimePointer;
				if (source(input) & (Constant | Unsupported | PointerOffset)) {
					result |= Unsupported;
				}
			}
		}
		return result;
	}
	case Op::LoadOp:
		return (operation.getStamp() == Type::ptr ? RuntimePointer : RuntimeInteger) | memory;
	case Op::RuntimeBindingOp:
	case Op::AllocaOp:
		return RuntimePointer;
	case Op::FunctionAddressOfOp: {
		uint8_t result = RuntimePointer;
		if (const auto found = callees.find(&operation); found != callees.end()) {
			for (const auto* callee : found->second) {
				if (source(callee) & (Constant | Unsupported | PointerOffset)) {
					result |= Unsupported;
				}
			}
		}
		return result;
	}
	case Op::CastOp: {
		const auto inputType = inputs[0]->getStamp();
		const auto outputType = operation.getStamp();
		const auto numeric = [](Type type) {
			return isInteger(type) || isFloat(type) || type == Type::b;
		};
		if ((inputType != Type::ptr && !numeric(inputType)) || (outputType != Type::ptr && !numeric(outputType)) ||
		    (outputType == Type::ptr && inputType != Type::ptr && !isInteger(inputType))) {
			return Unsupported;
		}
		auto result = source(inputs[0]);
		if ((isFloat(inputType) || isFloat(outputType)) && (result & RuntimePointer)) {
			result = (result & ~RuntimePointer) | Unsupported;
		}
		if (outputType == Type::ptr && (result & RuntimeInteger)) {
			result = (result & ~RuntimeInteger) | RuntimePointer;
		} else if (outputType != Type::ptr && getBitWith(outputType) < static_cast<int>(sizeof(void*) * 8) &&
		           (result & RuntimePointer)) {
			result = (result & ~RuntimePointer) | Unsupported;
		}
		return result;
	}
	case Op::ReturnOp:
		return inputs.empty() ? uint8_t {0} : source(inputs[0]);
	case Op::SelectOp: {
		const auto* select = operation.dynCast<ir::SelectOperation>();
		if (const auto condition = constantCondition(select->getCondition())) {
			return source(*condition ? select->getTrueValue() : select->getFalseValue());
		}
		return source(select->getTrueValue()) | source(select->getFalseValue());
	}
	case Op::AddOp:
	case Op::SubOp:
	case Op::MulOp:
	case Op::DivOp:
	case Op::ModOp:
	case Op::BinaryComp:
	case Op::ShiftOp:
	case Op::AndOp:
	case Op::OrOp: {
		uint8_t result = 0;
		for (const auto left : {RuntimeInteger, RuntimePointer, Constant, Unsupported, NullPointer}) {
			for (const auto right : {RuntimeInteger, RuntimePointer, Constant, Unsupported, NullPointer}) {
				if (!(source(inputs[0]) & left) || !(source(inputs[1]) & right)) {
					continue;
				}
				if (left == Unsupported || right == Unsupported || left == NullPointer || right == NullPointer) {
					result |= Unsupported;
				} else if (left == RuntimePointer || right == RuntimePointer) {
					const auto op = operation.getOperationType();
					const bool pointerOffset =
					    ((op == Op::AddOp || op == Op::SubOp) && left == RuntimePointer && right != RuntimePointer) ||
					    (op == Op::AddOp && right == RuntimePointer && left != RuntimePointer);
					result |= pointerOffset ? RuntimePointer : Unsupported;
				} else {
					result |= left | right;
				}
			}
		}
		if ((result & RuntimePointer) && ((source(inputs[0]) | source(inputs[1])) & (Constant | PointerOffset))) {
			result |= PointerOffset;
		}
		return result;
	}
	case Op::NegateOp:
	case Op::NotOp: {
		const auto result = source(inputs[0]);
		return (result & (RuntimePointer | NullPointer))
		           ? uint8_t((result & ~(RuntimePointer | NullPointer)) | Unsupported)
		           : result;
	}
	default:
		return Unsupported;
	}
}

} // namespace

bool hasOnlyCacheInvariantScalars(const ir::IRGraph& graph, std::string* rejection) {
	if (rejection != nullptr) {
		rejection->clear();
	}
	const auto reject = [&](const ir::Operation* operation, OperationLocation location, const std::string& cause) {
		if (rejection != nullptr) {
			*rejection = describeOperation(operation, location) + " cause=" + cause;
		}
		return false;
	};
	std::unordered_set<const ir::FunctionOperation*> functions;
	std::unordered_map<const ir::BasicBlock*, const ir::FunctionOperation*> blocks;
	AddressValues arguments;
	AddressValues included;
	std::vector<std::pair<const ir::Operation*, OperationLocation>> operations;
	const auto include = [&](const ir::Operation* operation, OperationLocation location) {
		if (included.insert(operation).second) {
			operations.emplace_back(operation, location);
		}
	};
	for (const auto* function : graph.getFunctionOperations()) {
		if (function == nullptr || function->getBasicBlocks().empty()) {
			return reject(function, {function, nullptr}, "missing_function_body");
		}
		functions.insert(function);
		include(function, {function, nullptr});
		for (const auto* block : function->getBasicBlocks()) {
			if (block == nullptr) {
				return reject(function, {function, nullptr}, "null_basic_block");
			}
			const auto [owner, inserted] = blocks.emplace(block, function);
			if (!inserted && owner->second != function) {
				return reject(function, {function, block}, "shared_basic_block");
			}
			for (const auto* argument : block->getArguments()) {
				if (argument == nullptr) {
					return reject(argument, {function, block}, "null_block_argument");
				}
				arguments.insert(argument);
				include(argument, {function, block});
			}
			for (const auto* operation : block->getOperations()) {
				include(operation, {function, block});
			}
		}
	}
	const auto& table = graph.getFunctionTable();
	const auto targetRejection = [&](ir::FunctionId id) -> std::string {
		if (!table.contains(id)) {
			return "missing_function_target=" + std::to_string(id);
		}
		const auto& target = table.get(id);
		if (target.getId() != id) {
			return "mismatched_function_target=" + std::to_string(id);
		}
		switch (target.getLinkage()) {
		case ir::Linkage::Internal:
			if (!functions.contains(target.getDefinition())) {
				return "internal_definition_outside_graph=" + std::to_string(id);
			}
			break;
		case ir::Linkage::External:
		case ir::Linkage::Intrinsic:
			if (target.getAddress() == nullptr) {
				return "missing_native_address=" + std::to_string(id);
			}
			break;
		default:
			return "unsupported_function_linkage=" + std::to_string(id);
		}
		return {};
	};
	for (std::size_t index = 0; index < operations.size(); ++index) {
		const auto [operation, location] = operations[index];
		if (operation == nullptr) {
			return reject(operation, location, "null_operand");
		}
		const auto inputs = operation->getInputs();
		for (const auto* input : inputs) {
			if (input == nullptr) {
				return reject(operation, location, "null_input");
			}
			include(input, location);
		}
		const auto includeDestructors = [&](const auto& call) {
			for (const auto& destructor : call.getDestructors()) {
				if (destructor.functionPtr == nullptr) {
					return reject(operation, location, "missing_cleanup_function");
				}
				include(destructor.address, location);
			}
			return true;
		};
		std::optional<std::size_t> arity;
		using Op = ir::Operation::OperationType;
		switch (operation->getOperationType()) {
		case Op::ConstIntOp:
			if (!isInteger(operation->getStamp()) ||
			    operation->dynCast<ir::ConstIntOperation>()->getConstantOrigin() != ConstantOrigin::CacheInvariant) {
				return reject(operation, location, "uncertified_scalar");
			}
			arity = 0;
			break;
		case Op::ConstBooleanOp:
			if (operation->getStamp() != Type::b ||
			    operation->dynCast<ir::ConstBooleanOperation>()->getConstantOrigin() !=
			        ConstantOrigin::CacheInvariant) {
				return reject(operation, location, "uncertified_scalar");
			}
			arity = 0;
			break;
		case Op::ConstFloatOp:
			if (!isFloat(operation->getStamp()) ||
			    operation->dynCast<ir::ConstFloatOperation>()->getConstantOrigin() != ConstantOrigin::CacheInvariant) {
				return reject(operation, location, "uncertified_scalar");
			}
			arity = 0;
			break;
		case Op::ConstPtrOp:
			if (operation->dynCast<ir::ConstPtrOperation>()->getValue() != nullptr) {
				return reject(operation, location, "embedded_non_null_pointer");
			}
			arity = 0;
			break;
		case Op::BasicBlockArgument:
			if (!arguments.contains(operation)) {
				return reject(operation, location, "block_argument_outside_graph");
			}
			arity = 0;
			break;
		case Op::FunctionOp:
			if (!functions.contains(operation->dynCast<ir::FunctionOperation>())) {
				return reject(operation, location, "function_outside_graph");
			}
			arity = 0;
			break;
		case Op::FunctionAddressOfOp: {
			const auto cause = targetRejection(operation->dynCast<ir::FunctionAddressOfOperation>()->getCalleeId());
			if (!cause.empty()) {
				return reject(operation, location, cause);
			}
			arity = 0;
			break;
		}
		case Op::CallOp: {
			const auto* call = operation->dynCast<ir::CallOperation>();
			const auto cause = targetRejection(call->getCalleeId());
			if (!cause.empty()) {
				return reject(operation, location, cause);
			}
			const auto& target = table.get(call->getCalleeId());
			const auto parameters = target.getParamTypes();
			if (parameters.size() != inputs.size() || target.getResultType() != operation->getStamp()) {
				return reject(operation, location, "callee_signature_mismatch");
			}
			for (std::size_t argument = 0; argument < inputs.size(); ++argument) {
				if (parameters[argument] != inputs[argument]->getStamp()) {
					return reject(operation, location, "callee_argument_type_mismatch=" + std::to_string(argument));
				}
			}
			if (!includeDestructors(*call)) {
				return false;
			}
			break;
		}
		case Op::IndirectCallOp:
			if (inputs.empty() || inputs.front()->getStamp() != Type::ptr) {
				return reject(operation, location, "invalid_indirect_callee_operand");
			}
			if (!includeDestructors(*operation->dynCast<ir::IndirectCallOperation>())) {
				return false;
			}
			break;
		case Op::BranchOp:
			include(&operation->dynCast<ir::BranchOperation>()->getNextBlockInvocation(), location);
			arity = 0;
			break;
		case Op::IfOp: {
			const auto* branch = operation->dynCast<ir::IfOperation>();
			include(&branch->getTrueBlockInvocation(), location);
			include(&branch->getFalseBlockInvocation(), location);
			arity = 1;
			break;
		}
		case Op::BlockInvocation: {
			const auto* invocation = operation->dynCast<ir::BasicBlockInvocation>();
			const auto owner = blocks.find(invocation->getBlock());
			if (owner == blocks.end() || owner->second != location.function) {
				return reject(operation, location, "branch_target_outside_function");
			}
			const auto& parameters = invocation->getBlock()->getArguments();
			if (parameters.size() != inputs.size()) {
				return reject(operation, location, "branch_argument_count_mismatch");
			}
			for (std::size_t argument = 0; argument < inputs.size(); ++argument) {
				if (parameters[argument]->getStamp() != inputs[argument]->getStamp()) {
					return reject(operation, location, "branch_argument_type_mismatch=" + std::to_string(argument));
				}
			}
			break;
		}
		case Op::RuntimeBindingOp:
		case Op::AllocaOp:
			arity = 0;
			break;
		case Op::CastOp:
		case Op::LoadOp:
		case Op::NotOp:
		case Op::NegateOp:
			arity = 1;
			break;
		case Op::AddOp:
		case Op::SubOp:
		case Op::MulOp:
		case Op::DivOp:
		case Op::ModOp:
		case Op::BinaryComp:
		case Op::ShiftOp:
		case Op::AndOp:
		case Op::OrOp:
		case Op::CompareOp:
		case Op::StoreOp:
			arity = 2;
			break;
		case Op::SelectOp:
			arity = 3;
			break;
		case Op::ReturnOp:
			if (inputs.size() > 1) {
				return reject(operation, location, "invalid_return_arity");
			}
			break;
		default:
			return reject(operation, location, "unsupported_operation");
		}
		if (arity && inputs.size() != *arity) {
			return reject(operation, location, "invalid_operand_count");
		}
	}
	for (std::size_t index = 0; index < table.size(); ++index) {
		const auto cause = targetRejection(static_cast<ir::FunctionId>(index));
		if (!cause.empty()) {
			return reject(nullptr, {}, cause);
		}
	}
	return true;
}

bool containsNonRelocatablePointer(const ir::IRGraph& graph, const std::vector<std::string>& exports,
                                   std::string* rejection) {
	if (rejection != nullptr) {
		rejection->clear();
	}
	OperationLocations operationLocations;
	const auto reject = [&](const ir::Operation* operation, const std::string& cause) {
		if (rejection != nullptr) {
			*rejection = describeOperation(operation, operationLocations.at(operation)) + " cause=" + cause;
		}
		return true;
	};
	AddressSources sources;
	AddressCallees callees;
	AddressValues opaqueCalls;
	AddressValues functionParameters;
	std::unordered_map<const ir::Operation*, const ir::BasicBlock*> operationBlocks;
	std::unordered_map<const ir::BasicBlock*, AddressValues> nonNullValues;
	std::unordered_set<const ir::BasicBlock*> reachable;
	std::vector<const ir::Operation*> operations;
	std::vector<const ir::Operation*> pointerExpressions;
	std::vector<const ir::Operation*> cleanupAddresses;
	std::vector<const ir::Operation*> cleanupCalls;
	struct Edge {
		const ir::Operation* destination;
		const ir::Operation* input;
		const ir::BasicBlock* block;
		bool nonNull = false;
	};
	std::vector<Edge> edges;
	for (const auto* function : graph.getFunctionOperations()) {
		operationLocations.emplace(function, OperationLocation {function, nullptr});
		std::vector<const ir::BasicBlock*> pending {function->getEntryBlock()};
		while (!pending.empty()) {
			const auto* block = pending.back();
			pending.pop_back();
			if (!reachable.insert(block).second || block->getOperations().empty()) {
				continue;
			}
			for (const auto* successor : feasibleSuccessors(*block->getOperations().back())) {
				pending.push_back(successor->getBlock());
			}
		}
		std::unordered_map<const ir::BasicBlock*, std::unordered_set<const ir::BasicBlock*>> predecessors;
		for (const auto* block : function->getBasicBlocks()) {
			if (!block->getOperations().empty()) {
				for (const auto* successor : ir::getSuccessorInvocations(*block->getOperations().back())) {
					predecessors[successor->getBlock()].insert(block);
				}
			}
		}
		const bool validPredecessors = std::ranges::all_of(function->getBasicBlocks(), [&](const auto* block) {
			const std::unordered_set<const ir::BasicBlock*> actual(block->getPredecessors().begin(),
			                                                       block->getPredecessors().end());
			return actual == predecessors[block];
		});
		if (validPredecessors) {
			const ir::Dominators dominators(*function);
			for (const auto* block : function->getBasicBlocks()) {
				if (block->getOperations().empty()) {
					continue;
				}
				const auto* branch = block->getOperations().back()->dynCast<ir::IfOperation>();
				if (branch == nullptr ||
				    branch->getTrueBlockInvocation().getBlock() == branch->getFalseBlockInvocation().getBlock()) {
					continue;
				}
				for (const auto* successor : feasibleSuccessors(*block->getOperations().back())) {
					const auto* target = successor->getBlock();
					const auto* value =
					    nonNullCondition(branch->getValue(), successor == &branch->getTrueBlockInvocation());
					if (value == nullptr || target == block || !dominators.dominates(block, target) ||
					    !std::ranges::all_of(predecessors[target], [&](const auto* predecessor) {
						    return predecessor == block || dominators.dominates(target, predecessor);
					    })) {
						continue;
					}
					for (const auto* dominated : function->getBasicBlocks()) {
						if (dominators.dominates(target, dominated)) {
							nonNullValues[dominated].insert(value);
						}
					}
				}
			}
		}
		sources[function] = 0;
		const bool exported = std::ranges::find(exports, graph.getEmissionName(function)) != exports.end();
		for (auto* block : function->getBasicBlocks()) {
			for (const auto* argument : block->getArguments()) {
				operationLocations.emplace(argument, OperationLocation {function, block});
				if (block == function->getEntryBlock()) {
					functionParameters.insert(argument);
				}
				sources[argument] = exported && block == function->getEntryBlock()
				                        ? (argument->getStamp() == Type::ptr ? RuntimePointer : RuntimeInteger)
				                        : 0;
			}
			for (auto* operation : block->getOperations()) {
				operationLocations.emplace(operation, OperationLocation {function, block});
				sources[operation] = 0;
				operationBlocks.emplace(operation, block);
				operations.push_back(operation);
				if (!reachable.contains(block)) {
					continue;
				}
				if (operation->getOperationType() == ir::Operation::OperationType::ReturnOp) {
					edges.emplace_back(function, operation, block);
				}
				for (const auto* invocation : feasibleSuccessors(*operation)) {
					const auto* branch = operation->dynCast<ir::IfOperation>();
					const auto* nonNull =
					    branch ? nonNullCondition(branch->getValue(), invocation == &branch->getTrueBlockInvocation())
					           : nullptr;
					const auto& arguments = invocation->getBlock()->getArguments();
					for (std::size_t index = 0; index < arguments.size(); ++index) {
						const auto* input = invocation->getArguments()[index];
						edges.emplace_back(arguments[index], input, block, input == nonNull);
					}
				}
			}
		}
	}
	const auto includeInput = [&](const ir::Operation* input, OperationLocation location) {
		operationLocations.try_emplace(input, location);
		if (sources.try_emplace(input, 0).second) {
			operations.push_back(input);
		}
	};
	for (const auto& edge : edges) {
		includeInput(edge.input, operationLocations.at(edge.destination));
	}
	for (std::size_t index = 0; index < operations.size(); ++index) {
		const auto* operation = operations[index];
		for (const auto* input : operation->getInputs()) {
			includeInput(input, operationLocations.at(operation));
		}
		const auto* pointer = operation->dynCast<ir::ConstPtrOperation>();
		if (pointer != nullptr && pointer->getValue() != nullptr) {
			return reject(operation, "embedded_non_null_pointer");
		}
		const auto owner = operationBlocks.find(operation);
		if (owner != operationBlocks.end() && !reachable.contains(owner->second)) {
			continue;
		}
		const auto* block = owner == operationBlocks.end() ? nullptr : owner->second;
		const auto* cast = operation->dynCast<ir::CastOperation>();
		if (operation->getStamp() == Type::ptr &&
		    ((cast != nullptr && cast->getInput()->getStamp() != Type::ptr) ||
		     operation->getOperationType() == ir::Operation::OperationType::AddOp ||
		     operation->getOperationType() == ir::Operation::OperationType::SubOp ||
		     operation->getOperationType() == ir::Operation::OperationType::LoadOp ||
		     operation->getOperationType() == ir::Operation::OperationType::CallOp ||
		     operation->getOperationType() == ir::Operation::OperationType::IndirectCallOp)) {
			pointerExpressions.push_back(operation);
		}
		std::vector<const ir::FunctionOperation*> targets;
		std::span<ir::Operation* const> arguments;
		if (const auto* address = operation->dynCast<ir::FunctionAddressOfOperation>()) {
			if (!graph.getFunctionTable().contains(address->getCalleeId())) {
				return reject(operation, "missing_function_target=" + std::to_string(address->getCalleeId()));
			}
			const auto& target = graph.getFunctionTarget(address->getCalleeId());
			if (target.getLinkage() == ir::Linkage::Internal) {
				if (target.getDefinition() == nullptr) {
					return reject(operation, "missing_internal_definition=" + std::to_string(address->getCalleeId()));
				}
				callees[operation] = {target.getDefinition()};
			}
		} else if (const auto* call = operation->dynCast<ir::CallOperation>()) {
			for (const auto& destructor : call->getDestructors()) {
				includeInput(destructor.address, operationLocations.at(operation));
				cleanupAddresses.push_back(destructor.address);
				cleanupCalls.push_back(operation);
			}
			if (!graph.getFunctionTable().contains(call->getCalleeId())) {
				return reject(operation, "missing_function_target=" + std::to_string(call->getCalleeId()));
			}
			const auto& target = graph.getFunctionTarget(call->getCalleeId());
			if (target.getLinkage() == ir::Linkage::Internal) {
				if (target.getDefinition() == nullptr) {
					return reject(operation, "missing_internal_definition=" + std::to_string(call->getCalleeId()));
				}
				targets.push_back(target.getDefinition());
				arguments = call->getInputArguments();
			} else {
				opaqueCalls.insert(operation);
			}
		} else if (const auto* indirect = operation->dynCast<ir::IndirectCallOperation>()) {
			for (const auto& destructor : indirect->getDestructors()) {
				includeInput(destructor.address, operationLocations.at(operation));
				cleanupAddresses.push_back(destructor.address);
				cleanupCalls.push_back(operation);
			}
			arguments = indirect->getInputArguments();
			std::vector<const ir::Operation*> pending {indirect->getFunctionPtrOperand()};
			AddressValues visited;
			bool unknownTarget = false;
			while (!pending.empty()) {
				const auto* value = pending.back();
				pending.pop_back();
				if (!visited.insert(value).second) {
					continue;
				}
				operationLocations.try_emplace(value, operationLocations.at(operation));
				if (const auto* address = value->dynCast<ir::FunctionAddressOfOperation>()) {
					if (!graph.getFunctionTable().contains(address->getCalleeId())) {
						return reject(value, "missing_function_target=" + std::to_string(address->getCalleeId()));
					}
					const auto& target = graph.getFunctionTarget(address->getCalleeId());
					if (target.getLinkage() == ir::Linkage::Internal) {
						if (target.getDefinition() == nullptr) {
							return reject(value,
							              "missing_internal_definition=" + std::to_string(address->getCalleeId()));
						}
						if (std::ranges::find(targets, target.getDefinition()) == targets.end()) {
							targets.push_back(target.getDefinition());
						}
					} else {
						unknownTarget = true;
					}
				} else if (const auto* select = value->dynCast<ir::SelectOperation>()) {
					if (const auto condition = constantCondition(select->getCondition())) {
						pending.push_back(*condition ? select->getTrueValue() : select->getFalseValue());
					} else {
						pending.push_back(select->getTrueValue());
						pending.push_back(select->getFalseValue());
					}
				} else if (const auto* cast = value->dynCast<ir::CastOperation>();
				           cast && cast->getInput()->getStamp() == Type::ptr) {
					pending.push_back(cast->getInput());
				} else if (value->getOperationType() == ir::Operation::OperationType::BasicBlockArgument &&
				           !functionParameters.contains(value)) {
					bool found = false;
					for (const auto& edge : edges) {
						if (edge.destination == value) {
							pending.push_back(edge.input);
							found = true;
						}
					}
					unknownTarget |= !found;
				} else {
					unknownTarget = true;
				}
			}
			if (unknownTarget || targets.empty()) {
				for (const auto* candidate : graph.getFunctionOperations()) {
					const auto stamp = [](const auto* argument) {
						return argument->getStamp();
					};
					if (candidate->getOutputArg() == operation->getStamp() &&
					    std::ranges::equal(candidate->getEntryBlock()->getArguments(), arguments, {}, stamp, stamp) &&
					    std::ranges::find(targets, candidate) == targets.end()) {
						targets.push_back(candidate);
					}
				}
				opaqueCalls.insert(operation);
			}
		}
		if (!targets.empty()) {
			for (const auto* target : targets) {
				const auto& parameters = target->getEntryBlock()->getArguments();
				if (parameters.size() != arguments.size()) {
					return reject(operation, "callee_argument_count_mismatch function=" + target->getName());
				}
				for (std::size_t index = 0; index < parameters.size(); ++index) {
					edges.emplace_back(parameters[index], arguments[index], block);
				}
			}
			callees.emplace(operation, std::move(targets));
		}
	}
	if (pointerExpressions.empty() && opaqueCalls.empty() && cleanupAddresses.empty()) {
		return false;
	}
	std::vector<const ir::Operation*> escapedValues = cleanupAddresses;
	for (const auto* operation : operations) {
		if (opaqueCalls.contains(operation) || operation->getOperationType() == ir::Operation::OperationType::StoreOp) {
			escapedValues.insert(escapedValues.end(), operation->getInputs().begin(), operation->getInputs().end());
		}
	}
	AddressValues visitedEscapes;
	while (!escapedValues.empty()) {
		const auto* value = escapedValues.back();
		escapedValues.pop_back();
		if (!visitedEscapes.insert(value).second) {
			continue;
		}
		if (value->getOperationType() == ir::Operation::OperationType::FunctionAddressOfOp) {
			if (const auto found = callees.find(value); found != callees.end()) {
				for (const auto* callee : found->second) {
					for (const auto* parameter : callee->getEntryBlock()->getArguments()) {
						sources[parameter] |= parameter->getStamp() == Type::ptr ? RuntimePointer : RuntimeInteger;
					}
				}
			}
		}
		escapedValues.insert(escapedValues.end(), value->getInputs().begin(), value->getInputs().end());
		if (const auto found = callees.find(value); found != callees.end()) {
			escapedValues.insert(escapedValues.end(), found->second.begin(), found->second.end());
		}
		for (const auto& edge : edges) {
			if (edge.destination == value) {
				escapedValues.push_back(edge.input);
			}
		}
	}

	uint8_t memory = 0;
	bool changed;
	do {
		changed = false;
		const auto merge = [&](const ir::Operation* operation, uint8_t value) {
			auto& previous = sources.at(operation);
			const auto combined = previous | value;
			changed |= previous != combined;
			previous = combined;
		};
		for (const auto* operation : operations) {
			const auto owner = operationBlocks.find(operation);
			if (owner != operationBlocks.end() && !reachable.contains(owner->second)) {
				continue;
			}
			const auto* block = owner == operationBlocks.end() ? nullptr : owner->second;
			merge(operation, addressSources(*operation, sources, callees, opaqueCalls, nonNullValues[block], memory));
			const auto previousMemory = memory;
			if (const auto* store = operation->dynCast<ir::StoreOperation>()) {
				const auto value = sources.at(store->getValue());
				memory |= value & (RuntimePointer | PointerOffset | NullPointer);
				if (value & (Constant | Unsupported)) {
					memory |= Unsupported;
				}
			} else if (opaqueCalls.contains(operation)) {
				const auto* call = operation->dynCast<ir::CallOperation>();
				if (call == nullptr || call->getFunctionAttributes().modRefInfo == ModRefInfo::Mod ||
				    call->getFunctionAttributes().modRefInfo == ModRefInfo::ModRef || !call->getDestructors().empty()) {
					memory |= sources.at(operation) & (RuntimePointer | PointerOffset | Unsupported | NullPointer);
				}
			}
			changed |= previousMemory != memory;
		}
		for (const auto& [destination, input, block, nonNull] : edges) {
			merge(destination, nonNull || nonNullValues[block].contains(input) ? sources.at(input) & ~NullPointer
			                                                                   : sources.at(input));
		}
		for (const auto* address : cleanupAddresses) {
			const auto previousMemory = memory;
			memory |= sources.at(address) & RuntimePointer;
			if ((sources.at(address) | memory) & (Constant | Unsupported | PointerOffset)) {
				memory |= Unsupported;
			}
			changed |= previousMemory != memory;
		}
	} while (changed);

	const auto describeValue = [&](const ir::Operation* value) {
		return describeOperation(value, operationLocations.at(value)) +
		       " sources=" + describeAddressSources(sources.at(value));
	};
	const auto callRejection = [&](const ir::Operation* operation) {
		const auto inputs = operation->getInputs();
		if (opaqueCalls.contains(operation)) {
			for (std::size_t index = 0; index < inputs.size(); ++index) {
				if (sources.at(inputs[index]) & (Constant | Unsupported | PointerOffset)) {
					return "opaque_call input[" + std::to_string(index) + "]={" + describeValue(inputs[index]) + "}";
				}
			}
		}
		const auto* call = operation->dynCast<ir::CallOperation>();
		const auto effect = call == nullptr ? ModRefInfo::ModRef : call->getFunctionAttributes().modRefInfo;
		if (opaqueCalls.contains(operation) &&
		    (effect == ModRefInfo::Ref || effect == ModRefInfo::ModRef ||
		     (call != nullptr && !call->getDestructors().empty())) &&
		    (memory & (Unsupported | PointerOffset))) {
			return "opaque_call whole_memory sources=" + describeAddressSources(memory);
		}
		if (const auto found = callees.find(operation); found != callees.end()) {
			for (const auto* callee : found->second) {
				if (sources.at(callee) & (Constant | Unsupported | PointerOffset | NullPointer)) {
					return "callee_return={" + describeValue(callee) + "}";
				}
			}
		}
		return "call_result sources=" + describeAddressSources(sources.at(operation));
	};
	if (!cleanupAddresses.empty() && (memory & Unsupported)) {
		for (std::size_t index = 0; index < cleanupAddresses.size(); ++index) {
			if (sources.at(cleanupAddresses[index]) & (Constant | Unsupported | PointerOffset)) {
				return reject(cleanupCalls[index], "cleanup input={" + describeValue(cleanupAddresses[index]) + "}");
			}
		}
		return reject(cleanupCalls.front(), "cleanup whole_memory sources=" + describeAddressSources(memory));
	}
	for (const auto* operation : operations) {
		if (!opaqueCalls.contains(operation)) {
			continue;
		}
		const auto* call = operation->dynCast<ir::CallOperation>();
		if (call != nullptr && call->getFunctionAttributes().modRefInfo == ModRefInfo::NoModRef &&
		    call->getDestructors().empty()) {
			continue;
		}
		if (sources.at(operation) & Unsupported) {
			return reject(operation, callRejection(operation));
		}
	}
	for (const auto* expression : pointerExpressions) {
		const auto provenance = sources.at(expression);
		const auto type = expression->getOperationType();
		const bool nullable = type == ir::Operation::OperationType::LoadOp ||
		                      type == ir::Operation::OperationType::CallOp ||
		                      type == ir::Operation::OperationType::IndirectCallOp;
		if (provenance == 0 || (provenance & (Constant | Unsupported)) || (!nullable && (provenance & NullPointer))) {
			std::string cause = "pointer_expression sources=" + describeAddressSources(provenance);
			if (type == ir::Operation::OperationType::LoadOp && (memory & (Constant | Unsupported))) {
				cause += " whole_memory sources=" + describeAddressSources(memory);
			} else if (type == ir::Operation::OperationType::CallOp ||
			           type == ir::Operation::OperationType::IndirectCallOp) {
				cause += " " + callRejection(expression);
			} else {
				const auto inputs = expression->getInputs();
				for (std::size_t index = 0; index < inputs.size(); ++index) {
					cause += " input[" + std::to_string(index) + "]={" + describeValue(inputs[index]) + "}";
				}
			}
			return reject(expression, cause);
		}
	}
	return false;
}

namespace {

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

	bool scalarCertificate = false;
	std::string scalarRejection;
	auto ir = compiler.compileToIR(
	    functions, moduleOptions, statistics, compiler.irOptimizationLevel({"mlir"}),
	    [&](const ir::IRGraph& graph) { scalarCertificate = hasOnlyCacheInvariantScalars(graph, &scalarRejection); });
	if (statistics != nullptr) {
		statistics->set("cache.scalarCertificate", int64_t {scalarCertificate ? 1 : 0});
		statistics->set("cache.scalarRejection", std::move(scalarRejection));
	}
	std::string rejection;
	if (!scalarCertificate && containsNonRelocatablePointer(*ir, exports, &rejection)) {
		if (statistics != nullptr) {
			statistics->set("cache.rejection", std::move(rejection));
		}
		markCacheState(statistics, objectState, bytecodeState, true, "non_relocatable_pointer");
		return compiler.compileIR(ir, "mlir", moduleOptions, statistics);
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
