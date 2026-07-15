// Precompiled header for the per-request user translation unit. Covers the
// template-heavy public API so that compiling user_module.cpp stays fast.
// The .pch is generated in the runner image with the exact flags
// entrypoint.sh uses for user_module.cpp (see Dockerfile.runner).
#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/val.hpp>
#include <nautilus/val_ptr.hpp>
