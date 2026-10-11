#include "nautilus/serialization/ModuleSerialization.hpp"
#include "BufferView.hpp"
#include "SectionWriter.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include <algorithm>
#include <bit>
#include <cstring>
#include <fmt/format.h>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace nautilus::serialization {

namespace {

using detail::ExtraSection;
using detail::require;

/// Accumulates the manifest's string blob and record tables.
class ManifestWriter {
public:
	StringRef str(std::string_view text) {
		auto [it, inserted] = offsets.try_emplace(std::string(text), 0);
		if (inserted) {
			if (strings.size() + text.size() >= NONE) {
				throw SerializationException("module manifest exceeds the format's 32-bit limit");
			}
			it->second = static_cast<uint32_t>(strings.size());
			strings.append(text);
		}
		return StringRef {it->second, static_cast<uint32_t>(text.size())};
	}

	void function(const compiler::ir::FunctionOperation& function) {
		SignatureRecord record {};
		record.name = str(function.getName());
		record.result = static_cast<uint8_t>(function.getOutputArg());
		record.paramsBegin = static_cast<uint32_t>(types.size());
		for (const auto* argument : function.getEntryBlock()->getArguments()) {
			types.push_back(static_cast<uint8_t>(argument->getStamp()));
		}
		record.paramCount = static_cast<uint32_t>(types.size()) - record.paramsBegin;
		signatures.push_back(record);
	}

	void option(const std::string& key, const engine::OptionValue& value) {
		OptionRecord record {};
		record.key = str(key);
		record.text = StringRef {NONE, 0};
		if (const auto* i = std::get_if<int>(&value)) {
			record.kind = static_cast<uint8_t>(WireOptionKind::Int);
			record.scalar = static_cast<uint64_t>(static_cast<int64_t>(*i));
		} else if (const auto* d = std::get_if<double>(&value)) {
			record.kind = static_cast<uint8_t>(WireOptionKind::Double);
			record.scalar = std::bit_cast<uint64_t>(*d);
		} else if (const auto* text = std::get_if<std::string>(&value)) {
			record.kind = static_cast<uint8_t>(WireOptionKind::String);
			record.text = str(*text);
		} else {
			record.kind = static_cast<uint8_t>(WireOptionKind::Bool);
			record.scalar = std::get<bool>(value) ? 1 : 0;
		}
		options.push_back(record);
	}

	std::vector<ExtraSection> sections() const {
		std::vector<ExtraSection> out;
		out.push_back(section(TAG_MODULE_STRINGS, 1, strings.data(), strings.size()));
		out.push_back(section(TAG_MODULE_SIGNATURES, sizeof(SignatureRecord), signatures.data(), signatures.size()));
		out.push_back(section(TAG_MODULE_TYPES, 1, types.data(), types.size()));
		out.push_back(section(TAG_MODULE_OPTIONS, sizeof(OptionRecord), options.data(), options.size()));
		return out;
	}

private:
	static ExtraSection section(uint32_t tag, uint32_t stride, const void* data, size_t count) {
		ExtraSection extra {tag, stride, static_cast<uint32_t>(count), {}};
		extra.bytes.resize(stride * count);
		if (count != 0) {
			std::memcpy(extra.bytes.data(), data, extra.bytes.size());
		}
		return extra;
	}

	std::string strings;
	std::unordered_map<std::string, uint32_t> offsets;
	std::vector<SignatureRecord> signatures;
	std::vector<uint8_t> types;
	std::vector<OptionRecord> options;
};

const detail::SectionView& manifestSection(const detail::BufferView& view, uint32_t tag, uint32_t size,
                                           bool exact = false) {
	const auto it = view.extras.find(tag);
	if (it == view.extras.end()) {
		throw SerializationException("not a module image: the buffer has no module manifest");
	}
	require(exact ? it->second.stride == size : it->second.stride >= size,
	        "manifest section has an unexpected record size");
	return it->second;
}

std::string manifestString(const detail::SectionView& strings, StringRef ref) {
	require(ref.offset != NONE, "unexpected null string");
	require(detail::inRange(ref.offset, ref.length, strings.count), "string out of range");
	return {reinterpret_cast<const char*>(strings.data) + ref.offset, ref.length};
}

Type manifestType(uint8_t raw) {
	require(raw <= static_cast<uint8_t>(Type::ptr), "unknown type");
	return static_cast<Type>(raw);
}

} // namespace

std::vector<std::byte> saveModule(const engine::CompiledModule& module, const SerializeOptions& options) {
	const auto ir = module.getIR();
	if (!ir) {
		throw SerializationException(
		    "the module kept no IR to save; compile it with the 'engine.retainIR' option set to true");
	}
	ManifestWriter manifest;
	for (const auto* function : ir->getFunctionOperations()) {
		manifest.function(*function);
	}
	// Sorted, so the options' hash-map order cannot leak into the bytes.
	const auto compileOptions = module.getCompileOptions();
	const auto& values = compileOptions.getValues();
	std::vector<std::string> keys;
	keys.reserve(values.size());
	for (const auto& [key, value] : values) {
		keys.push_back(key);
	}
	std::sort(keys.begin(), keys.end());
	for (const auto& key : keys) {
		manifest.option(key, values.at(key));
	}
	const auto sections = manifest.sections();
	return detail::serializeWithSections(*ir, options, sections);
}

ModuleImageInfo inspectModule(std::span<const std::byte> image, bool verifyChecksum) {
	DeserializeOptions readOptions;
	readOptions.verifyChecksum = verifyChecksum;
	const detail::BufferView view(image, readOptions);
	const auto& strings = manifestSection(view, TAG_MODULE_STRINGS, 1, true);
	const auto& signatures = manifestSection(view, TAG_MODULE_SIGNATURES, sizeof(SignatureRecord));
	const auto& types = manifestSection(view, TAG_MODULE_TYPES, 1, true);
	const auto& options = manifestSection(view, TAG_MODULE_OPTIONS, sizeof(OptionRecord));

	ModuleImageInfo info;
	info.unitId = std::string(view.string(view.get<ModuleRecord>(view.module, 0).compilationUnitId));
	for (uint32_t i = 0; i < signatures.count; ++i) {
		const auto record = view.get<SignatureRecord>(signatures, i);
		FunctionSignature signature;
		signature.name = manifestString(strings, record.name);
		signature.result = manifestType(record.result);
		require(detail::inRange(record.paramsBegin, record.paramCount, types.count), "parameter types");
		for (uint32_t p = 0; p < record.paramCount; ++p) {
			signature.params.push_back(manifestType(view.get<uint8_t>(types, record.paramsBegin + p)));
		}
		info.functions.push_back(std::move(signature));
	}
	for (uint32_t i = 0; i < options.count; ++i) {
		const auto record = view.get<OptionRecord>(options, i);
		const auto key = manifestString(strings, record.key);
		switch (static_cast<WireOptionKind>(record.kind)) {
		case WireOptionKind::Int:
			info.options.setOption(key, static_cast<int>(static_cast<int64_t>(record.scalar)));
			break;
		case WireOptionKind::Double:
			info.options.setOption(key, std::bit_cast<double>(record.scalar));
			break;
		case WireOptionKind::String:
			info.options.setOption(key, manifestString(strings, record.text));
			break;
		case WireOptionKind::Bool:
			info.options.setOption(key, record.scalar != 0);
			break;
		default:
			require(false, "unknown option kind");
		}
	}
	return info;
}

engine::CompiledModule loadModule(const engine::NautilusEngine& engine, std::span<const std::byte> image,
                                  const DeserializeOptions& options, engine::ModuleOptions overrides) {
	return engine.compileIR(deserialize(image, options), std::move(overrides));
}

} // namespace nautilus::serialization
