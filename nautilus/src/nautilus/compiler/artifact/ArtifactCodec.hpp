#pragma once

#include "nautilus/Artifact.hpp"
#include <cstdint>
#include <span>
#include <string>
#include <string_view>

namespace nautilus::artifact::detail {

constexpr uint64_t MAX_DESCRIPTOR_SIZE = uint64_t {64} << 20U;
constexpr uint64_t MAX_ARTIFACT_SIZE = uint64_t {1} << 30U;
constexpr uint32_t MAX_RECORD_COUNT = 100000;

class Writer {
public:
	explicit Writer(uint64_t maximumSize = MAX_ARTIFACT_SIZE);
	void u8(uint8_t value);
	void u32(uint32_t value);
	void u64(uint64_t value);
	void bytes(std::span<const uint8_t> value);
	void string(std::string_view value);
	std::string take();

private:
	void require(std::size_t length) const;
	std::string data_;
	uint64_t maximumSize_;
};

class Reader {
public:
	explicit Reader(std::string_view data);
	uint8_t u8();
	uint32_t u32();
	uint64_t u64();
	uint32_t count();
	std::string string(uint64_t maximumSize = MAX_ARTIFACT_SIZE);
	std::span<const uint8_t> bytes(std::size_t length);
	void finish() const;

private:
	void require(std::size_t length) const;
	std::string_view data_;
	std::size_t position_ = 0;
};

std::string digest(std::string_view value);
bool isDigest(std::string_view value);
std::string encodeDescriptor(const Descriptor& descriptor);
Descriptor decodeDescriptor(std::string_view bytes);
void validateDescriptor(const ModuleArtifact& artifact);
void validatePayload(std::string_view bytes, std::string_view expectedDigest);
std::string optionsDigest(const engine::Options& options);

} // namespace nautilus::artifact::detail
