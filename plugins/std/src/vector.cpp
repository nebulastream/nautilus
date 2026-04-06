#include "nautilus/std/vector.h"
#include "nautilus/function.hpp"

namespace nautilus {
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
} // namespace nautilus
