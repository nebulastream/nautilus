#pragma once

#include "nautilus/JITCompiler.hpp"
#include "nautilus/options.hpp"
#include <memory>

namespace nautilus::cache {

std::unique_ptr<compiler::JITCompiler> createCompiler(const engine::Options& options);

}
