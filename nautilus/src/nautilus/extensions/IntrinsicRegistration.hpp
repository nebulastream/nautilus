#pragma once

#include "nautilus/common/ExceptionTransport.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include <cstdint>
#include <string>
#include <type_traits>

namespace nautilus::ext {

/// Attribute bits of 13-nir-spec.md §4.6.2.
enum IntrinsicAttr : uint32_t {
	AttrNoArgRead = 1u << 0,
	AttrNoArgWrite = 1u << 1,
	AttrNoOtherRead = 1u << 2,
	AttrNoOtherWrite = 1u << 3,
	AttrNoUnwind = 1u << 4,
	AttrWillReturn = 1u << 5,
};

namespace detail {

/// The type word of a C++ type at a native boundary (13 §4.5.1): narrow integers carry the extension their signedness
/// implies, which is what the native calling convention of the supported targets does.
template <class T>
constexpr nir::TypeWord typeWordOf() {
	using U = std::remove_cv_t<T>;
	if constexpr (std::is_void_v<U>) {
		return nir::makeTypeWord(nir::TypeId::Void);
	} else if constexpr (std::is_same_v<U, bool>) {
		return nir::makeTypeWord(nir::TypeId::Bool, nir::ExtKind::Zext);
	} else if constexpr (std::is_pointer_v<U> || std::is_null_pointer_v<U>) {
		return nir::makeTypeWord(nir::TypeId::Ptr);
	} else if constexpr (std::is_same_v<U, float>) {
		return nir::makeTypeWord(nir::TypeId::F32);
	} else if constexpr (std::is_same_v<U, double>) {
		return nir::makeTypeWord(nir::TypeId::F64);
	} else if constexpr (std::is_integral_v<U> || std::is_enum_v<U>) {
		constexpr auto ext = std::is_signed_v<U> ? nir::ExtKind::Sext : nir::ExtKind::Zext;
		switch (sizeof(U)) {
		case 1:
			return nir::makeTypeWord(nir::TypeId::I8, ext);
		case 2:
			return nir::makeTypeWord(nir::TypeId::I16, ext);
		case 4:
			return nir::makeTypeWord(nir::TypeId::I32, ext);
		default:
			return nir::makeTypeWord(nir::TypeId::I64);
		}
	} else {
		static_assert(!sizeof(U), "no NIR type for this C++ type; register it as a pointer or spell the signature");
	}
}

/// `NoUnwind` is a template parameter so that a noexcept function does not instantiate the capture thunk, which needs
/// the runtime's exception transport (the core registers its intrinsics from the library that lacks it).
template <bool NoUnwind, class R, class... Args>
IntrinsicInfo makeIntrinsic(std::string name, R (*fn)(Args...), uint32_t attrs) {
	IntrinsicInfo info;
	info.name = std::move(name);
	info.signature = {typeWordOf<R>(), typeWordOf<Args>()...};
	info.attrs = attrs | (NoUnwind ? uint32_t(AttrNoUnwind) : 0u);
	info.fallback = reinterpret_cast<void*>(fn);
	// a function that may throw is called through a capture thunk, which keeps the exception from crossing a frame
	// without unwind tables (04 §3.3)
	if constexpr (!NoUnwind) {
		info.captureThunk = reinterpret_cast<void*>(&compiler::captureThrowingCall<R, Args...>);
	}
	return info;
}

} // namespace detail

/// Registers `fn` as the intrinsic `name`, deriving the NIR signature from its C++ type. The fallback is `fn` itself;
/// a function whose type is not `noexcept` and whose attributes lack `AttrNoUnwind` gets a `captureThrowingCall` thunk.
/// (A C library function is `noexcept` only on some platforms; pass `AttrNoUnwind` to state it.)
template <class R, class... Args>
uint32_t registerIntrinsicFor(std::string name, R (*fn)(Args...), uint32_t attrs = 0, Owner owner = Owner::Plugin) {
	auto info = (attrs & AttrNoUnwind) != 0 ? detail::makeIntrinsic<true>(std::move(name), fn, attrs)
	                                         : detail::makeIntrinsic<false>(std::move(name), fn, attrs);
	return ExtensionRegistry::instance().registerIntrinsic(std::move(info), owner);
}

template <class R, class... Args>
uint32_t registerIntrinsicFor(std::string name, R (*fn)(Args...) noexcept, uint32_t attrs = 0,
                              Owner owner = Owner::Plugin) {
	return ExtensionRegistry::instance().registerIntrinsic(
	    detail::makeIntrinsic<true>(std::move(name), static_cast<R (*)(Args...)>(fn), attrs), owner);
}

} // namespace nautilus::ext
