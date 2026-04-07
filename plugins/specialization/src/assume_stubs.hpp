

#include <cstdint>

namespace nautilus {
void nautlis_assume_stub(bool);
void nautilus_assume_aligned_stub(void*, int);
void nautilus_assume_constant_stub(int64_t, int64_t);
void nautilus_assume_range_stub(int64_t, int64_t, int64_t);
void nautilus_assume_nonzero_stub(int64_t);
} // namespace nautilus
