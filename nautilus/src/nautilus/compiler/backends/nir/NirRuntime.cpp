#include "nautilus/compiler/backends/nir/NirRuntime.hpp"
#include "nautilus/common/ExceptionTransport.hpp"
#include "nautilus/exceptions/ArithmeticException.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

namespace nautilus::compiler::nirbackend {

void nautilus_rt_throw_arith() {
	throw ArithmeticException("integer division or remainder by zero, or the minimum divided by -1");
}

void* throwArithThunk() {
	return reinterpret_cast<void*>(&captureThrowingCall<void>);
}

void* throwArithTarget() {
	return reinterpret_cast<void*>(&nautilus_rt_throw_arith);
}

std::unordered_set<std::string> functionsNeedingCapture(const nir::Module& module) {
	const auto count = static_cast<uint32_t>(module.functions.size());
	std::vector<uint8_t> direct(count, 0);
	std::vector<std::vector<uint32_t>> callees(count);
	for (uint32_t f = 0; f < count; ++f) {
		for (const auto& inst : module.functionInsts(f)) {
			const auto op = static_cast<nir::Op>(inst.op);
			if (op == nir::Op::Div || op == nir::Op::Rem) {
				direct[f] = direct[f] || (inst.flags & 2) != 0;
			} else if (op == nir::Op::Call || op == nir::Op::CallIndirect) {
				const uint32_t* list = module.extra.data() + inst.c;
				const uint32_t argc = list[0];
				direct[f] = direct[f] || list[1 + argc] != nir::NO_UNWIND;
				if (op == nir::Op::Call &&
				    module.callees[inst.a].kind == static_cast<uint8_t>(nir::CalleeKind::Internal)) {
					callees[f].push_back(module.callees[inst.a].target);
				}
			}
		}
	}
	std::vector<uint8_t> needs = direct;
	for (bool changed = true; changed;) {
		changed = false;
		for (uint32_t f = 0; f < count; ++f) {
			if (!needs[f]) {
				for (auto callee : callees[f]) {
					if (needs[callee]) {
						needs[f] = 1;
						changed = true;
						break;
					}
				}
			}
		}
	}
	std::unordered_set<std::string> names;
	for (uint32_t f = 0; f < count; ++f) {
		if (needs[f]) {
			names.insert(std::string(module.name(module.functions[f].nameId)));
		}
	}
	return names;
}

float nautilus_rt_fmod_f32(float a, float b) {
	return std::fmod(a, b);
}

double nautilus_rt_fmod_f64(double a, double b) {
	return std::fmod(a, b);
}

namespace {

/// Saturating float to integer conversion without the undefined behavior of the C++ cast.
template <class D, class S>
D saturate(S v) {
	if (std::isnan(v)) {
		return 0;
	}
	constexpr S lo = static_cast<S>(std::numeric_limits<D>::min());
	// 2^bits is exactly representable in a float or double; the largest D is one less than it
	constexpr S hiExclusive = static_cast<S>(std::numeric_limits<D>::max()) + static_cast<S>(1);
	if (v <= lo) {
		return std::numeric_limits<D>::min();
	}
	if (v >= hiExclusive) {
		return std::numeric_limits<D>::max();
	}
	return static_cast<D>(v);
}

template <class D, class S>
D floatToInt(S v) {
	return saturate<D, S>(v);
}

template <class S>
void* pick(bool isSigned, unsigned bits) {
	switch (bits) {
	case 8:
		return isSigned ? reinterpret_cast<void*>(&floatToInt<int8_t, S>)
		                : reinterpret_cast<void*>(&floatToInt<uint8_t, S>);
	case 16:
		return isSigned ? reinterpret_cast<void*>(&floatToInt<int16_t, S>)
		                : reinterpret_cast<void*>(&floatToInt<uint16_t, S>);
	case 32:
		return isSigned ? reinterpret_cast<void*>(&floatToInt<int32_t, S>)
		                : reinterpret_cast<void*>(&floatToInt<uint32_t, S>);
	case 64:
		return isSigned ? reinterpret_cast<void*>(&floatToInt<int64_t, S>)
		                : reinterpret_cast<void*>(&floatToInt<uint64_t, S>);
	default:
		return nullptr;
	}
}

} // namespace

void* floatToIntHelper(bool signedResult, unsigned bits, unsigned sourceBits) {
	return sourceBits == 32 ? pick<float>(signedResult, bits) : pick<double>(signedResult, bits);
}

} // namespace nautilus::compiler::nirbackend
