#include "Crc32c.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/IntrinsicRegistry.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/SubOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
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
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/serialization/IRBinaryFormat.hpp"
#include "nautilus/serialization/IRSerialization.hpp"
#include <bit>
#include <cstring>
#include <fmt/format.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace nautilus::serialization {

using namespace compiler::ir;

namespace {

/// Throws unless @p condition holds. Every check on untrusted input goes
/// through here, so a malformed buffer always surfaces as one exception type.
void require(bool condition, std::string_view message) {
	if (!condition) {
		throw SerializationException(fmt::format("malformed buffer: {}", message));
	}
}

/// [begin, begin + count) lies within [0, size). Overflow-safe.
bool inRange(uint32_t begin, uint32_t count, uint64_t size) {
	return static_cast<uint64_t>(begin) + count <= size;
}

Type toType(uint8_t raw) {
	require(raw <= static_cast<uint8_t>(Type::ptr), "unknown type");
	return static_cast<Type>(raw);
}

FunctionAttributes toAttributes(uint8_t modRef, uint8_t flags) {
	require(modRef <= static_cast<uint8_t>(ModRefInfo::ModRef), "unknown mod/ref kind");
	FunctionAttributes attrs;
	attrs.modRefInfo = static_cast<ModRefInfo>(modRef);
	attrs.willReturn = (flags & ATTR_WILL_RETURN) != 0;
	attrs.noUnwind = (flags & ATTR_NO_UNWIND) != 0;
	return attrs;
}

CompareOperation::Comparator toComparator(uint32_t raw) {
	switch (static_cast<WireComparator>(raw)) {
	case WireComparator::EQ:
		return CompareOperation::EQ;
	case WireComparator::NE:
		return CompareOperation::NE;
	case WireComparator::LT:
		return CompareOperation::LT;
	case WireComparator::LE:
		return CompareOperation::LE;
	case WireComparator::GT:
		return CompareOperation::GT;
	case WireComparator::GE:
		return CompareOperation::GE;
	}
	throw SerializationException("malformed buffer: unknown comparator");
}

ShiftOperation::ShiftType toShift(uint32_t raw) {
	switch (static_cast<WireShift>(raw)) {
	case WireShift::Left:
		return ShiftOperation::LS;
	case WireShift::Right:
		return ShiftOperation::RS;
	}
	throw SerializationException("malformed buffer: unknown shift kind");
}

BinaryCompOperation::Type toBitwise(uint32_t raw) {
	switch (static_cast<WireBitwise>(raw)) {
	case WireBitwise::And:
		return BinaryCompOperation::BAND;
	case WireBitwise::Or:
		return BinaryCompOperation::BOR;
	case WireBitwise::Xor:
		return BinaryCompOperation::XOR;
	}
	throw SerializationException("malformed buffer: unknown bitwise kind");
}

struct SectionView {
	const std::byte* data = nullptr;
	uint32_t stride = 0;
	uint32_t count = 0;
	bool present = false;
};

/**
 * A validated, zero-copy view over a serialized buffer: the header and the
 * section directory are checked up front, records are read in place on
 * demand. Records are copied out with memcpy, so the buffer needs no
 * particular alignment (a network receive buffer rarely has any).
 */
class BufferView {
public:
	BufferView(std::span<const std::byte> buffer, const DeserializeOptions& options) : buffer(buffer) {
		require(buffer.size() >= sizeof(FileHeader), "shorter than the file header");
		std::memcpy(&header, buffer.data(), sizeof(FileHeader));
		require(std::memcmp(header.magic, MAGIC, sizeof(MAGIC)) == 0, "bad magic");
		if (header.versionMajor != VERSION_MAJOR) {
			throw SerializationException(fmt::format("unsupported format version {}.{} (this reader reads {}.x)",
			                                         header.versionMajor, header.versionMinor, VERSION_MAJOR));
		}
		if ((header.incompatFeatures & ~KNOWN_INCOMPAT_FEATURES) != 0) {
			throw SerializationException(fmt::format("buffer requires unsupported features {:#x}",
			                                         header.incompatFeatures & ~KNOWN_INCOMPAT_FEATURES));
		}
		require(header.headerSize >= sizeof(FileHeader) && header.headerSize <= buffer.size(), "bad header size");
		require(header.totalSize == buffer.size(), "size does not match the header");
		if (options.verifyChecksum) {
			require(crc32c(buffer.subspan(header.headerSize)) == header.checksum, "checksum mismatch");
		}
		require(header.sectionDirOffset <= buffer.size() &&
		            header.sectionCount <= (buffer.size() - header.sectionDirOffset) / sizeof(SectionEntry),
		        "section directory out of bounds");

		for (uint32_t i = 0; i < header.sectionCount; ++i) {
			SectionEntry entry {};
			std::memcpy(&entry, buffer.data() + header.sectionDirOffset + i * sizeof(SectionEntry),
			            sizeof(SectionEntry));
			const uint64_t bytes = static_cast<uint64_t>(entry.recordStride) * entry.recordCount;
			require(entry.offset <= buffer.size() && bytes <= buffer.size() - entry.offset, "section out of bounds");
			auto* view = sectionFor(entry.tag);
			if (view == nullptr) {
				if ((entry.flags & SECTION_REQUIRED) != 0) {
					throw SerializationException(fmt::format(
					    "buffer requires unknown section {:#010x}; it was written by a newer writer", entry.tag));
				}
				continue;
			}
			require(!view->present, "duplicate section");
			*view = SectionView {buffer.data() + entry.offset, entry.recordStride, entry.recordCount, true};
		}

		requireSection(strings, 1, true);
		requireSection(module, sizeof(ModuleRecord));
		requireSection(pointers, sizeof(PointerRecord));
		requireSection(targets, sizeof(TargetRecord));
		requireSection(types, 1, true);
		requireSection(functions, sizeof(FunctionRecord));
		requireSection(blocks, sizeof(BlockRecord));
		requireSection(ops, sizeof(OpRecord));
		requireSection(operands, sizeof(uint32_t), true);
		requireSection(edges, sizeof(EdgeRecord));
		requireSection(constants, sizeof(uint64_t), true);
		requireSection(calls, sizeof(CallRecord));
		requireSection(destructors, sizeof(DestructorRecord));
		requireSection(allocas, sizeof(AllocaRecord));
		requireSection(attributes, sizeof(AttributeRecord));
		requireSection(regions, sizeof(RegionRecord));
		requireSection(stringRefs, sizeof(StringRef));
		requireSection(indices, sizeof(uint32_t), true);
		requireSection(callSites, sizeof(CallSiteRecord));
		require(module.count == 1, "expected exactly one module record");
	}

	/// Record @p index of @p section. A record written with a larger stride
	/// by a newer minor version is read through its known prefix.
	template <typename T>
	T get(const SectionView& section, uint32_t index) const {
		require(index < section.count, "record index out of range");
		T value {};
		std::memcpy(&value, section.data + static_cast<uint64_t>(index) * section.stride, sizeof(T));
		return value;
	}

	std::string_view string(StringRef ref) const {
		require(ref.offset != NONE, "unexpected null string");
		require(inRange(ref.offset, ref.length, strings.count), "string out of range");
		return {reinterpret_cast<const char*>(strings.data) + ref.offset, ref.length};
	}

	FileHeader header {};
	SectionView strings, module, pointers, targets, types, functions, blocks, ops, operands, edges, constants, calls,
	    destructors, allocas, attributes, regions, stringRefs, indices, callSites;

private:
	SectionView* sectionFor(uint32_t tag) {
		switch (tag) {
		case TAG_STRINGS:
			return &strings;
		case TAG_MODULE:
			return &module;
		case TAG_POINTERS:
			return &pointers;
		case TAG_TARGETS:
			return &targets;
		case TAG_TYPES:
			return &types;
		case TAG_FUNCTIONS:
			return &functions;
		case TAG_BLOCKS:
			return &blocks;
		case TAG_OPERATIONS:
			return &ops;
		case TAG_OPERANDS:
			return &operands;
		case TAG_EDGES:
			return &edges;
		case TAG_CONSTANTS:
			return &constants;
		case TAG_CALLS:
			return &calls;
		case TAG_DESTRUCTORS:
			return &destructors;
		case TAG_ALLOCAS:
			return &allocas;
		case TAG_ATTRIBUTES:
			return &attributes;
		case TAG_REGIONS:
			return &regions;
		case TAG_STRING_REFS:
			return &stringRefs;
		case TAG_INDICES:
			return &indices;
		case TAG_CALL_SITES:
			return &callSites;
		default:
			return nullptr;
		}
	}

	/// A pool of scalars must use exactly its element size; a record section
	/// may use a larger stride (fields appended by a later minor version).
	static void requireSection(const SectionView& section, uint32_t size, bool exact = false) {
		require(section.present, "missing section");
		require(exact ? section.stride == size : section.stride >= size, "section has an unexpected record size");
	}

	std::span<const std::byte> buffer;
};

/// Rebuilds the IR from a BufferView. Creation order matters twice over:
/// op constructors read their inputs (an AddOp takes its stamp from its left
/// operand), so operands are created before their users; and terminators are
/// wired only once every value of the function exists, because a block
/// argument edge may name a value defined in a later block.
class Materializer {
public:
	Materializer(const BufferView& view, const DeserializeOptions& options) : view(view), options(options) {
	}

	std::shared_ptr<IRGraph> run(std::shared_ptr<IRGraph> graph) {
		this->graph = graph.get();
		arena = &graph->getArena();
		materializeTargets();

		std::vector<FunctionOperation*> functions;
		functions.reserve(view.functions.count);
		for (uint32_t i = 0; i < view.functions.count; ++i) {
			functions.push_back(materializeFunction(view.get<FunctionRecord>(view.functions, i)));
		}
		for (auto* function : functions) {
			require(graph->getFunctionOperation(function->getName()) == nullptr, "duplicate function name");
			graph->addFunctionOperation(function);
		}
		for (const auto& [id, functionIndex] : internalDefinitions) {
			require(functionIndex < functions.size(), "function table entry names a missing function");
			require(graph->getFunctionTable().findByDefinition(functions[functionIndex]) == INVALID_FUNCTION_ID,
			        "two function table entries share one definition");
			graph->defineFunction(id, functions[functionIndex]);
		}
		rebuildPredecessorLists(*graph);

		if (options.verifyIR) {
			const auto result = IRVerifier::verify(*graph);
			if (!result.ok()) {
				throw SerializationException("deserialized IR failed verification:\n" + result.toString());
			}
		}
		return graph;
	}

private:
	// ── scalars ─────────────────────────────────────────────────────────────

	/// A copy of @p ref that lives as long as the graph, for the IR fields
	/// that are `const char*` (region names, source locations).
	const char* arenaString(StringRef ref) {
		if (ref.offset == NONE) {
			return nullptr;
		}
		const auto text = view.string(ref);
		auto* copy = static_cast<char*>(arena->allocate(text.size() + 1, alignof(char)));
		std::memcpy(copy, text.data(), text.size());
		copy[text.size()] = '\0';
		return copy;
	}

	SourceLocation location(const SourceLocationRecord& record) {
		return SourceLocation {arenaString(record.file), arenaString(record.function), record.line, record.column};
	}

	uint64_t constant(uint32_t index) {
		return view.get<uint64_t>(view.constants, index);
	}

	void* pointer(uint32_t index) {
		if (const auto it = resolvedPointers.find(index); it != resolvedPointers.end()) {
			return it->second;
		}
		const auto record = view.get<PointerRecord>(view.pointers, index);
		void* address = nullptr;
		switch (static_cast<PointerKind>(record.kind)) {
		case PointerKind::Null:
			break;
		case PointerKind::Raw:
			if ((view.header.flags & FLAG_PROCESS_LOCAL) != 0 && view.header.processToken == currentProcessToken()) {
				address = reinterpret_cast<void*>(static_cast<uintptr_t>(record.address));
			} else if (record.symbol.offset != NONE) {
				address = resolveSymbol(view.string(record.symbol));
			} else {
				throw SerializationException(
				    "buffer holds a raw pointer written by another process; serialize with PointerMode::Portable");
			}
			break;
		case PointerKind::Symbol:
			address = resolveSymbol(view.string(record.symbol));
			break;
		default:
			require(false, "unknown pointer kind");
		}
		resolvedPointers.emplace(index, address);
		return address;
	}

	void* resolveSymbol(std::string_view symbol) {
		if (options.resolver == nullptr) {
			throw SerializationException(fmt::format("symbol '{}' needs a SymbolResolver", symbol));
		}
		auto* address = options.resolver->resolve(symbol);
		if (address == nullptr) {
			throw SerializationException(fmt::format("unresolved symbol '{}'", symbol));
		}
		return address;
	}

	// ── function table ──────────────────────────────────────────────────────

	void materializeTargets() {
		auto& table = graph->getFunctionTableMut();
		for (uint32_t i = 0; i < view.targets.count; ++i) {
			const auto record = view.get<TargetRecord>(view.targets, i);
			const auto attrs = toAttributes(record.modRef, record.attrFlags);
			CalleeDescriptor descriptor;
			// Interned without an identity key, so two entries that resolve to
			// the same address stay two entries -- the ids must match the
			// writer's one for one, because call sites store them.
			descriptor.key = nullptr;
			descriptor.mangledName = view.string(record.mangled);
			descriptor.demangledName = view.string(record.demangled);
			descriptor.customName = view.string(record.custom);
			descriptor.resultType = toType(record.resultType);
			descriptor.attrs = attrs;

			const auto linkage = static_cast<WireLinkage>(record.linkage);
			if (linkage == WireLinkage::Native) {
				descriptor.kind = CalleeDescriptor::Kind::External;
				require(inRange(record.paramTypesBegin, record.paramCount, view.types.count), "parameter types");
				for (uint32_t p = 0; p < record.paramCount; ++p) {
					descriptor.paramTypes.push_back(toType(view.get<uint8_t>(view.types, record.paramTypesBegin + p)));
				}
			} else {
				require(linkage == WireLinkage::Internal, "unknown linkage");
				descriptor.kind = CalleeDescriptor::Kind::Internal;
			}

			const auto id = graph->internCallee(descriptor);
			require(id == i, "function table ids are not dense");
			auto& target = table.getMut(id);
			target.getNameMut().setMinted(std::string(view.string(record.minted)));
			target.getNameMut().setEmission(std::string(view.string(record.emission)));

			if (linkage == WireLinkage::Native) {
				auto* address = pointer(record.payload);
				auto* native = target.getNativeMut();
				native->address = address;
				// Re-derived rather than shipped: intrinsic ids are minted per
				// process in registration order.
				native->intrinsic = IntrinsicRegistry::instance().lookup(address);
				if (address != nullptr && table.find(address) == INVALID_FUNCTION_ID) {
					table.alias(address, id);
				}
			} else {
				target.setDerivedAttributes(attrs);
				if (record.payload != NONE) {
					internalDefinitions.emplace_back(id, record.payload);
				}
			}
		}
	}

	// ── functions ───────────────────────────────────────────────────────────

	struct FunctionContext {
		const FunctionRecord& record;
		std::vector<OpRecord> ops;
		std::vector<Operation*> values;
		std::vector<BasicBlock*> blocks;
		std::vector<BlockRecord> blockRecords;
		uint32_t regionCount;
	};

	uint32_t operandAt(const FunctionContext& fn, const OpRecord& op, uint32_t k) const {
		const auto index = view.get<uint32_t>(view.operands, op.operandsBegin + k);
		require(index < fn.ops.size(), "operand out of range");
		return index;
	}

	Operation* operand(const FunctionContext& fn, const OpRecord& op, uint32_t k) const {
		return fn.values[operandAt(fn, op, k)];
	}

	void requireRegion(const FunctionContext& fn, uint16_t region) const {
		require(region == NO_REGION || region < fn.regionCount, "region index out of range");
	}

	/// Values @p op needs to exist before it can be constructed.
	std::vector<uint32_t> dependencies(const FunctionContext& fn, const OpRecord& op) const {
		std::vector<uint32_t> deps;
		const auto opcode = static_cast<WireOpcode>(op.opcode);
		// Branch and If edges are wired after every value exists.
		for (uint32_t k = 0; k < op.operandCount; ++k) {
			deps.push_back(operandAt(fn, op, k));
		}
		if (opcode == WireOpcode::Call || opcode == WireOpcode::IndirectCall) {
			const auto call = view.get<CallRecord>(view.calls, op.payload1);
			require(inRange(call.destructorsBegin, call.destructorCount, view.destructors.count), "destructors");
			for (uint32_t d = 0; d < call.destructorCount; ++d) {
				const auto destructor = view.get<DestructorRecord>(view.destructors, call.destructorsBegin + d);
				require(destructor.address < fn.ops.size(), "destructor operand out of range");
				deps.push_back(destructor.address);
			}
		}
		return deps;
	}

	/// Creates every value of the function, operands before their users.
	/// Iterative so a long def-use chain cannot overflow the stack.
	void createValues(FunctionContext& fn) {
		enum : uint8_t { Unvisited, InProgress, Done };
		std::vector<uint8_t> state(fn.ops.size(), Unvisited);
		for (size_t i = 0; i < fn.ops.size(); ++i) {
			if (fn.values[i] != nullptr) {
				state[i] = Done; // block arguments
			}
		}
		struct Frame {
			uint32_t value;
			std::vector<uint32_t> deps;
			size_t next;
		};
		std::vector<Frame> stack;
		for (uint32_t root = 0; root < fn.ops.size(); ++root) {
			if (state[root] != Unvisited) {
				continue;
			}
			state[root] = InProgress;
			stack.push_back({root, dependencies(fn, fn.ops[root]), 0});
			while (!stack.empty()) {
				auto& frame = stack.back();
				if (frame.next < frame.deps.size()) {
					const auto dep = frame.deps[frame.next++];
					require(state[dep] != InProgress, "operands form a cycle");
					if (state[dep] == Unvisited) {
						state[dep] = InProgress;
						stack.push_back({dep, dependencies(fn, fn.ops[dep]), 0});
					}
					continue;
				}
				fn.values[frame.value] = createOperation(fn, fn.ops[frame.value]);
				state[frame.value] = Done;
				stack.pop_back();
			}
		}
	}

	template <typename Destructor>
	std::vector<Destructor> readDestructors(const FunctionContext& fn, const CallRecord& call) {
		std::vector<Destructor> result;
		for (uint32_t d = 0; d < call.destructorCount; ++d) {
			const auto record = view.get<DestructorRecord>(view.destructors, call.destructorsBegin + d);
			result.push_back(Destructor {fn.values[record.address], std::string(view.string(record.symbol)),
			                             std::string(view.string(record.name)), pointer(record.functionPtr)});
		}
		return result;
	}

	FunctionId calleeId(uint32_t raw) const {
		require(raw == INVALID_FUNCTION_ID || raw < view.targets.count, "callee id out of range");
		return raw;
	}

	Operation* createOperation(FunctionContext& fn, const OpRecord& op) {
		const auto opcode = static_cast<WireOpcode>(op.opcode);
		const auto type = toType(op.type);
		const OperationIdentifier id {op.identifier};
		const auto expectOperands = [&](uint32_t count) {
			require(op.operandCount == count, "wrong operand count");
		};
		const auto in = [&](uint32_t k) {
			return operand(fn, op, k);
		};
		auto& a = *arena;

		Operation* result = nullptr;
		switch (opcode) {
		case WireOpcode::BlockArgument:
			require(false, "block argument outside a block's argument list");
			break;
		case WireOpcode::ConstInt:
			expectOperands(0);
			result = a.create<ConstIntOperation>(a, id, static_cast<int64_t>(constant(op.payload0)), type);
			break;
		case WireOpcode::ConstFloat:
			expectOperands(0);
			result = a.create<ConstFloatOperation>(a, id, std::bit_cast<double>(constant(op.payload0)), type);
			break;
		case WireOpcode::ConstBoolean:
			expectOperands(0);
			require(op.aux <= 1, "boolean constant is neither 0 nor 1");
			result = a.create<ConstBooleanOperation>(a, id, op.aux == 1);
			break;
		case WireOpcode::ConstPtr:
			expectOperands(0);
			result = a.create<ConstPtrOperation>(a, id, pointer(op.payload0));
			break;
		case WireOpcode::Add:
			expectOperands(2);
			result = a.create<AddOperation>(a, id, in(0), in(1));
			break;
		case WireOpcode::Sub:
			expectOperands(2);
			result = a.create<SubOperation>(a, id, in(0), in(1));
			break;
		case WireOpcode::Mul:
			expectOperands(2);
			result = a.create<MulOperation>(a, id, in(0), in(1));
			break;
		case WireOpcode::Div:
			expectOperands(2);
			result = a.create<DivOperation>(a, id, in(0), in(1));
			break;
		case WireOpcode::Mod:
			expectOperands(2);
			result = a.create<ModOperation>(a, id, in(0), in(1));
			break;
		case WireOpcode::And:
			expectOperands(2);
			result = a.create<AndOperation>(a, id, in(0), in(1));
			break;
		case WireOpcode::Or:
			expectOperands(2);
			result = a.create<OrOperation>(a, id, in(0), in(1));
			break;
		case WireOpcode::Not:
			expectOperands(1);
			result = a.create<NotOperation>(a, id, in(0));
			break;
		case WireOpcode::Negate:
			expectOperands(1);
			result = a.create<NegateOperation>(a, id, in(0));
			break;
		case WireOpcode::Compare:
			expectOperands(2);
			result = a.create<CompareOperation>(a, id, in(0), in(1), toComparator(op.aux));
			break;
		case WireOpcode::Shift:
			expectOperands(2);
			result = a.create<ShiftOperation>(a, id, in(0), in(1), toShift(op.aux));
			break;
		case WireOpcode::BinaryComp:
			expectOperands(2);
			result = a.create<BinaryCompOperation>(a, id, in(0), in(1), toBitwise(op.aux));
			break;
		case WireOpcode::Cast:
			expectOperands(1);
			result = a.create<CastOperation>(a, id, in(0), type);
			break;
		case WireOpcode::Load:
			expectOperands(1);
			result = a.create<LoadOperation>(a, id, in(0), type);
			break;
		case WireOpcode::Store:
			expectOperands(2);
			result = a.create<StoreOperation>(a, in(0), in(1));
			break;
		case WireOpcode::Select:
			expectOperands(3);
			result = a.create<SelectOperation>(a, id, in(0), in(1), in(2), type);
			break;
		case WireOpcode::Alloca:
			expectOperands(0);
			require(op.payload0 < fn.record.allocaCount, "alloca index out of range");
			result = a.create<AllocaOperation>(a, id, op.payload0);
			break;
		case WireOpcode::Call: {
			const auto call = view.get<CallRecord>(view.calls, op.payload1);
			std::vector<Operation*> args;
			for (uint32_t k = 0; k < op.operandCount; ++k) {
				args.push_back(in(k));
			}
			result = a.create<CallOperation>(
			    a, std::string(view.string(call.symbol)), std::string(view.string(call.name)),
			    pointer(call.functionPtr), id, std::span<Operation* const>(args), type,
			    toAttributes(call.modRef, call.attrFlags), calleeId(op.payload0),
			    readDestructors<CallOperation::Destructor>(fn, call), (call.callFlags & CALL_EXCEPTION_HANDLING) != 0,
			    pointer(call.captureFunc), (call.callFlags & CALL_IS_NAUTILUS_CALL) != 0);
			break;
		}
		case WireOpcode::IndirectCall: {
			require(op.operandCount >= 1, "indirect call without a callee operand");
			const auto call = view.get<CallRecord>(view.calls, op.payload1);
			std::vector<Operation*> args;
			for (uint32_t k = 1; k < op.operandCount; ++k) {
				args.push_back(in(k));
			}
			result = a.create<IndirectCallOperation>(
			    a, id, in(0), std::span<Operation* const>(args), type, toAttributes(call.modRef, call.attrFlags),
			    readDestructors<IndirectCallOperation::Destructor>(fn, call),
			    (call.callFlags & CALL_EXCEPTION_HANDLING) != 0, pointer(call.captureFunc));
			break;
		}
		case WireOpcode::FunctionAddressOf: {
			expectOperands(0);
			const auto call = view.get<CallRecord>(view.calls, op.payload1);
			result = a.create<FunctionAddressOfOperation>(a, std::string(view.string(call.symbol)),
			                                              std::string(view.string(call.name)),
			                                              pointer(call.functionPtr), id, calleeId(op.payload0));
			break;
		}
		case WireOpcode::Branch:
			expectOperands(0);
			result = a.create<BranchOperation>();
			break;
		case WireOpcode::If:
			expectOperands(1);
			result = a.create<IfOperation>(a, in(0), std::bit_cast<double>(constant(op.payload2)));
			break;
		case WireOpcode::Return:
			require(op.operandCount <= 1, "return with more than one value");
			result = op.operandCount == 0 ? a.create<ReturnOperation>(a) : a.create<ReturnOperation>(a, in(0));
			break;
		default:
			throw SerializationException(fmt::format("unknown opcode {}; the buffer needs a newer reader", op.opcode));
		}
		// The constructors derive most stamps from the operands; a record that
		// disagrees describes IR this reader cannot reproduce faithfully.
		require(result->getStamp() == type, "operation type does not match its operands");
		requireRegion(fn, op.region);
		result->setRegionIndex(op.region);
		return result;
	}

	void wireEdge(FunctionContext& fn, BasicBlockInvocation& invocation, uint32_t edgeIndex) {
		const auto edge = view.get<EdgeRecord>(view.edges, edgeIndex);
		require(edge.targetBlock < fn.blocks.size(), "edge target out of range");
		require(edge.argumentCount == fn.blockRecords[edge.targetBlock].argumentCount,
		        "edge argument count does not match the target block");
		invocation.setBlock(fn.blocks[edge.targetBlock]);
		for (uint32_t k = 0; k < edge.argumentCount; ++k) {
			const auto value = view.get<uint32_t>(view.operands, edge.argumentsBegin + k);
			require(value < fn.values.size(), "edge argument out of range");
			invocation.addArgument(*arena, fn.values[value]);
		}
	}

	FunctionOperation* materializeFunction(const FunctionRecord& record) {
		const uint64_t blockCount = static_cast<uint64_t>(record.bodyBlockCount) + record.extraBlockCount;
		require(record.bodyBlockCount > 0, "function without blocks");
		require(blockCount <= view.blocks.count && record.blocksBegin <= view.blocks.count - blockCount, "block range");
		require(inRange(record.opsBegin, record.opCount, view.ops.count), "operation range");
		require(inRange(record.regionsBegin, record.regionCount, view.regions.count), "region range");
		require(inRange(record.allocasBegin, record.allocaCount, view.allocas.count), "alloca range");
		require(record.regionCount <= NO_REGION, "too many regions");

		FunctionContext fn {record, {}, {}, {}, {}, record.regionCount};
		fn.ops.reserve(record.opCount);
		for (uint32_t i = 0; i < record.opCount; ++i) {
			fn.ops.push_back(view.get<OpRecord>(view.ops, record.opsBegin + i));
		}
		fn.values.assign(record.opCount, nullptr);

		// Blocks tile the value range exactly, in order: arguments, then ops.
		uint64_t cursor = 0;
		for (uint32_t b = 0; b < blockCount; ++b) {
			const auto block = view.get<BlockRecord>(view.blocks, record.blocksBegin + b);
			require(block.firstValue == cursor, "blocks do not tile the function's values");
			cursor += static_cast<uint64_t>(block.argumentCount) + block.operationCount;
			require(cursor <= record.opCount, "block overruns the function's values");
			requireRegion(fn, block.region);

			std::vector<BasicBlockArgument*> arguments;
			for (uint32_t k = 0; k < block.argumentCount; ++k) {
				const auto index = block.firstValue + k;
				const auto& op = fn.ops[index];
				require(static_cast<WireOpcode>(op.opcode) == WireOpcode::BlockArgument && op.operandCount == 0,
				        "block argument expected");
				requireRegion(fn, op.region);
				auto* argument =
				    arena->create<BasicBlockArgument>(OperationIdentifier {op.identifier}, toType(op.type));
				argument->setRegionIndex(op.region);
				arguments.push_back(argument);
				fn.values[index] = argument;
			}
			for (uint32_t k = 0; k < block.operationCount; ++k) {
				const auto& op = fn.ops[block.firstValue + block.argumentCount + k];
				require(static_cast<WireOpcode>(op.opcode) != WireOpcode::BlockArgument,
				        "block argument outside a block's argument list");
			}
			auto* irBlock = arena->create<BasicBlock>(*arena, BlockIdentifier {block.identifier}, std::move(arguments));
			irBlock->setRegionIndex(block.region);
			fn.blocks.push_back(irBlock);
			fn.blockRecords.push_back(block);
		}
		require(cursor == record.opCount, "values outside every block");

		createValues(fn);

		for (uint32_t b = 0; b < blockCount; ++b) {
			const auto& block = fn.blockRecords[b];
			for (uint32_t k = 0; k < block.operationCount; ++k) {
				const auto index = block.firstValue + block.argumentCount + k;
				auto* op = fn.values[index];
				const auto& opRecord = fn.ops[index];
				if (auto* branch = dyn_cast<BranchOperation>(op)) {
					wireEdge(fn, branch->getNextBlockInvocation(), opRecord.payload0);
				} else if (auto* ifOp = dyn_cast<IfOperation>(op)) {
					require(opRecord.payload0 != NONE, "if without edges");
					wireEdge(fn, ifOp->getTrueBlockInvocation(), opRecord.payload0);
					wireEdge(fn, ifOp->getFalseBlockInvocation(), opRecord.payload0 + 1);
					if (opRecord.payload1 != NONE) {
						require(opRecord.payload1 < fn.blocks.size(), "merge block out of range");
						ifOp->setMergeBlock(fn.blocks[opRecord.payload1]);
					}
				}
				fn.blocks[b]->addOperation(op);
			}
		}

		std::vector<Type> inputTypes;
		require(inRange(record.inputTypesBegin, record.inputTypeCount, view.types.count), "argument types");
		for (uint32_t i = 0; i < record.inputTypeCount; ++i) {
			inputTypes.push_back(toType(view.get<uint8_t>(view.types, record.inputTypesBegin + i)));
		}
		std::vector<std::string> argNames;
		require(inRange(record.argNamesBegin, record.argNameCount, view.stringRefs.count), "argument names");
		for (uint32_t i = 0; i < record.argNameCount; ++i) {
			argNames.emplace_back(view.string(view.get<StringRef>(view.stringRefs, record.argNamesBegin + i)));
		}
		std::vector<AllocaSpec> allocaSpecs;
		for (uint32_t i = 0; i < record.allocaCount; ++i) {
			const auto alloca = view.get<AllocaRecord>(view.allocas, record.allocasBegin + i);
			allocaSpecs.push_back(AllocaSpec {static_cast<size_t>(alloca.size), static_cast<size_t>(alloca.align)});
		}
		std::unordered_map<std::string, std::string> attributes;
		require(inRange(record.attributesBegin, record.attributeCount, view.attributes.count), "attributes");
		for (uint32_t i = 0; i < record.attributeCount; ++i) {
			const auto attribute = view.get<AttributeRecord>(view.attributes, record.attributesBegin + i);
			require(attributes.emplace(view.string(attribute.key), view.string(attribute.value)).second,
			        "duplicate function attribute");
		}
		std::vector<RegionSpec> regionSpecs;
		for (uint32_t i = 0; i < record.regionCount; ++i) {
			const auto region = view.get<RegionRecord>(view.regions, record.regionsBegin + i);
			require(region.parent == NO_REGION || region.parent < record.regionCount, "region parent out of range");
			RegionSpec spec;
			spec.attributes.name = arenaString(region.name);
			spec.attributes.location = location(region.location);
			spec.parent = region.parent;
			spec.id = region.id;
			regionSpecs.push_back(spec);
		}

		std::vector<BasicBlock*> body(fn.blocks.begin(), fn.blocks.begin() + record.bodyBlockCount);
		auto* function = arena->create<FunctionOperation>(
		    std::string(view.string(record.name)), std::move(body), std::move(inputTypes), std::move(argNames),
		    toType(record.outputType), std::move(allocaSpecs), std::move(attributes), std::move(regionSpecs),
		    location(record.location));

		if (record.hasExceptionRegion != 0) {
			FunctionExceptionRegion region;
			require(inRange(record.padsBegin, record.padCount, view.indices.count), "landing pads");
			for (uint32_t i = 0; i < record.padCount; ++i) {
				const auto block = view.get<uint32_t>(view.indices, record.padsBegin + i);
				require(block < fn.blocks.size(), "landing pad block out of range");
				region.pads.push_back(LandingPadBlock {fn.blocks[block]});
			}
			require(inRange(record.callSitesBegin, record.callSiteCount, view.callSites.count), "call sites");
			for (uint32_t i = 0; i < record.callSiteCount; ++i) {
				const auto site = view.get<CallSiteRecord>(view.callSites, record.callSitesBegin + i);
				require(site.call < fn.values.size(), "call site out of range");
				auto* call = fn.values[site.call];
				require(isa<CallOperation>(call) || isa<IndirectCallOperation>(call), "call site is not a call");
				require(site.padIndex == NONE || site.padIndex < record.padCount, "call site pad out of range");
				region.callSites.push_back(
				    ExceptionalCallSite {call, site.padIndex == NONE ? noLandingPad : site.padIndex});
			}
			function->exceptionRegion = std::move(region);
		}
		return function;
	}

	const BufferView& view;
	const DeserializeOptions& options;
	IRGraph* graph = nullptr;
	common::Arena* arena = nullptr;
	std::unordered_map<uint32_t, void*> resolvedPointers;
	std::vector<std::pair<FunctionId, uint32_t>> internalDefinitions;
};

std::shared_ptr<IRGraph> deserializeInto(const BufferView& view, std::shared_ptr<IRGraph> graph,
                                         const DeserializeOptions& options) {
	return Materializer(view, options).run(std::move(graph));
}

CompilationUnitID unitId(const BufferView& view, const DeserializeOptions& options) {
	if (options.compilationUnitId) {
		return *options.compilationUnitId;
	}
	return CompilationUnitID(view.string(view.get<ModuleRecord>(view.module, 0).compilationUnitId));
}

} // namespace

std::shared_ptr<IRGraph> deserialize(std::span<const std::byte> buffer, const DeserializeOptions& options) {
	const BufferView view(buffer, options);
	return deserializeInto(view, std::make_shared<IRGraph>(unitId(view, options)), options);
}

std::shared_ptr<IRGraph> deserialize(std::span<const std::byte> buffer, common::ArenaPool& pool,
                                     const DeserializeOptions& options) {
	const BufferView view(buffer, options);
	return deserializeInto(view, std::make_shared<IRGraph>(pool.acquire(), unitId(view, options)), options);
}

} // namespace nautilus::serialization
