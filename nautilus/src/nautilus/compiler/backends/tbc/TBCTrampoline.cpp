
#include "nautilus/compiler/backends/tbc/TBCTrampoline.hpp"
#include "nautilus/compiler/backends/tbc/TBCInterpreter.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <array>
#include <atomic>
#include <mutex>
#include <utility>

namespace nautilus::compiler::tbc {

namespace {

constexpr size_t kTrampolineSlots = 64;

struct SlotEntry {
	std::atomic<const TBCProgram*> program {nullptr};
	std::atomic<uint32_t> functionIndex {0};
};

std::array<SlotEntry, kTrampolineSlots>& slotTable() {
	static std::array<SlotEntry, kTrampolineSlots> table;
	return table;
}

std::mutex& slotMutex() {
	static std::mutex mutex;
	return mutex;
}

template <size_t>
using U64Arg = uint64_t;

/// One pre-compiled trampoline per (slot, arity). The C++ compiler emits it
/// with the native calling convention, so its address is a genuine function
/// pointer; the slot index is baked in as a template argument, giving the
/// body the context a C ABI cannot carry. Arguments travel as 64-bit
/// integer-class values: narrow arguments may carry garbage in their upper
/// bits (the caller only guarantees the declared width), which is harmless
/// because every VM consumer reads its exact declared width.
template <size_t Slot, class Seq>
struct Trampoline;

template <size_t Slot, size_t... I>
struct Trampoline<Slot, std::index_sequence<I...>> {
	static uint64_t call(U64Arg<I>... args) {
		const auto& entry = slotTable()[Slot];
		const TBCProgram* program = entry.program.load(std::memory_order_acquire);
		if (program == nullptr) {
			throw RuntimeException("tbc: call through released function trampoline");
		}
		[[maybe_unused]] const uint64_t values[] = {args..., 0};
		return invoke(*program, entry.functionIndex.load(std::memory_order_relaxed), values, sizeof...(I));
	}
};

template <size_t Slot, size_t... Arity>
constexpr std::array<void*, sizeof...(Arity)> makeSlotRow(std::index_sequence<Arity...>) {
	return {reinterpret_cast<void*>(&Trampoline<Slot, std::make_index_sequence<Arity>>::call)...};
}

template <size_t... Slot>
constexpr auto makeTable(std::index_sequence<Slot...>) {
	return std::array<std::array<void*, kTrampolineMaxArity + 1>, sizeof...(Slot)> {
	    makeSlotRow<Slot>(std::make_index_sequence<kTrampolineMaxArity + 1>())...};
}

const auto kTrampolinePointers = makeTable(std::make_index_sequence<kTrampolineSlots>());

bool isIntegerClass(Type t) {
	switch (t) {
	case Type::b:
	case Type::i8:
	case Type::i16:
	case Type::i32:
	case Type::i64:
	case Type::ui8:
	case Type::ui16:
	case Type::ui32:
	case Type::ui64:
	case Type::ptr:
		return true;
	default:
		return false;
	}
}

} // namespace

void* acquireTrampoline(const TBCProgram* program, uint32_t functionIndex) {
	const TBCFunction& function = program->functions[functionIndex];
	if (function.argTypes.size() > kTrampolineMaxArity) {
		throw NotImplementedException("tbc: escaping internal function pointer with more than 8 arguments");
	}
	for (const auto argType : function.argTypes) {
		if (!isIntegerClass(argType)) {
			throw NotImplementedException(
			    "tbc: escaping internal function pointer with float arguments is not supported");
		}
	}
	if (function.returnType != Type::v && !isIntegerClass(function.returnType)) {
		throw NotImplementedException("tbc: escaping internal function pointer with float return is not supported");
	}

	std::lock_guard<std::mutex> lock(slotMutex());
	auto& table = slotTable();
	// Reuse a slot this program already bound to the same function.
	for (size_t slot = 0; slot < kTrampolineSlots; ++slot) {
		if (table[slot].program.load(std::memory_order_relaxed) == program &&
		    table[slot].functionIndex.load(std::memory_order_relaxed) == functionIndex) {
			return kTrampolinePointers[slot][function.argTypes.size()];
		}
	}
	for (size_t slot = 0; slot < kTrampolineSlots; ++slot) {
		if (table[slot].program.load(std::memory_order_relaxed) == nullptr) {
			table[slot].functionIndex.store(functionIndex, std::memory_order_relaxed);
			table[slot].program.store(program, std::memory_order_release);
			return kTrampolinePointers[slot][function.argTypes.size()];
		}
	}
	throw RuntimeException("tbc: function trampoline pool exhausted (64 escaping internal function pointers)");
}

void releaseTrampolines(const TBCProgram* program) {
	std::lock_guard<std::mutex> lock(slotMutex());
	for (auto& entry : slotTable()) {
		if (entry.program.load(std::memory_order_relaxed) == program) {
			entry.program.store(nullptr, std::memory_order_release);
		}
	}
}

} // namespace nautilus::compiler::tbc
