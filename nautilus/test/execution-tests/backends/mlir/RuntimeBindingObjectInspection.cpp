#include <cstdint>
#include <exception>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/BinaryFormat/ELF.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCDisassembler/MCDisassembler.h>
#include <llvm/MC/MCInst.h>
#include <llvm/MC/MCInstrAnalysis.h>
#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCRegisterInfo.h>
#include <llvm/MC/MCSubtargetInfo.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Object/ELFObjectFile.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

void requireInspection(bool condition, const std::string& diagnostic) {
	if (!condition) {
		throw std::runtime_error(diagnostic);
	}
}

template <typename T>
T inspectionValue(llvm::Expected<T> value, llvm::StringRef operation) {
	if (!value) {
		throw std::runtime_error(operation.str() + ": " + llvm::toString(value.takeError()));
	}
	return std::move(*value);
}

void inspectBindingObject(llvm::StringRef objectPath, llvm::StringRef bindingSymbol) {
	const llvm::Triple host(llvm::sys::getProcessTriple());
	requireInspection(host.isOSLinux() && host.getArch() == llvm::Triple::x86_64,
	                  "Native binding relocation placement is checked only on Linux x86-64");
	requireInspection(!bindingSymbol.empty(), "Binding symbol must not be empty");
	requireInspection(!llvm::InitializeNativeTarget(), "Native LLVM target initialization failed");
	requireInspection(!llvm::InitializeNativeTargetDisassembler(), "Native LLVM disassembler initialization failed");
	std::string targetError;
	const auto* target = llvm::TargetRegistry::lookupTarget(host, targetError);
	requireInspection(target != nullptr, "Native LLVM target lookup failed: " + targetError);
#if LLVM_VERSION_MAJOR >= 22
	const auto& targetTriple = host;
#else
	const auto targetTriple = host.str();
#endif
	std::unique_ptr<llvm::MCRegisterInfo> registers(target->createMCRegInfo(targetTriple));
	requireInspection(registers != nullptr, "Native LLVM register information is unavailable");
	std::unique_ptr<llvm::MCAsmInfo> assembly(target->createMCAsmInfo(*registers, targetTriple, {}));
	requireInspection(assembly != nullptr, "Native LLVM assembly information is unavailable");
	std::unique_ptr<llvm::MCSubtargetInfo> subtarget(
	    target->createMCSubtargetInfo(targetTriple, llvm::sys::getHostCPUName(), ""));
	requireInspection(subtarget != nullptr, "Native LLVM subtarget information is unavailable");
	llvm::MCContext context(host, assembly.get(), registers.get(), subtarget.get());
	std::unique_ptr<llvm::MCDisassembler> disassembler(target->createMCDisassembler(*subtarget, context));
	requireInspection(disassembler != nullptr, "Native LLVM disassembler is unavailable");
	std::unique_ptr<llvm::MCInstrInfo> instructions(target->createMCInstrInfo());
	requireInspection(instructions != nullptr, "Native LLVM instruction information is unavailable");
	std::unique_ptr<llvm::MCInstrAnalysis> analysis(target->createMCInstrAnalysis(instructions.get()));
	requireInspection(analysis != nullptr, "Native LLVM instruction analysis is unavailable");

	auto binary = inspectionValue(llvm::object::ObjectFile::createObjectFile(objectPath), "Read emitted object");
	const auto* object = binary.getBinary();
	requireInspection(llvm::isa<llvm::object::ELF64LEObjectFile>(object), "Emitted object must be ELF64 little-endian");
	requireInspection(object->getArch() == llvm::Triple::x86_64, "Emitted object must target x86-64");
	requireInspection(object->isRelocatableObject(), "Emitted object must be relocatable");
	const auto symbols = object->symbols();
	auto functionSymbol = symbols.end();
	for (auto current = symbols.begin(); current != symbols.end(); ++current) {
		const auto name = inspectionValue(current->getName(), "Read object symbol name");
		if (name == "execute") {
			functionSymbol = current;
			break;
		}
	}
	requireInspection(functionSymbol != symbols.end(), "Emitted object is missing execute");
	requireInspection(llvm::object::ELFSymbolRef(*functionSymbol).getELFType() == llvm::ELF::STT_FUNC,
	                  "Emitted execute symbol must be a function");
	const auto functionSection = inspectionValue(functionSymbol->getSection(), "Read execute section");
	requireInspection(functionSection != object->section_end(), "Emitted execute symbol has no section");
	requireInspection(functionSection->isText(), "Emitted execute section must contain code");
	const auto functionAddress = inspectionValue(functionSymbol->getAddress(), "Read execute address");
	requireInspection(functionAddress >= functionSection->getAddress(), "Emitted execute address precedes its section");
	const auto functionStart = functionAddress - functionSection->getAddress();
	const auto functionSize = llvm::object::ELFSymbolRef(*functionSymbol).getSize();
	requireInspection(functionSize > 0, "Emitted execute function is empty");
	const auto contents = inspectionValue(functionSection->getContents(), "Read execute section contents");
	requireInspection(functionStart <= contents.size(), "Emitted execute address exceeds its section");
	requireInspection(functionSize <= contents.size() - functionStart, "Emitted execute size exceeds its section");
	const auto functionEnd = functionStart + functionSize;
	const llvm::ArrayRef<uint8_t> bytes(contents.bytes_begin() + functionStart, functionSize);
	std::vector<std::pair<uint64_t, uint64_t>> nativeLoops;
	for (auto offset = functionStart; offset < functionEnd;) {
		llvm::MCInst instruction;
		uint64_t size = 0;
		requireInspection(disassembler->getInstruction(instruction, size, bytes.drop_front(offset - functionStart),
		                                               offset, llvm::nulls()) == llvm::MCDisassembler::Success,
		                  "Cannot decode execute instruction at offset " + std::to_string(offset));
		requireInspection(size > 0 && size <= functionEnd - offset,
		                  "Invalid execute instruction size at offset " + std::to_string(offset));
		if (analysis->isBranch(instruction)) {
			requireInspection(!analysis->isIndirectBranch(instruction),
			                  "Indirect execute branch at offset " + std::to_string(offset));
			uint64_t destination = 0;
			requireInspection(analysis->evaluateBranch(instruction, offset, size, destination),
			                  "Cannot evaluate execute branch at offset " + std::to_string(offset));
			if (destination <= offset) {
				requireInspection(destination >= functionStart, "Native loop branch precedes execute");
				nativeLoops.emplace_back(destination, offset + size);
			}
		}
		offset += size;
	}
	requireInspection(!nativeLoops.empty(), "Emitted execute function has no native loops");

	std::size_t bindingRelocations = 0;
	for (const auto& section : object->sections()) {
		const auto relocatedSection = inspectionValue(section.getRelocatedSection(), "Read relocation section target");
		if (relocatedSection != functionSection) {
			continue;
		}
		for (const auto& relocation : section.relocations()) {
			const auto offset = relocation.getOffset();
			if (offset < functionStart || offset >= functionEnd) {
				continue;
			}
			const auto relocatedSymbol = relocation.getSymbol();
			if (relocatedSymbol == object->symbol_end()) {
				continue;
			}
			const auto name = inspectionValue(relocatedSymbol->getName(), "Read relocation symbol name");
			if (name == bindingSymbol) {
				++bindingRelocations;
				requireInspection(relocation.getType() == llvm::ELF::R_X86_64_64,
				                  "Binding relocation must be R_X86_64_64 at offset " + std::to_string(offset));
				requireInspection(sizeof(uint64_t) <= functionEnd - offset,
				                  "Binding relocation extends beyond execute");
				for (const auto& [loopStart, loopEnd] : nativeLoops) {
					requireInspection(offset + sizeof(uint64_t) <= loopStart || offset >= loopEnd,
					                  "Binding relocation at offset " + std::to_string(offset) +
					                      " overlaps native loop [" + std::to_string(loopStart) + ", " +
					                      std::to_string(loopEnd) + ")");
				}
			}
		}
	}
	requireInspection(bindingRelocations == 1,
	                  "Expected one execute binding relocation, found " + std::to_string(bindingRelocations));
}

} // namespace

int main(int argc, char** argv) {
	if (argc != 3) {
		llvm::errs() << "Usage: nautilus-binding-object-inspection <object-file> <binding-symbol>\n";
		return 2;
	}
	try {
		inspectBindingObject(argv[1], argv[2]);
		return 0;
	} catch (const std::exception& error) {
		llvm::errs() << "Runtime binding object inspection failed for " << argv[1] << ": " << error.what() << '\n';
		return 1;
	}
}
