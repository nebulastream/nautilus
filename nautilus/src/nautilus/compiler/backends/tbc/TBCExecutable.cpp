
#include "nautilus/compiler/backends/tbc/TBCExecutable.hpp"
#include "nautilus/compiler/backends/tbc/TBCInterpreter.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <any>
#include <cstring>

namespace nautilus::compiler::tbc {

namespace {

/// Normalize a C++ value into a 64-bit register slot per the declared IR type
/// (same convention as the interpreter's writeReg).
template <class T>
uint64_t normalizeAs(Type type, T value) {
	switch (type) {
	case Type::b:
		return value != T {} ? 1 : 0;
	case Type::i8:
		return static_cast<uint64_t>(static_cast<uint8_t>(static_cast<int8_t>(value)));
	case Type::ui8:
		return static_cast<uint64_t>(static_cast<uint8_t>(value));
	case Type::i16:
		return static_cast<uint64_t>(static_cast<uint16_t>(static_cast<int16_t>(value)));
	case Type::ui16:
		return static_cast<uint64_t>(static_cast<uint16_t>(value));
	case Type::i32:
		return static_cast<uint64_t>(static_cast<uint32_t>(static_cast<int32_t>(value)));
	case Type::ui32:
		return static_cast<uint64_t>(static_cast<uint32_t>(value));
	case Type::i64:
		return static_cast<uint64_t>(static_cast<int64_t>(value));
	case Type::ui64:
		return static_cast<uint64_t>(value);
	case Type::f32: {
		const auto f = static_cast<float>(value);
		uint64_t raw = 0;
		std::memcpy(&raw, &f, sizeof(f));
		return raw;
	}
	case Type::f64: {
		const auto d = static_cast<double>(value);
		uint64_t raw = 0;
		std::memcpy(&raw, &d, sizeof(d));
		return raw;
	}
	default:
		throw RuntimeException("tbc: unsupported argument type");
	}
}

template <class T>
bool tryDecode(const std::any& value, Type type, uint64_t& out) {
	if (const T* typed = std::any_cast<T>(&value)) {
		out = normalizeAs(type, *typed);
		return true;
	}
	return false;
}

/// Decode a boxed argument (see Executable::Invocable::getGenericArg for how
/// callers box: fundamentals as-is, pointers as void*, enums as their
/// underlying type). Distinct C++ types can share one nautilus Type (char vs
/// int8_t; long vs long long for i64 across platforms), so decoding probes
/// every fundamental candidate instead of assuming a single boxed type.
uint64_t decodeArgument(const std::any& value, Type type) {
	if (type == Type::ptr) {
		return reinterpret_cast<uint64_t>(std::any_cast<void*>(value));
	}
	uint64_t out = 0;
	if (tryDecode<bool>(value, type, out) || tryDecode<char>(value, type, out) ||
	    tryDecode<signed char>(value, type, out) || tryDecode<unsigned char>(value, type, out) ||
	    tryDecode<short>(value, type, out) || tryDecode<unsigned short>(value, type, out) ||
	    tryDecode<int>(value, type, out) || tryDecode<unsigned int>(value, type, out) ||
	    tryDecode<long>(value, type, out) || tryDecode<unsigned long>(value, type, out) ||
	    tryDecode<long long>(value, type, out) || tryDecode<unsigned long long>(value, type, out) ||
	    tryDecode<float>(value, type, out) || tryDecode<double>(value, type, out)) {
		return out;
	}
	throw RuntimeException("tbc: cannot decode argument of unexpected boxed type");
}

/// Box a raw 64-bit result slot per the function's IR return type, matching
/// exactly what Invocable::operator() unboxes.
std::any boxResult(Type type, uint64_t raw) {
	switch (type) {
	case Type::b:
		return std::any(raw != 0);
	case Type::i8:
		return std::any(static_cast<int8_t>(raw));
	case Type::ui8:
		return std::any(static_cast<uint8_t>(raw));
	case Type::i16:
		return std::any(static_cast<int16_t>(raw));
	case Type::ui16:
		return std::any(static_cast<uint16_t>(raw));
	case Type::i32:
		return std::any(static_cast<int32_t>(raw));
	case Type::ui32:
		return std::any(static_cast<uint32_t>(raw));
	case Type::i64:
		return std::any(static_cast<int64_t>(raw));
	case Type::ui64:
		return std::any(static_cast<uint64_t>(raw));
	case Type::f32: {
		float value;
		std::memcpy(&value, &raw, sizeof(value));
		return std::any(value);
	}
	case Type::f64: {
		double value;
		std::memcpy(&value, &raw, sizeof(value));
		return std::any(value);
	}
	case Type::ptr:
		return std::any(reinterpret_cast<void*>(raw));
	case Type::v:
		return {};
	}
	throw RuntimeException("tbc: unsupported return type");
}

class TBCGenericInvocable final : public Executable::GenericInvocable {
public:
	TBCGenericInvocable(std::shared_ptr<TBCProgram> program, uint32_t functionIndex)
	    : program(std::move(program)), functionIndex(functionIndex) {
	}

	std::any invokeGeneric(const std::vector<std::any>& args) override {
		const TBCFunction& function = program->functions[functionIndex];
		if (args.size() != function.argTypes.size()) {
			throw RuntimeException("tbc: argument count mismatch");
		}
		std::vector<uint64_t> slots(args.size());
		for (size_t i = 0; i < args.size(); ++i) {
			slots[i] = decodeArgument(args[i], function.argTypes[i]);
		}
		const uint64_t raw = invoke(*program, functionIndex, slots.data(), slots.size());
		return boxResult(function.returnType, raw);
	}

	bool invokeRaw(const uint64_t* args, size_t argCount, uint64_t* result) override {
		if (argCount != program->functions[functionIndex].argTypes.size()) {
			throw RuntimeException("tbc: argument count mismatch");
		}
		*result = invoke(*program, functionIndex, args, argCount);
		return true;
	}

private:
	std::shared_ptr<TBCProgram> program;
	uint32_t functionIndex;
};

} // namespace

TBCExecutable::TBCExecutable(std::shared_ptr<TBCProgram> program) : program(std::move(program)) {
}

void* TBCExecutable::getInvocableFunctionPtr(const std::string&) {
	throw RuntimeException("tbc: no native function pointers; use the generic invocable");
}

bool TBCExecutable::hasInvocableFunctionPtr() {
	return false;
}

std::unique_ptr<Executable::GenericInvocable> TBCExecutable::getGenericInvocable(const std::string& member) {
	const auto it = program->functionIndex.find(member);
	if (it == program->functionIndex.end()) {
		throw RuntimeException("tbc: unknown function \"" + member + "\"");
	}
	return std::make_unique<TBCGenericInvocable>(program, it->second);
}

} // namespace nautilus::compiler::tbc
