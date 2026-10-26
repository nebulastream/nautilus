#include "nautilus/nir/ConstPool.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus::nir;

TEST_CASE("constants are interned by type, provenance and value", "[nir][constpool]") {
	ConstPool pool;
	auto a = pool.intern(TypeId::I32, Provenance::Literal, 7);
	REQUIRE(isConstRef(a));
	REQUIRE(pool.intern(TypeId::I32, Provenance::Literal, 7) == a);
	REQUIRE(pool.intern(TypeId::I64, Provenance::Literal, 7) != a);
	REQUIRE(pool.intern(TypeId::I32, Provenance::AuditedInvariant, 7) != a);
	REQUIRE(pool.intern(TypeId::I32, Provenance::Literal, 8) != a);
	REQUIRE(pool.size() == 4);
	REQUIRE(pool.record(a).bits == 7);
	REQUIRE(pool.record(a).type == static_cast<uint8_t>(TypeId::I32));
}

TEST_CASE("floats are stored by bit pattern", "[nir][constpool]") {
	ConstPool pool;
	auto pz = pool.intern(TypeId::F64, Provenance::Literal, 0);
	auto nz = pool.intern(TypeId::F64, Provenance::Literal, 0x8000000000000000ull);
	REQUIRE(pz != nz);
}

TEST_CASE("wide constants live in constBytes and are interned by content", "[nir][constpool]") {
	ConstPool pool;
	std::byte x[16], y[16];
	for (int i = 0; i < 16; ++i) {
		x[i] = static_cast<std::byte>(i);
		y[i] = static_cast<std::byte>(i);
	}
	auto a = pool.internBytes(TypeId::FirstExtension, Provenance::Literal, x);
	REQUIRE(pool.internBytes(TypeId::FirstExtension, Provenance::Literal, y) == a);
	y[15] = std::byte {99};
	auto b = pool.internBytes(TypeId::FirstExtension, Provenance::Literal, y);
	REQUIRE(b != a);
	REQUIRE(pool.record(a).byteOffset == 0);
	REQUIRE(pool.record(b).byteOffset == 16);
	REQUIRE(pool.bytes().size() == 32);
	REQUIRE(pool.wideBytes(pool.record(b), 16)[15] == std::byte {99});
}

TEST_CASE("provenance order and attribute implication", "[nir][records]") {
	REQUIRE(leastTrusted(Provenance::Literal, Provenance::AuditedInvariant) == Provenance::AuditedInvariant);
	REQUIRE(leastTrusted(Provenance::RawProcessValue, Provenance::Literal) == Provenance::RawProcessValue);
	REQUIRE(attrsImply(AttrNoUnwind | AttrWillReturn, AttrNoUnwind));
	REQUIRE_FALSE(attrsImply(AttrNoUnwind, AttrNoUnwind | AttrWillReturn));
	REQUIRE(attrsImply(0, AttrDeclared));
	REQUIRE(attrsImply(0, 0));
}
