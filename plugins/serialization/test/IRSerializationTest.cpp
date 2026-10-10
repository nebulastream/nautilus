#include "Crc32c.hpp"
#include "IRRoundTrip.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/compiler/ir/passes/ExceptionRegionPreparationPass.hpp"
#include "nautilus/compiler/ir/passes/IRPassManager.hpp"
#include "nautilus/config.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/serialization/IRBinaryFormat.hpp"
#include "nautilus/serialization/IRSerialization.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include <catch2/catch_all.hpp>
#include <cstring>
#include <dlfcn.h>
#include <list>
#include <random>

namespace nautilus::engine {

namespace ser = nautilus::serialization;

namespace {

// ── traced fixtures ────────────────────────────────────────────────────────

int32_t serializationNativeScale(int32_t x) {
	return x * 3 + 1;
}

/// noexcept, so a call to it carries no exception-capture thunk: the only
/// pointer in its IR is the callee itself.
int32_t serializationNativeOffset(int32_t x) noexcept {
	return x + 7;
}

val<int32_t> noexceptRuntimeCall(val<int32_t> n) {
	return invoke(serializationNativeOffset, n) * 2;
}

val<int32_t> loopWithRuntimeCall(val<int32_t> n) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		sum = sum + invoke(serializationNativeScale, i);
	}
	return sum;
}

val<int32_t> serializationSquare(val<int32_t> x) {
	return x * x;
}

auto nautilusSerializationSquare = NautilusFunction {"square", serializationSquare};

val<int32_t> callsNautilusFunction(val<int32_t> x) {
	return nautilusSerializationSquare(x) + nautilusSerializationSquare(x + 1);
}

val<int64_t> branchesAndSelects(val<int64_t> x) {
	val<int64_t> result = x;
	if (x > 10) {
		result = x * 2;
	} else if (x < -5) {
		result = x - 3;
	}
	return result % 1000;
}

val<double> floatingPointLoop(val<int32_t> n) {
	val<double> acc = 0.5;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = acc * 1.5 + 0.25;
	}
	return acc;
}

template <typename R, typename... Args>
std::shared_ptr<compiler::ir::IRGraph> traceToIR(R (*fn)(Args...)) {
	common::Arena arena;
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", details::createFunctionWrapper(fn));
	auto trace = tracing::TraceContext::Trace(functions, Options {}, arena);
	auto afterSSA = tracing::SSACreationPhase().apply(std::shared_ptr<tracing::TraceModule>(std::move(trace)));
	auto ir = tracing::TraceToIRConversionPhase().apply(std::move(afterSSA));
	Options passOptions;
	compiler::ir::IRPassManager passManager(passOptions);
	passManager.addPass(std::make_unique<compiler::ir::ExceptionRegionPreparationPass>());
	passManager.run(*ir);
	return ir;
}

std::vector<std::string> backendNames() {
	std::vector<std::string> names;
#ifdef ENABLE_MLIR_BACKEND
	names.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	names.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	names.emplace_back("bc");
#endif
#ifdef ENABLE_TBC_BACKEND
	names.emplace_back("tbc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	names.emplace_back("asmjit");
#endif
	return names;
}

std::unique_ptr<compiler::Executable> compile(const std::shared_ptr<compiler::ir::IRGraph>& ir,
                                              const std::string& backend) {
	auto* compilationBackend = compiler::CompilationBackendRegistry::getInstance()->getBackend(backend);
	// DumpHandler keeps a reference to its options, so they must outlive compile().
	static Options dumpOptions;
	compiler::DumpHandler dumpHandler(dumpOptions, "ir-serialization-test");
	Options compileOptions;
	return compilationBackend->compile(ir, dumpHandler, compileOptions, nullptr);
}

/// Compiles the IR of @p fn as traced and as round-tripped through a portable
/// buffer, and checks both executables agree on every input.
template <typename R, typename Arg>
void requireSameResults(R (*fn)(val<Arg>), const std::vector<Arg>& inputs) {
	using Result = typename R::raw_type;
	auto original = traceToIR(fn);

	testing::SyntheticSymbols symbols;
	ser::SerializeOptions writeOptions;
	writeOptions.pointerMode = ser::PointerMode::Portable;
	writeOptions.namer = &symbols;
	ser::DeserializeOptions readOptions;
	readOptions.resolver = &symbols;
	readOptions.verifyIR = true;
	auto restored = ser::deserialize(ser::serialize(*original, writeOptions), readOptions);
	REQUIRE(restored->toString() == original->toString());

	for (const auto& backend : backendNames()) {
		DYNAMIC_SECTION(backend) {
			auto expected = compile(original, backend);
			auto actual = compile(restored, backend);
			auto expectedFn = expected->template getInvocableMember<Result, Arg>("execute");
			auto actualFn = actual->template getInvocableMember<Result, Arg>("execute");
			for (const auto input : inputs) {
				REQUIRE(actualFn(input) == expectedFn(input));
			}
		}
	}
}

// ── buffer surgery ─────────────────────────────────────────────────────────

struct Section {
	ser::SectionEntry entry;
	std::vector<std::byte> bytes;
};

ser::FileHeader readHeader(const std::vector<std::byte>& buffer) {
	ser::FileHeader header {};
	std::memcpy(&header, buffer.data(), sizeof(header));
	return header;
}

std::vector<Section> readSections(const std::vector<std::byte>& buffer) {
	const auto header = readHeader(buffer);
	std::vector<Section> sections;
	for (uint32_t i = 0; i < header.sectionCount; ++i) {
		Section section {};
		std::memcpy(&section.entry, buffer.data() + header.sectionDirOffset + i * sizeof(ser::SectionEntry),
		            sizeof(ser::SectionEntry));
		const auto size = static_cast<size_t>(section.entry.recordStride) * section.entry.recordCount;
		const auto* begin = buffer.data() + section.entry.offset;
		section.bytes.assign(begin, begin + size);
		sections.push_back(std::move(section));
	}
	return sections;
}

void resealChecksum(std::vector<std::byte>& buffer) {
	auto header = readHeader(buffer);
	header.checksum = ser::crc32c(std::span<const std::byte>(buffer).subspan(header.headerSize));
	std::memcpy(buffer.data(), &header, sizeof(header));
}

/// Lays @p sections out into a fresh, correctly checksummed buffer that
/// keeps @p original's header fields -- what a writer of another version
/// would have produced.
std::vector<std::byte> writeSections(const std::vector<std::byte>& original, std::vector<Section> sections) {
	auto header = readHeader(original);
	const auto align = [](uint64_t v) {
		return (v + 7) / 8 * 8;
	};
	uint64_t cursor = align(sizeof(ser::FileHeader) + sections.size() * sizeof(ser::SectionEntry));
	for (auto& section : sections) {
		section.entry.offset = cursor;
		cursor = align(cursor + section.bytes.size());
	}
	std::vector<std::byte> out(cursor, std::byte {0});
	for (size_t i = 0; i < sections.size(); ++i) {
		std::memcpy(out.data() + sizeof(ser::FileHeader) + i * sizeof(ser::SectionEntry), &sections[i].entry,
		            sizeof(ser::SectionEntry));
		std::memcpy(out.data() + sections[i].entry.offset, sections[i].bytes.data(), sections[i].bytes.size());
	}
	header.totalSize = out.size();
	header.sectionCount = static_cast<uint32_t>(sections.size());
	header.sectionDirOffset = sizeof(ser::FileHeader);
	std::memcpy(out.data(), &header, sizeof(header));
	resealChecksum(out);
	return out;
}

template <typename Mutation>
std::vector<std::byte> withHeader(std::vector<std::byte> buffer, Mutation mutate) {
	auto header = readHeader(buffer);
	mutate(header);
	std::memcpy(buffer.data(), &header, sizeof(header));
	resealChecksum(buffer);
	return buffer;
}

std::string deserializationError(std::span<const std::byte> buffer, const ser::DeserializeOptions& options = {}) {
	try {
		(void) ser::deserialize(buffer, options);
	} catch (const ser::SerializationException& e) {
		return e.what();
	}
	return "";
}

} // namespace

TEST_CASE("Deserialized IR executes like the traced IR") {
	SECTION("loop with a runtime call") {
		requireSameResults(loopWithRuntimeCall, std::vector<int32_t> {0, 1, 7, 100});
	}
	SECTION("nautilus function calls") {
		requireSameResults(callsNautilusFunction, std::vector<int32_t> {-3, 0, 5, 1000});
	}
	SECTION("branches") {
		requireSameResults(branchesAndSelects, std::vector<int64_t> {-100, -6, -5, 0, 10, 11, 12345});
	}
	SECTION("floating point") {
		requireSameResults(floatingPointLoop, std::vector<int32_t> {0, 1, 10});
	}
}

TEST_CASE("Serialized IR is deterministic") {
	auto first = traceToIR(loopWithRuntimeCall);
	auto second = traceToIR(loopWithRuntimeCall);
	testing::SyntheticSymbols symbolsA;
	testing::SyntheticSymbols symbolsB;
	ser::SerializeOptions optionsA;
	optionsA.pointerMode = ser::PointerMode::Portable;
	optionsA.namer = &symbolsA;
	ser::SerializeOptions optionsB = optionsA;
	optionsB.namer = &symbolsB;
	// Two independent traces, two namers: no address, pointer or hash-map
	// iteration order may leak into the bytes.
	REQUIRE(ser::serialize(*first, optionsA) == ser::serialize(*second, optionsB));
	REQUIRE(ser::serialize(*first) == ser::serialize(*first));
}

TEST_CASE("Serialized IR frames itself") {
	const auto buffer = ser::serialize(*traceToIR(branchesAndSelects));
	REQUIRE(ser::peekTotalSize(std::span(buffer).first(24)) == buffer.size());
	REQUIRE_FALSE(ser::peekTotalSize(std::span(buffer).first(10)).has_value());
	auto wrongMagic = buffer;
	wrongMagic[0] = std::byte {'X'};
	REQUIRE_FALSE(ser::peekTotalSize(wrongMagic).has_value());
}

TEST_CASE("Deserialization reads unaligned buffers") {
	const auto ir = traceToIR(callsNautilusFunction);
	const auto buffer = ser::serialize(*ir);
	std::vector<std::byte> shifted(buffer.size() + 1);
	std::memcpy(shifted.data() + 1, buffer.data(), buffer.size());
	REQUIRE(ser::deserialize(std::span(shifted).subspan(1))->toString() == ir->toString());
}

TEST_CASE("Deserialization can allocate from an arena pool") {
	const auto ir = traceToIR(branchesAndSelects);
	common::ArenaPool pool;
	ser::DeserializeOptions options;
	options.compilationUnitId = "renamed";
	auto restored = ser::deserialize(ser::serialize(*ir), pool, options);
	REQUIRE(restored->toString() == ir->toString());
	REQUIRE(restored->getId() == "renamed");
}

TEST_CASE("Malformed buffers are rejected, never trusted") {
	const auto buffer = ser::serialize(*traceToIR(loopWithRuntimeCall));

	SECTION("every truncation") {
		for (size_t size = 0; size < buffer.size(); ++size) {
			REQUIRE_THROWS_AS(ser::deserialize(std::span(buffer).first(size)), ser::SerializationException);
		}
	}

	SECTION("the checksum catches every single-bit flip in the body") {
		for (size_t offset = sizeof(ser::FileHeader); offset < buffer.size(); offset += 7) {
			auto corrupted = buffer;
			corrupted[offset] ^= std::byte {0x10};
			REQUIRE(deserializationError(corrupted).find("checksum") != std::string::npos);
		}
	}

	SECTION("random corruption without a checksum fails cleanly") {
		// With the checksum off, every structural check has to hold its own:
		// a mutated buffer may still describe valid IR, but it must never
		// crash the reader or escape as anything but a SerializationException.
		ser::DeserializeOptions options;
		options.verifyChecksum = false;
		std::mt19937 random(42);
		size_t rejected = 0;
		for (int round = 0; round < 3000; ++round) {
			auto corrupted = buffer;
			const auto flips = 1 + random() % 4;
			for (uint32_t i = 0; i < flips; ++i) {
				corrupted[random() % corrupted.size()] = static_cast<std::byte>(random());
			}
			try {
				auto graph = ser::deserialize(corrupted, options);
				(void) graph->toString();
			} catch (const ser::SerializationException&) {
				++rejected;
			}
		}
		REQUIRE(rejected > 0);
	}

	SECTION("bad magic") {
		auto corrupted = buffer;
		corrupted[3] = std::byte {'?'};
		REQUIRE(deserializationError(corrupted).find("bad magic") != std::string::npos);
	}
}

TEST_CASE("Versioning") {
	const auto ir = traceToIR(loopWithRuntimeCall);
	const auto buffer = ser::serialize(*ir);

	SECTION("another major version is rejected") {
		const auto newer = withHeader(buffer, [](auto& header) { header.versionMajor = ser::VERSION_MAJOR + 1; });
		REQUIRE(deserializationError(newer).find("unsupported format version") != std::string::npos);
	}

	SECTION("a newer minor version is read") {
		const auto newer = withHeader(buffer, [](auto& header) { header.versionMinor = ser::VERSION_MINOR + 5; });
		REQUIRE(ser::deserialize(newer)->toString() == ir->toString());
	}

	SECTION("unknown incompatible features are rejected, unknown compatible ones ignored") {
		const auto incompatible = withHeader(buffer, [](auto& header) { header.incompatFeatures = 1u << 7; });
		REQUIRE(deserializationError(incompatible).find("unsupported features") != std::string::npos);
		const auto compatible = withHeader(buffer, [](auto& header) { header.compatFeatures = 1u << 7; });
		REQUIRE(ser::deserialize(compatible)->toString() == ir->toString());
	}

	SECTION("unknown sections are skipped unless marked required") {
		auto sections = readSections(buffer);
		Section extra {};
		extra.entry = ser::SectionEntry {ser::makeTag('N', 'E', 'W', '!'), 0, 0, 1, 5};
		extra.bytes.assign(5, std::byte {0xAB});
		sections.push_back(extra);
		REQUIRE(ser::deserialize(writeSections(buffer, sections))->toString() == ir->toString());

		sections.back().entry.flags = ser::SECTION_REQUIRED;
		REQUIRE(deserializationError(writeSections(buffer, sections)).find("unknown section") != std::string::npos);
	}

	SECTION("records grown by a newer minor version are read through their known prefix") {
		auto sections = readSections(buffer);
		for (auto& section : sections) {
			if (section.entry.tag != ser::TAG_OPERATIONS && section.entry.tag != ser::TAG_FUNCTIONS) {
				continue;
			}
			// Append 8 bytes of "new fields" to every record.
			const auto stride = section.entry.recordStride;
			std::vector<std::byte> grown;
			for (uint32_t i = 0; i < section.entry.recordCount; ++i) {
				grown.insert(grown.end(), section.bytes.begin() + i * stride, section.bytes.begin() + (i + 1) * stride);
				grown.insert(grown.end(), 8, std::byte {0x5A});
			}
			section.entry.recordStride = stride + 8;
			section.bytes = std::move(grown);
		}
		REQUIRE(ser::deserialize(writeSections(buffer, sections))->toString() == ir->toString());
	}

	SECTION("a missing section is rejected") {
		auto sections = readSections(buffer);
		std::erase_if(sections, [](const auto& s) { return s.entry.tag == ser::TAG_EDGES; });
		REQUIRE(deserializationError(writeSections(buffer, sections)).find("missing section") != std::string::npos);
	}
}

TEST_CASE("Pointer handling") {
	const auto ir = traceToIR(noexceptRuntimeCall);
	const auto* callee = reinterpret_cast<const void*>(&serializationNativeOffset);

	SECTION("portable serialization needs a namer") {
		ser::SerializeOptions options;
		options.pointerMode = ser::PointerMode::Portable;
		REQUIRE_THROWS_AS(ser::serialize(*ir, options), ser::SerializationException);
	}

	SECTION("portable serialization fails on an address the namer cannot name") {
		ser::SymbolTable empty;
		ser::SerializeOptions options;
		options.pointerMode = ser::PointerMode::Portable;
		options.namer = &empty;
		REQUIRE_THROWS_WITH(ser::serialize(*ir, options), Catch::Matchers::ContainsSubstring("no name for address"));
	}

	SECTION("a throwing call also needs its exception-capture thunk named") {
		ser::SymbolTable symbols;
		symbols.add("scale", reinterpret_cast<const void*>(&serializationNativeScale));
		ser::SerializeOptions options;
		options.pointerMode = ser::PointerMode::Portable;
		options.namer = &symbols;
		REQUIRE_THROWS_WITH(ser::serialize(*traceToIR(loopWithRuntimeCall), options),
		                    Catch::Matchers::ContainsSubstring("exception capture thunk"));
	}

	SECTION("a portable buffer needs every symbol resolved") {
		ser::SymbolTable symbols;
		symbols.add("offset", callee);
		ser::SerializeOptions options;
		options.pointerMode = ser::PointerMode::Portable;
		options.namer = &symbols;
		const auto buffer = ser::serialize(*ir, options);

		REQUIRE(deserializationError(buffer).find("needs a SymbolResolver") != std::string::npos);
		ser::SymbolTable other;
		ser::DeserializeOptions unresolved;
		unresolved.resolver = &other;
		REQUIRE(deserializationError(buffer, unresolved).find("unresolved symbol 'offset'") != std::string::npos);

		ser::DeserializeOptions resolved;
		resolved.resolver = &symbols;
		REQUIRE(ser::deserialize(buffer, resolved)->toString() == ir->toString());
	}

	SECTION("raw pointers are only trusted inside the writing process") {
		const auto local = ser::serialize(*ir);
		const auto foreign = withHeader(local, [](auto& header) { header.processToken ^= 1; });
		REQUIRE(deserializationError(foreign).find("another process") != std::string::npos);

		// A process-local buffer written with a namer carries the names
		// alongside the addresses, so another process can still resolve it.
		ser::SymbolTable symbols;
		symbols.add("offset", callee);
		ser::SerializeOptions options;
		options.namer = &symbols;
		const auto named = withHeader(ser::serialize(*ir, options), [](auto& header) { header.processToken ^= 1; });
		ser::DeserializeOptions readOptions;
		readOptions.resolver = &symbols;
		REQUIRE(ser::deserialize(named, readOptions)->toString() == ir->toString());
	}

	SECTION("the dynamic linker names exported functions") {
		ser::DynamicLinkerSymbols linker;
		auto* address = dlsym(RTLD_DEFAULT, "dlsym");
		REQUIRE(address != nullptr);
		const auto name = linker.nameOf(address);
		REQUIRE(name.has_value());
		REQUIRE(linker.resolve(*name) == address);
		REQUIRE_FALSE(linker.nameOf(static_cast<const char*>(address) + 1).has_value());
	}
}

} // namespace nautilus::engine
