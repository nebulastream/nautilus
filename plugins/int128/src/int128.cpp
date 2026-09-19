#include <cstring>
#include <nautilus/config.hpp>
#include <nautilus/int128.hpp>

#ifdef ENABLE_MLIR_BACKEND
#include "MLIRInt128Intrinsics.hpp"
namespace {
struct Int128IntrinsicRegistrar {
	Int128IntrinsicRegistrar() {
		nautilus::compiler::mlir::RegisterMLIRInt128IntrinsicPlugin();
	}
};
static Int128IntrinsicRegistrar registrar;
} // namespace
#endif

namespace nautilus::detail {
namespace {
__extension__ typedef __int128 native_int;
__extension__ typedef unsigned __int128 native_uint;
native_int load(const int128_data* p) {
	return static_cast<native_int>((static_cast<native_uint>(static_cast<uint64_t>(p->high)) << 64) | p->low);
}
int128_data* save(native_int value) {
	static thread_local int128_data values[64];
	static thread_local unsigned next;
	auto* out = &values[next++ % 64];
	out->low = static_cast<uint64_t>(value);
	out->high = static_cast<int64_t>(value >> 64);
	return out;
}
} // namespace

int128_data* int128_make_impl(uint64_t l, int64_t h) {
	return save(static_cast<native_int>((static_cast<native_uint>(static_cast<uint64_t>(h)) << 64) | l));
}
int128_data* int128_add_impl(int128_data* a, int128_data* b) {
	return save(static_cast<native_int>(static_cast<native_uint>(load(a)) + static_cast<native_uint>(load(b))));
}
int128_data* int128_sub_impl(int128_data* a, int128_data* b) {
	return save(static_cast<native_int>(static_cast<native_uint>(load(a)) - static_cast<native_uint>(load(b))));
}
int128_data* int128_mul_impl(int128_data* a, int128_data* b) {
	return save(static_cast<native_int>(static_cast<native_uint>(load(a)) * static_cast<native_uint>(load(b))));
}
#define BINARY(NAME, OP)                                                                                               \
	int128_data* NAME(int128_data* a, int128_data* b) {                                                                \
		return save(load(a) OP load(b));                                                                               \
	}
BINARY(int128_div_impl, /)
BINARY(int128_rem_impl, %)
BINARY(int128_and_impl, &)
BINARY(int128_or_impl, |)
BINARY(int128_xor_impl, ^)
#undef BINARY
int128_data* int128_neg_impl(int128_data* a) {
	return save(static_cast<native_int>(-static_cast<native_uint>(load(a))));
}
int128_data* int128_not_impl(int128_data* a) {
	return save(~load(a));
}
int128_data* int128_shl_impl(int128_data* a, uint32_t n) {
	return save(static_cast<native_int>(static_cast<native_uint>(load(a)) << (n & 127)));
}
int128_data* int128_shr_impl(int128_data* a, uint32_t n) {
	return save(load(a) >> (n & 127));
}
bool int128_eq_impl(int128_data* a, int128_data* b) {
	return load(a) == load(b);
}
bool int128_lt_impl(int128_data* a, int128_data* b) {
	return load(a) < load(b);
}
uint64_t int128_low_impl(int128_data* a) {
	return a->low;
}
int64_t int128_high_impl(int128_data* a) {
	return a->high;
}
int128_data* int128_load_impl(const void* ptr) {
	int128_data value;
	std::memcpy(&value, ptr, sizeof(value));
	return save(load(&value));
}
void int128_store_impl(void* ptr, int128_data* value) {
	std::memcpy(ptr, value, sizeof(*value));
}
} // namespace nautilus::detail
