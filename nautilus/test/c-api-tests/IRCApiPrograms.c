#include "IRCApiPrograms.h"
#include <stddef.h>

#define CHECK(expr)                                                                                                    \
	do {                                                                                                               \
		if (!(expr)) {                                                                                                 \
			return 1;                                                                                                  \
		}                                                                                                              \
	} while (0)

int build_add(NautilusIRGraphRef graph) {
	NautilusIRFunctionBuilderRef fb = nautilus_ir_function_builder_create(graph, "add", NAUTILUS_IR_TYPE_I64);
	CHECK(fb);
	NautilusIRType params[] = {NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64};
	NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, params, 2);
	CHECK(entry);
	NautilusIRValueRef sum =
	    nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_ADD, nautilus_ir_block_get_argument(entry, 0),
	                             nautilus_ir_block_get_argument(entry, 1));
	CHECK(sum);
	CHECK(nautilus_ir_build_return(fb, entry, sum));
	CHECK(nautilus_ir_function_builder_finish(fb));
	return 0;
}

int build_sum_loop(NautilusIRGraphRef graph) {
	NautilusIRFunctionBuilderRef fb = nautilus_ir_function_builder_create(graph, "sum_to", NAUTILUS_IR_TYPE_I64);
	CHECK(fb);
	NautilusIRType entryParams[] = {NAUTILUS_IR_TYPE_I64};
	NautilusIRType loopParams[] = {NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64};
	NautilusIRType exitParams[] = {NAUTILUS_IR_TYPE_I64};
	NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, entryParams, 1);
	/* header(i, acc, n) */
	NautilusIRBlockRef header = nautilus_ir_function_builder_add_block(fb, loopParams, 3);
	/* body(i, acc, n) */
	NautilusIRBlockRef body = nautilus_ir_function_builder_add_block(fb, loopParams, 3);
	NautilusIRBlockRef exit = nautilus_ir_function_builder_add_block(fb, exitParams, 1);
	CHECK(entry && header && body && exit);

	NautilusIRValueRef zero = nautilus_ir_build_const_int(fb, entry, 0, NAUTILUS_IR_TYPE_I64);
	NautilusIRValueRef init[] = {zero, zero, nautilus_ir_block_get_argument(entry, 0)};
	CHECK(nautilus_ir_build_branch(fb, entry, header, init, 3));

	NautilusIRValueRef i = nautilus_ir_block_get_argument(header, 0);
	NautilusIRValueRef acc = nautilus_ir_block_get_argument(header, 1);
	NautilusIRValueRef n = nautilus_ir_block_get_argument(header, 2);
	NautilusIRValueRef cond = nautilus_ir_build_compare(fb, header, NAUTILUS_IR_CMP_LT, i, n);
	NautilusIRValueRef toBody[] = {i, acc, n};
	NautilusIRValueRef toExit[] = {acc};
	CHECK(nautilus_ir_build_if(fb, header, cond, body, toBody, 3, exit, toExit, 1, 0.9));

	NautilusIRValueRef bi = nautilus_ir_block_get_argument(body, 0);
	NautilusIRValueRef bacc = nautilus_ir_block_get_argument(body, 1);
	NautilusIRValueRef bn = nautilus_ir_block_get_argument(body, 2);
	NautilusIRValueRef nextAcc = nautilus_ir_build_binary(fb, body, NAUTILUS_IR_BINARY_ADD, bacc, bi);
	NautilusIRValueRef one = nautilus_ir_build_const_int(fb, body, 1, NAUTILUS_IR_TYPE_I64);
	NautilusIRValueRef nextI = nautilus_ir_build_binary(fb, body, NAUTILUS_IR_BINARY_ADD, bi, one);
	NautilusIRValueRef back[] = {nextI, nextAcc, bn};
	CHECK(nautilus_ir_build_branch(fb, body, header, back, 3));

	CHECK(nautilus_ir_build_return(fb, exit, nautilus_ir_block_get_argument(exit, 0)));
	CHECK(nautilus_ir_function_builder_finish(fb));
	return 0;
}

int build_max(NautilusIRGraphRef graph) {
	NautilusIRType params[] = {NAUTILUS_IR_TYPE_I32, NAUTILUS_IR_TYPE_I32};

	NautilusIRFunctionBuilderRef fb = nautilus_ir_function_builder_create(graph, "max_if", NAUTILUS_IR_TYPE_I32);
	CHECK(fb);
	NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, params, 2);
	NautilusIRBlockRef onTrue = nautilus_ir_function_builder_add_block(fb, NULL, 0);
	NautilusIRBlockRef onFalse = nautilus_ir_function_builder_add_block(fb, NULL, 0);
	NautilusIRValueRef a = nautilus_ir_block_get_argument(entry, 0);
	NautilusIRValueRef b = nautilus_ir_block_get_argument(entry, 1);
	NautilusIRValueRef cond = nautilus_ir_build_compare(fb, entry, NAUTILUS_IR_CMP_GT, a, b);
	CHECK(nautilus_ir_build_if(fb, entry, cond, onTrue, NULL, 0, onFalse, NULL, 0, 0.5));
	CHECK(nautilus_ir_build_return(fb, onTrue, a));
	CHECK(nautilus_ir_build_return(fb, onFalse, b));
	CHECK(nautilus_ir_function_builder_finish(fb));

	fb = nautilus_ir_function_builder_create(graph, "max_select", NAUTILUS_IR_TYPE_I32);
	CHECK(fb);
	entry = nautilus_ir_function_builder_add_block(fb, params, 2);
	a = nautilus_ir_block_get_argument(entry, 0);
	b = nautilus_ir_block_get_argument(entry, 1);
	cond = nautilus_ir_build_compare(fb, entry, NAUTILUS_IR_CMP_GT, a, b);
	CHECK(nautilus_ir_build_return(fb, entry, nautilus_ir_build_select(fb, entry, cond, a, b)));
	CHECK(nautilus_ir_function_builder_finish(fb));
	return 0;
}

int build_factorial(NautilusIRGraphRef graph) {
	NautilusIRFunctionBuilderRef fb = nautilus_ir_function_builder_create(graph, "factorial", NAUTILUS_IR_TYPE_I64);
	CHECK(fb);
	NautilusIRType params[] = {NAUTILUS_IR_TYPE_I64};
	NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, params, 1);
	NautilusIRBlockRef base = nautilus_ir_function_builder_add_block(fb, NULL, 0);
	NautilusIRBlockRef recurse = nautilus_ir_function_builder_add_block(fb, NULL, 0);
	NautilusIRValueRef n = nautilus_ir_block_get_argument(entry, 0);
	NautilusIRValueRef one = nautilus_ir_build_const_int(fb, entry, 1, NAUTILUS_IR_TYPE_I64);
	NautilusIRValueRef cond = nautilus_ir_build_compare(fb, entry, NAUTILUS_IR_CMP_LE, n, one);
	CHECK(nautilus_ir_build_if(fb, entry, cond, base, NULL, 0, recurse, NULL, 0, 0.1));
	CHECK(nautilus_ir_build_return(fb, base, nautilus_ir_build_const_int(fb, base, 1, NAUTILUS_IR_TYPE_I64)));

	NautilusIRValueRef rOne = nautilus_ir_build_const_int(fb, recurse, 1, NAUTILUS_IR_TYPE_I64);
	NautilusIRValueRef nMinusOne = nautilus_ir_build_binary(fb, recurse, NAUTILUS_IR_BINARY_SUB, n, rOne);
	NautilusIRValueRef args[] = {nMinusOne};
	NautilusIRValueRef sub = nautilus_ir_build_call(fb, recurse, nautilus_ir_function_builder_get_callee(fb), args, 1);
	CHECK(sub);
	CHECK(nautilus_ir_build_return(fb, recurse, nautilus_ir_build_binary(fb, recurse, NAUTILUS_IR_BINARY_MUL, n, sub)));
	CHECK(nautilus_ir_function_builder_finish(fb));
	return 0;
}

int build_call_external(NautilusIRGraphRef graph, int64_t (*fn)(int64_t, int64_t)) {
	NautilusIRType calleeParams[] = {NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64};
	NautilusIRFunctionAttributes attrs = nautilus_ir_function_attributes_default();
	attrs.no_unwind = 1;
	NautilusIRCalleeId callee = nautilus_ir_graph_declare_external_function(
	    graph, "external_helper", "external_helper", (void*) fn, NAUTILUS_IR_TYPE_I64, calleeParams, 2, attrs);
	CHECK(callee != NAUTILUS_IR_INVALID_CALLEE);

	NautilusIRFunctionBuilderRef fb = nautilus_ir_function_builder_create(graph, "call_external", NAUTILUS_IR_TYPE_I64);
	CHECK(fb);
	NautilusIRType params[] = {NAUTILUS_IR_TYPE_I64};
	NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, params, 1);
	NautilusIRValueRef args[] = {nautilus_ir_block_get_argument(entry, 0),
	                             nautilus_ir_build_const_int(fb, entry, 2, NAUTILUS_IR_TYPE_I64)};
	NautilusIRValueRef result = nautilus_ir_build_call(fb, entry, callee, args, 2);
	CHECK(result);
	CHECK(nautilus_ir_build_return(fb, entry, result));
	CHECK(nautilus_ir_function_builder_finish(fb));
	return 0;
}

int build_memory(NautilusIRGraphRef graph) {
	NautilusIRFunctionBuilderRef fb =
	    nautilus_ir_function_builder_create(graph, "store_through", NAUTILUS_IR_TYPE_VOID);
	CHECK(fb);
	uint32_t slot = nautilus_ir_function_builder_add_stack_slot(fb, sizeof(int64_t), _Alignof(int64_t));
	NautilusIRType params[] = {NAUTILUS_IR_TYPE_PTR, NAUTILUS_IR_TYPE_I64};
	NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, params, 2);
	NautilusIRValueRef out = nautilus_ir_block_get_argument(entry, 0);
	NautilusIRValueRef value = nautilus_ir_block_get_argument(entry, 1);
	NautilusIRValueRef tmp = nautilus_ir_build_alloca(fb, entry, slot);
	CHECK(tmp);
	CHECK(nautilus_ir_build_store(fb, entry, value, tmp));
	NautilusIRValueRef loaded = nautilus_ir_build_load(fb, entry, tmp, NAUTILUS_IR_TYPE_I64);
	NautilusIRValueRef one = nautilus_ir_build_const_int(fb, entry, 1, NAUTILUS_IR_TYPE_I64);
	CHECK(nautilus_ir_build_store(fb, entry, nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_ADD, loaded, one),
	                              out));
	CHECK(nautilus_ir_build_return(fb, entry, NULL));
	CHECK(nautilus_ir_function_builder_finish(fb));
	return 0;
}

int build_float(NautilusIRGraphRef graph) {
	NautilusIRFunctionBuilderRef fb = nautilus_ir_function_builder_create(graph, "scale", NAUTILUS_IR_TYPE_F64);
	CHECK(fb);
	NautilusIRType params[] = {NAUTILUS_IR_TYPE_I32};
	NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, params, 1);
	NautilusIRValueRef asDouble =
	    nautilus_ir_build_cast(fb, entry, nautilus_ir_block_get_argument(entry, 0), NAUTILUS_IR_TYPE_F64);
	NautilusIRValueRef half = nautilus_ir_build_const_float(fb, entry, 0.5, NAUTILUS_IR_TYPE_F64);
	CHECK(nautilus_ir_build_return(fb, entry,
	                               nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_MUL, asDouble, half)));
	CHECK(nautilus_ir_function_builder_finish(fb));
	return 0;
}
