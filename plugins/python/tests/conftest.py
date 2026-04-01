import os
import sys

# Auto-discover the build directory and add it to sys.path so that
# `import nautilus` (the Python package) and `import nautilus_native`
# (the compiled pybind11 module) both resolve correctly.
#
# The CMake build places the .so and a copy of the nautilus/ package in:
#   <repo>/build/nautilus/plugins/python/

_tests_dir = os.path.dirname(__file__)
_plugin_dir = os.path.dirname(_tests_dir)
_repo_root = os.path.dirname(os.path.dirname(_plugin_dir))

# Primary: CMake build output
_build_dir = os.path.join(_repo_root, "build", "nautilus", "plugins", "python")
if os.path.isdir(_build_dir) and _build_dir not in sys.path:
	sys.path.insert(0, _build_dir)

# Fallback: source tree (for the pure-Python nautilus/ package)
if _plugin_dir not in sys.path:
	sys.path.insert(0, _plugin_dir)
