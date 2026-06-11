
#include "nautilus/tracing/fiber/ContextSwitch.hpp"
#include "nautilus/tracing/fiber/FiberStack.hpp"
#include "nautilus/tracing/fiber/SanitizerFiberSupport.hpp"
#include <catch2/catch_all.hpp>
#include <cstring>
#include <vector>

#ifdef NAUTILUS_FIBER_SUPPORTED

namespace nautilus::tracing::detail {

namespace {

struct FiberFixture {
	FiberStack stack {1024 * 1024};
	CapturedContext hostContext = {};
	void* hostFakeStack = nullptr;
	void* fiberFakeStack = nullptr;
	const void* hostStackBottom = nullptr;
	size_t hostStackSize = 0;
	int sequence = 0;

	void enter(void (*entry)(void*)) {
		sanitizerStartSwitchFiber(&hostFakeStack, stack.bottom(), stack.usableSize());
		nautilus_enterOnStack(&hostContext, stack.top(), entry, this);
		sanitizerFinishSwitchFiber(hostFakeStack);
	}

	[[noreturn]] void backToHost() {
		sanitizerStartSwitchFiber(&fiberFakeStack, hostStackBottom, hostStackSize);
		nautilus_resumeContext(&hostContext);
	}

	void onFiber() {
		sanitizerFinishSwitchFiber(fiberFakeStack, &hostStackBottom, &hostStackSize);
	}
};

void simpleEntry(void* arg) {
	auto* fixture = static_cast<FiberFixture*>(arg);
	fixture->onFiber();
	fixture->sequence = 42;
	fixture->backToHost();
}

} // namespace

TEST_CASE("FiberContext - enter and leave a side stack") {
	FiberFixture fixture;
	fixture.enter(&simpleEntry);
	REQUIRE(fixture.sequence == 42);
}

namespace {

struct SnapshotFixture : FiberFixture {
	CapturedContext branchContext = {};
	std::vector<std::byte> stackCopy;
	std::byte* stackBase = nullptr;
	std::vector<int> takenBranches;

	void captureRegion() {
		auto* stackPointer = static_cast<std::byte*>(capturedStackPointer(branchContext));
		stackBase = stackPointer - 128;
		stackCopy.assign(stackBase, static_cast<std::byte*>(static_cast<void*>(stack.top())));
	}

	[[noreturn]] void restoreAndResume() {
		sanitizerUnpoisonStack(stackBase, static_cast<size_t>(stack.top() - stackBase));
		std::memcpy(stackBase, stackCopy.data(), stackCopy.size());
		CapturedContext target = branchContext;
		sanitizerStartSwitchFiber(&hostFakeStack, stack.bottom(), stack.usableSize());
		nautilus_resumeContext(&target);
	}
};

// Emulates the tracer's traceBool: capture at a branch, run the true side, get
// resumed at the same point with the captured frame restored, run the false side.
void branchingEntry(void* arg) {
	auto* fixture = static_cast<SnapshotFixture*>(arg);
	fixture->onFiber();
	int localValue = 7;
	bool branch;
	if (nautilus_captureContext(&fixture->branchContext) == 0) {
		fixture->captureRegion();
		branch = true;
	} else {
		sanitizerFinishSwitchFiber(fixture->fiberFakeStack);
		branch = false;
	}
	// The restored frame must carry the value from before the snapshot.
	fixture->takenBranches.push_back(branch ? localValue : -localValue);
	fixture->backToHost();
}

} // namespace

TEST_CASE("FiberContext - stack snapshot resumes the false branch") {
	SnapshotFixture fixture;
	// Scheduler-style single frame: the host context captured by enterOnStack is
	// resumed twice, so the capturing frame must stay live across both returns.
	sanitizerStartSwitchFiber(&fixture.hostFakeStack, fixture.stack.bottom(), fixture.stack.usableSize());
	nautilus_enterOnStack(&fixture.hostContext, fixture.stack.top(), &branchingEntry, &fixture);
	// Control lands here once per completed branch (setjmp-like multi-return).
	sanitizerFinishSwitchFiber(fixture.hostFakeStack);
	if (fixture.takenBranches.size() == 1) {
		REQUIRE(fixture.takenBranches == std::vector<int> {7});
		// Restore the snapshot; the fiber runs the false branch and resumes the host
		// context, which re-enters this frame right after the enterOnStack call above.
		fixture.restoreAndResume();
	}
	REQUIRE(fixture.takenBranches == std::vector<int> {7, -7});
}

} // namespace nautilus::tracing::detail

#endif // NAUTILUS_FIBER_SUPPORTED
