#include "nautilus/compiler/artifact/ArtifactCodec.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <limits>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/SHA256.h>
#include <type_traits>
#include <unordered_set>

namespace nautilus::artifact::detail {

Writer::Writer(uint64_t maximumSize) : maximumSize_(maximumSize) {
}
void Writer::require(std::size_t length) const {
	if (length > maximumSize_ - data_.size()) {
		throw RuntimeException("Artifact record exceeds the transport limit");
	}
}
void Writer::u8(uint8_t value) {
	require(1);
	data_.push_back(static_cast<char>(value));
}
void Writer::u32(uint32_t value) {
	for (uint32_t shift = 0; shift < 32; shift += 8) {
		u8(static_cast<uint8_t>(value >> shift));
	}
}
void Writer::u64(uint64_t value) {
	for (uint32_t shift = 0; shift < 64; shift += 8) {
		u8(static_cast<uint8_t>(value >> shift));
	}
}
void Writer::bytes(std::span<const uint8_t> value) {
	require(value.size());
	data_.append(reinterpret_cast<const char*>(value.data()), value.size());
}
void Writer::string(std::string_view value) {
	if (value.size() > MAX_ARTIFACT_SIZE) {
		throw RuntimeException("Artifact string exceeds the transport limit");
	}
	u64(value.size());
	require(value.size());
	data_.append(value);
}
std::string Writer::take() {
	return std::move(data_);
}

Reader::Reader(std::string_view data) : data_(data) {
}
uint8_t Reader::u8() {
	require(1);
	return static_cast<uint8_t>(data_[position_++]);
}
uint32_t Reader::u32() {
	uint32_t value = 0;
	for (uint32_t shift = 0; shift < 32; shift += 8) {
		value |= static_cast<uint32_t>(u8()) << shift;
	}
	return value;
}
uint64_t Reader::u64() {
	uint64_t value = 0;
	for (uint32_t shift = 0; shift < 64; shift += 8) {
		value |= static_cast<uint64_t>(u8()) << shift;
	}
	return value;
}
uint32_t Reader::count() {
	const auto value = u32();
	if (value > MAX_RECORD_COUNT || value > data_.size() - position_) {
		throw RuntimeException("Invalid artifact record count");
	}
	return value;
}
std::string Reader::string(uint64_t maximumSize) {
	const auto length = u64();
	if (length > maximumSize || length > data_.size() - position_) {
		throw RuntimeException("Invalid artifact string length");
	}
	const std::string value(data_.substr(position_, static_cast<std::size_t>(length)));
	position_ += static_cast<std::size_t>(length);
	return value;
}
std::span<const uint8_t> Reader::bytes(std::size_t length) {
	require(length);
	const auto* begin = reinterpret_cast<const uint8_t*>(data_.data() + position_);
	position_ += length;
	return {begin, length};
}
void Reader::finish() const {
	if (position_ != data_.size()) {
		throw RuntimeException("Trailing artifact bytes");
	}
}
void Reader::require(std::size_t length) const {
	if (length > data_.size() - position_) {
		throw RuntimeException("Truncated artifact record");
	}
}

std::string digest(std::string_view value) {
	llvm::SHA256 sha;
	sha.update(llvm::StringRef(value.data(), value.size()));
	const auto bytes = sha.final();
	static constexpr char digits[] = "0123456789abcdef";
	std::string result;
	result.reserve(bytes.size() * 2U);
	for (const auto byte : bytes) {
		result.push_back(digits[byte >> 4U]);
		result.push_back(digits[byte & 0x0fU]);
	}
	return result;
}
bool isDigest(std::string_view value) {
	return value.size() == 64 && std::ranges::all_of(value, [](char character) {
		       return (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f');
	       });
}

namespace {

void writeImage(Writer& writer, const NativeImage& image) {
	writer.string(image.buildId);
	writer.u64(image.loadOffset);
}
NativeImage readImage(Reader& reader) {
	return {reader.string(), reader.u64()};
}
bool validName(std::string_view name) {
	return !name.empty() && name.find('\0') == std::string_view::npos;
}
bool validImage(const NativeImage& image) {
	return image.buildId.size() >= 2 && image.buildId.size() <= 256 && image.buildId.size() % 2 == 0 &&
	       std::ranges::all_of(image.buildId, [](char character) {
		       return (character >= '0' && character <= '9') || (character >= 'a' && character <= 'f');
	       });
}
void validateStructure(const Descriptor& value) {
	const auto& compatibility = value.compatibility;
	if (value.version != 1 || value.exports.empty() || value.exports.size() > MAX_RECORD_COUNT ||
	    value.imports.size() > MAX_RECORD_COUNT || value.moduleManifest.empty() ||
	    value.moduleManifest.size() > MAX_DESCRIPTOR_SIZE || !isDigest(value.bytecodeDigest) ||
	    !isDigest(value.objectDigest) || !validImage(compatibility.compilerImage) ||
	    !validImage(compatibility.producerImage) || compatibility.llvmVersion.empty() ||
	    compatibility.targetTriple.empty() || compatibility.cpu.empty() || compatibility.dataLayout.empty() ||
	    !isDigest(compatibility.optionsDigest) || compatibility.pointerSize == 0) {
		throw RuntimeException("Invalid artifact descriptor");
	}
	std::unordered_set<std::string> exports;
	for (const auto& entry : value.exports) {
		if (!validName(entry.name) || !exports.insert(entry.name).second || entry.callingConvention != 0 ||
		    entry.loweredABI.empty() || static_cast<uint8_t>(entry.returnType) > static_cast<uint8_t>(Type::ptr) ||
		    entry.argumentTypes.size() > MAX_RECORD_COUNT || entry.attributes.size() > MAX_RECORD_COUNT) {
			throw RuntimeException("Invalid artifact export descriptor");
		}
		for (const auto type : entry.argumentTypes) {
			if (type == Type::v || static_cast<uint8_t>(type) > static_cast<uint8_t>(Type::ptr)) {
				throw RuntimeException("Invalid artifact export argument type");
			}
		}
		std::unordered_set<std::string> attributes;
		for (const auto& [name, attribute] : entry.attributes) {
			if (!validName(name) || attribute.find('\0') != std::string::npos || !attributes.insert(name).second) {
				throw RuntimeException("Invalid artifact export attributes");
			}
		}
	}
	std::unordered_set<std::string> imports;
	for (const auto& entry : value.imports) {
		if (!validName(entry.symbol) || !validImage(entry.image) || !imports.insert(entry.symbol).second ||
		    exports.contains(entry.symbol)) {
			throw RuntimeException("Invalid artifact import descriptor");
		}
	}
}

} // namespace

std::string encodeDescriptor(const Descriptor& value) {
	Writer writer(MAX_DESCRIPTOR_SIZE);
	writer.u32(value.version);
	const auto& compatibility = value.compatibility;
	writeImage(writer, compatibility.compilerImage);
	writeImage(writer, compatibility.producerImage);
	for (const auto* field :
	     {&compatibility.llvmVersion, &compatibility.targetTriple, &compatibility.cpu, &compatibility.features,
	      &compatibility.dataLayout, &compatibility.optionsDigest, &compatibility.extensionFingerprint}) {
		writer.string(*field);
	}
	writer.u32(compatibility.pointerSize);
	writer.u8(compatibility.littleEndian);
	writer.u32(static_cast<uint32_t>(value.exports.size()));
	for (const auto& entry : value.exports) {
		writer.string(entry.name);
		writer.u8(static_cast<uint8_t>(entry.returnType));
		writer.u32(static_cast<uint32_t>(entry.argumentTypes.size()));
		for (const auto type : entry.argumentTypes) {
			writer.u8(static_cast<uint8_t>(type));
		}
		writer.u32(static_cast<uint32_t>(entry.attributes.size()));
		for (const auto& [name, attribute] : entry.attributes) {
			writer.string(name);
			writer.string(attribute);
		}
		writer.u32(entry.callingConvention);
		writer.string(entry.loweredABI);
	}
	writer.u32(static_cast<uint32_t>(value.imports.size()));
	for (const auto& entry : value.imports) {
		writer.string(entry.symbol);
		writeImage(writer, entry.image);
		writer.u8(entry.bytecodeImport);
	}
	writer.string(value.moduleManifest);
	writer.string(value.objectDigest);
	writer.string(value.bytecodeDigest);
	auto result = writer.take();
	if (result.size() > MAX_DESCRIPTOR_SIZE) {
		throw RuntimeException("Artifact descriptor exceeds the transport limit");
	}
	return result;
}

Descriptor decodeDescriptor(std::string_view bytes) {
	if (bytes.size() > MAX_DESCRIPTOR_SIZE) {
		throw RuntimeException("Artifact descriptor exceeds the transport limit");
	}
	Reader reader(bytes);
	Descriptor value;
	value.version = reader.u32();
	auto& compatibility = value.compatibility;
	compatibility.compilerImage = readImage(reader);
	compatibility.producerImage = readImage(reader);
	for (auto* field :
	     {&compatibility.llvmVersion, &compatibility.targetTriple, &compatibility.cpu, &compatibility.features,
	      &compatibility.dataLayout, &compatibility.optionsDigest, &compatibility.extensionFingerprint}) {
		*field = reader.string();
	}
	compatibility.pointerSize = reader.u32();
	const auto littleEndian = reader.u8();
	if (littleEndian > 1) {
		throw RuntimeException("Invalid artifact endianness");
	}
	compatibility.littleEndian = littleEndian;
	const auto exportCount = reader.count();
	for (uint32_t index = 0; index < exportCount; ++index) {
		ExportDescriptor entry;
		entry.name = reader.string();
		entry.returnType = static_cast<Type>(reader.u8());
		const auto argumentCount = reader.count();
		for (uint32_t argument = 0; argument < argumentCount; ++argument) {
			entry.argumentTypes.push_back(static_cast<Type>(reader.u8()));
		}
		const auto attributeCount = reader.count();
		for (uint32_t attribute = 0; attribute < attributeCount; ++attribute) {
			auto name = reader.string();
			auto attributeValue = reader.string();
			entry.attributes.emplace_back(std::move(name), std::move(attributeValue));
		}
		entry.callingConvention = reader.u32();
		entry.loweredABI = reader.string();
		value.exports.push_back(std::move(entry));
	}
	const auto importCount = reader.count();
	for (uint32_t index = 0; index < importCount; ++index) {
		ImportDescriptor entry;
		entry.symbol = reader.string();
		entry.image = readImage(reader);
		const auto bytecodeImport = reader.u8();
		if (bytecodeImport > 1) {
			throw RuntimeException("Invalid artifact import kind");
		}
		entry.bytecodeImport = bytecodeImport;
		value.imports.push_back(std::move(entry));
	}
	value.moduleManifest = reader.string();
	value.objectDigest = reader.string();
	value.bytecodeDigest = reader.string();
	reader.finish();
	validateStructure(value);
	return value;
}

void validateDescriptor(const ModuleArtifact& value) {
	validateStructure(value.descriptor);
	if (!isDigest(value.descriptorDigest) || digest(encodeDescriptor(value.descriptor)) != value.descriptorDigest) {
		throw RuntimeException("Artifact descriptor integrity mismatch");
	}
}
void validatePayload(std::string_view bytes, std::string_view expectedDigest) {
	if (bytes.empty() || bytes.size() > MAX_ARTIFACT_SIZE || !isDigest(expectedDigest) ||
	    digest(bytes) != expectedDigest) {
		throw RuntimeException("Artifact payload integrity mismatch");
	}
}

std::string optionsDigest(const engine::Options& options) {
	std::vector<std::pair<std::string, engine::OptionValue>> values;
	for (const auto& entry : options.getOptionValues()) {
		if (entry.first != "engine.backend") {
			values.push_back(entry);
		}
	}
	std::ranges::sort(values, {}, [](const auto& entry) { return entry.first; });
	Writer writer;
	writer.u32(static_cast<uint32_t>(values.size()));
	for (const auto& [name, value] : values) {
		writer.string(name);
		std::visit(
		    [&](const auto& typedValue) {
			    using T = std::decay_t<decltype(typedValue)>;
			    if constexpr (std::is_same_v<T, int>) {
				    writer.u8(1);
				    writer.u64(static_cast<uint64_t>(static_cast<int64_t>(typedValue)));
			    } else if constexpr (std::is_same_v<T, double>) {
				    writer.u8(2);
				    writer.u64(std::bit_cast<uint64_t>(typedValue));
			    } else if constexpr (std::is_same_v<T, std::string>) {
				    writer.u8(3);
				    writer.string(typedValue);
			    } else if constexpr (std::is_same_v<T, bool>) {
				    writer.u8(4);
				    writer.u8(typedValue);
			    }
		    },
		    value);
	}
	return digest(writer.take());
}

} // namespace nautilus::artifact::detail

namespace nautilus::artifact {
namespace {
constexpr std::string_view MAGIC = "NAUTILUS-ARTIFACT-1";
}
std::string encode(const ModuleArtifact& value) {
	detail::validateDescriptor(value);
	detail::validatePayload(value.object, value.descriptor.objectDigest);
	detail::validatePayload(value.bytecode, value.descriptor.bytecodeDigest);
	detail::Writer writer(2 * detail::MAX_ARTIFACT_SIZE + detail::MAX_DESCRIPTOR_SIZE + 1024);
	writer.string(MAGIC);
	writer.string(detail::encodeDescriptor(value.descriptor));
	writer.string(value.descriptorDigest);
	writer.string(value.object);
	writer.string(value.bytecode);
	return writer.take();
}
ModuleArtifact decode(std::string_view bytes) {
	if (bytes.size() > 2 * detail::MAX_ARTIFACT_SIZE + detail::MAX_DESCRIPTOR_SIZE + 1024) {
		throw RuntimeException("Artifact envelope exceeds the transport limit");
	}
	detail::Reader reader(bytes);
	if (reader.string() != MAGIC) {
		throw RuntimeException("Invalid artifact magic");
	}
	ModuleArtifact value;
	value.descriptor = detail::decodeDescriptor(reader.string(detail::MAX_DESCRIPTOR_SIZE));
	value.descriptorDigest = reader.string();
	value.object = reader.string();
	value.bytecode = reader.string();
	reader.finish();
	detail::validateDescriptor(value);
	detail::validatePayload(value.object, value.descriptor.objectDigest);
	detail::validatePayload(value.bytecode, value.descriptor.bytecodeDigest);
	return value;
}
} // namespace nautilus::artifact
