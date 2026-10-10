/* Links only libnautilus-c, as a binding would: builds, compiles and runs
 * programs through the C API without any C++ in the executable. */
#include "IRCApiPrograms.h"
#include <stdio.h>

static int64_t helper(int64_t a, int64_t b) {
	return a * 10 + b;
}

#define EXPECT(expr)                                                                                                   \
	do {                                                                                                               \
		if (!(expr)) {                                                                                                 \
			NautilusStringRef message = nautilus_last_error_message();                                                 \
			fprintf(stderr, "%s:%d: %s failed (last error: %.*s)\n", __FILE__, __LINE__, #expr, (int) message.length,  \
			        message.data);                                                                                     \
			return 1;                                                                                                  \
		}                                                                                                              \
	} while (0)

static NautilusFunctionPointer lookup(NautilusExecutableRef executable, const char* name) {
	NautilusFunctionPointer fn = NULL;
	return nautilus_executable_get_function(executable, nautilus_string_ref(name), &fn) == NAUTILUS_OK ? fn : NULL;
}

int main(void) {
	EXPECT(nautilus_c_api_version() >> 16 == NAUTILUS_C_API_VERSION_MAJOR);
	EXPECT((nautilus_c_api_version() & 0xFFFF) >= NAUTILUS_C_API_VERSION_MINOR);

	NautilusIRGraphRef graph = nautilus_ir_graph_create(nautilus_string_ref("shared-library-test"));
	EXPECT(graph);
	EXPECT(build_add(graph) == 0);
	EXPECT(build_sum_loop(graph) == 0);
	EXPECT(build_factorial(graph) == 0);
	EXPECT(build_call_external(graph, &helper) == 0);
	EXPECT(build_memory(graph) == 0);
	EXPECT(nautilus_ir_graph_verify(graph, NULL) == NAUTILUS_OK);

	NautilusEngineRef engine = nautilus_engine_create(NULL);
	EXPECT(engine);
	NautilusExecutableRef executable = nautilus_engine_compile(engine, graph, NULL);
	EXPECT(executable);
	nautilus_engine_dispose(engine);
	nautilus_ir_graph_dispose(graph);

	int64_t (*add)(int64_t, int64_t) = (int64_t(*)(int64_t, int64_t)) lookup(executable, "add");
	int64_t (*sumTo)(int64_t) = (int64_t(*)(int64_t)) lookup(executable, "sum_to");
	int64_t (*factorial)(int64_t) = (int64_t(*)(int64_t)) lookup(executable, "factorial");
	int64_t (*callExternal)(int64_t) = (int64_t(*)(int64_t)) lookup(executable, "call_external");
	void (*storeThrough)(int64_t*, int64_t) = (void (*)(int64_t*, int64_t)) lookup(executable, "store_through");
	EXPECT(add && sumTo && factorial && callExternal && storeThrough);
	EXPECT(add(40, 2) == 42);
	EXPECT(sumTo(10) == 45);
	EXPECT(factorial(10) == 3628800);
	EXPECT(callExternal(4) == 42);
	int64_t out = 0;
	storeThrough(&out, 41);
	EXPECT(out == 42);

	nautilus_executable_dispose(executable);
	return 0;
}
