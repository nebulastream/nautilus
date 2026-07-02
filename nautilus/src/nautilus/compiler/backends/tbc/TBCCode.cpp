
#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include "nautilus/compiler/backends/tbc/TBCTrampoline.hpp"
#include <sstream>

namespace nautilus::compiler::tbc {

TBCProgram::~TBCProgram() {
	releaseTrampolines(this);
}

namespace {
const char* const kOpNames[] = {
#define TBC_NAME_V(name, fn) #name,
    TBC_VALUE_OPCODE_LIST(TBC_NAME_V)
#undef TBC_NAME_V
#define TBC_NAME_C(name) #name,
        TBC_CONTROL_SIMPLE_LIST(TBC_NAME_C)
#undef TBC_NAME_C
#define TBC_NAME_F(name, ctype, cmp) #name,
            TBC_CJMP_FUSED_LIST(TBC_NAME_F)
#undef TBC_NAME_F
};
static_assert(sizeof(kOpNames) / sizeof(kOpNames[0]) == kOpCount);

/// Number of instruction words the opcode at `pc` occupies.
size_t instrWords(Op op) {
	switch (op) {
	case Op::SELECT:
#define TBC_WORDS_F(name, ctype, cmp) case Op::name:
		TBC_CJMP_FUSED_LIST(TBC_WORDS_F)
#undef TBC_WORDS_F
		return 2;
	default:
		return 1;
	}
}

bool isFusedBranch(Op op) {
	switch (op) {
#define TBC_ISF(name, ctype, cmp) case Op::name:
		TBC_CJMP_FUSED_LIST(TBC_ISF)
#undef TBC_ISF
		return true;
	default:
		return false;
	}
}
} // namespace

const char* opName(Op op) {
	const auto idx = opIndex(op);
	return idx < kOpCount ? kOpNames[idx] : "<invalid>";
}

std::string TBCFunction::toString() const {
	std::stringstream ss;
	ss << "function " << name << " (regs=" << regSlots << ", frameSlots=" << frameSlots
	   << ", allocaBytes=" << allocaBytes << ")\n";
	for (size_t pc = 0; pc < code.size();) {
		const Instr& inst = code[pc];
		const auto op = static_cast<Op>(inst.op);
		ss << "  " << pc << ":\t" << opName(op);
		switch (op) {
		case Op::JMP:
			ss << " -> " << static_cast<int64_t>(pc) + packedOffset(inst);
			break;
		case Op::CJMP:
			ss << " r" << inst.a << " -> " << static_cast<int64_t>(pc) + packedOffset(inst);
			break;
		case Op::SELECT:
			ss << " r" << inst.a << " = r" << inst.b << " ? r" << inst.c << " : r" << code[pc + 1].a;
			break;
		case Op::RET:
			if (inst.a != kNoReg) {
				ss << " r" << inst.a;
			}
			break;
		case Op::CALL:
			ss << " fn" << inst.b << " site" << inst.c << (inst.a != kNoReg ? " -> r" + std::to_string(inst.a) : "");
			break;
		case Op::CALL_EXT:
			ss << " site" << inst.b << (inst.a != kNoReg ? " -> r" + std::to_string(inst.a) : "");
			break;
		case Op::CALL_IND:
			ss << " [r" << inst.b << "] site" << inst.c
			   << (inst.a != kNoReg ? " -> r" + std::to_string(inst.a) : "");
			break;
		default:
			if (isFusedBranch(op)) {
				ss << " r" << inst.a << ", r" << inst.b << " -> "
				   << static_cast<int64_t>(pc) + packedOffsetLoHi(code[pc + 1]);
			} else {
				ss << " r" << inst.a << ", r" << inst.b << ", r" << inst.c;
			}
			break;
		}
		ss << "\n";
		pc += instrWords(op);
	}
	return ss.str();
}

} // namespace nautilus::compiler::tbc
