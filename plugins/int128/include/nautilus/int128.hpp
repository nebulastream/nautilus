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

/// A traceable signed 128-bit integer. Values are opaque pointers in Nautilus
/// IR: scalar backends call the portable runtime helpers, while the MLIR
/// plugin replaces those calls with native i128 operations.
class int128 {
public:
	using storage_type = detail::int128_data;
	explicit int128(val<storage_type*> value) : value_(value) {
	}
	int128(val<uint64_t> low, val<int64_t> high = 0) : value_(invoke(detail::int128_make_impl, low, high)) {
	}
	int128(int64_t value) : int128(static_cast<uint64_t>(value), value < 0 ? -1 : 0) {
	}
	template <typename T>
	    requires(std::integral<T> && !std::same_as<T, bool> && sizeof(T) <= sizeof(uint64_t))
	explicit int128(val<T> value)
	    : int128(static_cast<val<uint64_t>>(value), [](val<T> v) -> val<int64_t> {
		      if constexpr (std::signed_integral<T>) {
			      return static_cast<val<int64_t>>(v) >> 63;
		      }
		      return 0;
	      }(value)) {
	}

	/// Loads 16 bytes without imposing an alignment requirement on ptr.
	static int128 Load(val<const void*> ptr) {
		return int128(invoke(detail::int128_load_impl, ptr));
	}
	/// Stores 16 bytes without imposing an alignment requirement on ptr.
	void Store(val<void*> ptr) const {
		invoke(detail::int128_store_impl, ptr, value_);
	}

	val<uint64_t> low() const {
		return invoke(detail::int128_low_impl, value_);
	}
	val<int64_t> high() const {
		return invoke(detail::int128_high_impl, value_);
	}
	val<storage_type*> data() const {
		return value_;
	}
	template <typename T>
	    requires(std::integral<T> && !std::same_as<T, bool> && sizeof(T) <= sizeof(uint64_t))
	explicit operator val<T>() const {
		return static_cast<val<T>>(low());
	}
	explicit operator val<bool>() const {
		return *this != int128(0);
	}

#define NAUTILUS_INT128_BINARY(OP, FN)                                                                                 \
	int128 operator OP(const int128& rhs) const {                                                                      \
		return int128(invoke(detail::FN, value_, rhs.value_));                                                         \
	}
	NAUTILUS_INT128_BINARY(+, int128_add_impl)
	NAUTILUS_INT128_BINARY(-, int128_sub_impl)
	NAUTILUS_INT128_BINARY(*, int128_mul_impl)
	NAUTILUS_INT128_BINARY(/, int128_div_impl)
	NAUTILUS_INT128_BINARY(%, int128_rem_impl)
	NAUTILUS_INT128_BINARY(&, int128_and_impl)
	NAUTILUS_INT128_BINARY(|, int128_or_impl)
	NAUTILUS_INT128_BINARY(^, int128_xor_impl)
#undef NAUTILUS_INT128_BINARY
	int128 operator-() const {
		return int128(invoke(detail::int128_neg_impl, value_));
	}
	int128 operator~() const {
		return int128(invoke(detail::int128_not_impl, value_));
	}
	int128 operator<<(val<uint32_t> amount) const {
		return int128(invoke(detail::int128_shl_impl, value_, amount));
	}
	int128 operator>>(val<uint32_t> amount) const {
		return int128(invoke(detail::int128_shr_impl, value_, amount));
	}
	val<bool> operator==(const int128& rhs) const {
		return invoke(detail::int128_eq_impl, value_, rhs.value_);
	}
	val<bool> operator!=(const int128& rhs) const {
		return !(*this == rhs);
	}
	val<bool> operator<(const int128& rhs) const {
		return invoke(detail::int128_lt_impl, value_, rhs.value_);
	}
	val<bool> operator>(const int128& rhs) const {
		return rhs < *this;
	}
	val<bool> operator<=(const int128& rhs) const {
		return !(*this > rhs);
	}
	val<bool> operator>=(const int128& rhs) const {
		return !(*this < rhs);
	}

private:
	val<storage_type*> value_;
};

} // namespace nautilus
