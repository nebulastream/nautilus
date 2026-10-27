#include "nautilus/nir/ExtensionIds.hpp"
#include "nautilus/extensions/ExtensionRegistry.hpp"
#include <map>

namespace nautilus::nir {

namespace {

constexpr uint8_t FIRST_EXTENSION_TYPE = 16;

/// Calls `onOp` for every extension opcode and `onType` for every extension type id that `m` holds, in a fixed order
/// (signatures, constants, bindings, instructions); both take a reference and, with `mutate`, may change it. The shape
/// of an extension instruction's payload is that of the process opcode, which is the new one if `shapeFromNew`.
template <class OpF, class TypeF>
void visitIds(Module& m, bool mutate, bool shapeFromNew, OpF onOp, TypeF onType) {
	auto& registry = ext::ExtensionRegistry::instance();
	auto word = [&](uint32_t& w) {
		auto t = static_cast<uint8_t>(w & 0xFF);
		if (t >= FIRST_EXTENSION_TYPE) {
			onType(t);
			if (mutate) {
				w = (w & ~0xFFu) | t;
			}
		}
	};
	for (auto& s : m.signatures) {
		word(s.result);
		for (uint32_t k = 0; k < s.paramCount && s.paramTypes + k < m.extra.size(); ++k) {
			word(m.extra[s.paramTypes + k]);
		}
	}
	{
		std::vector<ConstRecord> records(m.consts.records().begin(), m.consts.records().end());
		for (auto& c : records) {
			if (c.type >= FIRST_EXTENSION_TYPE) {
				onType(c.type);
			}
		}
		if (mutate) {
			m.consts.reset(std::move(records), {m.consts.bytes().begin(), m.consts.bytes().end()});
		}
	}
	for (auto& b : m.bindings) {
		word(b.type);
	}
	for (auto& inst : m.insts) {
		if (inst.op >= FIRST_EXTENSION_OP) {
			auto oldOp = inst.op;
			auto op = oldOp;
			onOp(op);
			if (mutate) {
				inst.op = op;
			}
			auto* info = registry.op(shapeFromNew ? op : oldOp);
			if (info != nullptr && info->shape.c == ext::FieldKind::List && inst.c < m.extra.size()) {
				uint32_t n = m.extra[inst.c];
				for (uint32_t k = 0; k < n && inst.c + 1 + k < m.extra.size(); ++k) {
					auto& shape = info->shape;
					auto kind = k < shape.prefix.size() ? shape.prefix[k]
					            : shape.group.empty()   ? ext::WordKind::Imm
					                                    : shape.group[(k - shape.prefix.size()) % shape.group.size()];
					if (kind == ext::WordKind::Type) {
						auto t = static_cast<uint8_t>(m.extra[inst.c + 1 + k] & 0xFF);
						if (t >= FIRST_EXTENSION_TYPE) {
							onType(t);
							if (mutate) {
								m.extra[inst.c + 1 + k] = t;
							}
						}
					}
				}
			}
		}
		if (inst.type >= FIRST_EXTENSION_TYPE) {
			auto t = inst.type;
			onType(t);
			if (mutate) {
				inst.type = t;
			}
		}
	}
}

std::string withVersion(const std::string& name, uint32_t version) {
	return name + "@" + std::to_string(version);
}

std::string bareName(std::string_view full) {
	std::string_view bare;
	uint32_t version;
	return std::string(ext::splitNameVersion(full, bare, version) ? bare : full);
}

} // namespace

std::vector<ExtensionEntry> extensionEntries(const Module& m) {
	auto& registry = ext::ExtensionRegistry::instance();
	std::vector<ExtensionEntry> entries;
	std::map<std::pair<uint8_t, std::string>, bool> known;
	for (auto& e : m.extensions) {
		auto name = std::string(m.name(e.nameId));
		known[{e.kind, bareName(name)}] = true;
		entries.push_back({std::move(name), static_cast<ExtensionKind>(e.kind)});
	}
	// scanning does not modify the module
	visitIds(
	    const_cast<Module&>(m), false, false,
	    [&](uint16_t& op) {
		    if (auto* info = registry.op(op)) {
			    if (known
			            .emplace(std::pair<uint8_t, std::string> {static_cast<uint8_t>(ExtensionKind::Operation),
			                                                      info->name},
			                     true)
			            .second) {
				    entries.push_back({withVersion(info->name, info->version), ExtensionKind::Operation});
			    }
		    }
	    },
	    [&](uint8_t& type) {
		    if (auto* info = registry.type(type)) {
			    if (known
			            .emplace(
			                std::pair<uint8_t, std::string> {static_cast<uint8_t>(ExtensionKind::Type), info->name},
			                true)
			            .second) {
				    entries.push_back({withVersion(info->name, info->version), ExtensionKind::Type});
			    }
		    }
	    });
	return entries;
}

Module toImageIds(const Module& source) {
	auto& registry = ext::ExtensionRegistry::instance();
	auto entries = extensionEntries(source);
	Module m = source;
	// position of each operation and type entry among the entries of its kind
	std::map<std::string, uint32_t> opRank, typeRank;
	uint32_t ops = 0, types = 0;
	for (auto& e : entries) {
		auto name = bareName(e.name);
		if (e.kind == ExtensionKind::Operation) {
			opRank.emplace(name, ops++);
		} else if (e.kind == ExtensionKind::Type) {
			typeRank.emplace(name, types++);
		}
	}
	visitIds(
	    m, true, false,
	    [&](uint16_t& op) {
		    if (auto* info = registry.op(op)) {
			    op = static_cast<uint16_t>(FIRST_EXTENSION_OP + opRank.at(info->name));
		    }
	    },
	    [&](uint8_t& type) {
		    if (auto* info = registry.type(type)) {
			    type = static_cast<uint8_t>(FIRST_EXTENSION_TYPE + typeRank.at(info->name));
		    }
	    });
	m.extensions.clear();
	for (auto& e : entries) {
		m.extensions.push_back(ExtensionRecord {m.internName(e.name), static_cast<uint8_t>(e.kind), {}});
	}
	return m;
}

std::optional<IdError> toProcessIds(Module& m) {
	auto& registry = ext::ExtensionRegistry::instance();
	std::vector<uint16_t> ops;
	std::vector<uint8_t> types;
	for (auto& e : m.extensions) {
		auto full = std::string(m.name(e.nameId));
		std::string_view bare;
		uint32_t version = 0;
		if (!ext::splitNameVersion(full, bare, version)) {
			return IdError {DiagCode::L010, "the extension '" + full + "' has no version"};
		}
		std::string name(bare);
		uint32_t current = 0, oldest = 0;
		switch (static_cast<ExtensionKind>(e.kind)) {
		case ExtensionKind::Operation: {
			auto id = registry.findOp(name);
			if (!id) {
				return IdError {DiagCode::L010, "the extension operation '" + name + "' is not registered"};
			}
			current = registry.op(*id)->version;
			oldest = registry.op(*id)->minCompatibleVersion;
			ops.push_back(*id);
			break;
		}
		case ExtensionKind::Type: {
			auto id = registry.findType(name);
			if (!id) {
				return IdError {DiagCode::L010, "the extension type '" + name + "' is not registered"};
			}
			current = registry.type(*id)->version;
			oldest = registry.type(*id)->minCompatibleVersion;
			types.push_back(*id);
			break;
		}
		case ExtensionKind::Intrinsic: {
			auto id = registry.findIntrinsic(name);
			if (!id) {
				return IdError {DiagCode::L010, "the intrinsic '" + name + "' is not registered"};
			}
			current = registry.intrinsic(*id)->version;
			oldest = registry.intrinsic(*id)->minCompatibleVersion;
			break;
		}
		default:
			return IdError {DiagCode::L010, "the extension '" + full + "' has an unknown kind"};
		}
		if (version < oldest || version > current) {
			return IdError {DiagCode::L011, "version " + std::to_string(version) + " of '" + name +
			                                    "' is outside the accepted range " + std::to_string(oldest) + " to " +
			                                    std::to_string(current)};
		}
	}
	std::optional<IdError> error;
	visitIds(
	    m, true, true,
	    [&](uint16_t& op) {
		    uint32_t rank = op - FIRST_EXTENSION_OP;
		    if (rank < ops.size()) {
			    op = ops[rank];
		    } else if (!error) {
			    error = IdError {DiagCode::L010, "opcode " + std::to_string(op) + " is outside the extensions section"};
		    }
	    },
	    [&](uint8_t& type) {
		    uint32_t rank = type - FIRST_EXTENSION_TYPE;
		    if (rank < types.size()) {
			    type = types[rank];
		    } else if (!error) {
			    error = IdError {DiagCode::L010, "type " + std::to_string(type) + " is outside the extensions section"};
		    }
	    });
	if (error) {
		return error;
	}
	// an intrinsic callee has the registered signature
	for (uint32_t c = 0; c < m.callees.size(); ++c) {
		auto& callee = m.callees[c];
		if (callee.kind != static_cast<uint8_t>(CalleeKind::Intrinsic) || callee.target >= m.extensions.size() ||
		    callee.signature >= m.signatures.size()) {
			continue;
		}
		auto name = bareName(m.name(m.extensions[callee.target].nameId));
		auto id = registry.findIntrinsic(name);
		if (!id) {
			continue;
		}
		auto* info = registry.intrinsic(*id);
		auto& s = m.signatures[callee.signature];
		bool same = info->signature.size() == s.paramCount + 1 && info->signature[0] == s.result;
		for (uint32_t k = 0; same && k < s.paramCount; ++k) {
			same = s.paramTypes + k < m.extra.size() && info->signature[1 + k] == m.extra[s.paramTypes + k];
		}
		if (!same) {
			return IdError {DiagCode::L012, "callee #c" + std::to_string(c) + ": the signature of the intrinsic '" +
			                                    name + "' differs from the registered one"};
		}
	}
	return std::nullopt;
}

} // namespace nautilus::nir
