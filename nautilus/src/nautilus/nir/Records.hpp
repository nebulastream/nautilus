#pragma once

#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Types.hpp"
#include <cstdint>

namespace nautilus::nir {

// The fixed-size records of the module sections (13-nir-spec.md §4). Sizes are part of the image format.

enum class Provenance : uint8_t { Literal = 0, AuditedInvariant = 1, RawProcessValue = 2 };

/// The least trusted class wins when constants are folded (§4.2).
constexpr Provenance leastTrusted(Provenance a, Provenance b) {
	return static_cast<uint8_t>(a) >= static_cast<uint8_t>(b) ? a : b;
}

struct ConstRecord {
	uint8_t type;
	uint8_t provenance;
	uint16_t pad;
	uint32_t byteOffset;
	uint64_t bits;
	bool operator==(const ConstRecord&) const = default;
};
static_assert(sizeof(ConstRecord) == 16);

struct BlockRecord {
	uint32_t first;
	uint32_t count;
	uint16_t argCount;
	uint16_t flags;
	uint32_t pad;
	bool operator==(const BlockRecord&) const = default;
};
static_assert(sizeof(BlockRecord) == 16);
constexpr uint16_t BLOCK_LANDING_PAD = 1;

enum class Level : uint8_t { PreSSA = 0, SSA = 1 };
enum class Visibility : uint8_t { Internal = 0, Exported = 1 };

struct FunctionRecord {
	uint32_t nameId;
	uint32_t signature;
	uint32_t firstInst, instCount;
	uint32_t firstBlock, blockCount;
	uint32_t firstAlloca, allocaCount;
	uint32_t attrs;
	uint8_t level;
	uint8_t visibility;
	uint16_t pad;
	bool operator==(const FunctionRecord&) const = default;
};
static_assert(sizeof(FunctionRecord) == 40);

struct SignatureRecord {
	uint32_t result;
	uint32_t paramCount;
	uint32_t paramTypes;
	bool operator==(const SignatureRecord&) const = default;
};
static_assert(sizeof(SignatureRecord) == 12);

struct AllocaRecord {
	uint32_t size;
	uint32_t align;
	bool operator==(const AllocaRecord&) const = default;
};
static_assert(sizeof(AllocaRecord) == 8);

enum class CalleeKind : uint8_t { Internal = 0, External = 1, Imported = 2, Intrinsic = 3 };

struct CalleeRecord {
	uint8_t kind;
	uint8_t pad[3];
	uint32_t target;
	uint32_t signature;
	uint32_t attrs;
	bool operator==(const CalleeRecord&) const = default;
};
static_assert(sizeof(CalleeRecord) == 16);

/// Attribute bits (§4.6.2): guarantees; zero promises nothing.
enum Attr : uint32_t {
	AttrNoArgRead = 1u << 0,
	AttrNoArgWrite = 1u << 1,
	AttrNoOtherRead = 1u << 2,
	AttrNoOtherWrite = 1u << 3,
	AttrNoUnwind = 1u << 4,
	AttrWillReturn = 1u << 5,
	AttrDeclared = 1u << 6, ///< not a guarantee
};
constexpr uint32_t GUARANTEE_MASK = 0x3F;
/// `have` implies `need` exactly when it has every guarantee bit `need` has.
constexpr bool attrsImply(uint32_t have, uint32_t need) {
	return (need & ~have & GUARANTEE_MASK) == 0;
}

enum class PointerKind : uint8_t {
	Callee = 0,
	CaptureThunk = 1,
	Destructor = 2,
	TracedAddress = 3,
	ExtensionPayload = 4
};
enum class PointerMode : uint8_t { ProcessLocal = 0, PortableByName = 1, PortableBuildId = 2 };

struct PointerRecord {
	uint8_t kind;
	uint8_t mode;
	uint8_t provenance;
	uint8_t pad;
	uint32_t nameId;
	uint64_t address;
	uint64_t reserved;
	bool operator==(const PointerRecord&) const = default;
};
static_assert(sizeof(PointerRecord) == 24);

struct BindingRecord {
	uint32_t nameId;
	uint32_t type;
	uint32_t size;
	uint32_t align;
	bool operator==(const BindingRecord&) const = default;
};
static_assert(sizeof(BindingRecord) == 16);

struct NameRecord {
	uint32_t offset;
	uint32_t length;
	bool operator==(const NameRecord&) const = default;
};
static_assert(sizeof(NameRecord) == 8);

struct ExtensionRecord {
	uint32_t nameId;
	uint32_t kindAndVersion; // layout defined with the registry (M1.7)
	bool operator==(const ExtensionRecord&) const = default;
};
static_assert(sizeof(ExtensionRecord) == 8);

struct DebugLocationRecord {
	uint32_t file, line, column;
	bool operator==(const DebugLocationRecord&) const = default;
};
static_assert(sizeof(DebugLocationRecord) == 12);

struct RegionRecord {
	uint32_t function, nameId, location, parent;
	bool operator==(const RegionRecord&) const = default;
};
static_assert(sizeof(RegionRecord) == 16);

struct ModuleRecord {
	uint32_t nameId;
	uint32_t flags;
	uint64_t processToken;
	bool operator==(const ModuleRecord&) const = default;
};
static_assert(sizeof(ModuleRecord) == 16);

struct ImportRecord {
	uint32_t moduleNameId;
	uint32_t functionNameId;
	uint32_t signature;
	uint32_t attrs;
	bool operator==(const ImportRecord&) const = default;
};
static_assert(sizeof(ImportRecord) == 16);

struct DataRecord {
	uint32_t size;
	uint32_t align;
	uint32_t byteOffset;
	uint32_t flags;
	bool operator==(const DataRecord&) const = default;
};
static_assert(sizeof(DataRecord) == 16);

struct FunctionAttrRecord {
	uint32_t function;
	uint32_t keyNameId;
	uint32_t valueNameId;
	uint32_t pad;
	bool operator==(const FunctionAttrRecord&) const = default;
};
static_assert(sizeof(FunctionAttrRecord) == 16);

struct DebugFunctionRecord {
	uint32_t function, location, firstParam, paramCount;
	bool operator==(const DebugFunctionRecord&) const = default;
};
static_assert(sizeof(DebugFunctionRecord) == 16);

} // namespace nautilus::nir
