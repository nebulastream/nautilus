// Anchor TU for libplayground_core.so. The library's real content is the
// whole-archive of libnautilus.a (plus its transitive MLIR/backend
// dependencies) pulled in by the runner CMakeLists; this file only gives the
// shared library a translation unit of its own.

extern "C" int playground_core_present() {
	return 1;
}
