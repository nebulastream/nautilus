#pragma once

// A reference evaluator for SSA-level NIR, for tests only: it follows 13-nir-spec.md §5 operation by operation
// (wrapping integers, checked division that throws, IEEE floats, saturating float to integer casts, masked shifts) and
// runs external calls on the host. It is slow and simple on purpose; it is the oracle against which the conversion, and
// later the backends, are compared.

#include "nautilus/nir/Module.hpp"
#include "nautilus/nir/Semantics.hpp"
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <exception>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace nautilus::nir::test {

/// What a checked division throws (D10).
class ArithmeticError : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

class Evaluator {
public:
	explicit Evaluator(const Module& m) : m_(m) {
	}

	/// Calls the function `name` with raw argument bits (floats as their bit patterns, pointers as addresses) and
	/// returns the raw bits of the result, zero-extended; 0 for a void function. Throws what the function throws.
	uint64_t call(std::string_view name, std::span<const uint64_t> args) {
		for (uint32_t f = 0; f < m_.functions.size(); ++f) {
			if (m_.name(m_.functions[f].nameId) == name) {
				return run(f, args);
			}
		}
		throw std::runtime_error("no function named " + std::string(name));
	}

	static uint64_t bitsOf(float v) {
		return std::bit_cast<uint32_t>(v);
	}
	static uint64_t bitsOf(double v) {
		return std::bit_cast<uint64_t>(v);
	}

private:
	static uint32_t widthOf(TypeId t) {
		return semantics::widthOf(t);
	}
	static uint64_t mask(TypeId t) {
		return semantics::mask(t);
	}
	static uint64_t canon(uint64_t v, TypeId t) {
		return semantics::canon(v, t);
	}
	static int64_t sext(uint64_t v, TypeId t) {
		return semantics::sext(v, t);
	}
	static bool isFloat(TypeId t) {
		return semantics::isFloatType(t);
	}

	struct Frame {
		const FunctionRecord* fn;
		uint32_t index;
		std::vector<uint64_t> value;
		std::vector<TypeId> type;
		std::vector<std::vector<std::max_align_t>> slots;
	};

	uint64_t get(const Frame& fr, Ref r) const {
		if (r == NONE) {
			return 0;
		}
		if (isConstRef(r)) {
			return m_.consts.record(r).bits;
		}
		return fr.value[r];
	}
	TypeId typeOf(const Frame& fr, Ref r) const {
		if (isConstRef(r)) {
			return static_cast<TypeId>(m_.consts.record(r).type);
		}
		return fr.type[r];
	}

	uint64_t run(uint32_t f, std::span<const uint64_t> args);
	uint64_t callNative(const void* fn, const std::vector<uint64_t>& args, TypeId result) const;
	uint64_t cast(uint8_t kind, uint64_t v, TypeId from, TypeId to) const;
	uint64_t compare(const Inst& inst, uint64_t a, uint64_t b, TypeId t) const;

	const Module& m_;
	/// A function that does not finish within this many instructions is reported, not waited for.
	uint64_t budget_ = 50'000'000;
};

inline uint64_t Evaluator::callNative(const void* fn, const std::vector<uint64_t>& args, TypeId result) const {
	if (args.size() > 6) {
		throw std::runtime_error("the evaluator calls natives with at most six arguments");
	}
	uint64_t a[6] = {};
	for (size_t i = 0; i < args.size(); ++i) {
		a[i] = args[i];
	}
	auto f = reinterpret_cast<uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)>(
	    const_cast<void*>(fn));
	uint64_t r = f(a[0], a[1], a[2], a[3], a[4], a[5]);
	return result == TypeId::Void ? 0 : canon(r, result);
}

inline uint64_t Evaluator::cast(uint8_t kind, uint64_t v, TypeId from, TypeId to) const {
	auto r = semantics::evalCast(kind, v, from, to);
	if (!r) {
		throw std::runtime_error("invalid cast kind");
	}
	return *r;
}

inline uint64_t Evaluator::compare(const Inst& inst, uint64_t a, uint64_t b, TypeId t) const {
	return semantics::evalCompare(inst.flags, a, b, t);
}

inline uint64_t Evaluator::run(uint32_t f, std::span<const uint64_t> args) {
	Frame fr;
	fr.fn = &m_.functions[f];
	fr.index = f;
	auto insts = m_.functionInsts(f);
	auto blocks = m_.functionBlocks(f);
	fr.value.assign(insts.size(), 0);
	fr.type.assign(insts.size(), TypeId::Void);
	for (const auto& a : m_.functionAllocas(f)) {
		fr.slots.emplace_back((a.size + sizeof(std::max_align_t) - 1) / sizeof(std::max_align_t) + 1);
	}

	uint32_t block = 0;
	// the parameters are the entry block's arguments
	for (uint32_t i = 0; i < blocks[0].argCount; ++i) {
		fr.value[i] = args[i];
		fr.type[i] = static_cast<TypeId>(insts[i].type);
		fr.value[i] = canon(fr.value[i], fr.type[i]);
	}
	std::exception_ptr pending;

	auto jump = [&](uint32_t target, const uint32_t* argList, uint32_t argc) {
		std::vector<uint64_t> values(argc);
		for (uint32_t i = 0; i < argc; ++i) {
			values[i] = get(fr, argList[i]);
		}
		const auto& tb = blocks[target];
		for (uint32_t i = 0; i < argc; ++i) {
			fr.value[tb.first + i] = values[i];
		}
		block = target;
	};

	for (;;) {
		const auto& blk = blocks[block];
		for (uint32_t k = blk.argCount; k < blk.count; ++k) {
			const uint32_t id = blk.first + k;
			const Inst& inst = insts[id];
			if (budget_-- == 0) {
				throw std::runtime_error("the evaluator ran out of its instruction budget");
			}
			const auto type = static_cast<TypeId>(inst.type);
			fr.type[id] = type;
			auto A = [&] {
				return get(fr, inst.a);
			};
			auto B = [&] {
				return get(fr, inst.b);
			};
			auto C = [&] {
				return get(fr, inst.c);
			};
			auto unwindTo = [&](uint32_t unwind) -> bool {
				// an exception reached an instruction: continue in its landing pad, or propagate
				if (unwind == NO_UNWIND || unwind == NO_CLEANUP) {
					return false;
				}
				pending = std::current_exception();
				block = unwind;
				return true;
			};
			bool jumped = false;
			try {
				switch (static_cast<Op>(inst.op)) {
				case Op::Add:
				case Op::Sub:
				case Op::Mul:
				case Op::And:
				case Op::Or:
				case Op::BAnd:
				case Op::BOr:
				case Op::BXor:
				case Op::Shl:
				case Op::Shr:
				case Op::Div:
				case Op::Rem: {
					auto r = semantics::evalBinary(static_cast<Op>(inst.op), inst.flags, type, A(), B());
					if (!r) {
						throw ArithmeticError("a division that throws");
					}
					fr.value[id] = *r;
					break;
				}
				case Op::FNeg:
				case Op::Not:
				case Op::BNot:
					fr.value[id] = *semantics::evalUnary(static_cast<Op>(inst.op), type, A());
					break;
				case Op::Cmp:
					fr.value[id] = compare(inst, A(), B(), typeOf(fr, inst.a));
					break;
				case Op::Select:
					fr.value[id] = (A() & 1) ? B() : C();
					break;
				case Op::Cast:
					fr.value[id] = cast(inst.flags & 15, A(), typeOf(fr, inst.a), type);
					break;
				case Op::Load: {
					const auto* p = reinterpret_cast<const unsigned char*>(A());
					uint64_t v = 0;
					std::memcpy(&v, p, type == TypeId::Bool ? 1 : widthOf(type) / 8);
					fr.value[id] = type == TypeId::Bool ? (v != 0) : v;
					break;
				}
				case Op::Store: {
					auto* p = reinterpret_cast<unsigned char*>(A());
					auto t = typeOf(fr, inst.b);
					uint64_t v = B();
					std::memcpy(p, &v, t == TypeId::Bool ? 1 : widthOf(t) / 8);
					break;
				}
				case Op::Alloca:
					fr.value[id] = reinterpret_cast<uint64_t>(fr.slots[inst.a].data());
					break;
				case Op::PtrConst:
					fr.value[id] = inst.a == NONE ? 0 : m_.pointers[inst.a].address;
					break;
				case Op::DataAddr:
					fr.value[id] = reinterpret_cast<uint64_t>(m_.dataBytes.data() + m_.data[inst.a].byteOffset);
					break;
				case Op::Call:
				case Op::CallIndirect: {
					const uint32_t* list = m_.extra.data() + inst.c;
					const uint32_t argc = list[0];
					std::vector<uint64_t> callArgs(argc);
					for (uint32_t i = 0; i < argc; ++i) {
						callArgs[i] = get(fr, list[1 + i]);
					}
					const uint32_t unwind = list[1 + argc];
					try {
						if (static_cast<Op>(inst.op) == Op::CallIndirect) {
							fr.value[id] = callNative(reinterpret_cast<const void*>(A()), callArgs, type);
						} else {
							const auto& callee = m_.callees[inst.a];
							switch (static_cast<CalleeKind>(callee.kind)) {
							case CalleeKind::Internal:
								fr.value[id] = run(callee.target, callArgs);
								break;
							case CalleeKind::External:
								fr.value[id] = callNative(
								    reinterpret_cast<const void*>(m_.pointers[callee.target].address), callArgs, type);
								break;
							default:
								throw std::runtime_error("the evaluator does not run imported or intrinsic callees");
							}
						}
					} catch (...) {
						if (!unwindTo(unwind)) {
							throw;
						}
						jumped = true;
					}
					break;
				}
				case Op::Br: {
					const uint32_t* list = m_.extra.data() + inst.c;
					jump(inst.a, list + 1, list[0]);
					jumped = true;
					break;
				}
				case Op::CondBr: {
					const uint32_t* list = m_.extra.data() + inst.c;
					const uint32_t tc = list[1];
					if (A() & 1) {
						jump(inst.b, list + 2, tc);
					} else {
						jump(list[2 + tc], list + 4 + tc, list[3 + tc]);
					}
					jumped = true;
					break;
				}
				case Op::Ret:
					return inst.a == NONE ? 0 : get(fr, inst.a);
				case Op::Resume:
					std::rethrow_exception(pending);
				case Op::BlockArg:
					break;
				default:
					throw std::runtime_error("the evaluator does not run opcode " + std::to_string(inst.op));
				}
			} catch (const ArithmeticError&) {
				// a checked division that throws unwinds like a call
				const bool checked =
				    (inst.op == static_cast<uint16_t>(Op::Div) || inst.op == static_cast<uint16_t>(Op::Rem)) &&
				    (inst.flags & 2);
				if (!checked || !unwindTo(inst.c)) {
					throw;
				}
				jumped = true;
			}
			if (jumped) {
				break;
			}
		}
	}
}

} // namespace nautilus::nir::test
