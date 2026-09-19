#pragma once

#include <cstdint>
#include <nautilus/function.hpp>
#include <nautilus/val.hpp>

namespace nautilus {

namespace detail {
struct int128_data {
	uint64_t low;
	int64_t high;
};

int128_data* int128_make_impl(uint64_t low, int64_t high);
int128_data* int128_add_impl(int128_data*, int128_data*);
int128_data* int128_sub_impl(int128_data*, int128_data*);
int128_data* int128_mul_impl(int128_data*, int128_data*);
int128_data* int128_div_impl(int128_data*, int128_data*);
int128_data* int128_rem_impl(int128_data*, int128_data*);
int128_data* int128_and_impl(int128_data*, int128_data*);
int128_data* int128_or_impl(int128_data*, int128_data*);
int128_data* int128_xor_impl(int128_data*, int128_data*);
int128_data* int128_neg_impl(int128_data*);
int128_data* int128_not_impl(int128_data*);
int128_data* int128_shl_impl(int128_data*, uint32_t);
int128_data* int128_shr_impl(int128_data*, uint32_t);
bool int128_eq_impl(int128_data*, int128_data*);
bool int128_lt_impl(int128_data*, int128_data*);
uint64_t int128_low_impl(int128_data*);
int64_t int128_high_impl(int128_data*);
int128_data* int128_load_impl(const void*);
void int128_store_impl(void*, int128_data*);
} // namespace detail

/// A first-class traceable signed 128-bit integer as a `val<detail::int128_t>`.
///
/// The value travels through the Nautilus IR as an opaque pointer. Every
/// operation is recorded as a traced `invoke` to a runtime helper in `detail`;
/// when the MLIR backend is enabled its intrinsic plugin replaces those calls
/// with native LLVM `i128` operations. Scalar backends (C++, BC, TBC, AsmJit)
/// fall back to the portable runtime helpers.
template <>
class val<detail::int128_t> : public val_base {
public:
	using storage_type = detail::int128_data;
	using basic_type = detail::int128_t;

	/// Wraps an existing boxed value (a runtime helper result).
	val(val<storage_type*> box) : box_(box) {
	}

	/// Constructs from explicit low and high halves.
	val(val<uint64_t> low, val<int64_t> high = 0) : val(invoke(detail::int128_make_impl, low, high)) {
	}

	/// Constructs from a raw host `__int128`.
	val(detail::int128_t value) : val((uint64_t) value, (int64_t) (value >> 64)) {
	}

	/// Constructs from a raw host `unsigned __int128`. The low/high halves are
	/// taken as the exact bit pattern; values above `INT128_MAX` wrap to their
	/// two's-complement representation, as with any signed 128-bit carrier.
	/// Constrained to the exact unsigned 128-bit type so ordinary integer
	/// literals (`I128(2)`) keep resolving to the signed constructor.
	template <typename T>
	    requires std::is_same_v<std::remove_cvref_t<T>, detail::uint128_t>
	val(T value) : val((uint64_t) value, (int64_t) (value >> 64)) {
	}

	/// Constructs from a narrower signed/unsigned integral `val<T>`, sign
	/// extending signed inputs into the 128-bit domain.
	template <typename T>
	    requires(std::integral<T> && !std::same_as<T, bool> && sizeof(T) <= sizeof(uint64_t))
	val(val<T> value)
	    : val(static_cast<val<uint64_t>>(value), [](val<T> v) -> val<int64_t> {
		      if constexpr (std::signed_integral<T>) {
			      return static_cast<val<int64_t>>(v) >> 63;
		      }
		      return 0;
	      }(value)) {
	}

	/// Loads 16 bytes without imposing an alignment requirement on ptr.
	static val<detail::int128_t> Load(val<const void*> ptr) {
		return val<detail::int128_t>(invoke(detail::int128_load_impl, ptr));
	}
	/// Stores 16 bytes without imposing an alignment requirement on ptr.
	void Store(val<void*> ptr) const {
		invoke(detail::int128_store_impl, ptr, box_);
	}

	val<uint64_t> low() const {
		return invoke(detail::int128_low_impl, box_);
	}
	val<int64_t> high() const {
		return invoke(detail::int128_high_impl, box_);
	}
	/// Exposes the underlying opaque boxed pointer.
	val<storage_type*> data() const {
		return box_;
	}

	template <typename T>
	    requires(std::integral<T> && !std::same_as<T, bool> && sizeof(T) <= sizeof(uint64_t))
	explicit operator val<T>() const {
		return static_cast<val<T>>(static_cast<val<uint64_t>>(low()));
	}
	explicit operator val<bool>() const {
		return (low() != val<uint64_t>(0)) || (high() != val<int64_t>(0));
	}

	val<detail::int128_t> operator-() const {
		return val<detail::int128_t>(invoke(detail::int128_neg_impl, box_));
	}
	val<detail::int128_t> operator<<(val<uint32_t> amount) const {
		return val<detail::int128_t>(invoke(detail::int128_shl_impl, box_, amount));
	}
	val<detail::int128_t> operator>>(val<uint32_t> amount) const {
		return val<detail::int128_t>(invoke(detail::int128_shr_impl, box_, amount));
	}

	[[nodiscard]] Type getType() const override {
		return Type::ptr;
	}
	[[nodiscard]] TypeId getTypeId() const override {
		return typeIdOf<val<detail::int128_t>>();
	}
#ifdef ENABLE_TRACING
	[[nodiscard]] tracing::TypedValueRef getState() const override {
		return box_.getState();
	}
#endif

private:
	val<storage_type*> box_;
};

// binary arithmetic operators (free, preferred over the val_arith generic)
#define NAUTILUS_INT128_FREE_BINARY(OP, FN)                                                                            \
	inline val<detail::int128_t> operator OP(const val<detail::int128_t>& lhs, const val<detail::int128_t>& rhs) {     \
		return val<detail::int128_t>(invoke(detail::FN, lhs.data(), rhs.data()));                                      \
	}
NAUTILUS_INT128_FREE_BINARY(+, int128_add_impl)
NAUTILUS_INT128_FREE_BINARY(-, int128_sub_impl)
NAUTILUS_INT128_FREE_BINARY(*, int128_mul_impl)
NAUTILUS_INT128_FREE_BINARY(/, int128_div_impl)
NAUTILUS_INT128_FREE_BINARY(%, int128_rem_impl)
NAUTILUS_INT128_FREE_BINARY(&, int128_and_impl)
NAUTILUS_INT128_FREE_BINARY(|, int128_or_impl)
NAUTILUS_INT128_FREE_BINARY(^, int128_xor_impl)
#undef NAUTILUS_INT128_FREE_BINARY

// bitwise NOT
inline val<detail::int128_t> operator~(const val<detail::int128_t>& value) {
	return val<detail::int128_t>(invoke(detail::int128_not_impl, value.data()));
}

// comparisons
inline val<bool> operator==(const val<detail::int128_t>& lhs, const val<detail::int128_t>& rhs) {
	return invoke(detail::int128_eq_impl, lhs.data(), rhs.data());
}
inline val<bool> operator!=(const val<detail::int128_t>& lhs, const val<detail::int128_t>& rhs) {
	return !(lhs == rhs);
}
inline val<bool> operator<(const val<detail::int128_t>& lhs, const val<detail::int128_t>& rhs) {
	return invoke(detail::int128_lt_impl, lhs.data(), rhs.data());
}
inline val<bool> operator>(const val<detail::int128_t>& lhs, const val<detail::int128_t>& rhs) {
	return rhs < lhs;
}
inline val<bool> operator<=(const val<detail::int128_t>& lhs, const val<detail::int128_t>& rhs) {
	return !(lhs > rhs);
}
inline val<bool> operator>=(const val<detail::int128_t>& lhs, const val<detail::int128_t>& rhs) {
	return !(lhs < rhs);
}

} // namespace nautilus