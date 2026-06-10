
#pragma once

#include "nautilus/tracing/Operations.hpp"
#include "nautilus/tracing/TypedValueRef.hpp"
#include "nautilus/tracing/Types.hpp"
#include <bit>
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

namespace nautilus::tracing {

/**
 * @brief One entry per TracingInterface call made by user code while the
 * ReplayTraceContext is in RECORD mode.
 *
 * Entries are stored in *call order* - the order user code observes - which is
 * invariant under processControlFlowMerge restructuring: the log stores the
 * returned value ref, not a position in the block graph. Replaying a recorded
 * path prefix (FOLLOW mode) is therefore a plain cursor walk over this vector.
 */
struct LogEntry {
	static constexpr uint8_t DECISION = 1;   // CMP only: branch direction taken
	static constexpr uint8_t PRUNED = 2;     // CMP only: constant-folded, nothing recorded
	static constexpr uint8_t HAS_RESULT = 4; // entry carries a value returned to user code

	Op op;
	uint8_t flags;
	Type resultType;
	TypedValueRef result;
	uint64_t operandHash; // determinism checksum, recomputed and compared during FOLLOW
	// PRUNED CMP entries only: key of the pruned branch instance (site tag,
	// static-state hash and decision), replayed in FOLLOW mode to rebuild the
	// pruned-site poison set deterministically.
	uint64_t prunedSiteKey = 0;

	bool decision() const {
		return (flags & DECISION) != 0;
	}
};

/**
 * @brief The not-yet-explored side of a recorded (non-pruned) CMP.
 *
 * logPos identifies the branch: the traced function is deterministic and the
 * prefix is replayed exactly, so the i-th interface call of an execution that
 * took a fixed decision sequence is a unique dynamic branch instance.
 */
struct PendingBranch {
	uint32_t logPos;       // index of the CMP's LogEntry in the path log
	uint32_t falseBlockId; // block pre-created empty by addCmpOperation
};

/// Details of a replay divergence, latched when FOLLOW-mode validation fails
/// and reported by the driver loop after the traced function returns.
struct DivergenceInfo {
	uint32_t callIndex;
	Op expectedOp;
	Op actualOp;
	uint64_t expectedHash;
	uint64_t actualHash;
	bool cursorOverrun; // function made more calls than the recorded prefix
};

/// FNV-1a style 64-bit mixer used for the per-entry operand checksum.
struct OperandHash {
	uint64_t value = 0xcbf29ce484222325ULL;

	OperandHash& mix(uint64_t v) {
		value ^= v;
		value *= 0x100000001b3ULL;
		return *this;
	}

	OperandHash& mix(Op op, Type resultType) {
		return mix((static_cast<uint64_t>(op) << 8) | static_cast<uint64_t>(resultType));
	}

	OperandHash& mix(const TypedValueRef& ref) {
		return mix((static_cast<uint64_t>(ref.ref) << 8) | static_cast<uint64_t>(ref.type));
	}

	OperandHash& mix(const ConstantLiteral& literal) {
		mix(static_cast<uint64_t>(literal.index()));
		std::visit(
		    [this](auto v) {
			    using T = decltype(v);
			    if constexpr (std::is_same_v<T, float>) {
				    mix(std::bit_cast<uint32_t>(v));
			    } else if constexpr (std::is_same_v<T, double>) {
				    mix(std::bit_cast<uint64_t>(v));
			    } else if constexpr (std::is_same_v<T, void*>) {
				    mix(reinterpret_cast<uint64_t>(v));
			    } else {
				    mix(static_cast<uint64_t>(v));
			    }
		    },
		    literal);
		return *this;
	}
};

} // namespace nautilus::tracing
