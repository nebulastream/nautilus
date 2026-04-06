#include "nautilus/std/vector.h"
#include "nautilus/function.hpp"

namespace nautilus {
// Fundamental element types
template class val<std::vector<int8_t>>;
template class val<std::vector<int16_t>>;
template class val<std::vector<int32_t>>;
template class val<std::vector<int64_t>>;
template class val<std::vector<uint8_t>>;
template class val<std::vector<uint16_t>>;
template class val<std::vector<uint32_t>>;
template class val<std::vector<uint64_t>>;
template class val<std::vector<float>>;
template class val<std::vector<double>>;

// Pointer element types
template class val<std::vector<int8_t*>>;
template class val<std::vector<int32_t*>>;
template class val<std::vector<int64_t*>>;
template class val<std::vector<float*>>;
template class val<std::vector<double*>>;
template class val<std::vector<void*>>;
} // namespace nautilus
