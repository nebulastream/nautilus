
#include <cassert>
#include <nautilus/compiler/backends/amsjit/A64LoweringProvider.hpp>
#include <nautilus/compiler/ir/operations/ArithmeticOperations/DivOperation.hpp>
#include <nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp>
#include <nautilus/compiler/ir/operations/LogicalOperations/AndOperation.hpp>
#include <nautilus/compiler/ir/operations/LogicalOperations/OrOperation.hpp>
#include <nautilus/compiler/ir/operations/Operation.hpp>
#include <nautilus/exceptions/NotImplementedException.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace nautilus::compiler::asmjit {

using namespace ::asmjit;

A64LoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> ir, CodeHolder& code)
    : cc(&code), ir(std::move(ir)) {
}

void* A64LoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir, ::asmjit::JitRuntime& runtime) {

	CodeHolder code; // Holds code and relocation information.

	code.init(runtime.environment(), // Initialize code to match the JIT environment.
	          runtime.cpuFeatures());
	auto ctx = LoweringContext(std::move(ir), code);
	ctx.process();

	void* fn;
	auto err = runtime.add(&fn, &code); // Add the generated code to the runtime.
	if (err) {
		return nullptr;
	};

	return fn;
}

std::string A64LoweringProvider::LoweringContext::getType(const Type& stamp) {
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
		;
	case Type::i64:
		return "int64_t";
		;
	case Type::ui8:
		return "uint8_t";
		;
	case Type::ui16:
		return "uint16_t";
		;
	case Type::ui32:
		return "uint32_t";
		;
	case Type::ui64:
		return "uint64_t";
		;
	case Type::f32:
		return "float";
	case Type::f64:
		return "double";
	case Type::ptr:
		return "uint8_t*";
	}
	assert(false);
}

void A64LoweringProvider::LoweringContext::process() {
	// Create and attach x86::Compiler to code.
	auto func = FuncSignature();
	auto functionOperation = ir->getRootOperation();
	RegisterFrame rootFrame;
	std::vector<std::string> arguments;
	auto functionBasicBlock = functionOperation->getFunctionBasicBlock();
	func._argCount = functionBasicBlock->getArguments().size();

	for (auto i = 0ull; i < functionBasicBlock->getArguments().size(); i++) {
		func.addArg(TypeId::kInt32);
	}

	auto funcNode = cc.addFunc(func); // Begin a function of `int fn(void)` signature.
	for (auto i = 0ull; i < functionBasicBlock->getArguments().size(); i++) {
		auto argument = functionBasicBlock->getArguments()[i].get();
		auto arg = cc.newIntPtr();
		rootFrame.setValue(argument->getIdentifier(), arg);
		funcNode->setArg(i, arg);
	}

	process(functionBasicBlock, rootFrame);
	cc.endFunc();  // End of the function body.
	cc.finalize(); // Translate and assemble the whole 'cc' content.
}

std::string A64LoweringProvider::LoweringContext::process(const ir::BasicBlock* block, RegisterFrame& frame) {
	// assume that all argument registers are correctly set
	auto entry = activeBlocks.find(block->getIdentifier());
	if (entry == activeBlocks.end()) {
		/*for (auto& arg : block->getArguments()) {
		    if (!frame.contains(arg->getIdentifier())) {
		        auto var = getVariable(arg->getIdentifier());
		        blockArguments << getType(arg->getStamp()) << " " << var << ";\n";
		        frame.setValue(arg->getIdentifier(), var);
		    }
		}*/
		// create bytecode block;
		auto blockName = "Block_" + block->getIdentifier();
		cc.newNamedLabel(blockName.c_str());
		activeBlocks.emplace(block->getIdentifier(), blockName);
		for (auto& opt : block->getOperations()) {
			this->process(opt, frame);
		}
		return blockName;
	} else {
		return entry->second;
	}
}

void A64LoweringProvider::LoweringContext::process(const std::unique_ptr<ir::Operation>& opt, RegisterFrame& frame) {
	switch (opt->getOperationType()) {
	case ir::Operation::OperationType::ConstBooleanOp: {
	}
	}
}

} // namespace nautilus::compiler::asmjit
