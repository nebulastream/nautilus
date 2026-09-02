#include "nautilus/compiler/ir/OperationEffects.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"

namespace nautilus::compiler::ir {

OperationEffects effectsOf(const IRGraph& ir, const Operation& op) {
	const auto opType = op.getOperationType();

	if (opType == Operation::OperationType::CallOp) {
		const auto* call = cast<CallOperation>(&op);
		const auto calleeId = call->getCalleeId();
		if (calleeId == INVALID_FUNCTION_ID || !ir.getFunctionTable().contains(calleeId)) {
			// No resolvable callee: keep the opcode table's worst case.
			return OperationEffects {};
		}

		// Two sources could describe this call: the callee's table entry, and
		// whatever the call site declared. They are not equally informative,
		// and which one to trust depends on the linkage.
		//
		// A default-constructed FunctionAttributes is not a neutral element.
		// Its modRefInfo is ModRef and both guarantees are false -- the most
		// pessimistic value on every axis -- and that is exactly what every
		// traced call to a Nautilus function passes, because the tracer has
		// nothing to say about a function whose body it has not converted
		// yet. Combining it with the callee's own attributes, in either
		// direction, can therefore only destroy information: a union pins
		// modRef at ModRef, an intersection pins both guarantees at false.
		// Attribute inference would be unable to have any effect at all.
		//
		// So an internal callee is described by its entry alone, which is
		// derived from its body and is the only real evidence available.
		const auto& target = ir.getFunctionTable().get(calleeId);
		auto attrs = target.getAttributes();

		if (target.getLinkage() != Linkage::Internal) {
			// A native callee is different: its entry holds what the *first*
			// call site declared via invoke(), and this call site's own
			// annotation is equally authoritative. Combine them
			// conservatively -- effects union, guarantees intersect -- so a
			// second call site can add an effect the first did not mention.
			const auto atCallSite = call->getFunctionAttributes();
			attrs.modRefInfo = static_cast<ModRefInfo>(static_cast<uint8_t>(attrs.modRefInfo) |
			                                           static_cast<uint8_t>(atCallSite.modRefInfo));
			attrs.willReturn = attrs.willReturn && atCallSite.willReturn;
			attrs.noUnwind = attrs.noUnwind && atCallSite.noUnwind;
		}

		OperationEffects effects;
		effects.mayRead = (static_cast<uint8_t>(attrs.modRefInfo) & static_cast<uint8_t>(ModRefInfo::Ref)) != 0;
		effects.mayWrite = (static_cast<uint8_t>(attrs.modRefInfo) & static_cast<uint8_t>(ModRefInfo::Mod)) != 0;
		// A call is only safe to move or drop when it touches no memory *and*
		// is guaranteed to come back -- an infinite loop or a throw is an
		// observable effect even from a function that reads and writes nothing.
		effects.isPure = !effects.mayRead && !effects.mayWrite && attrs.willReturn && attrs.noUnwind;
		return effects;
	}

	// Everything else: exactly what the opcode says, which for these
	// operations is exact rather than conservative.
	OperationEffects effects;
	effects.mayRead = mayReadMemory(opType);
	effects.mayWrite = mayWriteMemory(opType);
	effects.isPure = isPureOp(opType);
	return effects;
}

} // namespace nautilus::compiler::ir
