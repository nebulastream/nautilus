#include "nautilus/extensions/ExtensionRegistry.hpp"
#include <atomic>
#include <catch2/catch_all.hpp>
#include <thread>

namespace nautilus::ext {

/// The test-only reset (04 §3.7): it exists only in this binary.
struct ExtensionRegistryTestAccess {
	static void reset();
};

void ExtensionRegistryTestAccess::reset() {
	auto& r = ExtensionRegistry::instance();
	std::lock_guard<std::mutex> lock(r.mutex_);
	r.frozen_.store(false);
	r.types_.clear();
	r.ops_.clear();
	r.intrinsics_.clear();
	r.passes_.clear();
	r.byName_.clear();
	r.byFallback_.clear();
	r.locks_.store(0);
}

} // namespace nautilus::ext

using namespace nautilus::ext;
using nautilus::nir::TypeId;

namespace {

ExtensionRegistry& fresh() {
	ExtensionRegistryTestAccess::reset();
	return ExtensionRegistry::instance();
}

TypeInfo typeNamed(std::string name) {
	TypeInfo t;
	t.name = std::move(name);
	t.size = 16;
	t.align = 16;
	t.regClass = RegClass::Vector;
	return t;
}

ExtOpInfo opNamed(std::string name) {
	ExtOpInfo o;
	o.name = std::move(name);
	o.hasExpansion = true;
	return o;
}

void nativeFallback() {
}
void otherFallback() {
}
void thunk() {
}

IntrinsicInfo intrinsicNamed(std::string name, void (*fallback)() = nativeFallback) {
	IntrinsicInfo i;
	i.name = std::move(name);
	i.signature = {0}; // void()
	i.attrs = 1u << 4; // noUnwind
	i.fallback = reinterpret_cast<void*>(fallback);
	return i;
}

} // namespace

TEST_CASE("types and operations get dense ids", "[registry]") {
	auto& r = fresh();
	REQUIRE(r.registerType(typeNamed("simd.vec<f32,8>")) == 16);
	REQUIRE(r.registerType(typeNamed("builtin.i128")) == 17);
	REQUIRE(r.registerOp(opNamed("simd.add")) == 256);
	REQUIRE(r.registerOp(opNamed("simd.sub")) == 257);
	REQUIRE(r.registerIntrinsic(intrinsicNamed("builtin.prefetch")) == 0);
	REQUIRE(r.registerPass("simd.legalize") == 0);
	r.freeze();
	REQUIRE(*r.findType("builtin.i128") == 17);
	REQUIRE(r.type(16)->name == "simd.vec<f32,8>");
	REQUIRE(r.type(15) == nullptr);
	REQUIRE(r.type(18) == nullptr);
	REQUIRE(*r.findOp("simd.sub") == 257);
	REQUIRE(r.op(256)->name == "simd.add");
	REQUIRE(r.op(255) == nullptr);
	REQUIRE(r.op(258) == nullptr);
	REQUIRE(*r.findIntrinsic("builtin.prefetch") == 0);
	REQUIRE_FALSE(r.findType("nope.type"));
	REQUIRE_FALSE(r.findOp("simd.vec<f32,8>")); // a type is not an op
	REQUIRE(r.typeCount() == 2);
	REQUIRE(r.opCount() == 2);
	REQUIRE(r.intrinsicCount() == 1);
}

TEST_CASE("at most 240 extension types", "[registry]") {
	auto& r = fresh();
	for (int i = 0; i < 240; ++i) {
		REQUIRE(r.registerType(typeNamed("test.t" + std::to_string(i))) == 16 + i);
	}
	REQUIRE_THROWS_AS(r.registerType(typeNamed("test.one_more")), RegistrationError);
}

TEST_CASE("duplicates are rejected", "[registry]") {
	auto& r = fresh();
	r.registerType(typeNamed("test.t"));
	r.registerOp(opNamed("test.op"));
	r.registerIntrinsic(intrinsicNamed("test.fn"));
	r.registerPass("test.pass");
	REQUIRE_THROWS_AS(r.registerType(typeNamed("test.t")), RegistrationError);
	REQUIRE_THROWS_AS(r.registerOp(opNamed("test.op")), RegistrationError);
	REQUIRE_THROWS_AS(r.registerIntrinsic(intrinsicNamed("test.fn", otherFallback)), RegistrationError);
	REQUIRE_THROWS_AS(r.registerPass("test.pass"), RegistrationError);
	// the same name in another namespace of the registry is fine
	REQUIRE_NOTHROW(r.registerOp(opNamed("test.t")));
}

TEST_CASE("an invalid registration is rejected", "[registry]") {
	auto& r = fresh();
	auto badAlign = typeNamed("test.align");
	badAlign.align = 3;
	REQUIRE_THROWS_AS(r.registerType(badAlign), RegistrationError);
	auto noParts = typeNamed("test.parts");
	noParts.parts = 0;
	REQUIRE_THROWS_AS(r.registerType(noParts), RegistrationError);
	ExtOpInfo noFallback;
	noFallback.name = "test.nofallback";
	REQUIRE_THROWS_AS(r.registerOp(noFallback), RegistrationError);
	noFallback.targets = 1; // target-restricted operations may omit the fallback
	REQUIRE_NOTHROW(r.registerOp(noFallback));
	auto noFn = intrinsicNamed("test.nofn");
	noFn.fallback = nullptr;
	REQUIRE_THROWS_AS(r.registerIntrinsic(noFn), RegistrationError);
	auto noSig = intrinsicNamed("test.nosig");
	noSig.signature.clear();
	REQUIRE_THROWS_AS(r.registerIntrinsic(noSig), RegistrationError);
}

TEST_CASE("a may-throw intrinsic needs a capture thunk", "[registry]") {
	auto& r = fresh();
	auto i = intrinsicNamed("test.mayThrow");
	i.attrs = 0;
	REQUIRE_THROWS_AS(r.registerIntrinsic(i), RegistrationError);
	i.captureThunk = reinterpret_cast<void*>(thunk);
	REQUIRE_NOTHROW(r.registerIntrinsic(i));
	r.freeze();
	REQUIRE(r.intrinsic(*r.findIntrinsic("test.mayThrow"))->captureThunk == reinterpret_cast<void*>(thunk));
}

TEST_CASE("a traced fallback address finds its intrinsic", "[registry]") {
	auto& r = fresh();
	r.registerIntrinsic(intrinsicNamed("test.a", nativeFallback));
	r.registerIntrinsic(intrinsicNamed("test.b", otherFallback));
	r.freeze();
	REQUIRE(*r.findIntrinsicByFallback(reinterpret_cast<void*>(otherFallback)) == 1);
	REQUIRE_FALSE(r.findIntrinsicByFallback(reinterpret_cast<void*>(thunk)));
}

TEST_CASE("registration after the freeze throws and names the freeze point", "[registry]") {
	auto& r = fresh();
	r.registerType(typeNamed("test.early"));
	REQUIRE_FALSE(r.frozen());
	r.freeze();
	REQUIRE(r.frozen());
	r.freeze(); // idempotent
	for (auto&& attempt : std::vector<std::function<void()>> {
	         [&] { r.registerType(typeNamed("test.late")); }, [&] { r.registerOp(opNamed("test.late")); },
	         [&] { r.registerIntrinsic(intrinsicNamed("test.late")); }, [&] { r.registerPass("test.late"); }}) {
		try {
			attempt();
			FAIL("late registration did not throw");
		} catch (RegistrationError& e) {
			std::string m = e.what();
			REQUIRE(m.find("test.late") != std::string::npos);
			REQUIRE(m.find("before the first NautilusEngine") != std::string::npos);
		}
	}
	REQUIRE(r.typeCount() == 1);
}

TEST_CASE("reserved plugin names are rejected", "[registry]") {
	auto& r = fresh();
	// core mnemonics of 13 §5 and the first component of one
	for (auto* name : {"div.x", "rem.x", "cmp.x", "cast.x", "call.x", "ptr.x", "bind.x", "data.x", "shr.x", "add.x",
	                   "load.x", "select.x", "funcaddr.x"}) {
		INFO(name);
		REQUIRE_THROWS_AS(r.registerOp(opNamed(name)), RegistrationError);
	}
	// type names of §2
	for (auto* name : {"i64.x", "f32.x", "bool.x", "void.x", "i8.x"}) {
		INFO(name);
		REQUIRE_THROWS_AS(r.registerType(typeNamed(name)), RegistrationError);
	}
	// keywords of the text form and `nir`
	for (auto* name : {"nir.x", "func.x", "module.x", "extension.x", "unwind.x", "thunk.x", "pad.x", "attrs.x"}) {
		INFO(name);
		REQUIRE_THROWS_AS(r.registerIntrinsic(intrinsicNamed(name)), RegistrationError);
	}
	// the name must be <plugin>.<name> with a plugin of lowercase letters, digits and '_'
	for (auto* name : {"nodot", "trailing.", ".leading", "Upper.x", "9digit.x", "a-b.x"}) {
		INFO(name);
		REQUIRE_THROWS_AS(r.registerOp(opNamed(name)), RegistrationError);
	}
	REQUIRE_NOTHROW(r.registerOp(opNamed("simd.add")));
	REQUIRE_NOTHROW(r.registerOp(opNamed("my_plugin2.op")));
	REQUIRE_NOTHROW(r.registerType(typeNamed("builtin.i128")));
}

TEST_CASE("the core prefix is reserved for the core", "[registry]") {
	auto& r = fresh();
	REQUIRE_NOTHROW(r.registerIntrinsic(intrinsicNamed("core.memcpy"), Owner::Core));
	REQUIRE_THROWS_AS(r.registerIntrinsic(intrinsicNamed("core.memmove", otherFallback)), RegistrationError);
	REQUIRE_THROWS_AS(r.registerOp(opNamed("core.add")), RegistrationError);
	REQUIRE_THROWS_AS(r.registerType(typeNamed("core.t")), RegistrationError);
	// and the core registers nothing else
	REQUIRE_THROWS_AS(r.registerIntrinsic(intrinsicNamed("simd.x", otherFallback), Owner::Core), RegistrationError);
}

TEST_CASE("names split at the last @", "[registry]") {
	std::string_view name;
	uint32_t version = 0;
	REQUIRE(splitNameVersion("simd.add@3", name, version));
	REQUIRE(name == "simd.add");
	REQUIRE(version == 3);
	REQUIRE(splitNameVersion("simd.vec<f32,8>@12", name, version));
	REQUIRE(name == "simd.vec<f32,8>");
	REQUIRE(splitNameVersion("odd@name@7", name, version));
	REQUIRE(name == "odd@name");
	REQUIRE_FALSE(splitNameVersion("noversion", name, version));
	REQUIRE_FALSE(splitNameVersion("trailing@", name, version));
	REQUIRE_FALSE(splitNameVersion("@1", name, version));
	REQUIRE_FALSE(splitNameVersion("x@12ab", name, version));
	REQUIRE_FALSE(splitNameVersion("x@99999999999", name, version));
}

TEST_CASE("lookups take no lock after the freeze", "[registry]") {
	auto& r = fresh();
	r.registerType(typeNamed("test.t"));
	r.registerOp(opNamed("test.op"));
	r.registerIntrinsic(intrinsicNamed("test.fn"));
	auto beforeFreeze = r.lockAcquisitions();
	r.findType("test.t");
	REQUIRE(r.lockAcquisitions() > beforeFreeze); // before the freeze a lookup takes the mutex
	r.freeze();
	auto locks = r.lockAcquisitions();
	std::atomic<int> found {0};
	std::vector<std::thread> threads;
	for (int t = 0; t < 4; ++t) {
		threads.emplace_back([&] {
			for (int i = 0; i < 20000; ++i) {
				found += r.findType("test.t").has_value() + r.findOp("test.op").has_value() +
				         r.findIntrinsic("test.fn").has_value() + (r.type(16) != nullptr) + (r.op(256) != nullptr) +
				         (r.intrinsic(0) != nullptr) + (r.typeCount() == 1);
			}
		});
	}
	for (auto& t : threads) {
		t.join();
	}
	REQUIRE(found == 4 * 20000 * 7);
	REQUIRE(r.lockAcquisitions() == locks);
}

TEST_CASE("registration racing the first reader either lands before the freeze or throws", "[registry]") {
	for (int round = 0; round < 50; ++round) {
		auto& r = fresh();
		std::atomic<bool> go {false};
		std::atomic<int> registered {0}, rejected {0};
		std::vector<std::thread> threads;
		for (int t = 0; t < 4; ++t) {
			threads.emplace_back([&, t] {
				while (!go) {
				}
				for (int i = 0; i < 10; ++i) {
					try {
						r.registerType(typeNamed("race.t" + std::to_string(t) + "_" + std::to_string(i)));
						registered++;
					} catch (RegistrationError&) {
						rejected++;
					}
				}
			});
		}
		threads.emplace_back([&] {
			while (!go) {
			}
			r.freeze(); // the first reader
		});
		go = true;
		for (auto& t : threads) {
			t.join();
		}
		REQUIRE(registered + rejected == 40);
		// what was registered before the freeze is exactly what a reader sees
		REQUIRE(r.typeCount() == static_cast<size_t>(registered));
		REQUIRE(r.frozen());
	}
}

#include "nautilus/nir/ImageLoader.hpp"
#include "nautilus/nir/Parser.hpp"

TEST_CASE("an image load, a text parse or any first reader freezes the registry", "[registry]") {
	auto& r = fresh();
	r.registerType(typeNamed("test.before"));
	REQUIRE_FALSE(r.frozen());
	auto parsed = nautilus::nir::parse("nir 1.0\n");
	REQUIRE(r.frozen());
	REQUIRE_THROWS_AS(r.registerType(typeNamed("test.after")), RegistrationError);

	auto& r2 = fresh();
	REQUIRE_FALSE(r2.frozen());
	std::vector<uint8_t> notAnImage(8, 0);
	auto loaded = nautilus::nir::loadImage(notAnImage); // rejected, but still a first reader
	REQUIRE_FALSE(loaded.ok());
	REQUIRE(r2.frozen());
}
