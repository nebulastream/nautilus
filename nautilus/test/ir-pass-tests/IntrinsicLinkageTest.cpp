#include "nautilus/compiler/ir/FunctionTable.hpp"
#include "nautilus/compiler/ir/IntrinsicRegistry.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <catch2/catch_all.hpp>

namespace nautilus::testing {

namespace {

using compiler::ir::CalleeDescriptor;
using compiler::ir::FunctionTable;
using compiler::ir::IntrinsicId;
using compiler::ir::IntrinsicRegistry;
using compiler::ir::Linkage;

/// Stands in for an intrinsic's native implementation. It is never called
/// here, but it is a real function with a real address -- which is the whole
/// invariant under test.
int32_t fakeIntrinsicImplementation(int32_t value) {
	return value;
}

int32_t plainExternalFunction(int32_t value) {
	return value + 1;
}

CalleeDescriptor externalDescriptor(void* address, std::string name) {
	CalleeDescriptor descriptor;
	descriptor.kind = CalleeDescriptor::Kind::External;
	descriptor.key = address;
	descriptor.demangledName = std::move(name);
	descriptor.resultType = Type::i32;
	descriptor.paramTypes = {Type::i32};
	return descriptor;
}

} // namespace

TEST_CASE("IntrinsicRegistry: an address gets one id, however often it is claimed") {
	auto& registry = IntrinsicRegistry::instance();
	auto* address = reinterpret_cast<void*>(&fakeIntrinsicImplementation);

	const auto first = registry.registerIntrinsic(address, "fake.identity");
	REQUIRE(first != IntrinsicId::None);

	// Two backends each claiming the same intrinsic must agree on its id --
	// that is what lets the IR name one linkage that all of them read.
	REQUIRE(registry.registerIntrinsic(address, "fake.identity") == first);
	REQUIRE(registry.lookup(address) == first);
	REQUIRE(registry.getName(first) == "fake.identity");
}

TEST_CASE("IntrinsicRegistry: an unregistered address is not an intrinsic") {
	REQUIRE(IntrinsicRegistry::instance().lookup(reinterpret_cast<void*>(&plainExternalFunction)) == IntrinsicId::None);
	REQUIRE(IntrinsicRegistry::instance().lookup(nullptr) == IntrinsicId::None);
	REQUIRE(IntrinsicRegistry::instance().getName(IntrinsicId::None).empty());
}

TEST_CASE("IntrinsicRegistry: an intrinsic without a native implementation is rejected") {
	// The fallback invariant, at the only place it can be enforced. A backend
	// with no handler emits an ordinary call to the address; a null one would
	// surface as a jump through a null pointer rather than as the
	// registration bug it is.
	REQUIRE_THROWS_AS(IntrinsicRegistry::instance().registerIntrinsic(nullptr, "no.implementation"), RuntimeException);
}

TEST_CASE("FunctionTable: interning a registered address yields Intrinsic linkage") {
	auto* address = reinterpret_cast<void*>(&fakeIntrinsicImplementation);
	const auto id = IntrinsicRegistry::instance().registerIntrinsic(address, "fake.identity");

	FunctionTable table;
	const auto functionId = table.intern(externalDescriptor(address, "fakeIntrinsic"));
	const auto& target = table.get(functionId);

	REQUIRE(target.getLinkage() == Linkage::Intrinsic);
	REQUIRE(target.getIntrinsic() == id);
	// An intrinsic keeps its address. This is what lets the C++, bytecode and
	// threaded-bytecode backends support every intrinsic without implementing
	// one: they ignore getIntrinsic() and call getAddress().
	REQUIRE(target.getAddress() == address);
	REQUIRE(target.getName().get() == "fakeIntrinsic");
}

TEST_CASE("FunctionTable: an ordinary native callee stays External") {
	auto* address = reinterpret_cast<void*>(&plainExternalFunction);

	FunctionTable table;
	const auto functionId = table.intern(externalDescriptor(address, "plainExternal"));
	const auto& target = table.get(functionId);

	REQUIRE(target.getLinkage() == Linkage::External);
	REQUIRE(target.getIntrinsic() == IntrinsicId::None);
	REQUIRE(target.getAddress() == address);
}

TEST_CASE("FunctionTable: an internal target is never an intrinsic") {
	FunctionTable table;
	CalleeDescriptor descriptor;
	descriptor.kind = CalleeDescriptor::Kind::Internal;
	descriptor.customName = "tracedFunction";
	const auto functionId = table.intern(descriptor);
	const auto& target = table.get(functionId);

	REQUIRE(target.getLinkage() == Linkage::Internal);
	REQUIRE(target.getIntrinsic() == IntrinsicId::None);
	REQUIRE(target.getAddress() == nullptr);
}

} // namespace nautilus::testing
