#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <nautilus/Artifact.hpp>
#include <nautilus/Engine.hpp>
#include <nautilus/val_std.hpp>
#include <stdexcept>
#include <string>

namespace {
int32_t wrapperCalls = 0;
int64_t proxy(int64_t value) noexcept {
	return value * 3;
}
struct alignas(64) OwnedValue {
	int64_t* cleanup;
	int64_t value;
	OwnedValue(int64_t* cleanup, int64_t value) noexcept : cleanup(cleanup), value(value) {
	}
	~OwnedValue() noexcept {
		++*cleanup;
	}
};
int64_t useOwned(OwnedValue* value, bool throwing) {
	if (throwing) {
		throw std::runtime_error("Owned artifact exception");
	}
	return value->value;
}
void verify(nautilus::engine::CompiledModule& module) {
	if (module.getFunction<int64_t(int64_t)>("increment")(7) != 25) {
		throw std::runtime_error("Artifact consumer result mismatch");
	}
	const auto owned = module.getFunction<int64_t(int64_t*, int64_t, bool)>("owned");
	int64_t cleanup = 0;
	if (owned(&cleanup, 42, false) != 42 || cleanup != 1) {
		throw std::runtime_error("Owned artifact result mismatch");
	}
	try {
		owned(&cleanup, -9, true);
		throw std::runtime_error("Owned artifact did not throw");
	} catch (const std::runtime_error& error) {
		if (std::string(error.what()) != "Owned artifact exception" || cleanup != 2) {
			throw;
		}
	}
	if (owned(&cleanup, 7, false) != 7 || cleanup != 3) {
		throw std::runtime_error("Owned artifact recovery mismatch");
	}
}
} // namespace

int main(int argc, char** argv) {
	using namespace nautilus;
	using namespace nautilus::engine;
	try {
		Options options;
#ifdef ENABLE_MLIR_BACKEND
		options.setOption("engine.backend", std::string("mlir"));
#else
		options.setOption("engine.backend", std::string("cpp"));
#endif
		const std::string mode = argc > 1 ? argv[1] : "ordinary";
		if (mode == "native" || mode == "bytecode") {
			if (argc != 3) {
				throw std::runtime_error("Missing artifact file");
			}
			std::ifstream input(argv[2], std::ios::binary);
			if (!input) {
				throw std::runtime_error("Could not open artifact file");
			}
			std::string bytes {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
			const auto transported = artifact::decode(bytes);
			auto module = mode == "native" ? artifact::loadNative(transported, options)
			                               : artifact::loadBytecode(transported, options);
			verify(module);
			if (wrapperCalls != 0) {
				throw std::runtime_error("Artifact consumer executed a tracing wrapper");
			}
		} else {
			NautilusEngine engine(options);
			auto builder = engine.createModule();
			builder.registerFunction<val<int64_t>(val<int64_t>)>("increment", [](val<int64_t> value) {
				++wrapperCalls;
				return invoke(proxy, value) + cacheLiteral<int64_t {4}>();
			});
			builder.registerFunction<val<int64_t>(val<int64_t*>, val<int64_t>, val<bool>)>(
			    "owned", [](val<int64_t*> cleanup, val<int64_t> value, val<bool> throwing) {
				    ++wrapperCalls;
				    val<OwnedValue> object(cleanup, value);
				    return invoke(useOwned, &object, throwing);
			    });
			if (mode == "emit") {
#ifdef ENABLE_TRACING
				if (argc != 3 || !artifact::isSupported()) {
					throw std::runtime_error("Artifact emission is unsupported");
				}
				const auto transported = builder.createArtifact();
				const auto bytes = artifact::encode(transported);
				std::ofstream output(argv[2], std::ios::binary);
				output.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
				output.close();
				if (!output || wrapperCalls == 0) {
					throw std::runtime_error("Artifact consumer emission failed");
				}
#else
				throw std::runtime_error("Artifact emission requires tracing");
#endif
			} else {
				auto module = builder.compile();
				verify(module);
				if (wrapperCalls == 0) {
					throw std::runtime_error("Ordinary consumer did not execute its wrapper");
				}
#ifndef ENABLE_MLIR_BACKEND
				if (artifact::isSupported()) {
					throw std::runtime_error("Reduced build advertised unsupported artifact persistence");
				}
#endif
			}
		}
		std::cout << mode << " passed; wrappers=" << wrapperCalls << '\n';
		return 0;
	} catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}
