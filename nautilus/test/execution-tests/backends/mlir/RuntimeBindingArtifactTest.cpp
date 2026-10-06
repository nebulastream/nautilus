#include "nautilus/config.hpp"

#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_exception.hpp"
#include "catch2/matchers/catch_matchers_string.hpp"
#include "nautilus/Artifact.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/RuntimeBinding.hpp"
#include "nautilus/compiler/artifact/ArtifactCodec.hpp"
#include "nautilus/compiler/artifact/ArtifactSupport.hpp"
#include "nautilus/compiler/backends/mlir/MLIRArtifactValidation.hpp"
#include "nautilus/compiler/backends/mlir/MLIRCompilationBackend.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/select.hpp"
#include "nautilus/static.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/val_std.hpp"
#include <algorithm>
#include <array>
#include <barrier>
#include <bit>
#include <cstdint>
#include <exception>
#include <functional>
#include <llvm/BinaryFormat/ELF.h>
#include <llvm/Object/ELFObjectFile.h>
#include <llvm/Support/MemoryBuffer.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/IR/Verifier.h>
#include <mlir/Parser/Parser.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace nautilus::engine {
namespace {

void requireBindingArtifactSupport() {
#if !defined(__linux__) || !defined(__x86_64__)
	SKIP("Standalone runtime binding artifacts require the Linux x86-64 ELF producer");
#else
	REQUIRE(artifact::isSupported());
#endif
}

void requireBindingArtifactUnwindSupport() {
	requireBindingArtifactSupport();
#if !__has_include(<unwind.h>) || defined(__arm__) || defined(__USING_SJLJ_EXCEPTIONS__)
	SKIP("Runtime binding artifact cleanup requires native DWARF unwinding");
#endif
}

Options bindingArtifactOptions() {
	Options options;
	options.setOption("engine.backend", std::string("mlir"));
	options.setOption("mlir.enableMultithreading", false);
	options.setOption("ir.runOptimizationPasses", true);
	options.setOption("ir.enableLocalCSE", true);
	return options;
}

void resealRuntimeBindingDescriptor(artifact::ModuleArtifact& value) {
	value.descriptorDigest = artifact::detail::digest(artifact::detail::encodeDescriptor(value.descriptor));
}

std::string runtimeBindingEnvelope(const artifact::ModuleArtifact& original, std::string_view descriptorBytes) {
	const auto envelope = artifact::encode(original);
	artifact::detail::Reader reader(envelope);
	artifact::detail::Writer writer;
	writer.string(reader.string());
	writer.string(descriptorBytes);
	writer.string(artifact::detail::digest(descriptorBytes));
	writer.string(original.object);
	writer.string(original.bytecode);
	return writer.take();
}

std::size_t runtimeBindingRecordOffset(std::string_view descriptorBytes, std::string_view value) {
	artifact::detail::Writer writer;
	writer.string(value);
	const auto record = writer.take();
	const auto offset = descriptorBytes.find(record);
	REQUIRE(offset != std::string_view::npos);
	REQUIRE(descriptorBytes.find(record, offset + 1) == std::string_view::npos);
	return offset;
}

int64_t bindingArtifactProxy(int64_t value) noexcept {
	return value + 1;
}

int64_t bindingArtifactCheckedProxy(int64_t value) {
	if (value < 0) {
		throw std::runtime_error("standalone bound artifact proxy");
	}
	return value;
}

void bindingArtifactScalarCleanup(int64_t* value) noexcept {
	++*value;
}

int64_t bindingArtifactConsumeMemory(uintptr_t* address) noexcept {
	return *reinterpret_cast<int64_t*>(*address);
}

int64_t bindingArtifactConsumeCallback(uintptr_t (*callback)()) {
	return *reinterpret_cast<int64_t*>(callback());
}

struct RuntimeBindingArtifactFixture {
	RuntimeBindingArtifactFixture() {
		requireBindingArtifactSupport();
		auto observed = bindings.bind<int64_t>("b3/right", &right);
		auto state = bindings.bind<int64_t>("b3/left", &left);
		(void) bindings.bind<int64_t>("b3/unused", &unused);
		options.setRuntimeBindings(bindings);
		NautilusEngine engine(options);
		auto builder = engine.createModule();
		builder.registerFunction<val<int64_t>(val<int64_t>)>("execute", [state, observed, this](val<int64_t> delta) {
			++wrappers;
			*state.get() = val<int64_t>(*state.get()) + delta;
			return invoke(bindingArtifactProxy, val<int64_t>(*observed.get()));
		});
		original = builder.createArtifact();
		REQUIRE(wrappers > 0);
		REQUIRE(original.descriptor.version == 2);
		REQUIRE(original.descriptor.bindingSchema == bindings.schemaEntries());
		REQUIRE(original.descriptor.bindingSchema.size() == 3);
		REQUIRE_FALSE(original.descriptor.imports.empty());
		REQUIRE_NOTHROW(artifact::detail::validateDescriptor(original));
		REQUIRE(left == 42);
		REQUIRE(right == 97);
		REQUIRE(unused == 113);
	}

	void requireMalformed(artifact::ModuleArtifact& changed, const std::string& expected) {
		const auto coldWrappers = wrappers;
		resealRuntimeBindingDescriptor(changed);
		const auto descriptorBytes = artifact::detail::encodeDescriptor(changed.descriptor);
		REQUIRE(changed.descriptorDigest == artifact::detail::digest(descriptorBytes));
		REQUIRE(changed.descriptorDigest != original.descriptorDigest);
		REQUIRE(changed.object == original.object);
		REQUIRE(changed.bytecode == original.bytecode);
		REQUIRE_NOTHROW(artifact::detail::validatePayload(changed.object, changed.descriptor.objectDigest));
		REQUIRE_NOTHROW(artifact::detail::validatePayload(changed.bytecode, changed.descriptor.bytecodeDigest));
		const auto before = changed.descriptor;
		REQUIRE_THROWS_WITH(artifact::detail::validateDescriptor(changed), expected);
		REQUIRE_THROWS_WITH(artifact::detail::decodeDescriptor(descriptorBytes), expected);
		REQUIRE_THROWS_WITH(artifact::decode(runtimeBindingEnvelope(original, descriptorBytes)), expected);
		REQUIRE_THROWS_WITH(artifact::loadNative(changed, options), expected);
		REQUIRE_THROWS_WITH(artifact::loadBytecode(changed, options), expected);
		REQUIRE(changed.descriptor == before);
		REQUIRE(changed.descriptorDigest == artifact::detail::digest(descriptorBytes));
		REQUIRE(changed.object == original.object);
		REQUIRE(changed.bytecode == original.bytecode);
		REQUIRE(wrappers == coldWrappers);
		REQUIRE(left == 42);
		REQUIRE(right == 97);
		REQUIRE(unused == 113);
		REQUIRE_FALSE(tracing::inTracer());
	}

	int64_t left = 42;
	int64_t right = 97;
	int64_t unused = 113;
	int wrappers = 0;
	RuntimeBindings bindings;
	Options options = bindingArtifactOptions();
	artifact::ModuleArtifact original;
};

artifact::ModuleArtifact emitBindingAliasArtifact(int64_t* left, int64_t* right, int& wrappers) {
	RuntimeBindings bindings;
	auto state = bindings.bind<int64_t>("b3/left", left);
	auto observed = bindings.bind<int64_t>("b3/right", right);
	auto options = bindingArtifactOptions();
	options.setRuntimeBindings(bindings);
	NautilusEngine engine(options);
	auto builder = engine.createModule();
	builder.registerFunction<val<int64_t>(val<int64_t>)>("execute", [state, observed, &wrappers](val<int64_t> delta) {
		++wrappers;
		val<int64_t> before = *observed.get();
		*state.get() = val<int64_t>(*state.get()) + delta;
		return val<int64_t>(*observed.get()) - before;
	});
	return builder.createArtifact();
}

struct BindingArtifactCleanupState {
	int64_t total = 0;
	int64_t live = 0;
	int64_t calls = 0;
	int64_t cleanupOrder = 0;
	bool operator==(const BindingArtifactCleanupState&) const = default;
};

void startBindingArtifactResource(BindingArtifactCleanupState* state) noexcept {
	++state->live;
}

template <int32_t Marker>
void finishBindingArtifactResource(BindingArtifactCleanupState* state) noexcept {
	--state->live;
	state->cleanupOrder = state->cleanupOrder * 10 + Marker;
}

template <int32_t Marker>
class BindingArtifactNativeGuard {
public:
	explicit BindingArtifactNativeGuard(val<BindingArtifactCleanupState*> state) : state_(std::move(state)) {
		invoke(startBindingArtifactResource, state_);
		if (tracing::inTracer()) {
			tracing::registerDestructor(state_.getState(),
			                            reinterpret_cast<void*>(finishBindingArtifactResource<Marker>));
		}
	}

	~BindingArtifactNativeGuard() noexcept {
		if (tracing::inTracer()) {
			tracing::unregisterDestructor(state_.getState());
		}
		invoke(finishBindingArtifactResource<Marker>, state_);
	}

	BindingArtifactNativeGuard(const BindingArtifactNativeGuard&) = delete;
	BindingArtifactNativeGuard& operator=(const BindingArtifactNativeGuard&) = delete;

private:
	val<BindingArtifactCleanupState*> state_;
};

template <int32_t Marker>
struct BindingArtifactAllocatedResource {
	explicit BindingArtifactAllocatedResource(BindingArtifactCleanupState* state) noexcept : state(state) {
		startBindingArtifactResource(state);
	}

	~BindingArtifactAllocatedResource() noexcept {
		finishBindingArtifactResource<Marker>(state);
	}

	BindingArtifactCleanupState* state;
};

int64_t bindingArtifactGuardedProxy(BindingArtifactCleanupState* state, int64_t value) {
	++state->calls;
	if (value < 0) {
		throw std::runtime_error("standalone binding cleanup");
	}
	state->total += value;
	return state->total;
}

artifact::ModuleArtifact emitBindingCleanupArtifact(BindingArtifactCleanupState& state, int& wrappers,
                                                    const Options& baseOptions) {
	RuntimeBindings bindings;
	auto binding = bindings.bind<BindingArtifactCleanupState>("b3/guarded", &state);
	auto options = baseOptions;
	options.setRuntimeBindings(bindings);
	NautilusEngine engine(options);
	auto builder = engine.createModule();
	builder.registerFunction<val<int64_t>(val<int64_t>)>("execute", [binding, &wrappers](val<int64_t> value) {
		++wrappers;
		auto address = binding.get();
		BindingArtifactNativeGuard<1> outer(address);
		BindingArtifactNativeGuard<2> inner(address);
		return invoke(bindingArtifactGuardedProxy, address, value);
	});
	return builder.createArtifact();
}

ModuleFunction<int64_t(int64_t)> detachedBindingCleanupFunction(const artifact::ModuleArtifact& value,
                                                                BindingArtifactCleanupState& state, bool bytecode,
                                                                const Options& baseOptions) {
	RuntimeBindings bindings;
	(void) bindings.bind<BindingArtifactCleanupState>("b3/guarded", &state);
	auto options = baseOptions;
	options.setRuntimeBindings(bindings);
	auto module = bytecode ? artifact::loadBytecode(value, options) : artifact::loadNative(value, options);
	return module.getFunction<int64_t(int64_t)>("execute");
}

std::string defineBindingInExistingObjectData(std::string_view bytes, const std::string& bindingSymbol) {
	auto object = llvm::object::ObjectFile::createObjectFile(
	    llvm::MemoryBufferRef(llvm::StringRef(bytes.data(), bytes.size()), "runtime-binding-collision.o"));
	REQUIRE(static_cast<bool>(object));
	const auto* elf = llvm::dyn_cast<llvm::object::ELF64LEObjectFile>(object->get());
	REQUIRE(elf != nullptr);
	REQUIRE(elf->isRelocatableObject());
	REQUIRE(elf->getArch() == llvm::Triple::x86_64);
	std::optional<llvm::object::SectionRef> data;
	for (const auto& section : elf->sections()) {
		const llvm::object::ELFSectionRef native(section);
		if (native.getType() == llvm::ELF::SHT_PROGBITS && (native.getFlags() & llvm::ELF::SHF_ALLOC) &&
		    !(native.getFlags() & llvm::ELF::SHF_EXECINSTR) && section.getSize() > 0) {
			data = section;
			break;
		}
	}
	REQUIRE(data.has_value());
	REQUIRE(data->getIndex() > llvm::ELF::SHN_UNDEF);
	REQUIRE(data->getIndex() < llvm::ELF::SHN_LORESERVE);
	for (const auto& symbol : elf->symbols()) {
		auto name = symbol.getName();
		REQUIRE(static_cast<bool>(name));
		if (*name != bindingSymbol) {
			continue;
		}
		auto raw = elf->getSymbol(symbol.getRawDataRefImpl());
		REQUIRE(static_cast<bool>(raw));
		REQUIRE((*raw)->isUndefined());
		REQUIRE((*raw)->getBinding() == llvm::ELF::STB_GLOBAL);
		auto changed = **raw;
		changed.setType(llvm::ELF::STT_OBJECT);
		changed.st_shndx = static_cast<uint16_t>(data->getIndex());
		const auto offset = reinterpret_cast<const char*>(*raw) - bytes.data();
		REQUIRE(offset >= 0);
		REQUIRE(static_cast<std::size_t>(offset) + sizeof(changed) <= bytes.size());
		std::string result(bytes);
		result.replace(static_cast<std::size_t>(offset), sizeof(changed), reinterpret_cast<const char*>(&changed),
		               sizeof(changed));
		return result;
	}
	throw std::runtime_error("The emitted native object is missing its real runtime binding symbol");
}

} // namespace

TEST_CASE_METHOD(RuntimeBindingArtifactFixture, "Standalone bound artifact schemas reject resealed malformed records",
                 "[runtime-bindings][artifact][mlir][B3][validation]") {
	const std::vector<std::pair<std::string, std::function<void(artifact::Descriptor&)>>> changes {
	    {"empty identity",
	     [](auto& descriptor) {
		     auto& entry = descriptor.bindingSchema.front();
		     entry.identity.clear();
		     entry.symbol = runtime_binding::symbolName(entry.identity);
	     }},
	    {"unsorted identities", [](auto& descriptor) { std::ranges::reverse(descriptor.bindingSchema); }},
	    {"duplicate identity", [](auto& descriptor) { descriptor.bindingSchema[1] = descriptor.bindingSchema[0]; }},
	    {"identity and symbol disagree", [](auto& descriptor) { descriptor.bindingSchema.front().symbol += "_wrong"; }},
	    {"empty type", [](auto& descriptor) { descriptor.bindingSchema.front().type.clear(); }},
	    {"NUL in type", [](auto& descriptor) { descriptor.bindingSchema.front().type.push_back('\0'); }},
	    {"export collides with a binding",
	     [](auto& descriptor) { descriptor.exports.front().name = descriptor.bindingSchema.front().symbol; }},
	    {"native import collides with a binding",
	     [](auto& descriptor) { descriptor.imports.front().symbol = descriptor.bindingSchema.front().symbol; }},
	};
	for (const auto& [name, change] : changes) {
		CAPTURE(name);
		auto changed = original;
		change(changed.descriptor);
		requireMalformed(changed, name == "native import collides with a binding"
		                              ? "Invalid artifact import descriptor"
		                              : "Invalid artifact runtime binding schema");
	}
}

TEST_CASE_METHOD(RuntimeBindingArtifactFixture,
                 "Standalone bound artifacts require complete type-compatible load schemas",
                 "[runtime-bindings][artifact][mlir][B3][validation]") {
	const auto coldWrappers = wrappers;
	for (const std::string_view mismatch : {"missing binding", "missing unused binding", "same-sized different type",
	                                        "absent load registry", "extra load binding"}) {
		CAPTURE(mismatch);
		auto changed = original;
		auto loadOptions = options;
		if (mismatch == "missing binding") {
			changed.descriptor.bindingSchema.erase(changed.descriptor.bindingSchema.begin());
		} else if (mismatch == "missing unused binding") {
			changed.descriptor.bindingSchema.pop_back();
		} else if (mismatch == "same-sized different type") {
			changed.descriptor.bindingSchema.front().type = runtime_binding::typeSchema<uint64_t>();
		} else if (mismatch == "absent load registry") {
			loadOptions.setRuntimeBindings(RuntimeBindings {});
		} else {
			auto extra = bindings;
			(void) extra.bind<int64_t>("b3/extra", &unused);
			loadOptions.setRuntimeBindings(extra);
		}
		resealRuntimeBindingDescriptor(changed);
		REQUIRE_NOTHROW(artifact::detail::validateDescriptor(changed));
		REQUIRE_NOTHROW(artifact::detail::validatePayload(changed.object, changed.descriptor.objectDigest));
		REQUIRE_NOTHROW(artifact::detail::validatePayload(changed.bytecode, changed.descriptor.bytecodeDigest));
		const auto encoded = artifact::encode(changed);
		const auto decoded = artifact::decode(encoded);
		REQUIRE(decoded.descriptor == changed.descriptor);
		REQUIRE_THROWS_WITH(artifact::loadNative(decoded, loadOptions), "Artifact runtime binding schema mismatch");
		REQUIRE_THROWS_WITH(artifact::loadBytecode(decoded, loadOptions), "Artifact runtime binding schema mismatch");
		REQUIRE(artifact::encode(changed) == encoded);
		REQUIRE(wrappers == coldWrappers);
		REQUIRE(left == 42);
		REQUIRE(right == 97);
		REQUIRE(unused == 113);
		REQUIRE_FALSE(tracing::inTracer());
	}
}

TEST_CASE_METHOD(RuntimeBindingArtifactFixture,
                 "Standalone bound artifact codec bounds resealed binding counts and fields",
                 "[runtime-bindings][artifact][mlir][B3][codec]") {
	const auto pristine = artifact::detail::encodeDescriptor(original.descriptor);
	const auto& entry = original.descriptor.bindingSchema.front();
	const auto identityOffset = runtimeBindingRecordOffset(pristine, entry.identity);
	REQUIRE(identityOffset >= sizeof(uint32_t));
	const std::array<std::pair<std::size_t, std::string>, 3> fields {{
	    {identityOffset, entry.identity},
	    {identityOffset + sizeof(uint64_t) + entry.identity.size(), entry.type},
	    {identityOffset + 2 * sizeof(uint64_t) + entry.identity.size() + entry.type.size(), entry.symbol},
	}};
	const auto requireDecodeFailure = [&](const std::string& descriptorBytes, const std::string& expected) {
		const auto envelope = runtimeBindingEnvelope(original, descriptorBytes);
		artifact::detail::Reader reader(envelope);
		(void) reader.string();
		REQUIRE(reader.string() == descriptorBytes);
		REQUIRE(reader.string() == artifact::detail::digest(descriptorBytes));
		REQUIRE(artifact::detail::digest(descriptorBytes) != original.descriptorDigest);
		REQUIRE_THROWS_WITH(artifact::detail::decodeDescriptor(descriptorBytes), expected);
		REQUIRE_THROWS_WITH(artifact::decode(envelope), expected);
	};
	for (const bool exceedsLimit : {false, true}) {
		CAPTURE(exceedsLimit);
		REQUIRE(pristine.size() < artifact::detail::MAX_RECORD_COUNT);
		artifact::detail::Writer replacement;
		replacement.u32(exceedsLimit ? artifact::detail::MAX_RECORD_COUNT + 1 : static_cast<uint32_t>(pristine.size()));
		const auto count = replacement.take();
		auto changed = pristine;
		changed.replace(identityOffset - sizeof(uint32_t), count.size(), count);
		requireDecodeFailure(changed, "Invalid artifact record count");
	}
	for (const auto& [offset, field] : fields) {
		CAPTURE(offset, field);
		artifact::detail::Writer replacement;
		replacement.u64(artifact::detail::MAX_ARTIFACT_SIZE + 1);
		const auto length = replacement.take();
		auto changed = pristine;
		changed.replace(offset, length.size(), length);
		requireDecodeFailure(changed, "Invalid artifact string length");
		requireDecodeFailure(pristine.substr(0, offset + sizeof(uint64_t) - 1), "Truncated artifact record");
		REQUIRE_FALSE(field.empty());
		requireDecodeFailure(pristine.substr(0, offset + sizeof(uint64_t) + field.size() - 1),
		                     "Invalid artifact string length");
	}
}

TEST_CASE("Standalone native and bytecode bound loads concurrently retain distinct alias environments",
          "[runtime-bindings][artifact][mlir][B3][concurrent][lifetime]") {
	requireBindingArtifactSupport();
	std::array<int, 2> wrappers {};
	std::vector<runtime_binding::SchemaEntry> emittedSchema;
	for (std::size_t producer = 0; producer < wrappers.size(); ++producer) {
		CAPTURE(producer);
		const auto beforeWrappers = wrappers;
		const auto original = [&] {
			int64_t left = 10, right = 100;
			auto value = emitBindingAliasArtifact(&left, producer == 0 ? &right : &left, wrappers[producer]);
			REQUIRE(left == 10);
			REQUIRE(right == 100);
			return artifact::decode(artifact::encode(value));
		}();
		REQUIRE(wrappers[producer] > beforeWrappers[producer]);
		REQUIRE(wrappers[1 - producer] == beforeWrappers[1 - producer]);
		if (producer == 0) {
			emittedSchema = original.descriptor.bindingSchema;
		}
		REQUIRE(original.descriptor.bindingSchema == emittedSchema);
		const auto coldWrappers = wrappers;
		struct Storage {
			int64_t left;
			int64_t right;
		};
		std::array<Storage, 4> storage {{{10, 101}, {20, 211}, {30, 307}, {40, 401}}};
		std::array<std::optional<ModuleFunction<int64_t(int64_t)>>, 4> functions;
		std::array<const compiler::Executable*, 4> executables {};
		std::array<std::exception_ptr, 4> errors {};
		std::array<bool, 4> schemasMatch {}, tracerCleared {};
		std::barrier start(4);
		std::vector<std::jthread> threads;
		for (std::size_t index = 0; index < storage.size(); ++index) {
			threads.emplace_back([&, index] {
				bool arrived = false;
				try {
					RuntimeBindings bindings;
					(void) bindings.bind<int64_t>("b3/right",
					                              index % 2 == 0 ? &storage[index].right : &storage[index].left);
					(void) bindings.bind<int64_t>("b3/left", &storage[index].left);
					auto options = bindingArtifactOptions();
					options.setRuntimeBindings(bindings);
					schemasMatch[index] = bindings.schemaEntries() == original.descriptor.bindingSchema;
					arrived = true;
					start.arrive_and_wait();
					auto loaded =
					    index < 2 ? artifact::loadNative(original, options) : artifact::loadBytecode(original, options);
					executables[index] = loaded.getExecutable();
					functions[index].emplace(loaded.getFunction<int64_t(int64_t)>("execute"));
					tracerCleared[index] = !tracing::inTracer();
				} catch (...) {
					if (!arrived) {
						start.arrive_and_drop();
					}
					errors[index] = std::current_exception();
				}
			});
		}
		threads.clear();
		REQUIRE(wrappers == coldWrappers);
		for (std::size_t index = 0; index < storage.size(); ++index) {
			CAPTURE(index);
			if (errors[index]) {
				std::rethrow_exception(errors[index]);
			}
			REQUIRE(schemasMatch[index]);
			REQUIRE(tracerCleared[index]);
			REQUIRE(functions[index].has_value());
			REQUIRE(executables[index] != nullptr);
			for (std::size_t other = 0; other < index; ++other) {
				REQUIRE(executables[index] != executables[other]);
			}
			const auto before = storage[index];
			REQUIRE((*functions[index])(7) == (index % 2 == 0 ? 0 : 7));
			REQUIRE(storage[index].left == before.left + 7);
			REQUIRE(storage[index].right == before.right);
		}
		std::array<int, 4> failures {};
		for (std::size_t index = 0; index < storage.size(); ++index) {
			threads.emplace_back([&, index] {
				try {
					for (int iteration = 0; iteration < 64; ++iteration) {
						if ((*functions[index])(3) != (index % 2 == 0 ? 0 : 3)) {
							++failures[index];
						}
					}
				} catch (...) {
					errors[index] = std::current_exception();
				}
			});
		}
		threads.clear();
		for (std::size_t index = 0; index < storage.size(); ++index) {
			CAPTURE(index);
			if (errors[index]) {
				std::rethrow_exception(errors[index]);
			}
			REQUIRE(failures[index] == 0);
			REQUIRE(storage[index].left == static_cast<int64_t>((index + 1) * 10) + 7 + 64 * 3);
			REQUIRE((*functions[index])(-2) == (index % 2 == 0 ? 0 : -2));
		}
		REQUIRE(wrappers == coldWrappers);
		REQUIRE_FALSE(tracing::inTracer());
	}
}

TEST_CASE("Standalone valid bindings never waive strict preoptimization scalar and pointer audits",
          "[runtime-bindings][artifact][mlir][B3][preflight]") {
	requireBindingArtifactSupport();
	int64_t captured = 19, value = 41;
	const auto encoded = reinterpret_cast<uintptr_t>(&captured);
	uintptr_t scratch = encoded;
	RuntimeBindings bindings;
	auto state = bindings.bind<int64_t>("b3/state", &value);
	auto slot = bindings.bind<uintptr_t>("b3/scratch", &scratch);
	for (const std::string_view scheduling :
	     {"default", "passes disabled", "optimization disabled", "eight iterations"}) {
		for (const bool fold : {false, true}) {
			auto options = bindingArtifactOptions();
			options.setRuntimeBindings(bindings);
			options.setOption("engine.foldStaticConstants", fold);
			if (scheduling == "passes disabled") {
				options.setOption("ir.runPasses", false);
			} else if (scheduling == "optimization disabled") {
				options.setOption("ir.runOptimizationPasses", false);
			} else if (scheduling == "eight iterations") {
				options.setOption("ir.maxPipelineIterations", 8);
			}
			NautilusEngine engine(options);
			int wrappers = 0;
			NautilusFunction callback {"b3_encoded_callback",
			                           [encoded] { return val<uintptr_t>(encoded) + cacheLiteral<uintptr_t {0}>(); }};
			for (const std::string_view unsafe :
			     {"raw scalar", "raw boolean", "raw floating", "dead scalar", "raw captured pointer", "encoded address",
			      "folded zero", "memory", "partial bytes", "callback", "direct raw cleanup", "indirect raw cleanup",
			      "direct encoded cleanup", "indirect encoded cleanup", "direct folded cleanup",
			      "indirect folded cleanup"}) {
				CAPTURE(scheduling, fold, unsafe);
				auto builder = engine.createModule();
				builder.registerFunction<val<int64_t>(val<int64_t>, val<int64_t (*)(int64_t)>)>(
				    "unsafe",
				    [=, &captured, &callback, &wrappers](val<int64_t> input,
				                                         val<int64_t (*)(int64_t)> indirect) -> val<int64_t> {
					    ++wrappers;
					    val<int64_t> bound = *state.get();
					    if (unsafe == "raw scalar") {
						    return bound + input + int64_t {7};
					    }
					    if (unsafe == "raw boolean") {
						    return select(val<bool>(true), bound + input, bound);
					    }
					    if (unsafe == "raw floating") {
						    return bound + input + static_cast<val<int64_t>>(val<double>(1.25));
					    }
					    if (unsafe == "dead scalar") {
						    return select(cacheLiteral<true>(), bound + input, val<int64_t>(7));
					    }
					    if (unsafe == "raw captured pointer") {
						    return bound + val<int64_t>(*val<int64_t*>(&captured));
					    }
					    if (unsafe == "encoded address") {
						    return bound + static_cast<val<int64_t>>(val<uintptr_t>(encoded));
					    }
					    if (unsafe == "folded zero") {
						    return val<int64_t>(*(state.get() + 0)) + input;
					    }
					    if (unsafe == "memory") {
						    *slot.get() = val<uintptr_t>(encoded) + cacheLiteral<uintptr_t {0}>();
						    return bound + invoke(bindingArtifactConsumeMemory, slot.get());
					    }
					    if (unsafe == "partial bytes") {
						    auto bytes = static_cast<val<uint8_t*>>(slot.get());
						    const auto parts = std::bit_cast<std::array<uint8_t, sizeof(uintptr_t)>>(encoded);
						    for (static_val<std::size_t> index = 0; index < sizeof(uintptr_t) / 2; ++index) {
							    bytes[cacheInvariant(static_cast<std::size_t>(index))] = parts[index];
						    }
						    return bound + invoke(bindingArtifactConsumeMemory, slot.get());
					    }
					    if (unsafe == "callback") {
						    return bound + invoke(bindingArtifactConsumeCallback, callback.getFuncPtr());
					    }
					    val<int64_t*> cleanupOnly(nullptr);
					    if (unsafe.ends_with("raw cleanup")) {
						    cleanupOnly = val<int64_t*>(&captured);
					    } else if (unsafe.ends_with("folded cleanup")) {
						    cleanupOnly = state.get() + 0;
					    } else {
						    cleanupOnly = val<uintptr_t>(encoded) + cacheLiteral<uintptr_t {0}>();
					    }
					    tracing::registerDestructor(cleanupOnly.getState(),
					                                reinterpret_cast<void*>(bindingArtifactScalarCleanup));
					    auto result = unsafe.starts_with("indirect") ? indirect(input)
					                                                 : invoke(bindingArtifactCheckedProxy, input);
					    tracing::unregisterDestructor(cleanupOnly.getState());
					    return bound + result;
				    });
				const auto reason = unsafe == "raw captured pointer" || unsafe.ends_with("raw cleanup")
				                        ? "embedded_non_null_pointer"
				                        : "uncertified_scalar";
				const auto beforeWrappers = wrappers;
				REQUIRE_THROWS_MATCHES(
				    builder.createArtifact(), RuntimeException,
				    Catch::Matchers::MessageMatches(Catch::Matchers::StartsWith("Artifact preflight failed:") &&
				                                    Catch::Matchers::ContainsSubstring(reason)));
				REQUIRE(wrappers > beforeWrappers);
				REQUIRE_FALSE(tracing::inTracer());
				REQUIRE(captured == 19);
				REQUIRE(value == 41);
				REQUIRE(scratch == encoded);
			}
			const auto rejectedWrappers = wrappers;
			int safeWrappers = 0;
			auto safe = engine.createModule();
			safe.registerFunction<val<int64_t>()>("safe", [state, &safeWrappers] {
				++safeWrappers;
				return val<int64_t>(*(state.get() + cacheLiteral<std::size_t {0}>())) + cacheLiteral<int64_t {7}>();
			});
			const auto recovered = safe.createArtifact();
			REQUIRE(safeWrappers > 0);
			const auto coldSafeWrappers = safeWrappers;
			for (const bool bytecode : {false, true}) {
				auto loaded =
				    bytecode ? artifact::loadBytecode(recovered, options) : artifact::loadNative(recovered, options);
				REQUIRE(loaded.getFunction<int64_t()>("safe")() == 48);
				REQUIRE(wrappers == rejectedWrappers);
				REQUIRE(safeWrappers == coldSafeWrappers);
				REQUIRE_FALSE(tracing::inTracer());
			}
		}
	}
}

TEST_CASE("Standalone bound cleanup artifacts unwind independently after native and bytecode rebinds",
          "[runtime-bindings][artifact][mlir][B3][cleanup][lifetime]") {
	requireBindingArtifactUnwindSupport();
	for (const bool passes : {false, true}) {
		CAPTURE(passes);
		auto options = bindingArtifactOptions();
		options.setOption("ir.runPasses", passes);
		BindingArtifactCleanupState first {10}, nativeState {100}, bytecodeState {1000};
		int wrappers = 0;
		const auto original = artifact::decode(artifact::encode(emitBindingCleanupArtifact(first, wrappers, options)));
		REQUIRE(wrappers > 0);
		const auto coldWrappers = wrappers;
		REQUIRE(original.descriptor.bindingSchema.size() == 1);
		REQUIRE(std::ranges::any_of(original.descriptor.imports,
		                            [](const auto& entry) { return entry.symbol == "__gxx_personality_v0"; }));
		::mlir::MLIRContext context;
		context.disableMultithreading();
		context.loadDialect<::mlir::LLVM::LLVMDialect>();
		auto lowered = ::mlir::parseSourceString<::mlir::ModuleOp>(original.bytecode, &context);
		REQUIRE(static_cast<bool>(lowered));
		REQUIRE(::mlir::succeeded(::mlir::verify(*lowered)));
		std::size_t allocations = 0;
		lowered->walk([&](::mlir::LLVM::AllocaOp) { ++allocations; });
		REQUIRE(allocations == 0);
		auto firstFunction = detachedBindingCleanupFunction(original, first, false, options);
		auto nativeFunction = detachedBindingCleanupFunction(original, nativeState, false, options);
		auto bytecodeFunction = detachedBindingCleanupFunction(original, bytecodeState, true, options);
		REQUIRE(first == BindingArtifactCleanupState {10});
		REQUIRE(nativeState == BindingArtifactCleanupState {100});
		REQUIRE(bytecodeState == BindingArtifactCleanupState {1000});
		const auto check = [](auto& execute, BindingArtifactCleanupState& state) {
			const auto before = state.total;
			REQUIRE(execute(7) == before + 7);
			REQUIRE(state.live == 0);
			REQUIRE(state.cleanupOrder == 21);
			REQUIRE(state.calls == 1);
			REQUIRE_THROWS_WITH(execute(-1), "standalone binding cleanup");
			REQUIRE(state.total == before + 7);
			REQUIRE(state.live == 0);
			REQUIRE(state.cleanupOrder == 2121);
			REQUIRE(state.calls == 2);
			REQUIRE(execute(3) == before + 10);
			REQUIRE(state.live == 0);
			REQUIRE(state.cleanupOrder == 212121);
			REQUIRE(state.calls == 3);
		};
		check(firstFunction, first);
		const auto firstAfter = first;
		check(nativeFunction, nativeState);
		REQUIRE(first == firstAfter);
		const auto nativeAfter = nativeState;
		check(bytecodeFunction, bytecodeState);
		REQUIRE(first == firstAfter);
		REQUIRE(nativeState == nativeAfter);
		const auto bytecodeAfter = bytecodeState;
		REQUIRE(firstFunction(1) == 21);
		REQUIRE(first.live == 0);
		REQUIRE(first.cleanupOrder == 21212121);
		REQUIRE(first.calls == 4);
		REQUIRE(nativeState == nativeAfter);
		REQUIRE(bytecodeState == bytecodeAfter);
		REQUIRE(wrappers == coldWrappers);
		REQUIRE_FALSE(tracing::inTracer());
	}
}

TEST_CASE("Standalone bound artifacts preserve the original native-resource alloca guard",
          "[runtime-bindings][artifact][mlir][B3][preflight][cleanup]") {
	requireBindingArtifactSupport();
	for (const bool bound : {false, true}) {
		for (const bool passes : {false, true}) {
			CAPTURE(bound, passes);
			BindingArtifactCleanupState state {10};
			RuntimeBindings bindings;
			auto binding = bindings.bind<BindingArtifactCleanupState>("b3/guarded", &state);
			auto options = bindingArtifactOptions();
			options.setOption("ir.runPasses", passes);
			if (bound) {
				options.setRuntimeBindings(bindings);
			}
			NautilusEngine engine(options);
			auto builder = engine.createModule();
			int wrappers = 0;
			builder.registerFunction<val<int64_t>(val<BindingArtifactCleanupState*>, val<int64_t>)>(
			    "execute", [binding, bound, &wrappers](val<BindingArtifactCleanupState*> argument, val<int64_t> value) {
				    ++wrappers;
				    auto address = bound ? binding.get() : argument;
				    val<BindingArtifactAllocatedResource<1>> outer(address);
				    val<BindingArtifactAllocatedResource<2>> inner(address);
				    return invoke(bindingArtifactGuardedProxy, address, value);
			    });
			REQUIRE_THROWS_WITH(builder.createArtifact(),
			                    "Artifact preflight failed: allocation_metadata_origins_unavailable");
			REQUIRE(wrappers > 0);
			REQUIRE(state == BindingArtifactCleanupState {10});
			REQUIRE_FALSE(tracing::inTracer());
		}
	}
}

TEST_CASE_METHOD(RuntimeBindingArtifactFixture,
                 "Checked native bound artifacts reject stale and missing binding imports",
                 "[runtime-bindings][artifact][mlir][B3][validation][native]") {
	auto imports = artifact::detail::resolveImports(original.descriptor, false, options);
	const auto names = artifact::detail::exportNames(original.descriptor);
	const auto symbol = bindings.entries().at("b3/left")->symbol;
	const auto found = std::ranges::find(imports.symbols, symbol);
	REQUIRE(found != imports.symbols.end());
	const auto index = static_cast<std::size_t>(found - imports.symbols.begin());
	REQUIRE(imports.addresses[index] == &left);
	int64_t replacement = 211;
	std::string expected;
	SECTION("wrong live binding address") {
		imports.addresses[index] = &replacement;
		expected = "MLIR artifact runtime binding address does not match the load environment";
	}
	SECTION("stale imports after a schema-compatible rebind") {
		RuntimeBindings rebound;
		(void) rebound.bind<int64_t>("b3/left", &replacement);
		(void) rebound.bind<int64_t>("b3/right", &right);
		(void) rebound.bind<int64_t>("b3/unused", &unused);
		REQUIRE(rebound.schemaEntries() == original.descriptor.bindingSchema);
		options.setRuntimeBindings(rebound);
		expected = "MLIR artifact runtime binding address does not match the load environment";
	}
	SECTION("missing used binding import") {
		imports.symbols.erase(imports.symbols.begin() + index);
		imports.addresses.erase(imports.addresses.begin() + index);
		expected = "MLIR artifact object contains undeclared import";
	}
	SECTION("null binding address") {
		imports.addresses[index] = nullptr;
		expected = "MLIR artifact external symbol manifest is invalid";
	}
	const auto beforeObject = original.object;
	const auto beforeSymbols = imports.symbols;
	const auto beforeAddresses = imports.addresses;
	const auto beforeBindings = options.getRuntimeBindings().entries();
	const auto coldWrappers = wrappers;
	REQUIRE_THROWS_WITH(compiler::mlir::validateArtifactObjectSymbols(original.object, names, imports.symbols,
	                                                                  imports.addresses, options),
	                    Catch::Matchers::StartsWith(expected));
	compiler::mlir::MLIRCompilationBackend backend;
	REQUIRE_THROWS_WITH(
	    backend.compileCachedObject(original.object, imports.symbols, imports.addresses, names, options, nullptr),
	    Catch::Matchers::StartsWith(expected));
	REQUIRE(original.object == beforeObject);
	REQUIRE(imports.symbols == beforeSymbols);
	REQUIRE(imports.addresses == beforeAddresses);
	REQUIRE(options.getRuntimeBindings().entries() == beforeBindings);
	REQUIRE(wrappers == coldWrappers);
	REQUIRE(left == 42);
	REQUIRE(right == 97);
	REQUIRE(unused == 113);
	REQUIRE(replacement == 211);
	const auto refreshed = artifact::detail::resolveImports(original.descriptor, false, options);
	REQUIRE_NOTHROW(compiler::mlir::validateArtifactObjectSymbols(original.object, names, refreshed.symbols,
	                                                              refreshed.addresses, options));
	auto loaded = artifact::loadNative(original, options);
	REQUIRE(loaded.getFunction<int64_t(int64_t)>("execute")(0) == 98);
	REQUIRE(wrappers == coldWrappers);
	REQUIRE_FALSE(tracing::inTracer());
}

TEST_CASE("Checked native bound artifacts reject real defined-data collisions after payload resealing",
          "[runtime-bindings][artifact][mlir][B3][validation][native]") {
	requireBindingArtifactUnwindSupport();
	BindingArtifactCleanupState state {10};
	int wrappers = 0;
	auto options = bindingArtifactOptions();
	const auto original = emitBindingCleanupArtifact(state, wrappers, options);
	REQUIRE(wrappers > 0);
	const auto coldWrappers = wrappers;
	RuntimeBindings bindings;
	(void) bindings.bind<BindingArtifactCleanupState>("b3/guarded", &state);
	options.setRuntimeBindings(bindings);
	const auto symbol = bindings.entries().at("b3/guarded")->symbol;
	const auto imports = artifact::detail::resolveImports(original.descriptor, false, options);
	const auto names = artifact::detail::exportNames(original.descriptor);
	const auto pristineSymbols = compiler::mlir::inspectArtifactObject(original.object, options);
	REQUIRE(std::ranges::find(pristineSymbols.undefinedSymbols, symbol) != pristineSymbols.undefinedSymbols.end());
	auto changed = original;
	changed.object = defineBindingInExistingObjectData(original.object, symbol);
	REQUIRE(changed.object != original.object);
	const auto inspected = compiler::mlir::inspectArtifactObject(changed.object, options);
	REQUIRE(std::ranges::find(inspected.undefinedSymbols, symbol) == inspected.undefinedSymbols.end());
	REQUIRE(std::ranges::find(inspected.definedSymbols, symbol) != inspected.definedSymbols.end());
	REQUIRE(std::ranges::find(inspected.definedFunctionSymbols, symbol) == inspected.definedFunctionSymbols.end());
	const auto beforeObject = changed.object;
	const auto beforeSymbols = imports.symbols;
	const auto beforeAddresses = imports.addresses;
	const auto expected = "MLIR artifact import conflicts with a defined symbol '" + symbol + "'";
	REQUIRE_THROWS_WITH(compiler::mlir::validateArtifactObjectSymbols(changed.object, names, imports.symbols,
	                                                                  imports.addresses, options),
	                    expected);
	compiler::mlir::MLIRCompilationBackend backend;
	REQUIRE_THROWS_WITH(
	    backend.compileCachedObject(changed.object, imports.symbols, imports.addresses, names, options, nullptr),
	    expected);
	REQUIRE(changed.object == beforeObject);
	REQUIRE(imports.symbols == beforeSymbols);
	REQUIRE(imports.addresses == beforeAddresses);
	changed.descriptor.objectDigest = artifact::detail::digest(changed.object);
	resealRuntimeBindingDescriptor(changed);
	REQUIRE_NOTHROW(artifact::detail::validateDescriptor(changed));
	REQUIRE_NOTHROW(artifact::detail::validatePayload(changed.object, changed.descriptor.objectDigest));
	const auto decoded = artifact::decode(artifact::encode(changed));
	REQUIRE_THROWS_WITH(artifact::loadNative(decoded, options), expected);
	REQUIRE(state == BindingArtifactCleanupState {10});
	auto independentBytecode = detachedBindingCleanupFunction(decoded, state, true, options);
	REQUIRE(independentBytecode(7) == 17);
	REQUIRE(state.live == 0);
	REQUIRE(state.cleanupOrder == 21);
	REQUIRE(state.calls == 1);
	REQUIRE(wrappers == coldWrappers);
	REQUIRE_FALSE(tracing::inTracer());
}

} // namespace nautilus::engine

#endif
