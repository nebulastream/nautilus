// This TU is intentionally built with -fno-rtti (see CMakeLists.txt), matching
// how LLVM itself is built. The listener subclass's vtable would otherwise
// reference typeinfo symbols that LLVM does not export.

#include "MLIRJitEventListenerTestFixture.hpp"

#include <atomic>
#include <llvm/ExecutionEngine/JITEventListener.h>

namespace nautilus::testing {

namespace {

class CountingJITEventListener : public llvm::JITEventListener {
public:
	std::atomic<int> num_objects_loaded {0};
	std::atomic<int> num_objects_freed {0};

	void notifyObjectLoaded(ObjectKey, const llvm::object::ObjectFile&,
	                        const llvm::RuntimeDyld::LoadedObjectInfo&) override {
		num_objects_loaded.fetch_add(1);
	}

	void notifyFreeingObject(ObjectKey) override {
		num_objects_freed.fetch_add(1);
	}
};

} // namespace

struct CountingJITEventListenerHandle::Impl {
	CountingJITEventListener listener;
};

CountingJITEventListenerHandle::CountingJITEventListenerHandle() : impl_(new Impl()) {
}

CountingJITEventListenerHandle::~CountingJITEventListenerHandle() {
	delete impl_;
}

llvm::JITEventListener* CountingJITEventListenerHandle::listener() const {
	return &impl_->listener;
}

int CountingJITEventListenerHandle::numObjectsLoaded() const {
	return impl_->listener.num_objects_loaded.load();
}

int CountingJITEventListenerHandle::numObjectsFreed() const {
	return impl_->listener.num_objects_freed.load();
}

} // namespace nautilus::testing
