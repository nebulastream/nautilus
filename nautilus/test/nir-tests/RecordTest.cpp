#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Types.hpp"
#include <catch2/catch_all.hpp>
#include <set>

using namespace nautilus::nir;

TEST_CASE("value references", "[nir][inst]") {
	REQUIRE(isValueRef(0));
	REQUIRE(isValueRef(0x7FFFFFFE));
	REQUIRE_FALSE(isValueRef(NONE));
	REQUIRE(isConstRef(makeConstRef(0)));
	REQUIRE(isConstRef(makeConstRef(5)));
	REQUIRE(constIndexOf(makeConstRef(5)) == 5);
	REQUIRE_FALSE(isConstRef(NONE));
	REQUIRE(isReservedRef(0x7FFFFFFF));
	REQUIRE(isReservedRef(0xFFFFFFFE));
	REQUIRE_FALSE(isConstRef(0xFFFFFFFE));
}

TEST_CASE("type words", "[nir][types]") {
	REQUIRE(isValidTypeWord(makeTypeWord(TypeId::Void)));
	REQUIRE(isValidTypeWord(makeTypeWord(TypeId::Bool, ExtKind::Zext)));
	REQUIRE_FALSE(isValidTypeWord(makeTypeWord(TypeId::Bool)));
	REQUIRE_FALSE(isValidTypeWord(makeTypeWord(TypeId::I8)));
	REQUIRE(isValidTypeWord(makeTypeWord(TypeId::I8, ExtKind::Sext)));
	REQUIRE(isValidTypeWord(makeTypeWord(TypeId::I32)));
	REQUIRE(isValidTypeWord(makeTypeWord(TypeId::I32, ExtKind::Zext)));
	REQUIRE_FALSE(isValidTypeWord(makeTypeWord(TypeId::I64, ExtKind::Zext)));
	REQUIRE_FALSE(isValidTypeWord(makeTypeWord(TypeId::Ptr, ExtKind::Sext)));
	REQUIRE_FALSE(isValidTypeWord(makeTypeWord(TypeId::I16) | (3u << 8)));
	REQUIRE_FALSE(isValidTypeWord(makeTypeWord(TypeId::I32) | (1u << 10)));
	REQUIRE(typeOf(makeTypeWord(TypeId::I16, ExtKind::Zext)) == TypeId::I16);
	REQUIRE(extKindOf(makeTypeWord(TypeId::I16, ExtKind::Zext)) == ExtKind::Zext);
}

TEST_CASE("type properties", "[nir][types]") {
	REQUIRE(sizeOf(TypeId::Bool) == 1);
	REQUIRE(sizeOf(TypeId::I16) == 2);
	REQUIRE(sizeOf(TypeId::F32) == 4);
	REQUIRE(sizeOf(TypeId::Ptr) == 8);
	REQUIRE(sizeOf(TypeId::Void) == 0);
	REQUIRE(isInteger(TypeId::I8));
	REQUIRE_FALSE(isInteger(TypeId::Bool));
	REQUIRE(isFloat(TypeId::F64));
	REQUIRE(isScalar(TypeId::Ptr));
	REQUIRE_FALSE(isScalar(TypeId::Void));
	REQUIRE(isExtension(static_cast<TypeId>(16)));
	REQUIRE_FALSE(isExtension(TypeId::Ptr));
}

TEST_CASE("opcode table", "[nir][opcodes]") {
	std::set<uint16_t> seen;
	int count = 0;
#define NIR_OP(OPNAME, CODE, TEXT, KA, KB, KC, FL)                                                                       \
	{                                                                                                                  \
		REQUIRE(seen.insert(CODE).second);                                                                             \
		REQUIRE(static_cast<uint16_t>(Op::OPNAME) == CODE);                                                              \
		auto* info = coreOpInfo(CODE);                                                                                 \
		REQUIRE(info != nullptr);                                                                                      \
		REQUIRE(std::string(info->name) == TEXT);                                                                      \
		REQUIRE(info->a == OperandKind::KA);                                                                            \
		++count;                                                                                                       \
	}
#include "nautilus/nir/Opcodes.def"
#undef NIR_OP
	REQUIRE(count == 34);
	REQUIRE(coreOpInfo(0) == nullptr);
	REQUIRE(coreOpInfo(7) == nullptr);
	REQUIRE(coreOpInfo(256) == nullptr);
	REQUIRE((coreOpInfo(static_cast<uint16_t>(Op::Br))->flags & OpTerminator) != 0);
	REQUIRE((coreOpInfo(static_cast<uint16_t>(Op::Var))->flags & OpPreSSAOnly) != 0);
}
