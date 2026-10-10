/*
 * Nautilus C API: the IR.
 *
 * Builds, inspects, verifies and compiles Nautilus IR graphs without the C++
 * tracing frontend. See nautilus/c/common.h for the conventions every
 * function here follows (errors, strings, arrays, thread safety) and
 * docs/c-api.md for a guide.
 *
 * Ownership
 * ---------
 * - NautilusIRGraphRef is owned by the caller. It owns every function, block
 *   and value in it; their refs are borrowed from the graph and valid until it
 *   is disposed. Optimizing or compiling rewrites the graph in place, after
 *   which refs taken before may dangle: re-read them from the graph.
 * - NautilusIRFunctionBuilderRef is owned by the caller until
 *   nautilus_ir_function_builder_finish() consumes it (on success and on
 *   failure); nautilus_ir_function_builder_dispose() abandons it instead.
 *
 * Example: int64_t add(int64_t a, int64_t b) { return a + b; }
 *
 *     NautilusIRGraphRef g = nautilus_ir_graph_create(nautilus_string_ref("example"));
 *     NautilusIRFunctionBuilderRef fb =
 *         nautilus_ir_function_builder_create(g, nautilus_string_ref("add"), NAUTILUS_IR_TYPE_I64);
 *     NautilusIRType params[] = {NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64};
 *     NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, params, 2);
 *     NautilusIRValueRef a = nautilus_ir_block_get_argument(entry, 0);
 *     NautilusIRValueRef b = nautilus_ir_block_get_argument(entry, 1);
 *     nautilus_ir_build_return(fb, entry, nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_ADD, a, b));
 *     nautilus_ir_function_builder_finish(fb);
 *
 *     NautilusExecutableRef exe = nautilus_ir_graph_compile(g, nautilus_string_ref("bc"), NULL);
 *     NautilusFunctionPointer fn = NULL;
 *     nautilus_executable_get_function(exe, nautilus_string_ref("add"), &fn);
 *     ((int64_t (*)(int64_t, int64_t)) fn)(40, 2);
 *     nautilus_executable_dispose(exe);
 *     nautilus_ir_graph_dispose(g);
 */
#ifndef NAUTILUS_C_IR_H
#define NAUTILUS_C_IR_H

#include "nautilus/c/common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ── Handles ────────────────────────────────────────────────────────────── */

typedef struct NautilusIROpaqueGraph* NautilusIRGraphRef;
typedef struct NautilusIROpaqueFunctionBuilder* NautilusIRFunctionBuilderRef;
typedef struct NautilusIROpaqueFunction* NautilusIRFunctionRef;
typedef struct NautilusIROpaqueBlock* NautilusIRBlockRef;
/* Any operation. Operations that produce a result, and block arguments, are
 * the SSA values of the IR, so a value ref doubles as an operation ref. */
typedef struct NautilusIROpaqueValue* NautilusIRValueRef;

/* Entry in a graph's function table: the target of a call. */
typedef uint32_t NautilusIRCalleeId;
#define NAUTILUS_IR_INVALID_CALLEE ((NautilusIRCalleeId) 0xFFFFFFFFu)

/* ── Enumerations ───────────────────────────────────────────────────────── */

typedef uint32_t NautilusIRType;
enum {
	NAUTILUS_IR_TYPE_VOID = 0,
	NAUTILUS_IR_TYPE_BOOL = 1,
	NAUTILUS_IR_TYPE_I8 = 2,
	NAUTILUS_IR_TYPE_I16 = 3,
	NAUTILUS_IR_TYPE_I32 = 4,
	NAUTILUS_IR_TYPE_I64 = 5,
	NAUTILUS_IR_TYPE_UI8 = 6,
	NAUTILUS_IR_TYPE_UI16 = 7,
	NAUTILUS_IR_TYPE_UI32 = 8,
	NAUTILUS_IR_TYPE_UI64 = 9,
	NAUTILUS_IR_TYPE_F32 = 10,
	NAUTILUS_IR_TYPE_F64 = 11,
	NAUTILUS_IR_TYPE_PTR = 12,
};

typedef uint32_t NautilusIROpKind;
enum {
	/* An operation kind this version of the API does not know yet. */
	NAUTILUS_IR_OP_UNKNOWN = 0,
	NAUTILUS_IR_OP_BLOCK_ARGUMENT = 1,
	NAUTILUS_IR_OP_CONST_INT = 2,
	NAUTILUS_IR_OP_CONST_FLOAT = 3,
	NAUTILUS_IR_OP_CONST_BOOL = 4,
	NAUTILUS_IR_OP_CONST_PTR = 5,
	NAUTILUS_IR_OP_ADD = 6,
	NAUTILUS_IR_OP_SUB = 7,
	NAUTILUS_IR_OP_MUL = 8,
	NAUTILUS_IR_OP_DIV = 9,
	NAUTILUS_IR_OP_MOD = 10,
	NAUTILUS_IR_OP_LOGICAL_AND = 11,
	NAUTILUS_IR_OP_LOGICAL_OR = 12,
	NAUTILUS_IR_OP_NOT = 13,
	/* Bitwise and/or/xor; see nautilus_ir_value_get_bitwise_kind(). */
	NAUTILUS_IR_OP_BITWISE = 14,
	/* Shift left/right; see nautilus_ir_value_get_shift_kind(). */
	NAUTILUS_IR_OP_SHIFT = 15,
	NAUTILUS_IR_OP_NEGATE = 16,
	NAUTILUS_IR_OP_COMPARE = 17,
	NAUTILUS_IR_OP_CAST = 18,
	NAUTILUS_IR_OP_SELECT = 19,
	NAUTILUS_IR_OP_LOAD = 20,
	NAUTILUS_IR_OP_STORE = 21,
	NAUTILUS_IR_OP_ALLOCA = 22,
	NAUTILUS_IR_OP_CALL = 23,
	NAUTILUS_IR_OP_INDIRECT_CALL = 24,
	NAUTILUS_IR_OP_FUNCTION_ADDRESS = 25,
	NAUTILUS_IR_OP_BRANCH = 26,
	NAUTILUS_IR_OP_IF = 27,
	NAUTILUS_IR_OP_RETURN = 28,
};

/* Two-operand operations built by nautilus_ir_build_binary(). */
typedef uint32_t NautilusIRBinaryOp;
enum {
	NAUTILUS_IR_BINARY_ADD = 0,
	NAUTILUS_IR_BINARY_SUB = 1,
	NAUTILUS_IR_BINARY_MUL = 2,
	NAUTILUS_IR_BINARY_DIV = 3,
	NAUTILUS_IR_BINARY_MOD = 4,
	/* Logical and/or of bool operands; the result is bool. */
	NAUTILUS_IR_BINARY_LOGICAL_AND = 5,
	NAUTILUS_IR_BINARY_LOGICAL_OR = 6,
	/* Bitwise operations and shifts of integer operands. */
	NAUTILUS_IR_BINARY_BITWISE_AND = 7,
	NAUTILUS_IR_BINARY_BITWISE_OR = 8,
	NAUTILUS_IR_BINARY_BITWISE_XOR = 9,
	NAUTILUS_IR_BINARY_SHIFT_LEFT = 10,
	NAUTILUS_IR_BINARY_SHIFT_RIGHT = 11,
};

typedef uint32_t NautilusIRComparator;
enum {
	NAUTILUS_IR_CMP_EQ = 0,
	NAUTILUS_IR_CMP_NE = 1,
	NAUTILUS_IR_CMP_LT = 2,
	NAUTILUS_IR_CMP_LE = 3,
	NAUTILUS_IR_CMP_GT = 4,
	NAUTILUS_IR_CMP_GE = 5,
};

typedef uint32_t NautilusIRBitwiseKind;
enum {
	NAUTILUS_IR_BITWISE_AND = 0,
	NAUTILUS_IR_BITWISE_OR = 1,
	NAUTILUS_IR_BITWISE_XOR = 2,
};

typedef uint32_t NautilusIRShiftKind;
enum {
	NAUTILUS_IR_SHIFT_LEFT = 0,
	NAUTILUS_IR_SHIFT_RIGHT = 1,
};

typedef uint32_t NautilusIRLinkage;
enum {
	/* Defined in the graph. */
	NAUTILUS_IR_LINKAGE_INTERNAL = 0,
	/* Native code called through its address. */
	NAUTILUS_IR_LINKAGE_EXTERNAL = 1,
	/* Native code a backend may replace with instructions. */
	NAUTILUS_IR_LINKAGE_INTRINSIC = 2,
};

typedef uint32_t NautilusIRModRef;
enum {
	NAUTILUS_IR_MOD_REF_NONE = 0,
	NAUTILUS_IR_MOD_REF_READS = 1,
	NAUTILUS_IR_MOD_REF_WRITES = 2,
	NAUTILUS_IR_MOD_REF_READS_WRITES = 3,
};

typedef uint32_t NautilusIRFunctionFlags;
enum {
	/* Every call returns (or has undefined behavior). */
	NAUTILUS_IR_FUNCTION_WILL_RETURN = 1u << 0,
	/* The function never throws. Native functions called from compiled code
	 * must not throw C++ exceptions anyway; setting this lets the passes drop
	 * or move calls that are otherwise pure. */
	NAUTILUS_IR_FUNCTION_NO_UNWIND = 1u << 1,
};

/* What a callee may do, so the IR passes and backends can drop or reorder
 * calls. nautilus_ir_function_attributes_default() is the pessimistic
 * "may do anything" set; relax only what is known to hold. */
typedef struct {
	NautilusIRModRef mod_ref;
	NautilusIRFunctionFlags flags;
} NautilusIRFunctionAttributes;

typedef uint32_t NautilusIROptimizationLevel;
enum {
	/* Whatever the backend compiling the graph asks for. */
	NAUTILUS_IR_OPTIMIZE_BACKEND_DEFAULT = 0,
	/* Only the analyses every backend needs. */
	NAUTILUS_IR_OPTIMIZE_NONE = 1,
	/* Block-argument pruning only. */
	NAUTILUS_IR_OPTIMIZE_ARGUMENT_PRUNING = 2,
	/* Every optimization pass. */
	NAUTILUS_IR_OPTIMIZE_FULL = 3,
};

typedef struct {
	size_t size;
	size_t align;
} NautilusIRStackSlot;

/* One entry of a graph's function table. */
typedef struct {
	NautilusIRLinkage linkage;
	/* The identifier backends emit the callee under. Borrowed from the graph. */
	NautilusStringRef name;
	/* Native function of an external or intrinsic callee, NULL for an internal one. */
	NautilusFunctionPointer address;
	/* Definition of an internal callee; NULL for native callees and for an
	 * internal one whose builder has not been finished. */
	NautilusIRFunctionRef function;
} NautilusIRCalleeInfo;

/* Name of @p type ("i64", "ptr", ...), or an empty string if it is invalid. */
NAUTILUS_C_API NautilusStringRef nautilus_ir_type_name(NautilusIRType type);

NAUTILUS_C_API NautilusIRFunctionAttributes nautilus_ir_function_attributes_default(void);

/* ── Graphs ─────────────────────────────────────────────────────────────── */

NAUTILUS_C_API NautilusIRGraphRef nautilus_ir_graph_create(NautilusStringRef id);

/* Releases the graph, everything in it, and every builder not yet finished
 * or disposed (whose refs then dangle). */
NAUTILUS_C_API void nautilus_ir_graph_dispose(NautilusIRGraphRef graph);

/* Renders the graph in the textual form of Nautilus' IR dumps. */
NAUTILUS_C_API NautilusStatus nautilus_ir_graph_to_string(NautilusIRGraphRef graph, NautilusString* out);

/* Runs the IR verifier. Returns NAUTILUS_OK for a well-formed graph, or
 * NAUTILUS_ERROR_VERIFICATION_FAILED and, if @p diagnostics is not NULL,
 * every problem found, one per line. */
NAUTILUS_C_API NautilusStatus nautilus_ir_graph_verify(NautilusIRGraphRef graph, NautilusString* diagnostics);

/* Finished functions, in the order they were finished. Copy-out accessor. */
NAUTILUS_C_API size_t nautilus_ir_graph_get_functions(NautilusIRGraphRef graph, NautilusIRFunctionRef* out,
                                                      size_t capacity);

/* NAUTILUS_ERROR_NOT_FOUND if no finished function has that name. */
NAUTILUS_C_API NautilusStatus nautilus_ir_graph_find_function(NautilusIRGraphRef graph, NautilusStringRef name,
                                                              NautilusIRFunctionRef* out);

/* Declares the native function @p address (cast to NautilusFunctionPointer)
 * so IR in the graph may call it. Declaring the same function again returns
 * the same id. @p symbol is the
 * linker-level name and @p display_name the name shown in dumps; either may
 * be empty. The function must not throw C++ exceptions. */
NAUTILUS_C_API NautilusStatus nautilus_ir_graph_declare_external_function(
    NautilusIRGraphRef graph, NautilusStringRef symbol, NautilusStringRef display_name, NautilusFunctionPointer address,
    NautilusIRType result_type, const NautilusIRType* param_types, size_t param_count,
    NautilusIRFunctionAttributes attributes, NautilusIRCalleeId* out);

/* Number of entries in the function table; ids are 0 .. count - 1. */
NAUTILUS_C_API size_t nautilus_ir_graph_get_callee_count(NautilusIRGraphRef graph);

NAUTILUS_C_API NautilusStatus nautilus_ir_graph_get_callee_info(NautilusIRGraphRef graph, NautilusIRCalleeId callee,
                                                                NautilusIRCalleeInfo* out);

/* ── Function builders ──────────────────────────────────────────────────── */

/* Starts a function; names must be unique within the graph. Fails once the
 * graph has been optimized or compiled. */
NAUTILUS_C_API NautilusIRFunctionBuilderRef nautilus_ir_function_builder_create(NautilusIRGraphRef graph,
                                                                                NautilusStringRef name,
                                                                                NautilusIRType return_type);

/* Abandons an unfinished function. Its blocks and values stay in the graph's
 * memory but belong to no function. If its callee id was already taken (see
 * below) and is still called, the graph can no longer be compiled. */
NAUTILUS_C_API void nautilus_ir_function_builder_dispose(NautilusIRFunctionBuilderRef builder);

/* The function's callee id, so it can be called (including recursively)
 * before it is finished. */
NAUTILUS_C_API NautilusStatus nautilus_ir_function_builder_get_callee(NautilusIRFunctionBuilderRef builder,
                                                                      NautilusIRCalleeId* out);

/* Sets a string attribute on the function (e.g. "entry" = "true"). */
NAUTILUS_C_API NautilusStatus nautilus_ir_function_builder_set_attribute(NautilusIRFunctionBuilderRef builder,
                                                                         NautilusStringRef key,
                                                                         NautilusStringRef value);

/* Adds a block with the given argument types. The first block added is the
 * entry block; its arguments are the function's parameters. */
NAUTILUS_C_API NautilusIRBlockRef nautilus_ir_function_builder_add_block(NautilusIRFunctionBuilderRef builder,
                                                                         const NautilusIRType* arg_types,
                                                                         size_t arg_count);

/* Reserves a stack slot in the function's frame, for nautilus_ir_build_alloca(). */
NAUTILUS_C_API NautilusStatus nautilus_ir_function_builder_add_stack_slot(NautilusIRFunctionBuilderRef builder,
                                                                          size_t size, size_t align,
                                                                          uint32_t* out_slot);

/* Adds the function to the graph and consumes the builder, also on failure.
 * Every block must end in a terminator (branch, if or return). */
NAUTILUS_C_API NautilusIRFunctionRef nautilus_ir_function_builder_finish(NautilusIRFunctionBuilderRef builder);

/* ── Instruction building ───────────────────────────────────────────────────
 * Each nautilus_ir_build_* call appends one operation to the end of @p block,
 * which must belong to @p builder, and returns it (NULL on failure). Nothing
 * can follow a terminator. Operands must be values of the same function,
 * defined before their use; nautilus_ir_graph_verify() checks what the
 * builders cannot check locally. */

/* Integer constant. For unsigned types, @p value holds the bits. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_const_int(NautilusIRFunctionBuilderRef builder,
                                                              NautilusIRBlockRef block, int64_t value,
                                                              NautilusIRType type);
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_const_float(NautilusIRFunctionBuilderRef builder,
                                                                NautilusIRBlockRef block, double value,
                                                                NautilusIRType type);
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_const_bool(NautilusIRFunctionBuilderRef builder,
                                                               NautilusIRBlockRef block, bool value);
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_const_ptr(NautilusIRFunctionBuilderRef builder,
                                                              NautilusIRBlockRef block, void* value);

NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_binary(NautilusIRFunctionBuilderRef builder,
                                                           NautilusIRBlockRef block, NautilusIRBinaryOp op,
                                                           NautilusIRValueRef lhs, NautilusIRValueRef rhs);
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_compare(NautilusIRFunctionBuilderRef builder,
                                                            NautilusIRBlockRef block, NautilusIRComparator comparator,
                                                            NautilusIRValueRef lhs, NautilusIRValueRef rhs);
/* Logical not of a bool. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_not(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                        NautilusIRValueRef value);
/* Bitwise complement of an integer. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_negate(NautilusIRFunctionBuilderRef builder,
                                                           NautilusIRBlockRef block, NautilusIRValueRef value);
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_cast(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                         NautilusIRValueRef value, NautilusIRType target_type);
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_select(NautilusIRFunctionBuilderRef builder,
                                                           NautilusIRBlockRef block, NautilusIRValueRef condition,
                                                           NautilusIRValueRef true_value,
                                                           NautilusIRValueRef false_value);

NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_load(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                         NautilusIRValueRef address, NautilusIRType type);
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_store(NautilusIRFunctionBuilderRef builder,
                                                          NautilusIRBlockRef block, NautilusIRValueRef value,
                                                          NautilusIRValueRef address);
/* Pointer to stack slot @p slot of the function. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_alloca(NautilusIRFunctionBuilderRef builder,
                                                           NautilusIRBlockRef block, uint32_t slot);

/* Direct call. The result type is the callee's; a void call produces no value. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_call(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                         NautilusIRCalleeId callee, const NautilusIRValueRef* args,
                                                         size_t arg_count);
/* Call through a function-pointer value. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_indirect_call(NautilusIRFunctionBuilderRef builder,
                                                                  NautilusIRBlockRef block,
                                                                  NautilusIRValueRef function_pointer,
                                                                  const NautilusIRValueRef* args, size_t arg_count,
                                                                  NautilusIRType result_type,
                                                                  NautilusIRFunctionAttributes attributes);
/* Address of a callee, as a pointer value. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_function_address(NautilusIRFunctionBuilderRef builder,
                                                                     NautilusIRBlockRef block,
                                                                     NautilusIRCalleeId callee);

/* Terminators. Branch arguments bind, in order, to the target's arguments;
 * the entry block cannot be a target. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_branch(NautilusIRFunctionBuilderRef builder,
                                                           NautilusIRBlockRef block, NautilusIRBlockRef target,
                                                           const NautilusIRValueRef* args, size_t arg_count);
/* @p probability is the expected likelihood of the true edge, in [0, 1]. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_if(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                       NautilusIRValueRef condition, NautilusIRBlockRef true_block,
                                                       const NautilusIRValueRef* true_args, size_t true_arg_count,
                                                       NautilusIRBlockRef false_block,
                                                       const NautilusIRValueRef* false_args, size_t false_arg_count,
                                                       double probability);
/* @p value is NULL for a void return. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_build_return(NautilusIRFunctionBuilderRef builder,
                                                           NautilusIRBlockRef block, NautilusIRValueRef value);

/* ── Functions ──────────────────────────────────────────────────────────── */

/* Borrowed from the graph. */
NAUTILUS_C_API NautilusStringRef nautilus_ir_function_get_name(NautilusIRFunctionRef function);
NAUTILUS_C_API NautilusIRType nautilus_ir_function_get_return_type(NautilusIRFunctionRef function);
NAUTILUS_C_API NautilusIRCalleeId nautilus_ir_function_get_callee(NautilusIRGraphRef graph,
                                                                  NautilusIRFunctionRef function);
NAUTILUS_C_API NautilusIRBlockRef nautilus_ir_function_get_entry_block(NautilusIRFunctionRef function);
/* Copy-out accessors. */
NAUTILUS_C_API size_t nautilus_ir_function_get_blocks(NautilusIRFunctionRef function, NautilusIRBlockRef* out,
                                                      size_t capacity);
NAUTILUS_C_API size_t nautilus_ir_function_get_stack_slots(NautilusIRFunctionRef function, NautilusIRStackSlot* out,
                                                           size_t capacity);
/* NAUTILUS_ERROR_NOT_FOUND when the attribute is not set. */
NAUTILUS_C_API NautilusStatus nautilus_ir_function_get_attribute(NautilusIRFunctionRef function, NautilusStringRef key,
                                                                 NautilusString* out);

/* ── Blocks ─────────────────────────────────────────────────────────────── */

NAUTILUS_C_API uint32_t nautilus_ir_block_get_id(NautilusIRBlockRef block);
/* Copy-out accessors. */
NAUTILUS_C_API size_t nautilus_ir_block_get_arguments(NautilusIRBlockRef block, NautilusIRValueRef* out,
                                                      size_t capacity);
NAUTILUS_C_API size_t nautilus_ir_block_get_operations(NautilusIRBlockRef block, NautilusIRValueRef* out,
                                                       size_t capacity);
/* Argument @p index, or NULL (recording NAUTILUS_ERROR_INVALID_ARGUMENT) if
 * out of range. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_block_get_argument(NautilusIRBlockRef block, size_t index);
/* The block's terminator, or NULL while it has none. */
NAUTILUS_C_API NautilusIRValueRef nautilus_ir_block_get_terminator(NautilusIRBlockRef block);

/* ── Values and operations ──────────────────────────────────────────────── */

NAUTILUS_C_API NautilusIROpKind nautilus_ir_value_get_kind(NautilusIRValueRef value);
/* Result type; NAUTILUS_IR_TYPE_VOID for operations without a result. */
NAUTILUS_C_API NautilusIRType nautilus_ir_value_get_type(NautilusIRValueRef value);
/* SSA id, as printed in dumps ($<id>). */
NAUTILUS_C_API uint32_t nautilus_ir_value_get_id(NautilusIRValueRef value);
NAUTILUS_C_API bool nautilus_ir_value_is_terminator(NautilusIRValueRef value);

/* SSA operands, a copy-out accessor. Binary operations: lhs, rhs. Select:
 * condition, true value, false value. Store: value, address. Call: the
 * arguments. Indirect call: the function pointer, then the arguments. If: the
 * condition. Return: the returned value, if any. Branch: none. Block
 * arguments passed along control-flow edges are read per successor. */
NAUTILUS_C_API size_t nautilus_ir_value_get_operands(NautilusIRValueRef value, NautilusIRValueRef* out,
                                                     size_t capacity);

/* Successor blocks of a branch (1) or if (true, then false); none for other
 * operations. Copy-out accessor. */
NAUTILUS_C_API size_t nautilus_ir_value_get_successors(NautilusIRValueRef value, NautilusIRBlockRef* out,
                                                       size_t capacity);
/* Values passed to the arguments of successor @p successor. Copy-out
 * accessor; returns 0 and records NAUTILUS_ERROR_INVALID_ARGUMENT if there is
 * no such successor. */
NAUTILUS_C_API size_t nautilus_ir_value_get_successor_arguments(NautilusIRValueRef value, size_t successor,
                                                                NautilusIRValueRef* out, size_t capacity);

/* Kind-specific details. Each fails with NAUTILUS_ERROR_INVALID_ARGUMENT for a
 * value of another kind. */
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_const_int(NautilusIRValueRef value, int64_t* out);
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_const_float(NautilusIRValueRef value, double* out);
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_const_bool(NautilusIRValueRef value, bool* out);
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_const_ptr(NautilusIRValueRef value, void** out);
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_comparator(NautilusIRValueRef value, NautilusIRComparator* out);
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_bitwise_kind(NautilusIRValueRef value, NautilusIRBitwiseKind* out);
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_shift_kind(NautilusIRValueRef value, NautilusIRShiftKind* out);
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_stack_slot(NautilusIRValueRef value, uint32_t* out);
/* Call and function-address operations. */
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_callee(NautilusIRValueRef value, NautilusIRCalleeId* out);
/* If operations. */
NAUTILUS_C_API NautilusStatus nautilus_ir_value_get_branch_probability(NautilusIRValueRef value, double* out);

/* ── Optimization and compilation ───────────────────────────────────────── */

/* True if the backend ("mlir", "cpp", "bc", "tbc", "asmjit") is compiled into
 * this build. */
NAUTILUS_C_API bool nautilus_ir_backend_is_available(NautilusStringRef backend);

/* Runs the IR pass pipeline on the graph, in place. Compiling runs it too;
 * call this to inspect the optimized IR or to pick the level yourself. Runs
 * at most once per graph: later calls, and compiles, reuse the result.
 * NAUTILUS_IR_OPTIMIZE_BACKEND_DEFAULT means FULL here. @p options may be
 * NULL. Every builder must be finished or disposed first. */
NAUTILUS_C_API NautilusStatus nautilus_ir_graph_optimize(NautilusIRGraphRef graph, NautilusIROptimizationLevel level,
                                                         NautilusOptionsRef options);

/* Optimizes the graph (at the backend's level, unless already optimized) and
 * compiles every function in it with @p backend; an empty name picks the
 * build's default backend. @p options may be NULL. To compile through a
 * configured engine instead, see nautilus/c/engine.h. */
NAUTILUS_C_API NautilusExecutableRef nautilus_ir_graph_compile(NautilusIRGraphRef graph, NautilusStringRef backend,
                                                               NautilusOptionsRef options);

#ifdef __cplusplus
}
#endif

#endif /* NAUTILUS_C_IR_H */
