#include "nautilus/Engine.hpp"

namespace nautilus::engine {
NautilusEngine::NautilusEngine() : jit(), options() {
}

NautilusEngine::NautilusEngine(const Options& options) : jit(options), options(options) {
}
} // namespace nautilus::engine
