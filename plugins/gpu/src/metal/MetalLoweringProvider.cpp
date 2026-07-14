
#include "MetalLoweringProvider.hpp"
#include <cassert>
#include <regex>
#include <string>
#include <utility>

namespace nautilus::compiler::metal {

// ============================================================================
// MetalLoweringProvider
// ============================================================================

MetalLoweringResult MetalLoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir, const engine::Options& /*options*/) {
	MetalLoweringResult result;
	// Device code (MSL). The address-space qualifier on every pointer cast/decl
	// is applied per function inside process() (see rewriteAddressSpaces).
	auto deviceCtx = DeviceContext(ir);
	result.deviceCode = deviceCtx.process().str();
	// Host code (C++ with Metal API)
	auto hostCtx = HostContext(std::move(ir));
	result.hostCode = hostCtx.process().str();
	return result;
}

// ============================================================================
// DeviceContext — generates .metal MSL kernel code
// ============================================================================

MetalLoweringProvider::DeviceContext::DeviceContext(std::shared_ptr<ir::IRGraph> irGraph) {
	ir = std::move(irGraph);
	registerGPUIntrinsics();
}

void MetalLoweringProvider::DeviceContext::registerGPUIntrinsics() {
	auto regDevice = [&](void* ptr, const std::string& expr) {
		gpu::registerNullaryIntrinsic(gpuIntrinsics, ptr, expr, "uint");
		deviceIntrinsics.insert(ptr);
	};
	regDevice(reinterpret_cast<void*>(nautilus_gpu_thread_idx_x), "nautilus_threadIdx.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_thread_idx_y), "nautilus_threadIdx.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_thread_idx_z), "nautilus_threadIdx.z");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_idx_x), "nautilus_blockIdx.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_idx_y), "nautilus_blockIdx.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_idx_z), "nautilus_blockIdx.z");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_dim_x), "nautilus_blockDim.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_dim_y), "nautilus_blockDim.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_block_dim_z), "nautilus_blockDim.z");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_grid_dim_x), "nautilus_gridDim.x");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_grid_dim_y), "nautilus_gridDim.y");
	regDevice(reinterpret_cast<void*>(nautilus_gpu_grid_dim_z), "nautilus_gridDim.z");
	auto syncPtr = reinterpret_cast<void*>(nautilus_gpu_sync_threads);
	gpu::registerVoidIntrinsic(gpuIntrinsics, syncPtr, "threadgroup_barrier(mem_flags::mem_threadgroup)");
	deviceIntrinsics.insert(syncPtr);

	// Block-shared memory: emit a `threadgroup` array sized by the constant byte
	// argument and return its address. The result pointer (and everything
	// derived from it) is retagged to the threadgroup address space by the
	// address-space pass in process(); see analyzeThreadgroupVars().
	auto sharedPtr = reinterpret_cast<void*>(nautilus_gpu_shared_alloc);
	gpuIntrinsics[sharedPtr] = [](ir::ProxyCallOperation* call, short blockIndex, gpu::RegisterFrame& frame,
	                              std::stringstream& blockArguments, std::vector<std::stringstream>& blocks,
	                              std::string (*getVariable)(const ir::OperationIdentifier&)) -> bool {
		auto inputs = call->getInputArguments();
		if (inputs.empty() || inputs[0]->getOperationType() != ir::Operation::OperationType::ConstIntOp) {
			// Emitting a zero-length threadgroup array would silently miscompile.
			throw NotImplementedException("nautilus_gpu_shared_alloc requires a compile-time constant byte size");
		}
		auto bytes = static_cast<uint64_t>(ir::as<ir::ConstIntOperation>(inputs[0])->getValue());
		auto resultVar = getVariable(call->getIdentifier());
		auto bufName = "nautilus_shared_" + std::to_string(call->getIdentifier().getId());
		blockArguments << "threadgroup uchar " << bufName << "[" << bytes << "];\n";
		if (!frame.contains(call->getIdentifier())) {
			// Declared as device here; rewritten to threadgroup by the
			// address-space pass (single source of truth for the retag).
			blockArguments << "device uchar* " << resultVar << ";\n";
			frame.setValue(call->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = " << bufName << ";\n";
		return true;
	};
	deviceIntrinsics.insert(sharedPtr);

	// Launch config intrinsics are host-side only — not registered here.
}

std::string MetalLoweringProvider::DeviceContext::getType(const Type& stamp) {
	switch (stamp) {
	case Type::v:
		return "void";
	case Type::b:
		return "bool";
	case Type::i8:
		return "char";
	case Type::i16:
		return "short";
	case Type::i32:
		return "int";
	case Type::i64:
		return "long";
	case Type::ui8:
		return "uchar";
	case Type::ui16:
		return "ushort";
	case Type::ui32:
		return "uint";
	case Type::ui64:
		return "ulong";
	case Type::f32:
		return "float";
	case Type::f64:
		return "float"; // Metal lacks native double
	case Type::ptr:
		return "device uchar*";
	}
	assert(false);
	return "unknown";
}

MetalLoweringProvider::DeviceContext::Code MetalLoweringProvider::DeviceContext::process() {
	Code code;
	code << "#include <metal_stdlib>\nusing namespace metal;\n\n";

	const auto& functionOperations = ir->getFunctionOperations();
	classifyKernelFunctions();

	auto emitFunction = [&](const ir::FunctionOperation& func) {
		resetFunctionState();
		setupFunctionAllocaSlots(func);
		gpu::RegisterFrame rootFrame;
		const auto& functionBasicBlock = func.getFunctionBasicBlock();

		bool isKernel = kernelFunctions.contains(func.getName());
		std::vector<std::string> arguments;
		// Mutable copies of scalar kernel args (see below).
		std::stringstream scalarArgDecls;
		std::stringstream scalarArgInit;
		for (auto i = 0ull; i < functionBasicBlock.getArguments().size(); i++) {
			auto argument = functionBasicBlock.getArguments()[i];
			auto var = getVariable(argument->getIdentifier());
			rootFrame.setValue(argument->getIdentifier(), var);
			if (isKernel) {
				if (argument->getStamp() == Type::ptr) {
					arguments.emplace_back("device uchar* " + var + " [[buffer(" + std::to_string(i) + ")]]");
				} else {
					// Scalar args are passed as `constant T&` (const). If such a
					// value is live across control flow it becomes an SSA phi and
					// the trampoline reassigns it (`var = temp;`), which is illegal
					// on a const reference. Bind the buffer to a `_arg` parameter
					// and copy it into a mutable local of the SSA name, so any
					// later reassignment is well-formed.
					auto type = getType(argument->getStamp());
					arguments.emplace_back("constant " + type + "& " + var + "_arg [[buffer(" + std::to_string(i) +
					                       ")]]");
					scalarArgDecls << type << " " << var << ";\n";
					scalarArgInit << var << " = " << var << "_arg;\n";
				}
			} else {
				arguments.emplace_back(getType(argument->getStamp()) + " " + var);
			}
		}
		processBlock(&functionBasicBlock, rootFrame);

		// Determine which pointer variables live in threadgroup (shared) memory
		// so the address-space pass can retag their declarations and accesses.
		auto tgPtrs = analyzeThreadgroupPtrs(func);
		std::unordered_set<std::string> tgVars;
		for (const auto* op : tgPtrs) {
			tgVars.insert(getVariable(op->getIdentifier()));
		}

		// Build the function into a local buffer, then run the address-space
		// rewrite over it (per function, so var names don't collide).
		Code fnCode;
		if (isKernel) {
			fnCode << "kernel void " << func.getName() << "(\n";
			for (size_t i = 0; i < arguments.size(); i++) {
				fnCode << "    " << arguments[i] << ",\n";
			}
			fnCode << "    uint3 nautilus_threadIdx [[thread_position_in_threadgroup]],\n";
			fnCode << "    uint3 nautilus_blockIdx [[threadgroup_position_in_grid]],\n";
			fnCode << "    uint3 nautilus_blockDim [[threads_per_threadgroup]],\n";
			fnCode << "    uint3 nautilus_gridDim [[threadgroups_per_grid]]\n";
		} else {
			fnCode << getType(func.getOutputArg()) << " " << func.getName() << "(";
			for (size_t i = 0; i < arguments.size(); i++) {
				if (i != 0)
					fnCode << ",";
				fnCode << arguments[i] << " ";
			}
		}
		fnCode << ") {\n" << scalarArgDecls.str() << blockArguments.str() << scalarArgInit.str();
		if (blocks.size() > 1) {
			// Multi-block: emit while(true)/switch dispatch for Metal (no goto/labels).
			fnCode << "int __pc = 0;\nwhile (true) {\nswitch (__pc) {\n";
			for (size_t bi = 0; bi < blocks.size(); bi++) {
				auto blockStr = blocks[bi].str();
				// Strip the "Block_X:\n" label from the beginning of each block
				auto nlPos = blockStr.find('\n');
				if (nlPos != std::string::npos) {
					blockStr = blockStr.substr(nlPos + 1);
				}
				// Replace "goto Block_X;" with "__pc = X; continue;"
				// We use continue (not break) to restart the while loop.
				std::regex gotoRe(R"(goto (Block_\w+);)");
				std::smatch m;
				std::string transformed;
				std::string remaining = blockStr;
				while (std::regex_search(remaining, m, gotoRe)) {
					transformed += m.prefix().str();
					// Extract the block index from the label name
					auto targetLabel = m[1].str();
					// Find which case index this label corresponds to
					for (size_t ti = 0; ti < blocks.size(); ti++) {
						auto blockHead = blocks[ti].str();
						if (blockHead.find(targetLabel + ":\n") == 0) {
							transformed += "__pc = " + std::to_string(ti) + "; continue;";
							break;
						}
					}
					remaining = m.suffix().str();
				}
				transformed += remaining;
				fnCode << "case " << bi << ": {\n" << transformed << "}\n";
			}
			fnCode << "}\n}\n"; // close switch + while
		} else {
			// Single block: just strip the label
			for (auto& block : blocks) {
				auto blockStr = block.str();
				auto nlPos = blockStr.find('\n');
				if (nlPos != std::string::npos) {
					blockStr = blockStr.substr(nlPos + 1);
				}
				fnCode << blockStr << "\n";
			}
		}
		fnCode << "}\n\n";
		code << rewriteAddressSpaces(fnCode.str(), tgVars);
	};

	// Emit non-root functions that are kernels (use device intrinsics)
	for (size_t i = 1; i < functionOperations.size(); i++) {
		if (kernelFunctions.contains(functionOperations[i]->getName())) {
			emitFunction(*functionOperations[i]);
		}
	}
	// Emit root only if it's a kernel
	if (!functionOperations.empty()) {
		const auto& rootFunc = *functionOperations[0];
		if (kernelFunctions.contains(rootFunc.getName())) {
			emitFunction(rootFunc);
		}
	}

	return code;
}

std::unordered_set<const ir::Operation*>
MetalLoweringProvider::DeviceContext::analyzeThreadgroupPtrs(const ir::FunctionOperation& func) {
	using OT = ir::Operation::OperationType;
	std::unordered_set<const ir::Operation*> tg;

	// Collect all blocks reachable from the entry via terminator invocations.
	std::vector<const ir::BasicBlock*> allBlocks;
	std::unordered_set<const ir::BasicBlock*> seen;
	std::vector<const ir::BasicBlock*> worklist {&func.getFunctionBasicBlock()};
	while (!worklist.empty()) {
		const auto* b = worklist.back();
		worklist.pop_back();
		if (b == nullptr || !seen.insert(b).second) {
			continue;
		}
		allBlocks.push_back(b);
		for (auto* op : b->getOperations()) {
			if (op->getOperationType() == OT::BranchOp) {
				worklist.push_back(ir::as<ir::BranchOperation>(op)->getNextBlockInvocation().getBlock());
			} else if (op->getOperationType() == OT::IfOp) {
				auto* io = ir::as<ir::IfOperation>(op);
				worklist.push_back(io->getTrueBlockInvocation().getBlock());
				worklist.push_back(io->getFalseBlockInvocation().getBlock());
			}
		}
	}

	// Seed: results of nautilus_gpu_shared_alloc proxy calls.
	auto sharedPtr = reinterpret_cast<void*>(nautilus_gpu_shared_alloc);
	for (const auto* b : allBlocks) {
		for (auto* op : b->getOperations()) {
			if (op->getOperationType() == OT::ProxyCallOp &&
			    ir::as<ir::ProxyCallOperation>(op)->getFunctionPtr() == sharedPtr) {
				tg.insert(op);
			}
		}
	}

	// Propagate to a fixpoint: a pointer-typed op whose input is threadgroup is
	// threadgroup (pointer arithmetic, casts); block arguments inherit the space
	// of the value passed to them by each predecessor's invocation (phis).
	bool changed = true;
	while (changed) {
		changed = false;
		auto markTarget = [&](const ir::BasicBlockInvocation& inv) {
			auto srcArgs = inv.getArguments();
			const auto& tgtArgs = inv.getBlock()->getArguments();
			for (size_t i = 0; i < srcArgs.size() && i < tgtArgs.size(); i++) {
				if (tg.contains(srcArgs[i]) && !tg.contains(tgtArgs[i])) {
					tg.insert(tgtArgs[i]);
					changed = true;
				}
			}
		};
		for (const auto* b : allBlocks) {
			for (auto* op : b->getOperations()) {
				if (op->getStamp() == Type::ptr && !tg.contains(op)) {
					for (auto* in : op->getInputs()) {
						if (in != nullptr && tg.contains(in)) {
							tg.insert(op);
							changed = true;
							break;
						}
					}
				}
				if (op->getOperationType() == OT::BranchOp) {
					markTarget(ir::as<ir::BranchOperation>(op)->getNextBlockInvocation());
				} else if (op->getOperationType() == OT::IfOp) {
					auto* io = ir::as<ir::IfOperation>(op);
					markTarget(io->getTrueBlockInvocation());
					markTarget(io->getFalseBlockInvocation());
				}
			}
		}
	}
	return tg;
}

std::string MetalLoweringProvider::DeviceContext::rewriteAddressSpaces(const std::string& text,
                                                                       const std::unordered_set<std::string>& tgVars) {
	// Iterator-based conditional replacement that copies the gaps between matches
	// verbatim (preserving all whitespace), so kernels with no shared memory are
	// byte-identical to the plain device-qualified output.
	auto replaceAll = [](const std::string& in, const std::regex& re,
	                     const std::function<std::string(const std::smatch&)>& fn) {
		std::string out;
		size_t last = 0;
		for (auto it = std::sregex_iterator(in.begin(), in.end(), re); it != std::sregex_iterator(); ++it) {
			const auto& m = *it;
			out += in.substr(last, static_cast<size_t>(m.position()) - last);
			out += fn(m);
			last = static_cast<size_t>(m.position()) + static_cast<size_t>(m.length());
		}
		out += in.substr(last);
		return out;
	};

	std::string out = text;

	// Block-argument phi copies `device uchar* temp_i = var_$N;` inherit the
	// source pointer's address space (decl + initializer on one line).
	out = replaceAll(out, std::regex(R"(device uchar\* (temp_[0-9]+) = (var_\$[0-9]+);)"), [&](const std::smatch& m) {
		if (tgVars.contains(m[2].str())) {
			return "threadgroup uchar* " + m[1].str() + " = " + m[2].str() + ";";
		}
		return m.str(0);
	});

	// Pointer declarations `device uchar* var_$N;` (kernel args end in `[[...]]`,
	// not `;`, so they are untouched).
	out = replaceAll(out, std::regex(R"(device uchar\* (var_\$[0-9]+);)"), [&](const std::smatch& m) {
		if (tgVars.contains(m[1].str())) {
			return "threadgroup uchar* " + m[1].str() + ";";
		}
		return m.str(0);
	});

	// Pointer-cast LHS `var_$N = (device uchar*)`.
	out = replaceAll(out, std::regex(R"((var_\$[0-9]+) = \(device uchar\*\))"), [&](const std::smatch& m) {
		if (tgVars.contains(m[1].str())) {
			return m[1].str() + " = (threadgroup uchar*)";
		}
		return m.str(0);
	});

	// Load/store casts `((TYPE*)(ADDR))`: prefix with the address space of ADDR
	// (threadgroup for shared pointers, device otherwise). This also gives
	// ordinary global accesses their required `device` qualifier.
	out = replaceAll(out, std::regex(R"(\(\((\w+)\*\)\((var_\$[0-9]+)\))"), [&](const std::smatch& m) {
		std::string space = tgVars.contains(m[2].str()) ? "threadgroup " : "device ";
		return "((" + space + m[1].str() + "*)(" + m[2].str() + ")";
	});

	return out;
}

void MetalLoweringProvider::DeviceContext::processOperation(ir::Operation* opt, short blockIndex,
                                                            gpu::RegisterFrame& frame) {
	dispatchOperation(opt, blockIndex, frame);
}

void MetalLoweringProvider::DeviceContext::processGPUOperation(ir::Operation* opt, short blockIndex,
                                                               gpu::RegisterFrame& frame) {
	using OT = ir::Operation::OperationType;
	if (opt->getOperationType() == OT::ProxyCallOp) {
		processProxyCall(ir::as<ir::ProxyCallOperation>(opt), blockIndex, frame);
		return;
	}
	throw NotImplementedException("Operation is not implemented for Metal device backend");
}

void MetalLoweringProvider::DeviceContext::processProxyCall(ir::ProxyCallOperation* opt, short blockIndex,
                                                            gpu::RegisterFrame& frame) {
	auto it = gpuIntrinsics.find(opt->getFunctionPtr());
	if (it != gpuIntrinsics.end()) {
		if (it->second(opt, blockIndex, frame, blockArguments, blocks, getVariable)) {
			return;
		}
	}
	// Internal function call (device helper)
	bool isInternalFunction = ir->getFunctionOperation(opt->getFunctionName()) != nullptr;
	if (isInternalFunction) {
		std::stringstream args;
		for (size_t i = 0; i < opt->getInputArguments().size(); i++) {
			if (i != 0)
				args << ",";
			args << frame.getValue(opt->getInputArguments()[i]->getIdentifier());
		}
		if (opt->getStamp() != Type::v) {
			auto resultVar = getVariable(opt->getIdentifier());
			if (!frame.contains(opt->getIdentifier())) {
				blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
				frame.setValue(opt->getIdentifier(), resultVar);
			}
			blocks[blockIndex] << resultVar << " = ";
		}
		blocks[blockIndex] << opt->getFunctionName() << "(" << args.str() << ");\n";
		return;
	}
	throw NotImplementedException("External function calls are not supported in Metal kernels: " +
	                              opt->getFunctionName());
}

// ============================================================================
// HostContext — generates C++ host code with Metal API dispatch
// ============================================================================

MetalLoweringProvider::HostContext::HostContext(std::shared_ptr<ir::IRGraph> irGraph) {
	ir = std::move(irGraph);
	registerHostIntrinsics();
}

void MetalLoweringProvider::HostContext::registerHostIntrinsics() {
	// Device intrinsics classification (same as DeviceContext)
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_thread_idx_x));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_thread_idx_y));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_thread_idx_z));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_idx_x));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_idx_y));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_idx_z));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_dim_x));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_dim_y));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_block_dim_z));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_grid_dim_x));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_grid_dim_y));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_grid_dim_z));
	deviceIntrinsics.insert(reinterpret_cast<void*>(nautilus_gpu_sync_threads));

	// Launch config intrinsics: capture grid/block variable names
	gpu::registerLaunchConfigIntrinsic(gpuIntrinsics, reinterpret_cast<void*>(nautilus_gpu_set_grid), pendingGridX,
	                                   pendingGridY, pendingGridZ, hasLaunchConfig);
	gpu::registerLaunchConfigIntrinsic(gpuIntrinsics, reinterpret_cast<void*>(nautilus_gpu_set_block), pendingBlockX,
	                                   pendingBlockY, pendingBlockZ, hasLaunchConfig);
}

std::string MetalLoweringProvider::HostContext::getType(const Type& stamp) {
	// Host code uses standard C++ types
	switch (stamp) {
	case Type::v:
		return "void";
	case Type::b:
		return "bool";
	case Type::i8:
		return "int8_t";
	case Type::i16:
		return "int16_t";
	case Type::i32:
		return "int32_t";
	case Type::i64:
		return "int64_t";
	case Type::ui8:
		return "uint8_t";
	case Type::ui16:
		return "uint16_t";
	case Type::ui32:
		return "uint32_t";
	case Type::ui64:
		return "uint64_t";
	case Type::f32:
		return "float";
	case Type::f64:
		return "double";
	case Type::ptr:
		return "uint8_t*";
	}
	assert(false);
	return "unknown";
}

MetalLoweringProvider::HostContext::Code MetalLoweringProvider::HostContext::process() {
	Code code;
	code << "#include <cstdint>\n";
	code << "#include <Metal/Metal.h>\n\n";

	// Resolve each unified buffer's page-rounded length at dispatch time via the
	// runtime size table. Declared as an external symbol resolved against the
	// loading process at dlopen (the host dylib is linked with
	// -undefined dynamic_lookup), which keeps the generated code deterministic
	// — unlike embedding the runtime address as a literal.
	code << "extern \"C\" uint64_t nautilus_gpu_buffer_bytes(void*);\n\n";

	classifyKernelFunctions();

	// Host context emits exactly one function: the user-facing entry. It is
	// identified by the `entry` attribute set on the first traced function;
	// if no entry is present (all functions are kernels), host dispatch is
	// fully external and we emit nothing.
	const auto* hostFunc = getEntryFunction();
	if (hostFunc == nullptr) {
		return code;
	}
	const auto& rootFunc = *hostFunc;

	// Generate the host function body
	resetFunctionState();
	setupFunctionAllocaSlots(rootFunc);
	gpu::RegisterFrame rootFrame;
	std::vector<std::string> arguments;
	const auto& functionBasicBlock = rootFunc.getFunctionBasicBlock();
	for (auto i = 0ull; i < functionBasicBlock.getArguments().size(); i++) {
		auto argument = functionBasicBlock.getArguments()[i];
		auto var = getVariable(argument->getIdentifier());
		rootFrame.setValue(argument->getIdentifier(), var);
		arguments.emplace_back(getType(argument->getStamp()) + " " + var);
	}
	processBlock(&functionBasicBlock, rootFrame);

	auto retType = getType(rootFunc.getOutputArg());
	code << "extern \"C\" " << retType << " execute(";
	for (size_t i = 0; i < arguments.size(); i++) {
		if (i != 0)
			code << ",";
		code << arguments[i] << " ";
	}
	code << "){\n";
	code << blockArguments.str();
	if (!functions.str().empty())
		code << functions.str();
	for (auto& block : blocks)
		code << block.str() << "\n";
	code << "}\n";

	return code;
}

void MetalLoweringProvider::HostContext::processOperation(ir::Operation* opt, short blockIndex,
                                                          gpu::RegisterFrame& frame) {
	dispatchOperation(opt, blockIndex, frame);
}

void MetalLoweringProvider::HostContext::processGPUOperation(ir::Operation* opt, short blockIndex,
                                                             gpu::RegisterFrame& frame) {
	using OT = ir::Operation::OperationType;
	if (opt->getOperationType() == OT::ProxyCallOp) {
		processProxyCall(ir::as<ir::ProxyCallOperation>(opt), blockIndex, frame);
		return;
	}
	throw NotImplementedException("Operation is not implemented for Metal host backend");
}

void MetalLoweringProvider::HostContext::processProxyCall(ir::ProxyCallOperation* opt, short blockIndex,
                                                          gpu::RegisterFrame& frame) {
	// Check launch config intrinsics (setGrid/setBlock)
	auto it = gpuIntrinsics.find(opt->getFunctionPtr());
	if (it != gpuIntrinsics.end()) {
		if (it->second(opt, blockIndex, frame, blockArguments, blocks, getVariable)) {
			return;
		}
	}

	// Collect arguments
	std::stringstream args;
	for (size_t i = 0; i < opt->getInputArguments().size(); i++) {
		if (i != 0)
			args << ",";
		args << frame.getValue(opt->getInputArguments()[i]->getIdentifier());
	}

	bool isInternalFunction = ir->getFunctionOperation(opt->getFunctionName()) != nullptr;
	bool isKernelCall = isInternalFunction && kernelFunctions.contains(opt->getFunctionName());

	if (isKernelCall) {
		// Emit Metal API kernel dispatch
		auto kernelName = opt->getFunctionName();
		auto argList = opt->getInputArguments();

		blocks[blockIndex] << "// Metal kernel dispatch: " << kernelName << "\n";
		blocks[blockIndex] << "{\n";
		// Device, command queue, library, and compute pipeline state are built
		// once and cached in function-local statics (thread-safe one-time init).
		// Rebuilding them per launch — especially the pipeline-state compile and
		// the on-disk metallib reload — dominates the cost of looped/multi-kernel
		// batch workloads. Only the command buffer, encoder, buffer binding, and
		// dispatch happen per call.
		blocks[blockIndex] << "    static id<MTLDevice> device = MTLCreateSystemDefaultDevice();\n";
		blocks[blockIndex] << "    static id<MTLCommandQueue> queue = [device newCommandQueue];\n";
		blocks[blockIndex] << "    NSError* error = nil;\n";
		blocks[blockIndex] << "    static id<MTLLibrary> library = [device newLibraryWithURL:[NSURL "
		                      "fileURLWithPath:@\"__METALLIB_PATH__\"] error:&error];\n";
		blocks[blockIndex] << "    static id<MTLComputePipelineState> pipeline = [device "
		                      "newComputePipelineStateWithFunction:[library newFunctionWithName:@\""
		                   << kernelName << "\"] error:&error];\n";
		blocks[blockIndex] << "    id<MTLCommandBuffer> cmdBuf = [queue commandBuffer];\n";
		blocks[blockIndex] << "    id<MTLComputeCommandEncoder> encoder = [cmdBuf computeCommandEncoder];\n";
		blocks[blockIndex] << "    [encoder setComputePipelineState:pipeline];\n";

		// Set buffer arguments. Pointer arguments are unified buffers
		// (gpu::allocUnified/copy/wrap); wrap the caller's memory with no copy
		// using its page-rounded length from the runtime size table. The GPU
		// reads and writes the host memory directly, so no copy-back is needed.
		for (size_t i = 0; i < argList.size(); i++) {
			auto argVar = frame.getValue(argList[i]->getIdentifier());
			auto argStamp = argList[i]->getStamp();
			if (argStamp == Type::ptr) {
				blocks[blockIndex] << "    id<MTLBuffer> buf_" << i << " = [device newBufferWithBytesNoCopy:(void*)"
				                   << argVar << " length:nautilus_gpu_buffer_bytes((void*)" << argVar
				                   << ") options:MTLResourceStorageModeShared deallocator:nil];\n";
				blocks[blockIndex] << "    [encoder setBuffer:buf_" << i << " offset:0 atIndex:" << i << "];\n";
			} else {
				blocks[blockIndex] << "    [encoder setBytes:&" << argVar << " length:sizeof(" << getType(argStamp)
				                   << ") atIndex:" << i << "];\n";
			}
		}

		// Dispatch with grid/block dims
		if (hasLaunchConfig) {
			blocks[blockIndex] << "    MTLSize grid = MTLSizeMake(" << pendingGridX << "," << pendingGridY << ","
			                   << pendingGridZ << ");\n";
			blocks[blockIndex] << "    MTLSize block = MTLSizeMake(" << pendingBlockX << "," << pendingBlockY << ","
			                   << pendingBlockZ << ");\n";
		} else {
			blocks[blockIndex] << "    MTLSize grid = MTLSizeMake(1,1,1);\n";
			blocks[blockIndex] << "    MTLSize block = MTLSizeMake(256,1,1);\n";
		}
		blocks[blockIndex] << "    [encoder dispatchThreadgroups:grid threadsPerThreadgroup:block];\n";
		blocks[blockIndex] << "    [encoder endEncoding];\n";
		blocks[blockIndex] << "    [cmdBuf commit];\n";
		blocks[blockIndex] << "    [cmdBuf waitUntilCompleted];\n";

		blocks[blockIndex] << "}\n";
		hasLaunchConfig = false;
		return;
	}

	// Regular internal function call
	if (isInternalFunction) {
		if (opt->getStamp() != Type::v) {
			auto resultVar = getVariable(opt->getIdentifier());
			if (!frame.contains(opt->getIdentifier())) {
				blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
				frame.setValue(opt->getIdentifier(), resultVar);
			}
			blocks[blockIndex] << resultVar << " = ";
		}
		blocks[blockIndex] << opt->getFunctionName() << "(" << args.str() << ");\n";
		return;
	}

	// External function call via function pointer
	auto returnTypeStr = getType(opt->getStamp());
	std::stringstream argTypes;
	for (size_t i = 0; i < opt->getInputArguments().size(); i++) {
		if (i != 0)
			argTypes << ",";
		argTypes << getType(opt->getInputArguments()[i]->getStamp());
	}
	if (!functionNames.contains(opt->getFunctionSymbol())) {
		functions << "auto f_" << opt->getFunctionSymbol() << " = (" << returnTypeStr << "(*)(" << argTypes.str()
		          << "))" << opt->getFunctionPtr() << ";\n";
		functionNames.emplace(opt->getFunctionSymbol());
	}
	if (opt->getStamp() != Type::v) {
		auto resultVar = getVariable(opt->getIdentifier());
		if (!frame.contains(opt->getIdentifier())) {
			blockArguments << getType(opt->getStamp()) << " " << resultVar << ";\n";
			frame.setValue(opt->getIdentifier(), resultVar);
		}
		blocks[blockIndex] << resultVar << " = ";
	}
	blocks[blockIndex] << "f_" << opt->getFunctionSymbol() << "(" << args.str() << ");\n";
}

} // namespace nautilus::compiler::metal
