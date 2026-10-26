#include "nautilus/nir/Module.hpp"
#include <cstring>

namespace nautilus::nir {

uint32_t Module::internName(std::string_view s) {
	auto it = nameIndex_.find(std::string(s));
	if (it != nameIndex_.end()) {
		return it->second;
	}
	auto id = static_cast<uint32_t>(names.size());
	names.push_back(NameRecord {static_cast<uint32_t>(nameBytes.size()), static_cast<uint32_t>(s.size())});
	nameBytes.insert(nameBytes.end(), s.begin(), s.end());
	nameIndex_.emplace(std::string(s), id);
	return id;
}

std::string_view Module::name(uint32_t id) const {
	auto& n = names[id];
	return std::string_view(nameBytes.data() + n.offset, n.length);
}

uint32_t Module::internSignature(TypeWord result, std::span<const TypeWord> params) {
	std::vector<uint32_t> key;
	key.reserve(params.size() + 1);
	key.push_back(result);
	key.insert(key.end(), params.begin(), params.end());
	auto it = signatureIndex_.find(key);
	if (it != signatureIndex_.end()) {
		return it->second;
	}
	auto id = static_cast<uint32_t>(signatures.size());
	uint32_t offset = 0;
	if (!params.empty()) {
		offset = static_cast<uint32_t>(extra.size());
		extra.insert(extra.end(), params.begin(), params.end());
	}
	signatures.push_back(SignatureRecord {result, static_cast<uint32_t>(params.size()), offset});
	signatureIndex_.emplace(std::move(key), id);
	return id;
}

uint32_t Module::internCallee(const CalleeRecord& r) {
	std::array<uint32_t, 4> key {r.kind, r.target, r.signature, r.attrs};
	auto it = calleeIndex_.find(key);
	if (it != calleeIndex_.end()) {
		return it->second;
	}
	auto id = static_cast<uint32_t>(callees.size());
	callees.push_back(r);
	calleeIndex_.emplace(key, id);
	return id;
}

uint32_t Module::internPointer(const PointerRecord& r) {
	std::array<uint64_t, 4> key {static_cast<uint64_t>(r.kind) | (static_cast<uint64_t>(r.mode) << 8) |
	                                 (static_cast<uint64_t>(r.provenance) << 16),
	                             r.nameId, r.address, r.reserved};
	auto it = pointerIndex_.find(key);
	if (it != pointerIndex_.end()) {
		return it->second;
	}
	auto id = static_cast<uint32_t>(pointers.size());
	pointers.push_back(r);
	pointerIndex_.emplace(key, id);
	return id;
}

uint32_t Module::addBinding(uint32_t nameId, TypeId type, uint32_t size, uint32_t align) {
	bindings.push_back(BindingRecord {nameId, static_cast<uint32_t>(type), size, align});
	return static_cast<uint32_t>(bindings.size() - 1);
}

uint32_t Module::internData(std::span<const std::byte> bytes, uint32_t align) {
	auto key = std::make_pair(align, std::vector<std::byte>(bytes.begin(), bytes.end()));
	auto it = dataIndex_.find(key);
	if (it != dataIndex_.end()) {
		return it->second;
	}
	// contents are aligned to `align` inside dataBytes (§4.10)
	size_t offset = (dataBytes.size() + align - 1) / align * align;
	dataBytes.resize(offset, std::byte {0});
	dataBytes.insert(dataBytes.end(), bytes.begin(), bytes.end());
	auto id = static_cast<uint32_t>(data.size());
	data.push_back(DataRecord {static_cast<uint32_t>(bytes.size()), align, static_cast<uint32_t>(offset), 0});
	dataIndex_.emplace(std::move(key), id);
	return id;
}

uint32_t Module::addImport(const ImportRecord& r) {
	imports.push_back(r);
	return static_cast<uint32_t>(imports.size() - 1);
}

uint32_t Module::addExtra(std::span<const uint32_t> words) {
	auto offset = static_cast<uint32_t>(extra.size());
	extra.insert(extra.end(), words.begin(), words.end());
	return offset;
}

std::span<const Inst> Module::functionInsts(uint32_t f) const {
	auto& fn = functions[f];
	return std::span<const Inst>(insts).subspan(fn.firstInst, fn.instCount);
}

std::span<Inst> Module::functionInsts(uint32_t f) {
	auto& fn = functions[f];
	return std::span<Inst>(insts).subspan(fn.firstInst, fn.instCount);
}

std::span<const BlockRecord> Module::functionBlocks(uint32_t f) const {
	auto& fn = functions[f];
	return std::span<const BlockRecord>(blocks).subspan(fn.firstBlock, fn.blockCount);
}

std::span<const AllocaRecord> Module::functionAllocas(uint32_t f) const {
	auto& fn = functions[f];
	return std::span<const AllocaRecord>(allocas).subspan(fn.firstAlloca, fn.allocaCount);
}

} // namespace nautilus::nir
