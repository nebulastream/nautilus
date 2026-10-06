#include "nautilus/compiler/backends/mlir/ExceptionPersonality.hpp"

#if defined(__linux__) && __has_include(<unwind.h>) && !defined(__arm__) && !defined(__USING_SJLJ_EXCEPTIONS__)
#include <type_traits>
#include <unwind.h>

extern "C" std::remove_pointer_t<_Unwind_Personality_Fn> __gxx_personality_v0;

namespace nautilus::compiler::mlir {
namespace {

_Unwind_Reason_Code forwardCxxPersonality(int version, _Unwind_Action actions, _Unwind_Exception_Class exceptionClass,
                                          _Unwind_Exception* exception, _Unwind_Context* context) {
	return __gxx_personality_v0(version, actions, exceptionClass, exception, context);
}

[[noreturn]] void forwardUnwindResume(_Unwind_Exception* exception) {
	_Unwind_Resume(exception);
}

static_assert(std::is_same_v<decltype(&forwardCxxPersonality), _Unwind_Personality_Fn>);

} // namespace

void* getExceptionPersonalityAddress() {
	return reinterpret_cast<void*>(&forwardCxxPersonality);
}

void* getUnwindResumeAddress() {
	return reinterpret_cast<void*>(&forwardUnwindResume);
}

} // namespace nautilus::compiler::mlir
#else
#include "nautilus/exceptions/RuntimeException.hpp"
#include <llvm/Support/DynamicLibrary.h>

namespace nautilus::compiler::mlir {

void* getExceptionPersonalityAddress() {
	auto process = llvm::sys::DynamicLibrary::getPermanentLibrary(nullptr);
	auto* personality = process.getAddressOfSymbol("__gxx_personality_v0");
	if (personality == nullptr) {
		throw RuntimeException("Could not resolve C++ exception personality __gxx_personality_v0");
	}
	return personality;
}

void* getUnwindResumeAddress() {
	auto process = llvm::sys::DynamicLibrary::getPermanentLibrary(nullptr);
	auto* resume = process.getAddressOfSymbol("_Unwind_Resume");
	if (resume == nullptr) {
		throw RuntimeException("Could not resolve C++ unwind resume _Unwind_Resume");
	}
	return resume;
}

} // namespace nautilus::compiler::mlir
#endif
