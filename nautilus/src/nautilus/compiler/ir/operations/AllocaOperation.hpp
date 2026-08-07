#pragma once

#include "nautilus/common/ExceptionCleanup.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>

namespace nautilus::compiler::ir {
/// Pointer to a stack slot in the enclosing function's alloca table.
///
/// Each AllocaOperation references a (size, align) entry stored on the
/// FunctionOperation; backends emit the real allocation once per entry in the
/// function prologue and lower this op as a lookup of the i-th slot pointer.
/// Multiple AllocaOperation nodes may share an index when SSA value
/// propagation reuses the same slot, but every distinct trace-level alloca
/// site receives its own entry.
class AllocaOperation : public Operation {
public:
	AllocaOperation(common::Arena& arena, OperationIdentifier id, uint32_t allocaIndex,
	                std::optional<CleanupEffect> cleanupEffect = std::nullopt);

	~AllocaOperation() = default;

	uint32_t getIndex() const;
	[[nodiscard]] const std::optional<CleanupEffect>& getCleanupEffect() const;

	static bool classof(const Operation* op);

private:
	uint32_t allocaIndex;
	std::optional<CleanupEffect> cleanupEffect;
};
} // namespace nautilus::compiler::ir
