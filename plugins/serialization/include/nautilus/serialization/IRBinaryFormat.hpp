#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>

/**
 * On-wire layout of the Nautilus IR binary format (docs/ir-serialization.md).
 *
 * Every struct here is a plain, fixed-width record that is written to and read
 * from the buffer byte-for-byte; the static_asserts at the bottom pin each one's
 * size so a change to a record is a deliberate format change rather than an
 * accident of padding. All multi-byte integers are little-endian.
 *
 * Nothing in this header depends on the in-memory IR: the enums below are the
 * *wire* spellings, mapped to and from the IR's own enums by the writer and the
 * reader. Reordering an IR enum therefore cannot silently change the format.
 */
namespace nautilus::serialization {

static_assert(std::endian::native == std::endian::little,
              "the Nautilus IR binary format is little-endian; a big-endian reader needs byte-swapping loads");

inline constexpr char MAGIC[8] = {'N', 'A', 'U', 'T', 'I', 'R', '\r', '\n'};

/// Bumped on any change an older reader cannot interpret. A reader rejects
/// every major version but its own.
inline constexpr uint16_t VERSION_MAJOR = 1;
/// Bumped on additive changes (new sections, fields appended to a record,
/// new compat feature bits). A reader accepts any minor version.
inline constexpr uint16_t VERSION_MINOR = 0;

/// Every section starts at a multiple of this, so a reader that wants to map
/// records in place may.
inline constexpr uint32_t SECTION_ALIGNMENT = 8;

/// The "no value" sentinel for every u32 index field.
inline constexpr uint32_t NONE = std::numeric_limits<uint32_t>::max();

/// Header flag: the pointer table holds raw addresses that are only
/// meaningful inside the process named by `FileHeader::processToken`.
inline constexpr uint32_t FLAG_PROCESS_LOCAL = 1u << 0;

/// Incompatible features: a reader must reject a buffer that sets a bit it
/// does not know. None are defined in 1.0.
inline constexpr uint64_t KNOWN_INCOMPAT_FEATURES = 0;

struct FileHeader {
	char magic[8];
	uint16_t versionMajor;
	uint16_t versionMinor;
	/// Size of this header as written; a newer writer may append fields.
	uint32_t headerSize;
	/// Size of the whole buffer, header included. Lets a network reader frame
	/// a message from its first 24 bytes.
	uint64_t totalSize;
	uint64_t incompatFeatures;
	uint64_t compatFeatures;
	uint32_t flags;
	uint32_t sectionCount;
	uint64_t sectionDirOffset;
	/// Identity of the writing process; only consulted for FLAG_PROCESS_LOCAL.
	uint64_t processToken;
	/// CRC-32C over bytes [headerSize, totalSize).
	uint32_t checksum;
	uint32_t reserved;
};

/// Four-character section tags, stored little-endian so the bytes read as text.
constexpr uint32_t makeTag(char a, char b, char c, char d) {
	return static_cast<uint32_t>(static_cast<uint8_t>(a)) | (static_cast<uint32_t>(static_cast<uint8_t>(b)) << 8) |
	       (static_cast<uint32_t>(static_cast<uint8_t>(c)) << 16) |
	       (static_cast<uint32_t>(static_cast<uint8_t>(d)) << 24);
}

enum SectionTag : uint32_t {
	TAG_STRINGS = makeTag('S', 'T', 'R', 'S'),     ///< raw string bytes
	TAG_MODULE = makeTag('M', 'O', 'D', 'L'),      ///< ModuleRecord (exactly one)
	TAG_POINTERS = makeTag('P', 'T', 'R', 'S'),    ///< PointerRecord
	TAG_TARGETS = makeTag('F', 'T', 'A', 'B'),     ///< TargetRecord
	TAG_TYPES = makeTag('T', 'Y', 'P', 'S'),       ///< u8 type pool
	TAG_FUNCTIONS = makeTag('F', 'U', 'N', 'C'),   ///< FunctionRecord
	TAG_BLOCKS = makeTag('B', 'L', 'K', 'S'),      ///< BlockRecord
	TAG_OPERATIONS = makeTag('O', 'P', 'S', ' '),  ///< OpRecord
	TAG_OPERANDS = makeTag('O', 'P', 'N', 'D'),    ///< u32 value-index pool
	TAG_EDGES = makeTag('E', 'D', 'G', 'E'),       ///< EdgeRecord
	TAG_CONSTANTS = makeTag('C', 'N', 'S', 'T'),   ///< u64 constant pool
	TAG_CALLS = makeTag('C', 'A', 'L', 'L'),       ///< CallRecord
	TAG_DESTRUCTORS = makeTag('D', 'T', 'O', 'R'), ///< DestructorRecord
	TAG_ALLOCAS = makeTag('A', 'L', 'L', 'C'),     ///< AllocaRecord
	TAG_ATTRIBUTES = makeTag('A', 'T', 'T', 'R'),  ///< AttributeRecord
	TAG_REGIONS = makeTag('R', 'E', 'G', 'N'),     ///< RegionRecord
	TAG_STRING_REFS = makeTag('S', 'T', 'R', 'R'), ///< StringRef pool
	TAG_INDICES = makeTag('I', 'D', 'X', 'S'),     ///< u32 index pool (landing pads)
	TAG_CALL_SITES = makeTag('X', 'S', 'I', 'T'),  ///< CallSiteRecord
};

/// Section flag: a reader that does not know this section's tag must reject
/// the buffer. Unknown sections without it are skipped.
inline constexpr uint32_t SECTION_REQUIRED = 1u << 0;

struct SectionEntry {
	uint32_t tag;
	uint32_t flags;
	uint64_t offset;
	/// Bytes per record as written. A reader reads the prefix it knows and
	/// skips the rest, which is how a minor version appends record fields.
	uint32_t recordStride;
	uint32_t recordCount;
};

/// A string in the STRS blob. `offset == NONE` encodes a null `const char*`,
/// which is distinct from the empty string.
struct StringRef {
	uint32_t offset;
	uint32_t length;
};

struct SourceLocationRecord {
	StringRef file;
	StringRef function;
	uint32_t line;
	uint32_t column;
};

struct ModuleRecord {
	StringRef compilationUnitId;
};

enum class PointerKind : uint8_t {
	Null = 0,
	/// A raw address; only valid in the process that wrote it.
	Raw = 1,
	/// A symbol name the reader resolves through its SymbolResolver.
	Symbol = 2,
};

struct PointerRecord {
	uint8_t kind;
	uint8_t reserved[7];
	/// Set for Raw; informational otherwise.
	uint64_t address;
	/// Set for Symbol; for Raw it is a best-effort name a reader may fall back to.
	StringRef symbol;
};

enum class WireLinkage : uint8_t {
	Internal = 0,
	Native = 1,
};

/// Wire spelling of FunctionAttributes::willReturn / noUnwind.
inline constexpr uint8_t ATTR_WILL_RETURN = 1u << 0;
inline constexpr uint8_t ATTR_NO_UNWIND = 1u << 1;

struct TargetRecord {
	uint8_t linkage;
	uint8_t resultType;
	/// Native: the callee's attributes. Internal: the derived attributes.
	uint8_t modRef;
	uint8_t attrFlags;
	uint32_t paramTypesBegin; ///< into TYPS, native only
	uint32_t paramCount;
	/// Internal: index into FUNC, or NONE for a target never defined.
	/// Native: index into PTRS.
	uint32_t payload;
	StringRef mangled;
	StringRef demangled;
	StringRef custom;
	StringRef minted;
	StringRef emission;
};

struct FunctionRecord {
	StringRef name;
	uint8_t outputType;
	uint8_t hasExceptionRegion;
	uint16_t reserved;
	/// Blocks [blocksBegin, blocksBegin + bodyBlockCount) are the function body
	/// in order; the following `extraBlockCount` are blocks reachable only from
	/// side tables (landing pads).
	uint32_t blocksBegin;
	uint32_t bodyBlockCount;
	uint32_t extraBlockCount;
	/// Every value of the function, laid out block by block (arguments first).
	/// Value indices in OPND, EDGE, DTOR and XSIT are relative to opsBegin.
	uint32_t opsBegin;
	uint32_t opCount;
	uint32_t inputTypesBegin; ///< into TYPS
	uint32_t inputTypeCount;
	uint32_t argNamesBegin; ///< into STRR
	uint32_t argNameCount;
	uint32_t allocasBegin;
	uint32_t allocaCount;
	uint32_t attributesBegin;
	uint32_t attributeCount;
	uint32_t regionsBegin;
	uint32_t regionCount;
	uint32_t padsBegin; ///< into IDXS: one function-relative block index per pad
	uint32_t padCount;
	uint32_t callSitesBegin;
	uint32_t callSiteCount;
	SourceLocationRecord location;
};

struct BlockRecord {
	uint32_t identifier;
	uint16_t region;
	uint16_t reserved;
	/// Function-relative value index of the block's first argument; the
	/// block's arguments and then its operations follow contiguously.
	uint32_t firstValue;
	uint32_t argumentCount;
	uint32_t operationCount;
};

/// Stable wire opcodes. Never renumber; only append.
enum class WireOpcode : uint8_t {
	BlockArgument = 0,
	ConstInt = 1,
	ConstFloat = 2,
	ConstBoolean = 3,
	ConstPtr = 4,
	Add = 5,
	Sub = 6,
	Mul = 7,
	Div = 8,
	Mod = 9,
	And = 10,
	Or = 11,
	Not = 12,
	Negate = 13,
	Compare = 14,
	Shift = 15,
	BinaryComp = 16,
	Cast = 17,
	Load = 18,
	Store = 19,
	Select = 20,
	Alloca = 21,
	Call = 22,
	IndirectCall = 23,
	FunctionAddressOf = 24,
	Branch = 25,
	If = 26,
	Return = 27,
};
inline constexpr uint8_t WIRE_OPCODE_COUNT = 28;

/// Wire values of the `aux` field for the ops that carry a sub-kind.
enum class WireComparator : uint8_t { EQ = 0, NE = 1, LT = 2, LE = 3, GT = 4, GE = 5 };
enum class WireShift : uint8_t { Left = 0, Right = 1 };
enum class WireBitwise : uint8_t { And = 0, Or = 1, Xor = 2 };

/**
 * One operation. The meaning of the payload fields depends on the opcode:
 *
 *   ConstInt / ConstFloat   payload0 = CNST index
 *   ConstBoolean            aux = value
 *   ConstPtr                payload0 = PTRS index
 *   Compare / Shift / BinaryComp   aux = wire sub-kind
 *   Alloca                  payload0 = alloca index
 *   Call                    payload0 = callee FunctionId, payload1 = CALL index
 *   IndirectCall            payload1 = CALL index (operand 0 is the callee)
 *   FunctionAddressOf       payload0 = callee FunctionId, payload1 = CALL index
 *   Branch                  payload0 = EDGE index
 *   If                      payload0 = EDGE index of the true edge (false edge
 *                           follows), payload1 = merge block or NONE,
 *                           payload2 = CNST index of the probability
 *
 * Unused payload fields are NONE.
 */
struct OpRecord {
	uint8_t opcode;
	uint8_t type;
	uint16_t region;
	uint32_t identifier;
	uint32_t operandsBegin; ///< into OPND
	uint32_t operandCount;
	uint32_t payload0;
	uint32_t payload1;
	uint32_t payload2;
	uint32_t aux;
};

struct EdgeRecord {
	uint32_t targetBlock;    ///< function-relative block index
	uint32_t argumentsBegin; ///< into OPND
	uint32_t argumentCount;
};

inline constexpr uint8_t CALL_EXCEPTION_HANDLING = 1u << 0;
inline constexpr uint8_t CALL_IS_NAUTILUS_CALL = 1u << 1;

struct CallRecord {
	StringRef symbol;
	StringRef name;
	uint32_t functionPtr; ///< PTRS index
	uint32_t captureFunc; ///< PTRS index
	uint8_t modRef;
	uint8_t attrFlags;
	uint8_t callFlags;
	uint8_t reserved;
	uint32_t destructorsBegin;
	uint32_t destructorCount;
};

struct DestructorRecord {
	uint32_t address;     ///< function-relative value index
	uint32_t functionPtr; ///< PTRS index
	StringRef symbol;
	StringRef name;
};

struct AllocaRecord {
	uint64_t size;
	uint64_t align;
};

struct AttributeRecord {
	StringRef key;
	StringRef value;
};

struct RegionRecord {
	StringRef name;
	SourceLocationRecord location;
	uint16_t parent;
	uint16_t reserved;
	uint32_t id;
};

struct CallSiteRecord {
	uint32_t call;     ///< function-relative value index
	uint32_t padIndex; ///< index into the function's pads, or NONE
};

static_assert(sizeof(FileHeader) == 72);
static_assert(sizeof(SectionEntry) == 24);
static_assert(sizeof(StringRef) == 8);
static_assert(sizeof(SourceLocationRecord) == 24);
static_assert(sizeof(ModuleRecord) == 8);
static_assert(sizeof(PointerRecord) == 24);
static_assert(sizeof(TargetRecord) == 56);
static_assert(sizeof(FunctionRecord) == 112);
static_assert(sizeof(BlockRecord) == 20);
static_assert(sizeof(OpRecord) == 32);
static_assert(sizeof(EdgeRecord) == 12);
static_assert(sizeof(CallRecord) == 36);
static_assert(sizeof(DestructorRecord) == 24);
static_assert(sizeof(AllocaRecord) == 16);
static_assert(sizeof(AttributeRecord) == 16);
static_assert(sizeof(RegionRecord) == 40);
static_assert(sizeof(CallSiteRecord) == 8);

} // namespace nautilus::serialization
