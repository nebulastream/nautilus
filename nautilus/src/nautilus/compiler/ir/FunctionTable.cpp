#include "nautilus/compiler/ir/FunctionTable.hpp"
#include "nautilus/compiler/ir/IntrinsicRegistry.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <cassert>
#include <fmt/format.h>

namespace nautilus::compiler::ir {

FunctionTarget::FunctionTarget(FunctionId id, FunctionName name, FunctionOperation* definition)
    : id_(id), name_(std::move(name)), payload_(definition) {
}

FunctionTarget::FunctionTarget(FunctionId id, FunctionName name, NativeTarget native)
    : id_(id), name_(std::move(name)), payload_(std::move(native)) {
}

Linkage FunctionTarget::getLinkage() const {
	if (std::holds_alternative<FunctionOperation*>(payload_)) {
		return Linkage::Internal;
	}
	const auto& native = std::get<NativeTarget>(payload_);
	return native.intrinsic == IntrinsicId::None ? Linkage::External : Linkage::Intrinsic;
}

FunctionOperation* FunctionTarget::getDefinition() const {
	const auto* definition = std::get_if<FunctionOperation*>(&payload_);
	return definition != nullptr ? *definition : nullptr;
}

const NativeTarget* FunctionTarget::getNative() const {
	return std::get_if<NativeTarget>(&payload_);
}

NativeTarget* FunctionTarget::getNativeMut() {
	return std::get_if<NativeTarget>(&payload_);
}

void FunctionTarget::setDefinition(FunctionOperation* definition) {
	assert(std::holds_alternative<FunctionOperation*>(payload_) &&
	       "only an internal target can be bound to a definition");
	payload_ = definition;
}

Type FunctionTarget::getResultType() const {
	if (auto* definition = getDefinition(); definition != nullptr) {
		return definition->getOutputArg();
	}
	if (const auto* native = getNative(); native != nullptr) {
		return native->resultType;
	}
	// An internal target whose body has not been traced yet. The verifier
	// rejects this once conversion has finished; until then there is nothing
	// truthful to report.
	return Type::v;
}

std::vector<Type> FunctionTarget::getParamTypes() const {
	if (auto* definition = getDefinition(); definition != nullptr) {
		// Trace-generated functions carry their parameters as entry-block
		// arguments, not in FunctionOperation::getInputArgs() -- which the
		// conversion phase leaves empty. Every backend already reads the block
		// arguments; read the same thing here so a call site is checked against
		// what the body actually takes.
		std::vector<Type> params;
		const auto& blockArgs = definition->getFunctionBasicBlock().getArguments();
		params.reserve(blockArgs.size());
		for (const auto* argument : blockArgs) {
			params.push_back(argument->getStamp());
		}
		return params;
	}
	if (const auto* native = getNative(); native != nullptr) {
		return native->paramTypes;
	}
	return {};
}

FunctionAttributes FunctionTarget::getAttributes() const {
	if (const auto* native = getNative(); native != nullptr) {
		return native->attrs;
	}
	// An internal function declares nothing at its call sites -- every one of
	// them passes a default-constructed FunctionAttributes. What it actually
	// does is derived from its own body, which is the one case where that is
	// possible at all.
	return derivedAttrs_;
}

void FunctionTarget::setDerivedAttributes(FunctionAttributes attrs) {
	derivedAttrs_ = attrs;
}

void* FunctionTarget::getAddress() const {
	const auto* native = getNative();
	return native != nullptr ? native->address : nullptr;
}

IntrinsicId FunctionTarget::getIntrinsic() const {
	const auto* native = getNative();
	return native != nullptr ? native->intrinsic : IntrinsicId::None;
}

FunctionId FunctionTable::find(void* key) const {
	const auto it = byKey_.find(key);
	return it != byKey_.end() ? it->second : INVALID_FUNCTION_ID;
}

std::string FunctionTable::mintEmissionName(const FunctionName& name) {
	std::string candidate = sanitizeIdentifier(name.get());
	if (candidate.empty()) {
		// Nothing usable in any of the provenance fields -- fall back to the
		// minted name, which is an identifier by construction.
		candidate = name.getMinted();
	}
	if (candidate.empty()) {
		candidate = "fn";
	}

	// Uniquify. Two distinct callees can sanitise to the same spelling (two
	// overloads, or two names differing only in punctuation), and no single
	// backend could ever detect that -- which is exactly why this happens
	// here, once, for all of them.
	auto [it, inserted] = emissionNameUses_.try_emplace(candidate, 1);
	if (inserted) {
		return candidate;
	}
	std::string unique;
	do {
		++it->second;
		unique = candidate + "_" + std::to_string(it->second);
	} while (emissionNameUses_.contains(unique));
	emissionNameUses_.emplace(unique, 1);
	return unique;
}

FunctionId FunctionTable::intern(const CalleeDescriptor& descriptor) {
	if (descriptor.key != nullptr) {
		if (const auto existing = find(descriptor.key); existing != INVALID_FUNCTION_ID) {
			return existing;
		}
	}

	const auto id = static_cast<FunctionId>(targets_.size());

	FunctionName name;
	name.setMangled(descriptor.mangledName);
	name.setDemangled(descriptor.demangledName);
	name.setCustom(descriptor.customName);
	// The minted fallback only has to exist when nothing else can name this
	// callee -- get() reaches it last. Skipping it otherwise avoids an
	// allocation per entry on the trace-to-IR path.
	if (descriptor.mangledName.empty() && descriptor.demangledName.empty() && descriptor.customName.empty()) {
		name.setMinted("fn" + std::to_string(id));
	}
	name.setEmission(mintEmissionName(name));

	if (descriptor.kind == CalleeDescriptor::Kind::Internal) {
		targets_.emplace_back(id, std::move(name), static_cast<FunctionOperation*>(nullptr));
	} else {
		NativeTarget native;
		native.address = descriptor.key;
		// Resolve the linkage now, once, rather than leaving every backend to
		// pattern-match the address at every call site it lowers. A registered
		// address keeps its address either way -- an intrinsic is always a
		// valid external call as well, which is what a backend with no handler
		// for it falls back to.
		native.intrinsic = IntrinsicRegistry::instance().lookup(descriptor.key);
		native.resultType = descriptor.resultType;
		native.paramTypes = descriptor.paramTypes;
		native.attrs = descriptor.attrs;
		targets_.emplace_back(id, std::move(name), std::move(native));
	}

	if (descriptor.key != nullptr) {
		byKey_.emplace(descriptor.key, id);
	}
	return id;
}

FunctionId FunctionTable::findByDefinition(const FunctionOperation* definition) const {
	const auto it = byDefinition_.find(definition);
	return it != byDefinition_.end() ? it->second : INVALID_FUNCTION_ID;
}

void FunctionTable::alias(void* key, FunctionId id) {
	if (key == nullptr) {
		return;
	}
	assert(contains(id) && "alias() called with an id this table never minted");
	byKey_[key] = id;
}

void FunctionTable::define(FunctionId id, FunctionOperation* definition) {
	assert(contains(id) && "define() called with an id this table never minted");
	targets_[id].setDefinition(definition);
	byDefinition_.emplace(definition, id);
}

const FunctionTarget& FunctionTable::get(FunctionId id) const {
	// Throw rather than assert: an unresolved id in a release build would
	// otherwise index past the end of the vector and hand a backend a garbage
	// target, which is precisely the failure mode this table exists to remove.
	if (!contains(id)) {
		throw RuntimeException(fmt::format("unresolved FunctionId {} -- no such entry in the module function table",
		                                   static_cast<uint32_t>(id)));
	}
	return targets_[id];
}

FunctionTarget& FunctionTable::getMut(FunctionId id) {
	if (!contains(id)) {
		throw RuntimeException(fmt::format("unresolved FunctionId {} -- no such entry in the module function table",
		                                   static_cast<uint32_t>(id)));
	}
	return targets_[id];
}

} // namespace nautilus::compiler::ir
