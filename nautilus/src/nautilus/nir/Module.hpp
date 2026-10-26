#pragma once

#include "nautilus/nir/ConstPool.hpp"
#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Records.hpp"
#include <array>
#include <cstdint>
#include <map>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nautilus::nir {

/// A module: the unit of compilation, loading, caching and serialization (13-nir-spec.md §4).
/// The sections are flat arrays that the image maps one to one; the interning tables are derived and not part of it.
class Module {
public:
	// canonical sections (§4.1)
	std::vector<Inst> insts;
	std::vector<uint32_t> extra;
	ConstPool consts;
	std::vector<BlockRecord> blocks;
	std::vector<FunctionRecord> functions;
	std::vector<SignatureRecord> signatures;
	std::vector<CalleeRecord> callees;
	std::vector<AllocaRecord> allocas;
	std::vector<PointerRecord> pointers;
	std::vector<BindingRecord> bindings;
	std::vector<NameRecord> names;
	std::vector<char> nameBytes;
	std::vector<ImportRecord> imports;
	std::vector<DataRecord> data;
	std::vector<std::byte> dataBytes;
	std::vector<FunctionAttrRecord> functionAttrs;
	std::vector<ExtensionRecord> extensions;
	ModuleRecord module {NONE, 0, 0};

	// optional debug sections (§4.9)
	std::vector<DebugLocationRecord> debugLocations;
	std::vector<uint32_t> debugInstLocations;
	std::vector<RegionRecord> debugRegions;
	std::vector<uint32_t> debugInstRegions;
	std::vector<NameRecord> debugNames;
	std::vector<char> debugNameBytes;
	std::vector<DebugFunctionRecord> debugFunctions;
	std::vector<uint32_t> debugParamNames;

	/// Names are interned: equal strings are one id.
	uint32_t internName(std::string_view s);
	std::string_view name(uint32_t id) const;

	/// Signatures are interned: equal result and type words are one signature (§4.5).
	uint32_t internSignature(TypeWord result, std::span<const TypeWord> params);
	TypeWord paramType(const SignatureRecord& sig, uint32_t i) const {
		return extra[sig.paramTypes + i];
	}

	/// Callees and pointers are interned: equal records are one entry (§4.6, §4.7).
	uint32_t internCallee(const CalleeRecord& r);
	uint32_t internPointer(const PointerRecord& r);

	/// Bindings are never interned (§4.8).
	uint32_t addBinding(uint32_t nameId, TypeId type, uint32_t size, uint32_t align);
	/// Data objects may share an address, so equal ones are one object (§4.10).
	uint32_t internData(std::span<const std::byte> bytes, uint32_t align);
	uint32_t addImport(const ImportRecord& r);

	/// Appends a list to `extra` and returns its word offset.
	uint32_t addExtra(std::span<const uint32_t> words);

	/// Recomputes the interning tables from the sections, for modules assembled by writing the arrays directly (the
	/// parser, the image loader). Of equal records the first one wins.
	void rebuildIndexes();

	/// Instructions, blocks and allocas of function `f`.
	std::span<const Inst> functionInsts(uint32_t f) const;
	std::span<Inst> functionInsts(uint32_t f);
	std::span<const BlockRecord> functionBlocks(uint32_t f) const;
	std::span<const AllocaRecord> functionAllocas(uint32_t f) const;

private:
	std::unordered_map<std::string, uint32_t> nameIndex_;
	std::map<std::vector<uint32_t>, uint32_t> signatureIndex_;
	std::map<std::array<uint32_t, 4>, uint32_t> calleeIndex_;
	std::map<std::array<uint64_t, 4>, uint32_t> pointerIndex_;
	std::map<std::pair<uint32_t, std::vector<std::byte>>, uint32_t> dataIndex_;
};

/// The 64-bit token of this process, drawn at random once and never 0 (13-nir-spec.md §4.4).
uint64_t currentProcessToken();

} // namespace nautilus::nir
