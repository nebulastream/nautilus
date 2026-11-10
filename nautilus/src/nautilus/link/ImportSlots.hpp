#pragma once

#include <atomic>
#include <cstddef>
#include <memory>

namespace nautilus::link {

/// The import slots of one linked module (13-nir-spec.md §4.4.2, step 4): one cell per import that holds the entry
/// point of the exporter's current code. Compiled code loads the cell and calls through it (§5.7); the engine writes it
/// with a release store when the exporter is linked and again each time the exporter is promoted (§4.4.2). The cell's
/// address is stable for the life of the object, so a backend may embed it in the code it generates.
///
/// Next to each cell the object keeps the two typed thunks the C++ front end registers for the import
/// (`engine::details::ImportDeclaration`): a call that may throw goes through the one that suits the importer's
/// exception propagation (`captureThunk` for backends that capture, `frameThunk` for backends that unwind natively),
/// which bridges the exporter's mode whatever tier it is currently in. Both are null for an import built without a
/// C++ signature (a hand-written module), whose calls are then made directly.
class ImportSlots {
public:
	explicit ImportSlots(size_t count);

	[[nodiscard]] size_t size() const noexcept {
		return count_;
	}

	/// Release store of the exporter's entry point.
	void publish(size_t index, void* entry) noexcept {
		slots_[index].entry.store(entry, std::memory_order_release);
	}
	/// Acquire load of the current entry point.
	[[nodiscard]] void* load(size_t index) const noexcept {
		return slots_[index].entry.load(std::memory_order_acquire);
	}
	/// The address of the cell: what compiled code loads from. Valid as long as this object.
	[[nodiscard]] void* address(size_t index) const noexcept {
		return const_cast<std::atomic<void*>*>(&slots_[index].entry);
	}

	void setThunks(size_t index, void* captureThunk, void* frameThunk) noexcept {
		slots_[index].captureThunk = captureThunk;
		slots_[index].frameThunk = frameThunk;
	}
	/// `R captureThrowingCall<R, Args...>(R (*)(Args...), Args...)`: catches what the target throws into the exception
	/// frame of the thread, for a caller that checks for a pending exception after the call. Null if not registered.
	[[nodiscard]] void* captureThunk(size_t index) const noexcept {
		return slots_[index].captureThunk;
	}
	/// `R callInExceptionFrame<R, Args...>(R (*)(Args...), Args...)`: pushes an exception frame, calls the target and
	/// rethrows what a capturing target left in it, for a caller that unwinds natively. Null if not registered.
	[[nodiscard]] void* frameThunk(size_t index) const noexcept {
		return slots_[index].frameThunk;
	}

private:
	struct Slot {
		std::atomic<void*> entry {nullptr};
		void* captureThunk = nullptr;
		void* frameThunk = nullptr;
	};
	static_assert(sizeof(std::atomic<void*>) == sizeof(void*) && std::atomic<void*>::is_always_lock_free,
	              "compiled code reads a slot as a plain pointer");

	size_t count_;
	std::unique_ptr<Slot[]> slots_;
};

} // namespace nautilus::link
