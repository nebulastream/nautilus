# IR Serialization

## Overview

Nautilus IR can be written to a compact binary buffer and read back into an `IRGraph`. The format is built to:

- **move IR between processes and machines**: trace on one node, compile on another, or keep compiled-from IR in a cache;
- **be versioned**, so readers and writers of different releases either interoperate or fail with a clear error;
- **be safe to read from the network**: every offset, index and count is bounds-checked, and a malformed buffer surfaces as a `SerializationException`, never as a crash;
- **be cheap to read**: the buffer is a set of flat, fixed-width record tables that are read in place (no parsing pass, no intermediate tree), and the graph is materialized in one linear pass into its arena.

Both pre-pass IR (straight out of trace-to-IR conversion) and IR after any number of passes round-trip, including region tables and exception landing pads.

```cpp
#include "nautilus/compiler/ir/serialization/IRSerialization.hpp"

namespace ser = nautilus::compiler::ir::serialization;

// Sender
ser::SymbolTable symbols;                 // or your own SymbolNamer
symbols.add("hash_combine", reinterpret_cast<void*>(&hashCombine));
ser::SerializeOptions out;
out.pointerMode = ser::PointerMode::Portable;
out.namer = &symbols;
std::vector<std::byte> bytes = ser::serialize(*graph, out);

// Receiver
ser::DeserializeOptions in;
in.resolver = &symbols;                   // or your own SymbolResolver
in.verifyIR = true;                       // recommended for untrusted input
std::shared_ptr<IRGraph> graph = ser::deserialize(bytes, in);
```

The serialization API currently lives next to the IR in `src/nautilus/compiler/ir/serialization/`, like the rest of the IR. It is not yet exposed through `Engine`/`Module`.

## Pointers and symbol resolution

IR refers to process-local addresses in several places: native callees in the function table, call targets, exception-capture thunks, destructors and `ConstPtr` constants. How such an address is found again on the receiving side is up to the client, so the format never decides it. Instead, every pointer goes through one table in the buffer, written in one of two modes:

| Mode | What is written | Who can read it |
|------|-----------------|-----------------|
| `PointerMode::ProcessLocal` (default) | Raw addresses, plus a random token identifying the writing process. If a `SymbolNamer` is supplied, its names are recorded too. | The writing process. Another process can read it only if names were recorded and it has a `SymbolResolver` for them. |
| `PointerMode::Portable` | A symbol name for every non-null pointer. Serialization fails (naming the offending call or constant) if the namer cannot name one. | Any process with a `SymbolResolver` for those names. |

Clients plug in their scheme by implementing `SymbolNamer` (address → name) and `SymbolResolver` (name → address). Two implementations ship with Nautilus:

- **`SymbolTable`**: an explicit name ↔ address registry. Use it when both sides agree on a fixed set of runtime functions and globals. It is also the only way to ship a constant data pointer. It can fall back to another namer/resolver for addresses it does not hold.
- **`DynamicLinkerSymbols`**: `dladdr`/`dlsym`. Works for exported functions of shared libraries and of executables linked with `-rdynamic`.

Two details:

- A call to a throwing function also carries the address of its exception-capture thunk (`captureThrowingCall<…>`), which needs a name in portable mode too. Calls to `noexcept` functions have none.
- Calls between Nautilus functions in the same module store an identity key rather than code. Backends reach those callees through the function table, so portable mode writes that key as null and needs no name for it.

Intrinsic ids are never shipped. The reader re-derives them from the resolved address through the local `IntrinsicRegistry`, because ids are minted per process in registration order.

Source tags (the trace-time call stacks behind `showSourceLocations` and debug info) point into the tracer's arena. They are not serialized, so a deserialized graph has none.

## Network framing

The header is self-describing: `peekTotalSize(prefix)` returns the full size of a message from its first 24 bytes, so a stream reader can frame messages without understanding them. The format never compresses. Compressing would rule out in-place reads, so compression (e.g. zstd) belongs in the transport. Integrity is checked with a CRC-32C over everything after the header. `DeserializeOptions::verifyChecksum` can turn the check off when the transport already guarantees integrity.

Deserialization validates the buffer's *structure*: bounds, indices, operand counts, edge arities and type consistency. Set `DeserializeOptions::verifyIR` to also run the IR verifier on the result, which checks *semantics* such as dominance and terminators. Do this for buffers from a source you do not trust.

## Format

All integers are little-endian; every section starts on an 8-byte boundary.

```
FileHeader (72 bytes)
  magic        "NAUTIR\r\n"   the \r\n catches text-mode mangling, as in PNG
  u16 major, u16 minor
  u32 headerSize               newer writers may append header fields
  u64 totalSize                for framing
  u64 incompatFeatures         a reader rejects any bit it does not know
  u64 compatFeatures           a reader ignores bits it does not know
  u32 flags                    PROCESS_LOCAL
  u32 sectionCount, u64 sectionDirOffset
  u64 processToken             identity of the writer (process-local buffers)
  u32 checksum                 CRC-32C of bytes [headerSize, totalSize)
SectionEntry[sectionCount] { u32 tag, u32 flags, u64 offset, u32 recordStride, u32 recordCount }
sections...
```

| Tag | Contents |
|-----|----------|
| `STRS` | deduplicated string bytes, referenced as `(offset, length)`; offset `0xFFFFFFFF` is a null `const char*` |
| `MODL` | the module record (compilation unit id) |
| `PTRS` | pointer table: `Null`, `Raw` (address [+ name]) or `Symbol` (name) |
| `FTAB` | function table, one record per `FunctionId`, in id order |
| `FUNC` | functions: name, signature, block and value ranges, side tables |
| `BLKS` | blocks: identifier, region, value range |
| `OPS ` | operations, 32 bytes each: wire opcode, type, region, SSA id, operand range, payloads |
| `OPND` | operand pool: function-relative value indices |
| `EDGE` | successor edges: target block + argument range |
| `CNST` | 64-bit constant pool (integer values, float bits, branch probabilities) |
| `CALL`, `DTOR` | call details and their destructors |
| `ALLC`, `ATTR`, `REGN`, `STRR`, `IDXS`, `XSIT` | alloca specs, function attributes, region specs, argument names, landing pads, exceptional call sites |

The values of a function, its block arguments and its operations alike, are numbered densely, block by block: a block's arguments first, then its operations. Blocks tile this numbering exactly. Every reference is a 32-bit index into a table whose size the reader knows, which is what makes validation cheap and complete. Operations use their own stable *wire opcodes*, mapped explicitly from the C++ enum, so reordering `Operation::OperationType` cannot change the format.

### Versioning rules

- **Major version**: bumped for changes an older reader cannot interpret. Readers reject other majors.
- **Minor version**: additive changes only. Readers accept any minor.
- **New record fields** are appended. The section directory carries each section's record stride, and a reader reads the prefix it knows and skips the rest.
- **New sections** get a new tag. A reader skips sections it does not know, unless the writer flagged them `SECTION_REQUIRED`.
- **New opcodes** or anything else an older reader would silently misread must set an incompatible feature bit, or bump the major version.

Serialization is deterministic: equal graphs produce equal bytes, so a hash of a portable buffer works as a cache key.
