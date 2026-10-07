#pragma once

#include "nautilus/Module.hpp"
#include "nautilus/core.hpp"
#include "nautilus/options.hpp"
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace nautilus::artifact {

struct NativeImage {
	std::string buildId;
	uint64_t loadOffset = 0;
	bool operator==(const NativeImage&) const = default;
};

struct ExportDescriptor {
	std::string name;
	Type returnType = Type::v;
	std::vector<Type> argumentTypes;
	std::vector<std::pair<std::string, std::string>> attributes;
	uint32_t callingConvention = 0;
	std::string loweredABI;
	bool operator==(const ExportDescriptor&) const = default;
};

struct ImportDescriptor {
	std::string symbol;
	NativeImage image;
	bool bytecodeImport = true;
	bool operator==(const ImportDescriptor&) const = default;
};

struct Compatibility {
	NativeImage compilerImage;
	NativeImage producerImage;
	std::string llvmVersion;
	std::string targetTriple;
	std::string cpu;
	std::string features;
	std::string dataLayout;
	std::string optionsDigest;
	std::string extensionFingerprint;
	uint32_t pointerSize = 0;
	bool littleEndian = true;
	bool operator==(const Compatibility&) const = default;
};

struct Descriptor {
	uint32_t version = 2;
	Compatibility compatibility;
	std::vector<ExportDescriptor> exports;
	std::vector<ImportDescriptor> imports;
	std::vector<runtime_binding::SchemaEntry> bindingSchema;
	std::string moduleManifest;
	std::string objectDigest;
	std::string bytecodeDigest;
	bool operator==(const Descriptor&) const = default;
};

struct ModuleArtifact {
	Descriptor descriptor;
	std::string descriptorDigest;
	std::string object;
	std::string bytecode;
};

bool isSupported();
std::string encode(const ModuleArtifact& artifact);
ModuleArtifact decode(std::string_view bytes);
engine::CompiledModule loadNative(const ModuleArtifact& artifact, const engine::Options& options = {});
engine::CompiledModule loadBytecode(const ModuleArtifact& artifact, const engine::Options& options = {});

#ifdef ENABLE_TRACING
ModuleArtifact emit(std::list<compiler::CompilableFunction>& functions, const engine::ModuleOptions& options);
#endif

} // namespace nautilus::artifact
