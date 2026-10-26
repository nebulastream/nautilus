#include "nautilus/nir/Inst.hpp"
#include <array>

namespace nautilus::nir {

namespace {

struct Table {
	std::array<OpInfo, FIRST_EXTENSION_OP> info {};
	std::array<bool, FIRST_EXTENSION_OP> valid {};
	constexpr Table() {
#define NIR_OP(name, code, text, a, b, c, flags)                                                                       \
	info[code] = OpInfo {text, OperandKind::a, OperandKind::b, OperandKind::c, static_cast<uint8_t>(flags)};           \
	valid[code] = true;
#include "nautilus/nir/Opcodes.def"
#undef NIR_OP
	}
};

constexpr Table table;

} // namespace

const OpInfo* coreOpInfo(uint16_t op) {
	if (op >= FIRST_EXTENSION_OP || !table.valid[op]) {
		return nullptr;
	}
	return &table.info[op];
}

} // namespace nautilus::nir
