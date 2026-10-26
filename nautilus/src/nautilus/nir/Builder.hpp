#pragma once

#include "nautilus/nir/Module.hpp"
#include "nautilus/nir/Operands.hpp"
#include <span>
#include <vector>

namespace nautilus::nir {

/// Appends functions to a module: mints dense instruction and block ids and maintains the use count of every value.
/// One function is open at a time; instructions go to the current block.
///
/// SSA functions keep their blocks as contiguous instruction ranges. Pre-SSA functions keep instructions in recording
/// order and list each block's instruction ids in `extra` (§4.3), written when the block is closed.
class Builder {
public:
	explicit Builder(Module& m) : m_(m) {
	}

	/// Opens a function and returns its index.
	uint32_t beginFunction(uint32_t nameId, uint32_t signature, Visibility visibility, Level level, uint32_t attrs = 0);
	/// Adds a static stack slot to the open function and returns its function-local index.
	uint32_t addAlloca(uint32_t size, uint32_t align);
	/// Opens a block and returns its function-local id. Closes the previous one.
	uint32_t beginBlock(uint16_t argCount = 0, uint16_t flags = 0);
	/// Appends an instruction to the current block and returns its function-local id, which is also its value
	/// reference. Instructions without a result have ids too.
	Ref emit(Op op, TypeId type, uint8_t flags, uint32_t a = 0, uint32_t b = 0, uint32_t c = 0);
	Ref emit(const Inst& inst);
	/// Closes the open function.
	void endFunction();

	/// Number of uses of value `id` of the open function; constants have none.
	uint32_t useCount(Ref id) const {
		return useCount_[id];
	}
	std::span<const uint32_t> useCounts() const {
		return useCount_;
	}
	uint32_t instCount() const {
		return static_cast<uint32_t>(m_.insts.size()) - fn().firstInst;
	}
	Level level() const {
		return static_cast<Level>(fn().level);
	}

private:
	FunctionRecord& fn() {
		return m_.functions[fn_];
	}
	const FunctionRecord& fn() const {
		return m_.functions[fn_];
	}
	void closeBlock();

	Module& m_;
	uint32_t fn_ = NONE;
	bool blockOpen_ = false;
	std::vector<uint32_t> useCount_;
	std::vector<uint32_t> blockIds_; // pre-SSA: ids of the current block's instructions
};

} // namespace nautilus::nir
