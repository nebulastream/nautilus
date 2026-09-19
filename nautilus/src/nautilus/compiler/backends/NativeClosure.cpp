
#include "nautilus/compiler/backends/NativeClosure.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>

namespace nautilus::compiler {

namespace {

/// Packs a native value into the raw 64-bit register slot convention shared by
/// the bc and tbc interpreters: bool as 0/1, narrow integers zero-extended via
/// their unsigned representation, floats and pointers bit-exact.
template <class T>
uint64_t slotOf(T value) {
	if constexpr (std::is_same_v<T, bool>) {
		return value ? 1u : 0u;
	} else if constexpr (std::is_integral_v<T>) {
		return static_cast<uint64_t>(static_cast<std::make_unsigned_t<T>>(value));
	} else {
		uint64_t slot = 0;
		std::memcpy(&slot, &value, sizeof(T));
		return slot;
	}
}

} // namespace

#ifdef NAUTILUS_FFI_CLOSURES

namespace {

/// Map a nautilus Type to the libffi type used in the closure signature.
ffi_type* typeToFFIType(Type type) {
	switch (type) {
	case Type::b:
		// C++ bool is one byte; there is no ffi_type_bool.
		return &ffi_type_uint8;
	case Type::i8:
		return &ffi_type_sint8;
	case Type::i16:
		return &ffi_type_sint16;
	case Type::i32:
		return &ffi_type_sint32;
	case Type::i64:
		return &ffi_type_sint64;
	case Type::ui8:
		return &ffi_type_uint8;
	case Type::ui16:
		return &ffi_type_uint16;
	case Type::ui32:
		return &ffi_type_uint32;
	case Type::ui64:
		return &ffi_type_uint64;
	case Type::f32:
		return &ffi_type_float;
	case Type::f64:
		return &ffi_type_double;
	case Type::ptr:
		return &ffi_type_pointer;
	case Type::v:
		return &ffi_type_void;
	}
	return &ffi_type_void;
}

} // namespace

uint64_t ClosureArgs::next(Type type) {
	// libffi passes each argument as a pointer to its native value in args[i].
	void* arg = args_[index_++];
	switch (type) {
	case Type::b:
		// bool is mapped to ffi_type_uint8; read a single byte.
		return slotOf(*static_cast<uint8_t*>(arg) != 0);
	case Type::i8:
		return slotOf(*static_cast<int8_t*>(arg));
	case Type::i16:
		return slotOf(*static_cast<int16_t*>(arg));
	case Type::i32:
		return slotOf(*static_cast<int32_t*>(arg));
	case Type::i64:
		return slotOf(*static_cast<int64_t*>(arg));
	case Type::ui8:
		return slotOf(*static_cast<uint8_t*>(arg));
	case Type::ui16:
		return slotOf(*static_cast<uint16_t*>(arg));
	case Type::ui32:
		return slotOf(*static_cast<uint32_t*>(arg));
	case Type::ui64:
		return slotOf(*static_cast<uint64_t*>(arg));
	case Type::f32:
		return slotOf(*static_cast<float*>(arg));
	case Type::f64:
		return slotOf(*static_cast<double*>(arg));
	case Type::ptr:
		return reinterpret_cast<uint64_t>(*static_cast<void**>(arg));
	case Type::v:
		break;
	}
	return 0;
}

namespace {

/// libffi closure handler: runs the function body and writes the typed result
/// to `ret`.
void ffiHandler(ffi_cif* /*cif*/, void* ret, void** args, void* userdata) {
	auto* closure = static_cast<NativeClosure*>(userdata);
	ClosureArgs cursor(args);
	const uint64_t raw = closure->invokeBody(cursor);

	switch (closure->returnType()) {
	case Type::v:
		return;
	case Type::b:
		// Integral/pointer returns must be written at ffi_arg width; cast through
		// the concrete type first so libffi's truncation yields the right value.
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<bool>(raw));
		return;
	case Type::i8:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<int8_t>(raw));
		return;
	case Type::i16:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<int16_t>(raw));
		return;
	case Type::i32:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<int32_t>(raw));
		return;
	case Type::i64:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<int64_t>(raw));
		return;
	case Type::ui8:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<uint8_t>(raw));
		return;
	case Type::ui16:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<uint16_t>(raw));
		return;
	case Type::ui32:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<uint32_t>(raw));
		return;
	case Type::ui64:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<uint64_t>(raw));
		return;
	case Type::f32: {
		// The result register holds the float's bit pattern; reinterpret, do not
		// numerically convert.
		float value;
		std::memcpy(&value, &raw, sizeof(float));
		*static_cast<float*>(ret) = value;
		return;
	}
	case Type::f64: {
		double value;
		std::memcpy(&value, &raw, sizeof(double));
		*static_cast<double*>(ret) = value;
		return;
	}
	case Type::ptr:
		*static_cast<void**>(ret) = reinterpret_cast<void*>(raw);
		return;
	}
}

} // namespace

NativeClosure::NativeClosure(std::vector<Type> argTypes, Type returnType, ClosureHandler handler, void* userdata)
    : argTypes_(std::move(argTypes)), returnType_(returnType), handler_(handler), userdata_(userdata) {
	// Build a libffi closure with a static-trampoline thunk (no runtime RWX
	// memory). The cif and its argument-type array are members so they outlive
	// the closure.
	argFFITypes_.reserve(argTypes_.size());
	for (auto argType : argTypes_) {
		argFFITypes_.push_back(typeToFFIType(argType));
	}
	void* code = nullptr;
	closure_ = static_cast<ffi_closure*>(ffi_closure_alloc(sizeof(ffi_closure), &code));
	if (closure_ == nullptr) {
		throw RuntimeException("Failed to allocate libffi closure for an interpreted function");
	}
	if (ffi_prep_cif(&cif_, FFI_DEFAULT_ABI, static_cast<unsigned int>(argFFITypes_.size()), typeToFFIType(returnType_),
	                 argFFITypes_.data()) != FFI_OK) {
		ffi_closure_free(closure_);
		throw RuntimeException("Failed to prepare libffi call interface for an interpreted function");
	}
	if (ffi_prep_closure_loc(closure_, &cif_, ffiHandler, this, code) != FFI_OK) {
		ffi_closure_free(closure_);
		throw RuntimeException("Failed to prepare libffi closure for an interpreted function");
	}
	code_ = code;
}

NativeClosure::~NativeClosure() {
	if (closure_ != nullptr) {
		ffi_closure_free(closure_);
	}
}

#else // dyncall callbacks

namespace {

char typeToDCSigChar(Type type) {
	switch (type) {
	case Type::b:
		return DC_SIGCHAR_BOOL;
	case Type::i8:
		return DC_SIGCHAR_CHAR;
	case Type::i16:
		return DC_SIGCHAR_SHORT;
	case Type::i32:
		return DC_SIGCHAR_INT;
	case Type::i64:
		return DC_SIGCHAR_LONG;
	case Type::ui8:
		return DC_SIGCHAR_UCHAR;
	case Type::ui16:
		return DC_SIGCHAR_USHORT;
	case Type::ui32:
		return DC_SIGCHAR_UINT;
	case Type::ui64:
		return DC_SIGCHAR_ULONG;
	case Type::f32:
		return DC_SIGCHAR_FLOAT;
	case Type::f64:
		return DC_SIGCHAR_DOUBLE;
	case Type::ptr:
		return DC_SIGCHAR_POINTER;
	case Type::v:
		return DC_SIGCHAR_VOID;
	}
	return DC_SIGCHAR_VOID;
}

std::string buildDCSignature(const std::vector<Type>& argTypes, Type returnType) {
	std::string sig;
	for (auto t : argTypes) {
		sig += typeToDCSigChar(t);
	}
	sig += DC_SIGCHAR_ENDARG;
	sig += typeToDCSigChar(returnType);
	return sig;
}

} // namespace

uint64_t ClosureArgs::next(Type type) {
	// dyncall reads arguments sequentially from the DCArgs cursor, so callers
	// must consume them in declaration order.
	switch (type) {
	case Type::b:
		// dyncall's dcbArgBool returns DCbool (int); mask to the low byte
		// before converting to bool -- see Dyncall::callB.
		return slotOf((dcbArgBool(args_) & 0xFF) != 0);
	case Type::i8:
		return slotOf(static_cast<int8_t>(dcbArgChar(args_)));
	case Type::i16:
		return slotOf(static_cast<int16_t>(dcbArgShort(args_)));
	case Type::i32:
		return slotOf(static_cast<int32_t>(dcbArgInt(args_)));
	case Type::i64:
		return slotOf(static_cast<int64_t>(dcbArgLong(args_)));
	case Type::ui8:
		return slotOf(static_cast<uint8_t>(dcbArgUChar(args_)));
	case Type::ui16:
		return slotOf(static_cast<uint16_t>(dcbArgUShort(args_)));
	case Type::ui32:
		return slotOf(static_cast<uint32_t>(dcbArgUInt(args_)));
	case Type::ui64:
		return slotOf(static_cast<uint64_t>(dcbArgULong(args_)));
	case Type::f32:
		return slotOf(static_cast<float>(dcbArgFloat(args_)));
	case Type::f64:
		return slotOf(static_cast<double>(dcbArgDouble(args_)));
	case Type::ptr:
		return reinterpret_cast<uint64_t>(dcbArgPointer(args_));
	case Type::v:
		break;
	}
	return 0;
}

namespace {

/// dyncallback handler: runs the function body and writes the typed result.
DCsigchar dcHandler(DCCallback* /*pcb*/, DCArgs* args, DCValue* result, void* userdata) {
	auto* closure = static_cast<NativeClosure*>(userdata);
	ClosureArgs cursor(args);
	const uint64_t raw = closure->invokeBody(cursor);

	// Zero the result union so the full 64-bit register value is clean.
	// The dyncall ARM64 callback thunk loads all 8 bytes into x0 via
	// "ldr x0, [sp, #offset]", so sub-32-bit fields (c, s, C, S) must not leave
	// garbage in the upper bytes.
	result->l = 0;

	switch (closure->returnType()) {
	case Type::b:
		result->l = static_cast<bool>(raw);
		return DC_SIGCHAR_BOOL;
	case Type::i8:
		result->l = static_cast<int8_t>(raw);
		return DC_SIGCHAR_CHAR;
	case Type::i16:
		result->l = static_cast<int16_t>(raw);
		return DC_SIGCHAR_SHORT;
	case Type::i32:
		result->l = static_cast<int32_t>(raw);
		return DC_SIGCHAR_INT;
	case Type::i64:
		result->l = static_cast<int64_t>(raw);
		return DC_SIGCHAR_LONG;
	case Type::ui8:
		result->l = static_cast<uint8_t>(raw);
		return DC_SIGCHAR_UCHAR;
	case Type::ui16:
		result->l = static_cast<uint16_t>(raw);
		return DC_SIGCHAR_USHORT;
	case Type::ui32:
		result->l = static_cast<uint32_t>(raw);
		return DC_SIGCHAR_UINT;
	case Type::ui64:
		result->l = static_cast<uint64_t>(raw);
		return DC_SIGCHAR_ULONG;
	case Type::f32: {
		// The result register holds the float's bit pattern; reinterpret, do not
		// numerically convert.
		float value;
		std::memcpy(&value, &raw, sizeof(float));
		result->f = value;
		return DC_SIGCHAR_FLOAT;
	}
	case Type::f64: {
		double value;
		std::memcpy(&value, &raw, sizeof(double));
		result->d = value;
		return DC_SIGCHAR_DOUBLE;
	}
	case Type::ptr:
		result->p = reinterpret_cast<void*>(raw);
		return DC_SIGCHAR_POINTER;
	case Type::v:
		return DC_SIGCHAR_VOID;
	}
	return DC_SIGCHAR_VOID;
}

} // namespace

NativeClosure::NativeClosure(std::vector<Type> argTypes, Type returnType, ClosureHandler handler, void* userdata)
    : argTypes_(std::move(argTypes)), returnType_(returnType), handler_(handler), userdata_(userdata) {
	const auto sig = buildDCSignature(argTypes_, returnType_);
	callback_ = dcbNewCallback(sig.c_str(), dcHandler, this);
	if (callback_ == nullptr) {
		throw RuntimeException("Failed to allocate a dyncall callback for an interpreted function");
	}
	code_ = reinterpret_cast<void*>(callback_);
}

NativeClosure::~NativeClosure() {
	if (callback_ != nullptr) {
		dcbFreeCallback(callback_);
	}
}

#endif

} // namespace nautilus::compiler
