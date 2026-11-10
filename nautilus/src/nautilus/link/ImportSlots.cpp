#include "nautilus/link/ImportSlots.hpp"

namespace nautilus::link {

ImportSlots::ImportSlots(size_t count) : count_(count), slots_(count == 0 ? nullptr : new Slot[count]) {
}

} // namespace nautilus::link
