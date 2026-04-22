#pragma once

namespace llvm {
class JITEventListener;
} // namespace llvm

namespace nautilus::testing {

// Owns a JITEventListener subclass that counts object-loaded / freed
// notifications. The subclass itself is defined in a separate TU built with
// -fno-rtti (to match how LLVM is built); this lets Catch2 tests keep RTTI
// enabled for typeid-based val<T> machinery while still poking into LLVM's
// listener interface.
class CountingJITEventListenerHandle {
public:
	CountingJITEventListenerHandle();
	~CountingJITEventListenerHandle();
	CountingJITEventListenerHandle(const CountingJITEventListenerHandle&) = delete;
	CountingJITEventListenerHandle& operator=(const CountingJITEventListenerHandle&) = delete;

	llvm::JITEventListener* listener() const;
	int numObjectsLoaded() const;
	int numObjectsFreed() const;

private:
	struct Impl;
	Impl* impl_;
};

} // namespace nautilus::testing
