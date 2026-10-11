#include "nautilus/tracing/phases/IRConversionSupport.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"

namespace nautilus::tracing {
using namespace compiler::ir;

OperationIdentifier createValueIdentifier(const TypedValueRef& val) {
	return {val.ref};
}

OperationIdentifier createValueIdentifier(const InputVariant& val) {
	if (const auto* valRef = std::get_if<TypedValueRef>(&val)) {
		return {valRef->ref};
	}
	throw NotImplementedException("wrong input variant");
}

/// Binds a traced body to the function-table entry its call sites minted.
///
/// A call to a Nautilus function is traced before that function's own body is,
/// so by the time we get here the id usually already exists -- keyed on the
/// definition pointer, never the name, so two functions sharing a name still
/// resolve to two entries. Functions nobody calls (the entry point) simply
/// intern here first.
void bindDefinition(compiler::ir::IRGraph& ir, TraceModule& traceModule, const std::string& functionName,
                    compiler::ir::FunctionOperation* functionOperation) {
	const auto* traceDefinition = traceModule.getFunctionDefinition(functionName);
	auto& table = ir.getFunctionTableMut();

	// One body can be denoted by several identities: a NautilusFunction may
	// share a name with a module-registered entry function, in which case the
	// trace loop traced the body once and recorded both. Call sites have
	// already minted an entry keyed on whichever identity they saw, so reuse
	// that entry rather than minting a second one -- two entries would carry
	// two emission names, and the call would look for a symbol the definition
	// was not emitted under.
	auto id = compiler::ir::INVALID_FUNCTION_ID;
	if (traceDefinition != nullptr) {
		for (const auto* definition : traceDefinition->definitions) {
			if (const auto existing = table.find(const_cast<void*>(definition));
			    existing != compiler::ir::INVALID_FUNCTION_ID) {
				id = existing;
				break;
			}
		}
	}

	if (id == compiler::ir::INVALID_FUNCTION_ID) {
		compiler::ir::CalleeDescriptor descriptor;
		descriptor.kind = compiler::ir::CalleeDescriptor::Kind::Internal;
		descriptor.customName = functionName;
		if (traceDefinition != nullptr && !traceDefinition->definitions.empty()) {
			descriptor.key = const_cast<void*>(traceDefinition->definitions.front());
		}
		id = ir.internCallee(descriptor);
	}

	// Point every remaining identity at that one entry.
	if (traceDefinition != nullptr) {
		for (const auto* definition : traceDefinition->definitions) {
			table.alias(const_cast<void*>(definition), id);
		}
	}
	ir.defineFunction(id, functionOperation);
}

size_t IRConversionSupport::RegionKeyHash::operator()(const RegionKey& key) const noexcept {
	auto hash = std::hash<const void*> {}(key.name);
	hash ^= std::hash<const void*> {}(key.file) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	hash ^= std::hash<uint64_t> {}((static_cast<uint64_t>(key.line) << 32) | key.column) + 0x9e3779b9 + (hash << 6) +
	        (hash >> 2);
	hash ^= std::hash<uint32_t> {}(key.parent) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	return hash;
}

compiler::ir::RegionIndex IRConversionSupport::mapRegion(RegionIndex traceRegion) {
	if (traceRegion == NO_REGION || traceRegion >= trace->getRegions().size()) {
		return compiler::ir::NO_REGION;
	}
	if (auto it = regionMap.find(traceRegion); it != regionMap.end()) {
		return it->second;
	}
	const auto& spec = trace->getRegions()[traceRegion];
	// Resolve the enclosing chain first: a call site is only the same call site as
	// another if it also sits in the same enclosing region.
	const auto parent = mapRegion(spec.parent);
	const RegionKey key {spec.attributes.name, spec.attributes.location.file, spec.attributes.location.line,
	                     spec.attributes.location.column, parent};
	compiler::ir::RegionIndex index;
	if (auto it = regionKeys.find(key); it != regionKeys.end()) {
		index = it->second;
	} else if (currentRegionSpecs.size() >= compiler::ir::NO_REGION) {
		// One short of the sentinel is the last usable index; beyond it an operation
		// would have to claim a region it does not belong to, so it claims none.
		return compiler::ir::NO_REGION;
	} else {
		index = static_cast<compiler::ir::RegionIndex>(currentRegionSpecs.size());
		currentRegionSpecs.push_back(compiler::ir::RegionSpec {spec.attributes, parent, ir->nextRegionId()});
		regionKeys[key] = index;
	}
	regionMap[traceRegion] = index;
	return index;
}

compiler::ir::OperationProvenance IRConversionSupport::provenanceOf(const TraceOperation& operation) {
	return {operation.tag.getTag(), mapRegion(operation.regionIndex)};
}

void IRConversionSupport::stampProvenance(compiler::ir::Operation& irOperation, const TraceOperation& operation) {
	const auto provenance = provenanceOf(operation);
	irOperation.setSourceTag(provenance.sourceTag);
	irOperation.setRegionIndex(provenance.region);
}

compiler::ir::FunctionId IRConversionSupport::internCallee(const FunctionCall& call, Type resultType,
                                                           const std::vector<Operation*>& arguments) {
	// Interning is idempotent, but building the descriptor is not free: it
	// copies three names and a parameter-type vector. Conversion walks every
	// call site, and a hot loop calls the same handful of functions over and
	// over, so resolve an already-known identity before paying for any of it.
	if (call.ptr != nullptr) {
		if (const auto existing = ir->getFunctionTable().find(call.ptr);
		    existing != compiler::ir::INVALID_FUNCTION_ID) {
			return existing;
		}
	}

	compiler::ir::CalleeDescriptor descriptor;
	descriptor.key = call.ptr;
	descriptor.resultType = resultType;
	descriptor.attrs = call.fnAttrs;
	descriptor.paramTypes.reserve(arguments.size());
	for (const auto* argument : arguments) {
		descriptor.paramTypes.push_back(argument->getStamp());
	}

	if (call.kind == CalleeKind::Internal) {
		// `ptr` is a NautilusFunctionDefinition, not code. Its name is one a
		// user chose, so it is a custom name rather than a symbol.
		descriptor.kind = compiler::ir::CalleeDescriptor::Kind::Internal;
		descriptor.customName = call.functionName;
	} else {
		descriptor.kind = compiler::ir::CalleeDescriptor::Kind::External;
		descriptor.mangledName = call.mangledName;
		// `functionName` is already rendered per the engine's naming options
		// (demangled, raw, or normalised), which is exactly the display name.
		descriptor.demangledName = call.functionName;
	}
	return ir->internCallee(descriptor);
}

std::vector<compiler::ir::AllocaSpec> IRConversionSupport::collectAllocaSpecs() const {
	std::vector<compiler::ir::AllocaSpec> specs;
	specs.reserve(trace->allocaSpecs.size());
	for (const auto& spec : trace->allocaSpecs) {
		specs.push_back({spec.size, spec.align});
	}
	return specs;
}

} // namespace nautilus::tracing
