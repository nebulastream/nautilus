#include "nautilus/RuntimeBindingInfo.hpp"
#include <stdexcept>
#include <utility>

namespace nautilus {

namespace {

void appendField(std::string& schema, const std::string& field) {
	schema += std::to_string(field.size()) + ":" + field;
}

} // namespace

std::shared_ptr<const runtime_binding::Entry> RuntimeBindings::add(std::string identity, std::string type,
                                                                   void* address) {
	if (identity.empty() || address == nullptr) {
		throw std::invalid_argument("Runtime binding requires a nonempty identity and nonnull address");
	}
	if (entries_.contains(identity)) {
		throw std::invalid_argument("Duplicate runtime binding identity: " + identity);
	}
	std::string symbol = "__nautilus_binding_";
	constexpr char digits[] = "0123456789abcdef";
	for (const unsigned char character : identity) {
		symbol.push_back(digits[character >> 4U]);
		symbol.push_back(digits[character & 15U]);
	}
	auto entry = std::make_shared<const runtime_binding::Entry>(
	    runtime_binding::Entry {std::move(identity), std::move(type), std::move(symbol), address});
	entries_.emplace(entry->identity, entry);
	return entry;
}

std::string RuntimeBindings::schema() const {
	std::string schema = "nautilus.runtime-bindings:";
	appendField(schema, std::to_string(entries_.size()));
	for (const auto& [identity, entry] : entries_) {
		appendField(schema, identity);
		appendField(schema, entry->type);
		appendField(schema, entry->symbol);
	}
	return schema;
}

} // namespace nautilus
