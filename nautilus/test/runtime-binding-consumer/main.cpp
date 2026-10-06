#include <array>
#include <cerrno>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <nautilus/Artifact.hpp>
#include <nautilus/CompilationStatistics.hpp>
#include <nautilus/Engine.hpp>
#include <nautilus/RuntimeBinding.hpp>
#include <nautilus/config.hpp>
#include <nautilus/nautilus_function.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#ifdef NAUTILUS_CACHE_EXPECTED
#include <nautilus/cache/plugin.hpp>
#endif
#ifdef __linux__
#include <sys/personality.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace {

using namespace nautilus;
using namespace nautilus::engine;
using Counter = std::shared_ptr<int>;
using Survivor = ModuleFunction<int64_t(int64_t)>;
using Addresses = std::array<uintptr_t, 4>;

static_assert(
    std::is_same_v<decltype(artifact::Descriptor {}.bindingSchema), std::vector<runtime_binding::SchemaEntry>>);

void require(bool condition, std::string_view message) {
	if (!condition) {
		throw std::runtime_error(std::string(message));
	}
}

template <typename F>
void requireRejected(F&& action, std::string_view reason) {
	try {
		action();
	} catch (const std::exception& error) {
		require(std::string_view(error.what()).find(reason) != std::string_view::npos, error.what());
		return;
	}
	throw std::runtime_error("Invalid artifact was accepted");
}

struct Storage {
	std::array<int64_t, 64> values {};
	uint32_t unused = 17;
	bool operator==(const Storage&) const = default;
};

struct Slots {
	size_t left;
	size_t alias;
};

Storage makeStorage(int64_t initial) {
	Storage storage;
	for (size_t index = 0; index < storage.values.size(); ++index) {
		storage.values[index] = initial + static_cast<int64_t>(index) * 13;
	}
	return storage;
}

Addresses addresses(Storage& storage, Slots slots) {
	return {reinterpret_cast<uintptr_t>(&storage.values[slots.left]),
	        reinterpret_cast<uintptr_t>(&storage.values[slots.alias]),
	        reinterpret_cast<uintptr_t>(&storage.values[slots.left]), reinterpret_cast<uintptr_t>(&storage.unused)};
}

struct Handles {
	RuntimeBinding<int64_t> left;
	RuntimeBinding<int64_t> alias;
	RuntimeBinding<const int64_t> observed;
};

Handles bind(RuntimeBindings& bindings, Storage& storage, Slots slots, bool reverse = false) {
	Handles handles;
	if (reverse) {
		handles.observed = bindings.bind<const int64_t>("operator/17/observed", &storage.values[slots.left]);
		handles.alias = bindings.bind<int64_t>("operator/17/alias", &storage.values[slots.alias]);
		handles.left = bindings.bind<int64_t>("operator/17/left", &storage.values[slots.left]);
	} else {
		handles.left = bindings.bind<int64_t>("operator/17/left", &storage.values[slots.left]);
		handles.alias = bindings.bind<int64_t>("operator/17/alias", &storage.values[slots.alias]);
		handles.observed = bindings.bind<const int64_t>("operator/17/observed", &storage.values[slots.left]);
	}
	(void) bindings.bind<uint32_t>("operator/17/unused", &storage.unused);
	return handles;
}

void registerProgram(NautilusModule& module, Handles handles, const Counter& wrappers) {
	module.registerFunction<val<int64_t>(val<int64_t>)>("update", [handles, wrappers](val<int64_t> delta) {
		++*wrappers;
		*handles.left.get() += delta;
		val<int64_t> alias = *handles.alias.get();
		val<int64_t> observed = *handles.observed.get();
		return alias + observed;
	});
	auto leafBody = [left = handles.left, wrappers](val<int64_t> delta) -> val<int64_t> {
		++*wrappers;
		*left.get() += delta;
		return *left.get();
	};
	auto leaf = std::make_shared<NautilusFunction<decltype(leafBody)>>("binding_leaf", std::move(leafBody));
	auto nestedBody = [leaf, wrappers](val<int64_t> delta) {
		++*wrappers;
		(*leaf)(delta);
		return (*leaf)(delta + cacheLiteral<int64_t {1}>());
	};
	auto nested = std::make_shared<NautilusFunction<decltype(nestedBody)>>("binding_nested", std::move(nestedBody));
	module.registerFunction<val<int64_t>(val<int64_t>)>("nested", [nested, wrappers](val<int64_t> delta) {
		++*wrappers;
		return (*nested)(delta);
	});
	module.registerFunction<val<int64_t>(val<int64_t>, val<int64_t>, val<int64_t>)>(
	    "loop", [handles, wrappers](val<int64_t> count, val<int64_t> split, val<int64_t> delta) {
		    ++*wrappers;
		    auto total = cacheLiteral<int64_t {0}>();
		    for (auto index = cacheLiteral<int64_t {0}>(); index < count; ++index) {
			    auto target = handles.left.get();
			    if (index >= split) {
				    target = handles.alias.get();
			    }
			    *target += delta;
			    total += *target;
		    }
		    return total;
	    });
	module.registerFunction<val<bool>()>("same_address", [handles, wrappers] {
		++*wrappers;
		return handles.left.get() == handles.alias.get();
	});
	module.registerFunction<val<int64_t*>()>("left_address", [handles, wrappers] {
		++*wrappers;
		return handles.left.get();
	});
	module.registerFunction<val<int64_t*>()>("alias_address", [handles, wrappers] {
		++*wrappers;
		return handles.alias.get();
	});
	module.registerFunction<val<const int64_t*>()>("observed_address", [handles, wrappers] {
		++*wrappers;
		return handles.observed.get();
	});
}

Survivor exercise(CompiledModule module, Storage& storage, Slots slots) {
	auto expected = storage;
	auto survivor = [&] {
		auto owner = std::move(module);
		require(owner.getFunction<int64_t*()>("left_address")() == &storage.values[slots.left],
		        "Left binding did not resolve to current storage");
		require(owner.getFunction<int64_t*()>("alias_address")() == &storage.values[slots.alias],
		        "Alias binding did not resolve to current storage");
		require(owner.getFunction<const int64_t*()>("observed_address")() == &storage.values[slots.left],
		        "Const binding did not resolve to current storage");
		require(owner.getFunction<bool()>("same_address")() == (slots.left == slots.alias),
		        "Binding identities incorrectly imply distinct addresses");
		auto update = owner.getFunction<int64_t(int64_t)>("update");
		for (const int64_t delta : {4, -3}) {
			expected.values[slots.left] += delta;
			require(update(delta) == expected.values[slots.alias] + expected.values[slots.left],
			        "Mutable or const-alias load mismatch");
			require(storage == expected, "Update touched unrelated storage");
			storage.values[slots.left] = expected.values[slots.left] = 1001;
		}
		auto nested = owner.getFunction<int64_t(int64_t)>("nested");
		expected.values[slots.left] += 11;
		require(nested(5) == expected.values[slots.left], "Nested binding result mismatch");
		require(storage == expected, "Nested calls touched unrelated storage");
		auto loop = owner.getFunction<int64_t(int64_t, int64_t, int64_t)>("loop");
		for (const auto [count, split] : std::array<std::pair<int64_t, int64_t>, 3> {{{0, 0}, {5, 2}, {3, 0}}}) {
			int64_t total = 0;
			for (int64_t index = 0; index < count; ++index) {
				auto& target = expected.values[index >= split ? slots.alias : slots.left];
				target += 3;
				total += target;
			}
			require(loop(count, split, 3) == total, "Loop backedge or branch merge binding mismatch");
			require(storage == expected, "Loop touched unrelated storage");
		}
		return nested;
	}();
	expected.values[slots.left] -= 3;
	require(survivor(-2) == expected.values[slots.left], "Function did not survive module destruction");
	require(storage == expected, "Surviving function touched unrelated storage");
	return survivor;
}

Options baseOptions(const std::string& backend) {
	Options options;
	options.setOption("mlir.enableMultithreading", false);
	if (backend == "interpreter") {
		options.setOption("engine.Compilation", false);
	} else if (backend == "reduced") {
		options.setOption("engine.tiered.backgroundPromotion", false);
	} else {
		options.setOption("engine.backend",
		                  backend == "tbc-auto" || backend == "tbc-jit" ? std::string("tbc") : backend);
		if (backend == "tbc-auto" || backend == "tbc-jit") {
			options.setOption("tbc.mode", backend == "tbc-auto" ? std::string("auto") : std::string("jit"));
		}
	}
	return options;
}

std::vector<std::string> configuredBackends() {
	std::vector<std::string> backends {"interpreter"};
#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_TBC_BACKEND
	backends.emplace_back("tbc");
#ifdef ENABLE_TBC_JIT
	backends.emplace_back("tbc-auto");
#endif
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
#else
	backends.emplace_back("reduced");
#endif
	return backends;
}

template <typename T>
T statistic(const CompiledModule& module, const std::string& name) {
	const auto statistics = module.getStatistics();
	require(statistics != nullptr, "Missing compilation statistics");
	const auto* value = statistics->find(name);
	require(value != nullptr && std::holds_alternative<T>(*value), "Missing or mistyped statistic: " + name);
	return std::get<T>(*value);
}

void requireNoFrontend(const CompiledModule& module, bool native) {
	if (const auto statistics = module.getStatistics()) {
		for (const auto* name : {"tracing.ms", "frontend.totalMs", "ssaCreation.ms", "irGeneration.ms"}) {
			require(!statistics->contains(name), "Artifact load executed the frontend");
		}
		for (const auto& [name, value] : *statistics) {
			require(!name.starts_with("ir.") && !name.starts_with("irPasses."), "Artifact load ran IR passes");
			if (native) {
				require(!name.starts_with("mlir.") && !name.starts_with("llvm.") && name != "jit.compile.ms",
				        "Native load executed code generation");
			}
		}
	}
}

CompiledModule compileSnapshot(Options options, Storage& storage, Slots slots, const Counter& wrappers,
                               bool fromOptions, bool cached) {
	RuntimeBindings bindings;
	auto handles = bind(bindings, storage, slots, fromOptions);
	const auto schema = bindings.schemaEntries();
	if (fromOptions) {
		options.setRuntimeBindings(bindings);
	}
	std::unique_ptr<NautilusEngine> engine;
#ifdef NAUTILUS_CACHE_EXPECTED
	if (cached) {
		engine = std::make_unique<NautilusEngine>(cache::createCompiler(options), options);
	} else
#else
	require(!cached, "Cache factory is unavailable in this package");
#endif
	{
		engine = std::make_unique<NautilusEngine>(options);
	}
	auto module = engine->createModule();
	if (!fromOptions) {
		module.setRuntimeBindings(bindings);
	}
	registerProgram(module, handles, wrappers);
	(void) bindings.bind<int64_t>("late-registration", &storage.values.back());
	bindings = RuntimeBindings {};
	(void) bindings.bind<int64_t>("operator/17/left", &storage.values.back());
	require(module.getOptions().getRuntimeBindings().schemaEntries() == schema, "Module binding snapshot changed");
	const auto before = storage;
	auto compiled = module.compile();
	require(storage == before, "Tracing mutated a binding pointee");
	return compiled;
}

void ordinary(const std::string& requested) {
	const auto configured = configuredBackends();
	auto backends = requested == "all" ? configured : std::vector<std::string> {requested};
	for (const auto& backend : backends) {
		bool available = false;
		for (const auto& candidate : configured) {
			available |= candidate == backend || (candidate == "tbc-auto" && backend == "tbc-jit");
		}
		require(available, "Requested backend is not configured in the installed package");
		for (const bool fromOptions : {false, true}) {
			for (const bool aliased : {false, true}) {
				auto storage = makeStorage(10);
				const Slots slots {0, aliased ? size_t {0} : size_t {1}};
				auto wrappers = std::make_shared<int>(0);
				auto options = baseOptions(backend);
				const auto directory = std::filesystem::current_path() / "ordinary-cache";
				const bool existed = std::filesystem::exists(directory);
				options.setOption("engine.cache.directory", directory.string());
				options.setOption("engine.cache.key", std::string("installed-runtime-bindings/i64/v1"));
				auto module = compileSnapshot(options, storage, slots, wrappers, fromOptions, false);
				const bool interpreted = backend == "interpreter" || backend == "reduced";
				require((module.getExecutable() == nullptr) == interpreted,
				        "Backend silently fell back to interpretation");
				const auto before = *wrappers;
				if (interpreted) {
					require(before == 0, "Interpreter unexpectedly traced");
					require(module.getStatistics() == nullptr, "Interpreter unexpectedly has compiler statistics");
				} else {
					require(before > 0, "Configured backend did not trace");
					const auto expectedBackend = backend == "tbc-auto" || backend == "tbc-jit" ? "tbc" : backend;
					require(statistic<std::string>(module, "backend.name") == expectedBackend,
					        "Wrong backend selected");
				}
				if (const auto statistics = module.getStatistics()) {
					for (const auto& [name, value] : *statistics) {
						require(!name.starts_with("cache."),
						        "Ordinary engine implicitly dispatched to the cache plugin");
					}
					if (backend == "tbc-auto" || backend == "tbc-jit") {
						const auto mode = statistic<std::string>(module, "tbc.mode");
						require(mode == "jit" || (backend == "tbc-auto" && mode == "interp"),
						        "Wrong TBC execution mode");
						std::cout << "tbc.mode=" << mode << '\n';
					}
				}
				auto survivor = exercise(std::move(module), storage, slots);
				require(interpreted ? *wrappers > before : *wrappers == before, "Unexpected wrapper execution");
				require(existed || !std::filesystem::exists(directory), "Ordinary engine created a persistent cache");
				std::cout << "ordinary " << backend << " bindings=" << (fromOptions ? "options" : "module")
				          << " aliased=" << aliased << " passed; wrappers=" << *wrappers << '\n';
			}
		}
	}
#if !defined(ENABLE_MLIR_BACKEND) || !defined(ENABLE_TRACING) || !defined(ENABLE_COMPILER)
	require(!artifact::isSupported(), "Reduced package advertised unsupported persistence");
#endif
}

std::string readBytes(const std::filesystem::path& path) {
	std::ifstream input(path, std::ios::binary);
	require(input.good(), "Could not open file: " + path.string());
	std::string bytes {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
	require(!input.bad(), "Could not read file: " + path.string());
	return bytes;
}

void writeBytes(const std::filesystem::path& path, std::string_view bytes) {
	if (!path.parent_path().empty()) {
		std::filesystem::create_directories(path.parent_path());
	}
	std::ofstream output(path, std::ios::binary | std::ios::trunc);
	output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
	output.close();
	require(output.good(), "Could not write file: " + path.string());
}

void requireNoAddresses(std::string_view bytes, const Addresses& values) {
	for (const auto address : values) {
		const std::string_view raw {reinterpret_cast<const char*>(&address), sizeof(address)};
		std::ostringstream hex;
		hex << std::hex << address;
		require(bytes.find(raw) == std::string_view::npos &&
		            bytes.find(std::to_string(address)) == std::string_view::npos &&
		            bytes.find(hex.str()) == std::string_view::npos,
		        "A current/producer binding address was persisted");
	}
}

void requireAslr() {
#ifdef __linux__
	const auto current = ::personality(0xffffffffUL);
	require(current != -1 && (current & ADDR_NO_RANDOMIZE) == 0, "Persistence consumer requires enabled ASLR");
	int setting = 0;
	std::ifstream kernel("/proc/sys/kernel/randomize_va_space");
	require(static_cast<bool>(kernel >> setting) && setting > 0, "Kernel ASLR is disabled");
#else
	throw std::runtime_error("Persistence consumer requires Linux ASLR");
#endif
}

void writeReport(const std::filesystem::path& path, const Addresses& values, std::string_view wrapperEvidence,
                 std::string_view key, const std::string& statistics = {}) {
	std::ostringstream report;
	for (const auto address : values) {
		report << address << ' ';
	}
	report << '\n' << key << '\n';
	report << wrapperEvidence << '\n' << statistics;
	writeBytes(path, report.str());
	std::cout << report.str();
}

struct Report {
	Addresses values {};
	std::string key;
};

Report readReport(const std::filesystem::path& path) {
	std::ifstream input(path);
	Report report;
	for (auto& address : report.values) {
		require(static_cast<bool>(input >> address), "Missing producer/cold address evidence: " + path.string());
	}
	require(static_cast<bool>(input >> report.key), "Missing producer/cold key evidence: " + path.string());
	return report;
}

void requireRebound(const Addresses& producer, const Addresses& current) {
	for (size_t index = 0; index < current.size(); ++index) {
		require(producer[index] != current[index], "Consumer reused a producer binding address; rerun under ASLR");
	}
}

void requireArtifact(const artifact::ModuleArtifact& value, const RuntimeBindings& bindings) {
	require(value.descriptor.version == 2, "Wrong runtime-binding artifact version");
	require(value.descriptor.bindingSchema == bindings.schemaEntries(), "Wrong complete binding schema");
	require(value.descriptor.bindingSchema.size() == 4, "Unused binding was omitted from the descriptor");
	require(value.descriptor.exports.size() == 7, "Runtime bindings changed the exported function set");
	for (const auto& entry : value.descriptor.exports) {
		const bool address =
		    entry.name == "left_address" || entry.name == "alias_address" || entry.name == "observed_address";
		const bool update = entry.name == "update" || entry.name == "nested";
		require(address || update || entry.name == "loop" || entry.name == "same_address", "Unexpected export name");
		const std::vector<Type> expected = update                 ? std::vector<Type> {Type::i64}
		                                   : entry.name == "loop" ? std::vector<Type> {Type::i64, Type::i64, Type::i64}
		                                                          : std::vector<Type> {};
		require(entry.argumentTypes == expected, "Runtime bindings added an ABI argument");
		require(entry.returnType == (address                        ? Type::ptr
		                             : entry.name == "same_address" ? Type::b
		                                                            : Type::i64) &&
		            entry.callingConvention == 0 && !entry.loweredABI.empty(),
		        "Runtime bindings changed an export ABI");
	}
}

void emit(const std::filesystem::path& path) {
#if defined(ENABLE_TRACING) && defined(ENABLE_MLIR_BACKEND)
	requireAslr();
	require(artifact::isSupported(), "Standalone artifacts are unsupported by this installed package");
	auto storage = std::make_unique<Storage>(makeStorage(10));
	const Slots slots {0, 0};
	auto wrappers = std::make_shared<int>(0);
	auto value = [&] {
		RuntimeBindings bindings;
		auto handles = bind(bindings, *storage, slots);
		auto options = baseOptions("mlir");
		NautilusEngine engine(options);
		auto module = engine.createModule();
		module.setRuntimeBindings(bindings);
		registerProgram(module, handles, wrappers);
		const auto before = *storage;
		auto transported = module.createArtifact();
		require(*storage == before && *wrappers > 0, "Artifact emission did not trace without touching pointees");
		requireArtifact(transported, bindings);
		return transported;
	}();
	const auto bytes = artifact::encode(value);
	require(artifact::decode(bytes).descriptor == value.descriptor, "Descriptor did not round trip");
	requireNoAddresses(bytes, addresses(*storage, slots));
	writeBytes(path, bytes);
	writeReport(path.string() + ".producer.report", addresses(*storage, slots), "wrappers=" + std::to_string(*wrappers),
	            "standalone");
#else
	(void) path;
	throw std::runtime_error("Standalone emission requires configured tracing and MLIR");
#endif
}

CompiledModule loadSnapshot(const artifact::ModuleArtifact& value, Storage& storage, Slots slots, bool native) {
	RuntimeBindings bindings;
	(void) bind(bindings, storage, slots, true);
	requireArtifact(value, bindings);
	auto options = baseOptions("mlir");
	options.setRuntimeBindings(bindings);
	auto module = native ? artifact::loadNative(value, options) : artifact::loadBytecode(value, options);
	require(module.getExecutable() != nullptr, "Standalone load did not create an executable");
	requireNoFrontend(module, native);
	return module;
}

void schemaMismatch(const artifact::ModuleArtifact& value, Storage& storage, Slots slots) {
	uint64_t wrongType = 31;
	int32_t wrongSize = 41;
	int32_t wrongUnused = 17;
	const auto before = storage;
	for (const auto* kind : {"missing", "renamed-unused", "unused-type", "same-size-type", "size", "const", "extra"}) {
		RuntimeBindings bindings;
		const std::string_view name(kind);
		if (name != "missing") {
			if (name == "same-size-type") {
				(void) bindings.bind<uint64_t>("operator/17/left", &wrongType);
			} else if (name == "size") {
				(void) bindings.bind<int32_t>("operator/17/left", &wrongSize);
			} else if (name == "const") {
				(void) bindings.bind<const int64_t>("operator/17/left", &storage.values[slots.left]);
			} else {
				(void) bindings.bind<int64_t>("operator/17/left", &storage.values[slots.left]);
			}
			(void) bindings.bind<int64_t>("operator/17/alias", &storage.values[slots.alias]);
			(void) bindings.bind<const int64_t>("operator/17/observed", &storage.values[slots.left]);
			if (name == "unused-type") {
				(void) bindings.bind<int32_t>("operator/17/unused", &wrongUnused);
			} else {
				(void) bindings.bind<uint32_t>(name == "renamed-unused" ? "renamed-unused" : "operator/17/unused",
				                               &storage.unused);
			}
			if (name == "extra") {
				(void) bindings.bind<int64_t>("extra", &storage.values.back());
			}
		}
		auto options = baseOptions("mlir");
		options.setRuntimeBindings(bindings);
		requireRejected([&] { (void) artifact::loadNative(value, options); }, "schema mismatch");
		requireRejected([&] { (void) artifact::loadBytecode(value, options); }, "schema mismatch");
		require(storage == before, "Rejected schema touched a pointee");
		std::cout << "schema-mismatch " << kind << " rejected by native and bytecode\n";
	}
	RuntimeBindings bindings;
	(void) bind(bindings, storage, slots);
	auto options = baseOptions("mlir");
	options.setRuntimeBindings(bindings);
	auto legacy = value;
	legacy.descriptor.version = 1;
	requireRejected([&] { (void) artifact::loadNative(legacy, options); }, "descriptor");
	requireRejected([&] { (void) artifact::loadBytecode(legacy, options); }, "descriptor");
}

void consume(const std::string& mode, const std::filesystem::path& path) {
	requireAslr();
	require(artifact::isSupported(), "Standalone artifacts are unsupported by this installed package");
	const auto bytes = readBytes(path);
	auto value = artifact::decode(bytes);
	const auto producer = readReport(path.string() + ".producer.report");
	auto storage = std::make_unique<Storage>(makeStorage(100));
	const Slots slots {17, 23};
	const auto current = addresses(*storage, slots);
	requireRebound(producer.values, current);
	requireNoAddresses(bytes, producer.values);
	requireNoAddresses(bytes, current);
	if (mode == "schema-mismatch") {
		schemaMismatch(value, *storage, slots);
	} else if (mode == "rebound") {
		auto secondStorage = std::make_unique<Storage>(makeStorage(211));
		auto thirdStorage = std::make_unique<Storage>(makeStorage(307));
		const Slots aliased {29, 29};
		const Slots separate {37, 41};
		auto first = exercise(loadSnapshot(value, *storage, slots, true), *storage, slots);
		const auto firstAfter = *storage;
		auto second = exercise(loadSnapshot(value, *secondStorage, aliased, true), *secondStorage, aliased);
		const auto secondAfter = *secondStorage;
		auto third = exercise(loadSnapshot(value, *thirdStorage, separate, false), *thirdStorage, separate);
		const auto thirdAfter = *thirdStorage;
		require(*storage == firstAfter && *secondStorage == secondAfter, "A rebound load changed an earlier module");
		require(first(2) == firstAfter.values[slots.left] + 5, "First native module lost its bindings");
		require(*secondStorage == secondAfter && *thirdStorage == thirdAfter, "Module bindings were globally replaced");
		require(second(3) == secondAfter.values[aliased.left] + 7, "Second native module lost its bindings");
		require(*thirdStorage == thirdAfter, "Native execution changed the bytecode-loaded module");
		requireNoAddresses(bytes, addresses(*secondStorage, aliased));
		requireNoAddresses(bytes, addresses(*thirdStorage, separate));
	} else {
		const bool damaged = mode == "native-no-bytecode" || mode == "native-corrupt-bytecode";
		if (damaged) {
			value.bytecode = mode == "native-no-bytecode" ? std::string() : std::string("corrupt binding bytecode");
			RuntimeBindings bindings;
			(void) bind(bindings, *storage, slots);
			auto options = baseOptions("mlir");
			options.setRuntimeBindings(bindings);
			requireRejected([&] { (void) artifact::loadBytecode(value, options); }, "payload");
		}
		auto survivor = exercise(loadSnapshot(value, *storage, slots, mode != "bytecode"), *storage, slots);
	}
	writeReport(path.string() + "." + mode + ".report", current, "frontend-wrapper=not_supplied", "standalone");
}

#ifdef NAUTILUS_CACHE_EXPECTED
std::filesystem::path uniqueArtifact(const std::filesystem::path& directory, std::string_view extension) {
	std::filesystem::path result;
	for (const auto& entry : std::filesystem::directory_iterator(directory)) {
		if (entry.path().extension() == extension) {
			require(result.empty(), "Cache mode requires a dedicated single-entry directory");
			result = entry.path();
		}
	}
	require(!result.empty(), "Missing published cache artifact");
	return result;
}

void cached(const std::string& mode, const std::filesystem::path& directory) {
	requireAslr();
	require(artifact::isSupported(), "Cache artifacts are unsupported by this installed package");
	const auto coldReport = directory.string() + ".cold.report";
	if (mode == "cold") {
		std::filesystem::remove_all(directory);
		std::filesystem::remove(coldReport);
	} else if (mode == "repair") {
		writeBytes(uniqueArtifact(directory, ".o"), "corrupt runtime-binding native object");
	}
	auto storage = std::make_unique<Storage>(makeStorage(mode == "cold" ? 10 : 101));
	const Slots slots = mode == "cold" ? Slots {0, 0} : mode == "warm" ? Slots {17, 23} : Slots {31, 31};
	const auto current = addresses(*storage, slots);
	auto wrappers = std::make_shared<int>(0);
	auto options = baseOptions("mlir");
	options.setOption("engine.cache.directory", directory.string());
	options.setOption("engine.cache.key", std::string("installed-runtime-bindings/i64/v1"));
	auto module = compileSnapshot(options, *storage, slots, wrappers, mode != "cold", true);
	const auto statistics = module.getStatistics();
	require(statistics != nullptr, "Cache did not publish statistics");
	const auto key = statistic<std::string>(module, "cache.key");
	require(statistic<int64_t>(module, "cache.tracingRan") == (mode == "cold" ? 1 : 0), "Wrong trace telemetry");
	require(mode == "cold" ? *wrappers > 0 : *wrappers == 0, "Unexpected independent wrapper execution");
	if (mode == "cold") {
		require(statistic<std::string>(module, "cache.object") == "written" &&
		            statistic<std::string>(module, "cache.mlir") == "written" &&
		            statistic<std::string>(module, "cache.fallback") == "none",
		        "Cold binding entry was not published");
		require(statistic<int64_t>(module, "cache.scalarCertificate") == 1, "Cold scalar certificate failed");
	} else {
		const auto producer = readReport(coldReport);
		require(key == producer.key, "Current binding addresses changed cache identity");
		requireRebound(producer.values, current);
		requireNoFrontend(module, mode == "warm");
		if (mode == "warm") {
			require(statistic<std::string>(module, "cache.object") == "hit" &&
			            statistic<std::string>(module, "cache.mlir") == "not_checked" &&
			            statistic<std::string>(module, "cache.fallback") == "none" &&
			            statistics->contains("jit.objectLoad.ms"),
			        "Warm request was not a native-first hit");
		} else {
			require(statistic<std::string>(module, "cache.object") == "invalid_rewritten" &&
			            statistic<std::string>(module, "cache.mlir") == "hit" &&
			            statistic<std::string>(module, "cache.fallback") == "invalid_object",
			        "Repair was not a bytecode hit/native rewrite");
		}
	}
	for (const auto* extension : {".o", ".mlirbc", ".manifest"}) {
		const auto file = uniqueArtifact(directory, extension);
		require(file.stem().string() == key, "Cache artifacts disagree with the reported key");
		const auto bytes = readBytes(file);
		requireNoAddresses(bytes, current);
		if (mode != "cold") {
			requireNoAddresses(bytes, readReport(coldReport).values);
		}
	}
	const auto traced = *wrappers;
	auto survivor = exercise(std::move(module), *storage, slots);
	require(*wrappers == traced, "Compiled cache execution ran a wrapper");
	writeReport(directory.string() + "." + mode + ".report", current, "wrappers=" + std::to_string(*wrappers), key,
	            statistics->toString());
}
#endif

void child(const std::vector<std::string>& arguments) {
#ifdef __linux__
	const auto executable = std::filesystem::read_symlink("/proc/self/exe").string();
	std::vector<std::string> command {executable};
	command.insert(command.end(), arguments.begin(), arguments.end());
	std::vector<char*> argv;
	for (auto& argument : command) {
		argv.push_back(argument.data());
	}
	argv.push_back(nullptr);
	std::cout.flush();
	std::cerr.flush();
	const auto pid = ::fork();
	require(pid >= 0, "Could not fork a fresh consumer");
	if (pid == 0) {
		const auto current = ::personality(0xffffffffUL);
		if (current == -1 ||
		    ((current & ADDR_NO_RANDOMIZE) != 0 && ::personality(current & ~ADDR_NO_RANDOMIZE) == -1)) {
			::_exit(124);
		}
		::execv(executable.c_str(), argv.data());
		::_exit(125);
	}
	int status = 0;
	pid_t waited;
	do {
		waited = ::waitpid(pid, &status, 0);
	} while (waited < 0 && errno == EINTR);
	require(waited == pid && WIFEXITED(status) && WEXITSTATUS(status) == 0, "Fresh consumer process failed");
#else
	(void) arguments;
	throw std::runtime_error("Fresh-process persistence checks require Linux");
#endif
}

} // namespace

int main(int argc, char** argv) {
	try {
		const std::string mode = argc > 1 ? argv[1] : "ordinary";
		if (mode == "ordinary") {
			require(argc <= 3, "Usage: runtime-binding-consumer ordinary [all|interpreter|reduced|backend]");
			ordinary(argc == 3 ? argv[2] : "all");
		} else {
			require(argc == 3, "Usage: runtime-binding-consumer MODE ARTIFACT_FILE_OR_DEDICATED_DIRECTORY");
			const auto path = std::filesystem::absolute(argv[2]);
			if (mode == "emit") {
				emit(path);
			} else if (mode == "native" || mode == "bytecode" || mode == "rebound" || mode == "schema-mismatch" ||
			           mode == "native-no-bytecode" || mode == "native-corrupt-bytecode") {
				consume(mode, path);
			} else if (mode == "artifact-roundtrip") {
				const auto file = (path / "bindings.artifact").string();
				for (const auto* request : {"emit", "native", "bytecode", "rebound", "schema-mismatch",
				                            "native-no-bytecode", "native-corrupt-bytecode"}) {
					child({request, file});
				}
			} else if (mode == "cold" || mode == "warm" || mode == "repair" || mode == "cache-roundtrip") {
#ifdef NAUTILUS_CACHE_EXPECTED
				if (mode == "cache-roundtrip") {
					for (const auto* request : {"cold", "warm", "repair", "warm"}) {
						child({request, path.string()});
					}
				} else {
					cached(mode, path);
				}
#else
				throw std::runtime_error("Cache modes require explicit linking to nautilus::nautilus-cache");
#endif
			} else {
				throw std::runtime_error("Unknown runtime-binding consumer mode: " + mode);
			}
			std::cout << mode << " passed\n";
		}
		return 0;
	} catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}
