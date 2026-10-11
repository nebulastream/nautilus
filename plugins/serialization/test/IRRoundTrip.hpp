#pragma once

#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/serialization/IRSerialization.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <unordered_map>

namespace nautilus::testing {

/**
 * Names every address it is asked about ("sym0", "sym1", ... in first-seen
 * order) and resolves those names back. Stands in for a real symbol scheme so
 * tests can drive the portable path on IR whose callees are not exported.
 */
class SyntheticSymbols final : public serialization::SymbolNamer, public serialization::SymbolResolver {
public:
	[[nodiscard]] std::optional<std::string> nameOf(const void* address) const override {
		auto [it, inserted] = names.try_emplace(address, "sym" + std::to_string(names.size()));
		if (inserted) {
			addresses.emplace(it->second, address);
		}
		return it->second;
	}

	[[nodiscard]] void* resolve(std::string_view symbol) const override {
		const auto it = addresses.find(std::string(symbol));
		return it != addresses.end() ? const_cast<void*>(it->second) : nullptr;
	}

private:
	mutable std::unordered_map<const void*, std::string> names;
	mutable std::unordered_map<std::string, const void*> addresses;
};

/**
 * Serializes @p graph in both pointer modes and checks that each buffer
 * materializes into a graph that prints identically, verifies no worse, and
 * re-serializes to the very same bytes.
 */
inline void requireSerializationRoundTrip(compiler::ir::IRGraph& graph) {
	namespace ser = nautilus::serialization;
	const auto expected = graph.toString();
	// The verifier relies on predecessor lists, which a graph straight out of
	// trace-to-IR conversion does not carry yet; the pass manager would build
	// them the same way. The deserializer always builds them.
	compiler::ir::rebuildPredecessorLists(graph);
	const bool verifiedBefore = compiler::ir::IRVerifier::verify(graph).ok();

	const auto local = ser::serialize(graph);
	auto fromLocal = ser::deserialize(local);
	REQUIRE(fromLocal->toString() == expected);
	REQUIRE(ser::serialize(*fromLocal) == local);
	if (verifiedBefore) {
		REQUIRE(compiler::ir::IRVerifier::verify(*fromLocal).ok());
	}

	SyntheticSymbols symbols;
	ser::SerializeOptions portableOptions;
	portableOptions.pointerMode = ser::PointerMode::Portable;
	portableOptions.namer = &symbols;
	ser::DeserializeOptions readOptions;
	readOptions.resolver = &symbols;
	const auto portable = ser::serialize(graph, portableOptions);
	auto fromPortable = ser::deserialize(portable, readOptions);
	REQUIRE(fromPortable->toString() == expected);
	REQUIRE(ser::serialize(*fromPortable, portableOptions) == portable);
}

} // namespace nautilus::testing
