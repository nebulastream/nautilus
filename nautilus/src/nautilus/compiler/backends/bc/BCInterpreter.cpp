
#include <cassert>
#include <nautilus/compiler/backends/bc/BCInterpreter.hpp>
#include <nautilus/compiler/backends/bc/Dyncall.hpp>
#include <sstream>
#include <utility>

namespace nautilus::compiler::bc {
void regMov(const OpCode& c, RegisterFile& regs) {
	regs[c.output] = regs[c.reg1];
}

void dyncallReset(const OpCode&, RegisterFile&) {
	Dyncall::getVM().reset();
}

void dyncallArgB(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<bool>(regs, op.reg1);
	Dyncall::getVM().addArgB(value);
}

void dyncallArgI8(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<int8_t>(regs, op.reg1);
	Dyncall::getVM().addArgI8(value);
}

void dyncallArgI16(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<int16_t>(regs, op.reg1);
	Dyncall::getVM().addArgI16(value);
}

void dyncallArgI32(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<int32_t>(regs, op.reg1);
	Dyncall::getVM().addArgI32(value);
}

void dyncallArgI64(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<int64_t>(regs, op.reg1);
	Dyncall::getVM().addArgI64(value);
}

void dyncallArgF(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<float>(regs, op.reg1);
	Dyncall::getVM().addArgF(value);
}

void dyncallArgD(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<double>(regs, op.reg1);
	Dyncall::getVM().addArgD(value);
}

void dyncallArgPtr(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<void*>(regs, op.reg1);
	Dyncall::getVM().addArgPtr(value);
}

void dyncallCallV(const OpCode& op, RegisterFile& regs) {
	auto value = readReg<void*>(regs, op.reg1);
	Dyncall::getVM().callVoid(value);
}

void dyncallCallB(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callB(address);
	writeReg<bool>(regs, op.output, returnValue);
}

void dyncallCallI8(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callI8(address);
	writeReg<int8_t>(regs, op.output, returnValue);
}

void dyncallCallI16(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callI16(address);
	writeReg<int16_t>(regs, op.output, returnValue);
}

void dyncallCallI32(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callI32(address);
	writeReg<int32_t>(regs, op.output, returnValue);
}

void dyncallCallI64(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callI64(address);
	writeReg<int64_t>(regs, op.output, returnValue);
}

void dyncallCallPtr(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callPtr(address);
	writeReg<void*>(regs, op.output, returnValue);
}

void dyncallCallf(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callF(address);
	writeReg<float>(regs, op.output, returnValue);
}

void dyncallCalld(const OpCode& op, RegisterFile& regs) {
	auto address = readReg<void*>(regs, op.reg1);
	auto returnValue = Dyncall::getVM().callD(address);
	writeReg<double>(regs, op.output, returnValue);
}

static Operation* OpTable[] = {
    (Operation*) regMov,
    // add
    (Operation*) add<int8_t>,
    (Operation*) add<int16_t>,
    (Operation*) add<int32_t>,
    (Operation*) add<int64_t>,
    (Operation*) add<uint8_t>,
    (Operation*) add<uint16_t>,
    (Operation*) add<uint32_t>,
    (Operation*) add<uint64_t>,
    (Operation*) add<float>,
    (Operation*) add<double>,
    // sub
    (Operation*) sub<int8_t>,
    (Operation*) sub<int16_t>,
    (Operation*) sub<int32_t>,
    (Operation*) sub<int64_t>,
    (Operation*) sub<uint8_t>,
    (Operation*) sub<uint16_t>,
    (Operation*) sub<uint32_t>,
    (Operation*) sub<uint64_t>,
    (Operation*) sub<float>,
    (Operation*) sub<double>,
    // mul
    (Operation*) mul<int8_t>,
    (Operation*) mul<int16_t>,
    (Operation*) mul<int32_t>,
    (Operation*) mul<int64_t>,
    (Operation*) mul<uint8_t>,
    (Operation*) mul<uint16_t>,
    (Operation*) mul<uint32_t>,
    (Operation*) mul<uint64_t>,
    (Operation*) mul<float>,
    (Operation*) mul<double>,
    // div
    (Operation*) div<int8_t>,
    (Operation*) div<int16_t>,
    (Operation*) div<int32_t>,
    (Operation*) div<int64_t>,
    (Operation*) div<uint8_t>,
    (Operation*) div<uint16_t>,
    (Operation*) div<uint32_t>,
    (Operation*) div<uint64_t>,
    (Operation*) div<float>,
    (Operation*) div<double>,
    // mod
    (Operation*) mod<int8_t>,
    (Operation*) mod<int16_t>,
    (Operation*) mod<int32_t>,
    (Operation*) mod<int64_t>,
    (Operation*) mod<uint8_t>,
    (Operation*) mod<uint16_t>,
    (Operation*) mod<uint32_t>,
    (Operation*) mod<uint64_t>,
    // equals
    (Operation*) equals<int8_t>,
    (Operation*) equals<int16_t>,
    (Operation*) equals<int32_t>,
    (Operation*) equals<int64_t>,
    (Operation*) equals<uint8_t>,
    (Operation*) equals<uint16_t>,
    (Operation*) equals<uint32_t>,
    (Operation*) equals<uint64_t>,
    (Operation*) equals<float>,
    (Operation*) equals<double>,
    (Operation*) equals<bool>,
    // less than
    (Operation*) lessThan<int8_t>,
    (Operation*) lessThan<int16_t>,
    (Operation*) lessThan<int32_t>,
    (Operation*) lessThan<int64_t>,
    (Operation*) lessThan<uint8_t>,
    (Operation*) lessThan<uint16_t>,
    (Operation*) lessThan<uint32_t>,
    (Operation*) lessThan<uint64_t>,
    (Operation*) lessThan<float>,
    (Operation*) lessThan<double>,
    // less than equals
    (Operation*) lessThanEquals<int8_t>,
    (Operation*) lessThanEquals<int16_t>,
    (Operation*) lessThanEquals<int32_t>,
    (Operation*) lessThanEquals<int64_t>,
    (Operation*) lessThanEquals<uint8_t>,
    (Operation*) lessThanEquals<uint16_t>,
    (Operation*) lessThanEquals<uint32_t>,
    (Operation*) lessThanEquals<uint64_t>,
    (Operation*) lessThanEquals<float>,
    (Operation*) lessThanEquals<double>,
    // greater than
    (Operation*) greaterThan<int8_t>,
    (Operation*) greaterThan<int16_t>,
    (Operation*) greaterThan<int32_t>,
    (Operation*) greaterThan<int64_t>,
    (Operation*) greaterThan<uint8_t>,
    (Operation*) greaterThan<uint16_t>,
    (Operation*) greaterThan<uint32_t>,
    (Operation*) greaterThan<uint64_t>,
    (Operation*) greaterThan<float>,
    (Operation*) greaterThan<double>,
    // greater than equals
    (Operation*) greaterThanEquals<int8_t>,
    (Operation*) greaterThanEquals<int16_t>,
    (Operation*) greaterThanEquals<int32_t>,
    (Operation*) greaterThanEquals<int64_t>,
    (Operation*) greaterThanEquals<uint8_t>,
    (Operation*) greaterThanEquals<uint16_t>,
    (Operation*) greaterThanEquals<uint32_t>,
    (Operation*) greaterThanEquals<uint64_t>,
    (Operation*) greaterThanEquals<float>,
    (Operation*) greaterThanEquals<double>,
    // not equals
    (Operation*) notEquals<bool>,
    (Operation*) notEquals<int8_t>,
    (Operation*) notEquals<int16_t>,
    (Operation*) notEquals<int32_t>,
    (Operation*) notEquals<int64_t>,
    (Operation*) notEquals<uint8_t>,
    (Operation*) notEquals<uint16_t>,
    (Operation*) notEquals<uint32_t>,
    (Operation*) notEquals<uint64_t>,
    (Operation*) notEquals<float>,
    (Operation*) notEquals<double>,
    // load
    (Operation*) load<int8_t>,
    (Operation*) load<int16_t>,
    (Operation*) load<int32_t>,
    (Operation*) load<int64_t>,
    (Operation*) load<uint8_t>,
    (Operation*) load<uint16_t>,
    (Operation*) load<uint32_t>,
    (Operation*) load<uint64_t>,
    (Operation*) load<float>,
    (Operation*) load<double>,
    (Operation*) load<bool>,
    // store
    (Operation*) store<int8_t>,
    (Operation*) store<int16_t>,
    (Operation*) store<int32_t>,
    (Operation*) store<int64_t>,
    (Operation*) store<uint8_t>,
    (Operation*) store<uint16_t>,
    (Operation*) store<uint32_t>,
    (Operation*) store<uint64_t>,
    (Operation*) store<float>,
    (Operation*) store<double>,
    (Operation*) store<bool>,
    // and
    (Operation*) andOp<bool>,
    (Operation*) orOp<bool>,
    (Operation*) notOp<bool>,
    // Casts
    // int to int
    (Operation*) cast<int8_t, int16_t>,
    (Operation*) cast<int8_t, int32_t>,
    (Operation*) cast<int8_t, int64_t>,
    (Operation*) cast<int16_t, int8_t>,
    (Operation*) cast<int16_t, int32_t>,
    (Operation*) cast<int16_t, int64_t>,
    (Operation*) cast<int32_t, int8_t>,
    (Operation*) cast<int32_t, int16_t>,
    (Operation*) cast<int32_t, int64_t>,
    (Operation*) cast<int64_t, int8_t>,
    (Operation*) cast<int64_t, int16_t>,
    (Operation*) cast<int64_t, int32_t>,
    // uint to int
    (Operation*) cast<uint8_t, int8_t>,
    (Operation*) cast<uint8_t, int16_t>,
    (Operation*) cast<uint8_t, int32_t>,
    (Operation*) cast<uint8_t, int64_t>,
    (Operation*) cast<uint16_t, int8_t>,
    (Operation*) cast<uint16_t, int16_t>,
    (Operation*) cast<uint16_t, int32_t>,
    (Operation*) cast<uint16_t, int64_t>,
    (Operation*) cast<uint32_t, int8_t>,
    (Operation*) cast<uint32_t, int16_t>,
    (Operation*) cast<uint32_t, int32_t>,
    (Operation*) cast<uint32_t, int64_t>,
    (Operation*) cast<uint64_t, int8_t>,
    (Operation*) cast<uint64_t, int16_t>,
    (Operation*) cast<uint64_t, int32_t>,
    (Operation*) cast<uint64_t, int64_t>,
    // uint to uint
    (Operation*) cast<uint8_t, uint16_t>,
    (Operation*) cast<uint8_t, uint32_t>,
    (Operation*) cast<uint8_t, uint64_t>,
    (Operation*) cast<uint16_t, uint8_t>,
    (Operation*) cast<uint16_t, uint32_t>,
    (Operation*) cast<uint16_t, uint64_t>,
    (Operation*) cast<uint32_t, uint8_t>,
    (Operation*) cast<uint32_t, uint16_t>,
    (Operation*) cast<uint32_t, uint64_t>,
    (Operation*) cast<uint64_t, uint8_t>,
    (Operation*) cast<uint64_t, uint16_t>,
    (Operation*) cast<uint64_t, uint32_t>,
    // int to uint
    (Operation*) cast<int8_t, uint8_t>,
    (Operation*) cast<int8_t, uint16_t>,
    (Operation*) cast<int8_t, uint32_t>,
    (Operation*) cast<int8_t, uint64_t>,
    (Operation*) cast<int16_t, uint8_t>,
    (Operation*) cast<int16_t, uint16_t>,
    (Operation*) cast<int16_t, uint32_t>,
    (Operation*) cast<int16_t, uint64_t>,
    (Operation*) cast<int32_t, uint8_t>,
    (Operation*) cast<int32_t, uint16_t>,
    (Operation*) cast<int32_t, uint32_t>,
    (Operation*) cast<int32_t, uint64_t>,
    (Operation*) cast<int64_t, uint8_t>,
    (Operation*) cast<int64_t, uint16_t>,
    (Operation*) cast<int64_t, uint32_t>,
    (Operation*) cast<int64_t, uint64_t>,
    // from float to int
    (Operation*) cast<float, int8_t>,
    (Operation*) cast<float, int16_t>,
    (Operation*) cast<float, int32_t>,
    (Operation*) cast<float, int64_t>,
    (Operation*) cast<float, uint8_t>,
    (Operation*) cast<float, uint16_t>,
    (Operation*) cast<float, uint32_t>,
    (Operation*) cast<float, uint64_t>,
    (Operation*) cast<float, double>,
    // from double to int
    (Operation*) cast<double, int8_t>,
    (Operation*) cast<double, int16_t>,
    (Operation*) cast<double, int32_t>,
    (Operation*) cast<double, int64_t>,
    (Operation*) cast<double, uint8_t>,
    (Operation*) cast<double, uint16_t>,
    (Operation*) cast<double, uint32_t>,
    (Operation*) cast<double, uint64_t>,
    (Operation*) cast<double, float>,
    // from int to floats
    (Operation*) cast<int8_t, float>,
    (Operation*) cast<int8_t, double>,
    (Operation*) cast<int16_t, float>,
    (Operation*) cast<int16_t, double>,
    (Operation*) cast<int32_t, float>,
    (Operation*) cast<int32_t, double>,
    (Operation*) cast<int64_t, float>,
    (Operation*) cast<int64_t, double>,
    // uint to float
    (Operation*) cast<uint8_t, float>,
    (Operation*) cast<uint8_t, double>,
    (Operation*) cast<uint16_t, float>,
    (Operation*) cast<uint16_t, double>,
    (Operation*) cast<uint32_t, float>,
    (Operation*) cast<uint32_t, double>,
    (Operation*) cast<uint64_t, float>,
    (Operation*) cast<uint64_t, double>,
    // FUNCTION CALLS
    (Operation*) dyncallReset,
    (Operation*) dyncallArgB,
    (Operation*) dyncallArgI8,
    (Operation*) dyncallArgI16,
    (Operation*) dyncallArgI32,
    (Operation*) dyncallArgI64,
    (Operation*) dyncallArgF,
    (Operation*) dyncallArgD,
    (Operation*) dyncallArgPtr,
    (Operation*) dyncallCallV,
    (Operation*) dyncallCallB,
    (Operation*) dyncallCallI8,
    (Operation*) dyncallCallI16,
    (Operation*) dyncallCallI32,
    (Operation*) dyncallCallI64,
    (Operation*) dyncallCallPtr,
    (Operation*) dyncallCallf,
    (Operation*) dyncallCalld,
    // BITWISE OPS
    // band
    (Operation*) bitwiseAnd<int8_t>,
    (Operation*) bitwiseAnd<int16_t>,
    (Operation*) bitwiseAnd<int32_t>,
    (Operation*) bitwiseAnd<int64_t>,
    (Operation*) bitwiseAnd<uint8_t>,
    (Operation*) bitwiseAnd<uint16_t>,
    (Operation*) bitwiseAnd<uint32_t>,
    (Operation*) bitwiseAnd<uint64_t>,
    // bor
    (Operation*) bitwiseOr<int8_t>,
    (Operation*) bitwiseOr<int16_t>,
    (Operation*) bitwiseOr<int32_t>,
    (Operation*) bitwiseOr<int64_t>,
    (Operation*) bitwiseOr<uint8_t>,
    (Operation*) bitwiseOr<uint16_t>,
    (Operation*) bitwiseOr<uint32_t>,
    (Operation*) bitwiseOr<uint64_t>,
    // bxor
    (Operation*) bitwiseXOr<int8_t>,
    (Operation*) bitwiseXOr<int16_t>,
    (Operation*) bitwiseXOr<int32_t>,
    (Operation*) bitwiseXOr<int64_t>,
    (Operation*) bitwiseXOr<uint8_t>,
    (Operation*) bitwiseXOr<uint16_t>,
    (Operation*) bitwiseXOr<uint32_t>,
    (Operation*) bitwiseXOr<uint64_t>,
    // blsh
    (Operation*) bitwiseLSH<int8_t>,
    (Operation*) bitwiseLSH<int16_t>,
    (Operation*) bitwiseLSH<int32_t>,
    (Operation*) bitwiseLSH<int64_t>,
    (Operation*) bitwiseLSH<uint8_t>,
    (Operation*) bitwiseLSH<uint16_t>,
    (Operation*) bitwiseLSH<uint32_t>,
    (Operation*) bitwiseLSH<uint64_t>,
    // brsh
    (Operation*) bitwiseRSH<int8_t>,
    (Operation*) bitwiseRSH<int16_t>,
    (Operation*) bitwiseRSH<int32_t>,
    (Operation*) bitwiseRSH<int64_t>,
    (Operation*) bitwiseRSH<uint8_t>,
    (Operation*) bitwiseRSH<uint16_t>,
    (Operation*) bitwiseRSH<uint32_t>,
    (Operation*) bitwiseRSH<uint64_t>,
    // bnot
    (Operation*) bitwiseNot<int64_t>,
    // select
    (Operation*) selectOp<int8_t>,
    (Operation*) selectOp<int16_t>,
    (Operation*) selectOp<int32_t>,
    (Operation*) selectOp<int64_t>,
    (Operation*) selectOp<uint8_t>,
    (Operation*) selectOp<uint16_t>,
    (Operation*) selectOp<uint32_t>,
    (Operation*) selectOp<uint64_t>,
    (Operation*) selectOp<float>,
    (Operation*) selectOp<double>,
    (Operation*) selectOp<bool>,
    (Operation*) selectOp<void*>,

};

FunctionCallTarget::FunctionCallTarget(std::vector<std::pair<short, Type>> arguments, void* functionPtr)
    : arguments(std::move(arguments)), functionPtr(functionPtr) {
}

BCInterpreter::BCInterpreter(Code code, RegisterFile registerFile)
    : code(std::move(code)), registerFile(std::move(registerFile)) {
	// The register file was built against a temporary Code instance whose allocaBuffers
	// may have been copied (and thus relocated) before reaching this constructor.
	// Re-point each alloca register at the actual buffer in *this* code object.
	for (const auto& [reg, bufIdx] : this->code.allocaRegisterMap) {
		this->registerFile[reg] = reinterpret_cast<int64_t>(this->code.allocaBuffers[bufIdx].data());
	}
}

BCExecutable::BCExecutable(std::unordered_map<std::string, void*> functionPtrs,
                           std::vector<std::unique_ptr<BCCallbackData>> callbackData,
                           std::vector<DCCallback*> callbacks)
    : functionPtrs_(std::move(functionPtrs)), callbackData_(std::move(callbackData)), callbacks_(std::move(callbacks)) {
}

BCExecutable::~BCExecutable() {
	for (auto* cb : callbacks_) {
		dcbFreeCallback(cb);
	}
}

void* BCExecutable::getInvocableFunctionPtr(const std::string& member) {
	auto it = functionPtrs_.find(member);
	if (it != functionPtrs_.end()) {
		return it->second;
	}
	return nullptr;
}

bool BCExecutable::hasInvocableFunctionPtr() {
	return true;
}

int64_t BCInterpreter::invoke(DCArgs* args, const std::vector<Type>& argTypes) {
	// Per-invocation copy of the register file (thread-safe + reentrant).
	RegisterFile regs = registerFile;

	// Per-invocation alloca buffers, zeroed for a clean stack frame.
	std::vector<std::vector<uint8_t>> localAllocaBuffers(code.allocaBuffers.size());
	for (const auto& [reg, bufIdx] : code.allocaRegisterMap) {
		localAllocaBuffers[bufIdx].resize(code.allocaBuffers[bufIdx].size(), 0);
		regs[reg] = reinterpret_cast<int64_t>(localAllocaBuffers[bufIdx].data());
	}

	// Read arguments from DCArgs directly into the local register file.
	for (size_t i = 0; i < argTypes.size(); i++) {
		auto reg = code.arguments[i];
		switch (argTypes[i]) {
		case Type::b:
			// dyncall's dcbArgBool returns DCbool (int); mask to the low
			// byte before converting to bool — see Dyncall::callB.
			writeReg<bool>(regs, reg, (dcbArgBool(args) & 0xFF) != 0);
			break;
		case Type::i8:
			writeReg<int8_t>(regs, reg, static_cast<int8_t>(dcbArgChar(args)));
			break;
		case Type::i16:
			writeReg<int16_t>(regs, reg, static_cast<int16_t>(dcbArgShort(args)));
			break;
		case Type::i32:
			writeReg<int32_t>(regs, reg, static_cast<int32_t>(dcbArgInt(args)));
			break;
		case Type::i64:
			writeReg<int64_t>(regs, reg, static_cast<int64_t>(dcbArgLong(args)));
			break;
		case Type::ui8:
			writeReg<uint8_t>(regs, reg, static_cast<uint8_t>(dcbArgUChar(args)));
			break;
		case Type::ui16:
			writeReg<uint16_t>(regs, reg, static_cast<uint16_t>(dcbArgUShort(args)));
			break;
		case Type::ui32:
			writeReg<uint32_t>(regs, reg, static_cast<uint32_t>(dcbArgUInt(args)));
			break;
		case Type::ui64:
			writeReg<uint64_t>(regs, reg, static_cast<uint64_t>(dcbArgULong(args)));
			break;
		case Type::f32:
			writeReg<float>(regs, reg, static_cast<float>(dcbArgFloat(args)));
			break;
		case Type::f64:
			writeReg<double>(regs, reg, static_cast<double>(dcbArgDouble(args)));
			break;
		case Type::ptr:
			regs[reg] = reinterpret_cast<int64_t>(dcbArgPointer(args));
			break;
		default:
			break;
		}
	}

	return execute(regs);
}

int64_t BCInterpreter::execute(RegisterFile& regs) const {
	// first block is always the entrypoint
	auto* currentBlock = &code.blocks[0];
	while (true) {
		// execute operations in block
		for (const auto& c : currentBlock->code) {
			OpTable[(int16_t) c.op](c, regs);
		}
		// handle terminator
		if (const auto* res = std::get_if<BranchOp>(&currentBlock->terminatorOp)) {
			currentBlock = &code.blocks[res->nextBlock];
		} else if (const auto* res = std::get_if<ConditionalJumpOp>(&currentBlock->terminatorOp)) {
			if (readReg<bool>(regs, res->conditionalReg)) {
				currentBlock = &code.blocks[res->trueBlock];
			} else {
				currentBlock = &code.blocks[res->falseBlock];
			}
		} else if (const auto* res = std::get_if<ReturnOp>(&currentBlock->terminatorOp)) {
			if (res->resultReg < 0) {
				return 0;
			}
			return regs[res->resultReg];
		}
	}
}

std::ostream& operator<<(std::ostream& os, const Code& code) {
	for (size_t i = 0; i < code.blocks.size(); i++) {
		os << "Block " << i << "\n";
		os << code.blocks[i];
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const CodeBlock& block) {
	for (auto& code : block.code) {
		os << "\t" << code << "\n";
	}

	// handle terminator
	if (const auto* res = std::get_if<BranchOp>(&block.terminatorOp)) {
		os << "\t" << "BR " << res->nextBlock << "\n";
	} else if (const auto* res = std::get_if<ConditionalJumpOp>(&block.terminatorOp)) {
		os << "\t" << "CMP " << "r" << res->conditionalReg << " " << res->trueBlock << " " << res->falseBlock << "\n";
	} else if (const auto* res = std::get_if<ReturnOp>(&block.terminatorOp)) {
		os << "\t" << "Return " << "r" << res->resultReg << "\n";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const OpCode& code) {
	// auto str = std::string(code.op);
	os << "str" << " r" << code.reg1;
	if (code.reg2 != -1) {
		os << " r" << code.reg2;
	}

	if (code.output != -1) {
		os << " r" << code.output;
	}

	return os;
}

std::string Code::toString() {
	std::stringstream ss;
	ss << *this;
	return ss.str();
}

} // namespace nautilus::compiler::bc
