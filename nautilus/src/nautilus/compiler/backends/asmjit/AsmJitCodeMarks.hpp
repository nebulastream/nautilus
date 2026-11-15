#pragma once

#include "nautilus/compiler/backends/NirCodeProfile.hpp"
#include "nautilus/nir/Inst.hpp"
#include "nautilus/options.hpp"
#include <asmjit/core.h>
#include <cstdint>
#include <vector>

namespace nautilus::compiler::asmjit {

/// Records where the code of each NIR instruction lands (D14, 05 §7): the lowering puts a marker before the code of an
/// instruction, and AsmJit reports the buffer offset of every marker when it serializes the node list (a patch of
/// `BaseBuilder::serializeTo`, marked `[Nautilus]`). A marker is a comment node: register allocation and the peephole
/// passes leave it alone, so it keeps its place in the final code, which a node of the instruction itself would not
/// (the peephole pass erases nodes). Off, `mark` does nothing and the node list is the one the lowering always built.
class CodeMarks {
public:
	/// Whether the options ask for the code of this module to be profiled (`perf` or `perf.sample`).
	static bool wanted(const engine::Options& options) {
		return options.getOptionOrDefault("perf", false) || options.getOptionOrDefault("perf.sample", false);
	}

	void attach(::asmjit::BaseBuilder& builder) {
		enabled_ = true;
		builder._markerFn = &CodeMarks::onMark;
		builder._markerCtx = this;
	}

	void mark(::asmjit::BaseBuilder& builder, uint32_t function, uint32_t inst) {
		if (!enabled_) {
			return;
		}
		::asmjit::CommentNode* node = nullptr;
		if (builder.newCommentNode(&node, nullptr, 0) == ::asmjit::kErrorOk && node != nullptr) {
			// never 0: that is "no marker"
			node->setUserDataAsUInt64((static_cast<uint64_t>(function) + 1) << 32 | inst);
			builder.addNode(node);
		}
	}

	std::vector<NirCodeMark> take() {
		return std::move(marks_);
	}

private:
	static void onMark(void* self, uint64_t data, size_t offset) {
		static_cast<CodeMarks*>(self)->marks_.push_back(NirCodeMark {static_cast<uint32_t>((data >> 32) - 1),
		                                                             static_cast<uint32_t>(data & 0xFFFFFFFFu),
		                                                             static_cast<uint32_t>(offset)});
	}

	bool enabled_ = false;
	std::vector<NirCodeMark> marks_;
};

} // namespace nautilus::compiler::asmjit
