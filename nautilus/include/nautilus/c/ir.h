/*
 * C API for the Nautilus IR.
 *
 * Builds, inspects, verifies and compiles Nautilus IR graphs from C (or any
 * language with a C FFI) without going through the C++ tracing frontend.
 *
 * Ownership model
 * ---------------
 * - A NautilusIRGraphRef owns every function, block and value created in it.
 *   Function, block and value refs are borrowed pointers into the graph and
 *   stay valid until the graph is disposed (or, for inspection refs, until a
 *   compile runs the IR passes, which may rewrite the graph in place).
 * - A NautilusIRFunctionBuilderRef is owned by its graph until it is finished
 *   with nautilus_ir_function_builder_finish(), which consumes it.
 * - A NautilusIRExecutableRef is independent of the graph that produced it and
 *   must be released with nautilus_ir_executable_dispose(). Function pointers
 *   obtained from it are valid only while it is alive.
 * - Strings returned as `char*` are heap-allocated and must be released with
 *   nautilus_ir_string_dispose(). Strings returned as `const char*` are
 *   borrowed from the graph.
 *
 * Errors
 * ------
 * Functions that can fail return NULL, NAUTILUS_IR_INVALID_CALLEE or a
 * non-zero status, and record a message retrievable on the same thread with
 * nautilus_ir_get_last_error().
 *
 * Example: int64_t add(int64_t a, int64_t b) { return a + b; }
 *
 *     NautilusIRGraphRef g = nautilus_ir_graph_create("example");
 *     NautilusIRFunctionBuilderRef fb = nautilus_ir_function_builder_create(g, "add", NAUTILUS_IR_TYPE_I64);
 *     NautilusIRType params[] = {NAUTILUS_IR_TYPE_I64, NAUTILUS_IR_TYPE_I64};
 *     NautilusIRBlockRef entry = nautilus_ir_function_builder_add_block(fb, params, 2);
 *     NautilusIRValueRef sum = nautilus_ir_build_binary(fb, entry, NAUTILUS_IR_BINARY_ADD,
 *                                                       nautilus_ir_block_get_argument(entry, 0),
 *                                                       nautilus_ir_block_get_argument(entry, 1));
 *     nautilus_ir_build_return(fb, entry, sum);
 *     nautilus_ir_function_builder_finish(fb);
 *
 *     NautilusIRExecutableRef exe = nautilus_ir_graph_compile(g, "bc", NULL);
 *     int64_t (*fn)(int64_t, int64_t) = (int64_t (*)(int64_t, int64_t)) nautilus_ir_executable_get_function(exe,
 * "add");
 *     ...
 *     nautilus_ir_executable_dispose(exe);
 *     nautilus_ir_graph_dispose(g);
 */
#ifndef NAUTILUS_C_IR_H
#define NAUTILUS_C_IR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ── Opaque handles ─────────────────────────────────────────────────────── */

typedef struct NautilusIROpaqueGraph* NautilusIRGraphRef;
typedef struct NautilusIROpaqueFunctionBuilder* NautilusIRFunctionBuilderRef;
typedef struct NautilusIROpaqueFunction* NautilusIRFunctionRef;
typedef struct NautilusIROpaqueBlock* NautilusIRBlockRef;
/* Any operation. Operations that produce a result (and block arguments) are
 * the SSA values of the IR, so a value ref doubles as an operation ref. */
typedef struct NautilusIROpaqueValue* NautilusIRValueRef;
typedef struct NautilusIROpaqueOptions* NautilusIROptionsRef;
typedef struct NautilusIROpaqueExecutable* NautilusIRExecutableRef;

/* Entry in the graph's function table; the target of a call. */
typedef uint32_t NautilusIRCalleeId;
#define NAUTILUS_IR_INVALID_CALLEE ((NautilusIRCalleeId) 0xFFFFFFFFu)

/* ── Enumerations ───────────────────────────────────────────────────────── */

typedef enum {
	NAUTILUS_IR_TYPE_VOID = 0,
	NAUTILUS_IR_TYPE_BOOL,
	NAUTILUS_IR_TYPE_I8,
	NAUTILUS_IR_TYPE_I16,
	NAUTILUS_IR_TYPE_I32,
	NAUTILUS_IR_TYPE_I64,
	NAUTILUS_IR_TYPE_UI8,
	NAUTILUS_IR_TYPE_UI16,
	NAUTILUS_IR_TYPE_UI32,
	NAUTILUS_IR_TYPE_UI64,
	NAUTILUS_IR_TYPE_F32,
	NAUTILUS_IR_TYPE_F64,
	NAUTILUS_IR_TYPE_PTR,
} NautilusIRType;

typedef enum {
	NAUTILUS_IR_OP_ADD = 0,
	NAUTILUS_IR_OP_AND,
	NAUTILUS_IR_OP_NOT,
	NAUTILUS_IR_OP_BLOCK_ARGUMENT,
	NAUTILUS_IR_OP_BLOCK_INVOCATION,
	NAUTILUS_IR_OP_BRANCH,
	NAUTILUS_IR_OP_CONST_INT,
	NAUTILUS_IR_OP_CONST_BOOL,
	NAUTILUS_IR_OP_CONST_PTR,
	NAUTILUS_IR_OP_CONST_FLOAT,
	NAUTILUS_IR_OP_CAST,
	NAUTILUS_IR_OP_COMPARE,
	NAUTILUS_IR_OP_DIV,
	NAUTILUS_IR_OP_MOD,
	NAUTILUS_IR_OP_FUNCTION,
	NAUTILUS_IR_OP_IF,
	NAUTILUS_IR_OP_LOAD,
	NAUTILUS_IR_OP_MUL,
	NAUTILUS_IR_OP_MLIR_YIELD,
	NAUTILUS_IR_OP_NEGATE,
	NAUTILUS_IR_OP_OR,
	NAUTILUS_IR_OP_CALL,
	NAUTILUS_IR_OP_INDIRECT_CALL,
	NAUTILUS_IR_OP_RETURN,
	NAUTILUS_IR_OP_SELECT,
	NAUTILUS_IR_OP_STORE,
	NAUTILUS_IR_OP_SUB,
	NAUTILUS_IR_OP_BITWISE,
	NAUTILUS_IR_OP_SHIFT,
	NAUTILUS_IR_OP_ALLOCA,
	NAUTILUS_IR_OP_FUNCTION_ADDRESS_OF,
} NautilusIROpKind;

/* Two-operand operations accepted by nautilus_ir_build_binary(). */
typedef enum {
	NAUTILUS_IR_BINARY_ADD = 0,
	NAUTILUS_IR_BINARY_SUB,
	NAUTILUS_IR_BINARY_MUL,
	NAUTILUS_IR_BINARY_DIV,
	NAUTILUS_IR_BINARY_MOD,
	/* Logical and/or on bool operands; the result is bool. */
	NAUTILUS_IR_BINARY_LOGICAL_AND,
	NAUTILUS_IR_BINARY_LOGICAL_OR,
	/* Bitwise and/or/xor on integer operands. */
	NAUTILUS_IR_BINARY_BITWISE_AND,
	NAUTILUS_IR_BINARY_BITWISE_OR,
	NAUTILUS_IR_BINARY_BITWISE_XOR,
	NAUTILUS_IR_BINARY_SHIFT_LEFT,
	NAUTILUS_IR_BINARY_SHIFT_RIGHT,
} NautilusIRBinaryOp;

typedef enum {
	NAUTILUS_IR_CMP_EQ = 0,
	NAUTILUS_IR_CMP_NE,
	NAUTILUS_IR_CMP_LT,
	NAUTILUS_IR_CMP_LE,
	NAUTILUS_IR_CMP_GT,
	NAUTILUS_IR_CMP_GE,
} NautilusIRComparator;

typedef enum {
	NAUTILUS_IR_BITWISE_AND = 0,
	NAUTILUS_IR_BITWISE_OR,
	NAUTILUS_IR_BITWISE_XOR,
} NautilusIRBitwiseKind;

typedef enum {
	NAUTILUS_IR_SHIFT_LEFT = 0,
	NAUTILUS_IR_SHIFT_RIGHT,
} NautilusIRShiftKind;

typedef enum {
	NAUTILUS_IR_LINKAGE_INTERNAL = 0,
	NAUTILUS_IR_LINKAGE_EXTERNAL,
	NAUTILUS_IR_LINKAGE_INTRINSIC,
} NautilusIRLinkage;

typedef enum {
	NAUTILUS_IR_MOD_REF_NONE = 0,
	NAUTILUS_IR_MOD_REF_REF = 1,
	NAUTILUS_IR_MOD_REF_MOD = 2,
	NAUTILUS_IR_MOD_REF_MOD_REF = 3,
} NautilusIRModRef;

/* What a callee may do; lets the IR passes and backends drop or reorder calls.
 * Start from nautilus_ir_function_attributes_default() (the pessimistic
 * "may do anything" set) and relax only what is known to hold. */
typedef struct {
	NautilusIRModRef mod_ref;
	int will_return;
	int no_unwind;
} NautilusIRFunctionAttributes;

/* How much of the IR optimization pipeline runs before the backend. */
typedef enum {
	/* Only the analyses every backend needs. */
	NAUTILUS_IR_OPTIMIZE_NONE = 0,
	/* Block-argument pruning only. */
	NAUTILUS_IR_OPTIMIZE_ARGUMENT_PRUNING,
	/* Every optimization pass. */
	NAUTILUS_IR_OPTIMIZE_FULL,
	/* Whatever the selected backend asks for. */
	NAUTILUS_IR_OPTIMIZE_BACKEND_DEFAULT,
} NautilusIROptimizationLevel;

/* ── Errors and strings ─────────────────────────────────────────────────── */

/* Message of the last failed call on this thread, or NULL. Borrowed; valid
 * until the next failing call on this thread. */
const char* nautilus_ir_get_last_error(void);

void nautilus_ir_string_dispose(char* str);

const char* nautilus_ir_type_name(NautilusIRType type);

NautilusIRFunctionAttributes nautilus_ir_function_attributes_default(void);

/* ── Graphs ─────────────────────────────────────────────────────────────── */

NautilusIRGraphRef nautilus_ir_graph_create(const char* id);

/* Releases the graph, every function, block and value in it, and every
 * builder that has not been finished. */
void nautilus_ir_graph_dispose(NautilusIRGraphRef graph);

/* Renders the graph in the textual form used by Nautilus' IR dumps. */
char* nautilus_ir_graph_to_string(NautilusIRGraphRef graph);

/* Runs the IR verifier. Returns 0 when the graph is well-formed; otherwise
 * returns non-zero and, if @p error_message is non-NULL, stores a
 * newline-separated list of problems in it (dispose with
 * nautilus_ir_string_dispose()). */
int nautilus_ir_graph_verify(NautilusIRGraphRef graph, char** error_message);

size_t nautilus_ir_graph_get_function_count(NautilusIRGraphRef graph);
NautilusIRFunctionRef nautilus_ir_graph_get_function(NautilusIRGraphRef graph, size_t index);
/* NULL when no finished function has that name. */
NautilusIRFunctionRef nautilus_ir_graph_get_function_by_name(NautilusIRGraphRef graph, const char* name);

/* Declares a native function at @p address that IR in this graph may call.
 * Declaring the same address twice returns the same id. @p symbol is the
 * linker-level name (may be NULL); @p display_name is shown in dumps. */
NautilusIRCalleeId nautilus_ir_graph_declare_external_function(NautilusIRGraphRef graph, const char* symbol,
                                                               const char* display_name, void* address,
                                                               NautilusIRType result_type,
                                                               const NautilusIRType* param_types, size_t param_count,
                                                               NautilusIRFunctionAttributes attributes);

size_t nautilus_ir_graph_get_callee_count(NautilusIRGraphRef graph);
NautilusIRLinkage nautilus_ir_graph_get_callee_linkage(NautilusIRGraphRef graph, NautilusIRCalleeId callee);
/* The identifier a backend emits the callee under. Borrowed. */
const char* nautilus_ir_graph_get_callee_name(NautilusIRGraphRef graph, NautilusIRCalleeId callee);
/* Native address of an external callee, NULL for an internal one. */
void* nautilus_ir_graph_get_callee_address(NautilusIRGraphRef graph, NautilusIRCalleeId callee);
/* Definition of an internal callee; NULL for external callees and for
 * internal ones whose builder has not been finished yet. */
NautilusIRFunctionRef nautilus_ir_graph_get_callee_function(NautilusIRGraphRef graph, NautilusIRCalleeId callee);

/* ── Function builders ──────────────────────────────────────────────────── */

/* Starts a function. Its callee id exists immediately, so the function can
 * be called (including recursively) before it is finished. Names must be
 * unique within the graph. */
NautilusIRFunctionBuilderRef nautilus_ir_function_builder_create(NautilusIRGraphRef graph, const char* name,
                                                                 NautilusIRType return_type);

NautilusIRCalleeId nautilus_ir_function_builder_get_callee(NautilusIRFunctionBuilderRef builder);

/* Sets a string attribute on the function (e.g. "entry" = "true"). */
int nautilus_ir_function_builder_set_attribute(NautilusIRFunctionBuilderRef builder, const char* key,
                                               const char* value);

/* Adds a basic block with the given argument types. The first block added is
 * the entry block, and its arguments are the function's parameters. */
NautilusIRBlockRef nautilus_ir_function_builder_add_block(NautilusIRFunctionBuilderRef builder,
                                                          const NautilusIRType* arg_types, size_t arg_count);

/* Reserves a stack slot in the function frame and returns its index, for use
 * with nautilus_ir_build_alloca(). */
uint32_t nautilus_ir_function_builder_add_stack_slot(NautilusIRFunctionBuilderRef builder, size_t size, size_t align);

/* Finishes the function and adds it to the graph. Consumes the builder, even
 * on failure. Every block must end in a terminator (branch, if or return). */
NautilusIRFunctionRef nautilus_ir_function_builder_finish(NautilusIRFunctionBuilderRef builder);

/* ── Instruction building ───────────────────────────────────────────────────
 * Every nautilus_ir_build_* call appends one operation to the end of @p block,
 * which must have been added to @p builder, and returns it. Nothing can be
 * appended after a terminator. Operands must be values of the same function
 * that are defined before their use (nautilus_ir_graph_verify() checks it). */

/* Integer constant of an integer type; the bits of @p value are reinterpreted
 * for unsigned types. */
NautilusIRValueRef nautilus_ir_build_const_int(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                               int64_t value, NautilusIRType type);
NautilusIRValueRef nautilus_ir_build_const_float(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                 double value, NautilusIRType type);
NautilusIRValueRef nautilus_ir_build_const_bool(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                int value);
NautilusIRValueRef nautilus_ir_build_const_ptr(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                               void* value);

NautilusIRValueRef nautilus_ir_build_binary(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRBinaryOp op, NautilusIRValueRef lhs, NautilusIRValueRef rhs);
NautilusIRValueRef nautilus_ir_build_compare(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                             NautilusIRComparator comparator, NautilusIRValueRef lhs,
                                             NautilusIRValueRef rhs);
/* Logical not of a bool. */
NautilusIRValueRef nautilus_ir_build_not(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                         NautilusIRValueRef value);
/* Bitwise complement of an integer. */
NautilusIRValueRef nautilus_ir_build_negate(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRValueRef value);
NautilusIRValueRef nautilus_ir_build_cast(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                          NautilusIRValueRef value, NautilusIRType target_type);
NautilusIRValueRef nautilus_ir_build_select(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRValueRef condition, NautilusIRValueRef true_value,
                                            NautilusIRValueRef false_value);

NautilusIRValueRef nautilus_ir_build_load(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                          NautilusIRValueRef address, NautilusIRType type);
NautilusIRValueRef nautilus_ir_build_store(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                           NautilusIRValueRef value, NautilusIRValueRef address);
/* Pointer to the stack slot @p slot of the enclosing function. */
NautilusIRValueRef nautilus_ir_build_alloca(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            uint32_t slot);

/* Direct call to a callee in the graph's function table. The result type is
 * the callee's; for a void callee the returned operation has no value. */
NautilusIRValueRef nautilus_ir_build_call(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                          NautilusIRCalleeId callee, const NautilusIRValueRef* args, size_t arg_count);
/* Call through a function-pointer value. */
NautilusIRValueRef nautilus_ir_build_indirect_call(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                   NautilusIRValueRef function_pointer, const NautilusIRValueRef* args,
                                                   size_t arg_count, NautilusIRType result_type,
                                                   NautilusIRFunctionAttributes attributes);
/* Address of a callee as a pointer value. */
NautilusIRValueRef nautilus_ir_build_function_address(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                                      NautilusIRCalleeId callee);

/* Terminators. Branch arguments bind, in order, to the target's arguments. */
NautilusIRValueRef nautilus_ir_build_branch(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRBlockRef target, const NautilusIRValueRef* args,
                                            size_t arg_count);
/* @p probability is the expected likelihood of taking the true edge (0..1). */
NautilusIRValueRef nautilus_ir_build_if(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                        NautilusIRValueRef condition, NautilusIRBlockRef true_block,
                                        const NautilusIRValueRef* true_args, size_t true_arg_count,
                                        NautilusIRBlockRef false_block, const NautilusIRValueRef* false_args,
                                        size_t false_arg_count, double probability);
/* @p value is NULL for a void return. */
NautilusIRValueRef nautilus_ir_build_return(NautilusIRFunctionBuilderRef builder, NautilusIRBlockRef block,
                                            NautilusIRValueRef value);

/* ── Function inspection ────────────────────────────────────────────────── */

const char* nautilus_ir_function_get_name(NautilusIRFunctionRef function);
NautilusIRType nautilus_ir_function_get_return_type(NautilusIRFunctionRef function);
NautilusIRCalleeId nautilus_ir_function_get_callee(NautilusIRGraphRef graph, NautilusIRFunctionRef function);
size_t nautilus_ir_function_get_block_count(NautilusIRFunctionRef function);
NautilusIRBlockRef nautilus_ir_function_get_block(NautilusIRFunctionRef function, size_t index);
size_t nautilus_ir_function_get_stack_slot_count(NautilusIRFunctionRef function);
int nautilus_ir_function_get_stack_slot(NautilusIRFunctionRef function, uint32_t slot, size_t* size, size_t* align);
/* NULL when the attribute is not set; dispose with nautilus_ir_string_dispose(). */
char* nautilus_ir_function_get_attribute(NautilusIRFunctionRef function, const char* key);

/* ── Block inspection ───────────────────────────────────────────────────── */

uint32_t nautilus_ir_block_get_id(NautilusIRBlockRef block);
size_t nautilus_ir_block_get_argument_count(NautilusIRBlockRef block);
NautilusIRValueRef nautilus_ir_block_get_argument(NautilusIRBlockRef block, size_t index);
size_t nautilus_ir_block_get_operation_count(NautilusIRBlockRef block);
NautilusIRValueRef nautilus_ir_block_get_operation(NautilusIRBlockRef block, size_t index);
/* The block's last operation if it is a terminator, else NULL. */
NautilusIRValueRef nautilus_ir_block_get_terminator(NautilusIRBlockRef block);

/* ── Operation / value inspection ───────────────────────────────────────── */

NautilusIROpKind nautilus_ir_value_get_kind(NautilusIRValueRef value);
/* Result type; NAUTILUS_IR_TYPE_VOID for operations without a result. */
NautilusIRType nautilus_ir_value_get_type(NautilusIRValueRef value);
/* SSA id as printed in dumps ($<id>). */
uint32_t nautilus_ir_value_get_id(NautilusIRValueRef value);
int nautilus_ir_value_is_terminator(NautilusIRValueRef value);

/* SSA operands. For branch and if, these are the block-invocation arguments;
 * use the successor accessors below for structured access. */
size_t nautilus_ir_value_get_operand_count(NautilusIRValueRef value);
NautilusIRValueRef nautilus_ir_value_get_operand(NautilusIRValueRef value, size_t index);

/* Constants. Each returns 0 on success and non-zero if the value has another kind. */
int nautilus_ir_value_get_const_int(NautilusIRValueRef value, int64_t* out);
int nautilus_ir_value_get_const_float(NautilusIRValueRef value, double* out);
int nautilus_ir_value_get_const_bool(NautilusIRValueRef value, int* out);
int nautilus_ir_value_get_const_ptr(NautilusIRValueRef value, void** out);

/* Kind-specific details; each returns 0 on success, non-zero on kind mismatch. */
int nautilus_ir_value_get_comparator(NautilusIRValueRef value, NautilusIRComparator* out);
int nautilus_ir_value_get_bitwise_kind(NautilusIRValueRef value, NautilusIRBitwiseKind* out);
int nautilus_ir_value_get_shift_kind(NautilusIRValueRef value, NautilusIRShiftKind* out);
int nautilus_ir_value_get_stack_slot(NautilusIRValueRef value, uint32_t* out);
/* Call and function-address-of only. */
int nautilus_ir_value_get_callee(NautilusIRValueRef value, NautilusIRCalleeId* out);
/* If only. */
int nautilus_ir_value_get_branch_probability(NautilusIRValueRef value, double* out);

/* Successors of a branch (1) or if (2: true, false); 0 for other operations. */
size_t nautilus_ir_value_get_successor_count(NautilusIRValueRef value);
NautilusIRBlockRef nautilus_ir_value_get_successor(NautilusIRValueRef value, size_t index);
size_t nautilus_ir_value_get_successor_argument_count(NautilusIRValueRef value, size_t index);
NautilusIRValueRef nautilus_ir_value_get_successor_argument(NautilusIRValueRef value, size_t index, size_t arg_index);

/* ── Compilation ────────────────────────────────────────────────────────── */

/* Engine/module options, as documented in docs/options.md (for example
 * "dump.all", "ir.enableLICM", "mlir.optimizationLevel"). */
NautilusIROptionsRef nautilus_ir_options_create(void);
void nautilus_ir_options_dispose(NautilusIROptionsRef options);
void nautilus_ir_options_set_bool(NautilusIROptionsRef options, const char* name, int value);
void nautilus_ir_options_set_int(NautilusIROptionsRef options, const char* name, int value);
void nautilus_ir_options_set_double(NautilusIROptionsRef options, const char* name, double value);
void nautilus_ir_options_set_string(NautilusIROptionsRef options, const char* name, const char* value);

/* Returns non-zero if a backend of that name ("mlir", "cpp", "bc", "tbc",
 * "asmjit") is compiled into this build. */
int nautilus_ir_backend_is_available(const char* backend);

/* Runs the IR pass pipeline on the graph in place. Compiling runs it too, so
 * calling this is only needed to inspect the optimized IR. Every function
 * must be finished first. */
int nautilus_ir_graph_optimize(NautilusIRGraphRef graph, NautilusIROptimizationLevel level,
                               NautilusIROptionsRef options);

/* Runs the IR pass pipeline (at the backend's default level) and compiles
 * every function in the graph with @p backend. @p options may be NULL. The
 * graph is optimized in place and can no longer be extended afterwards. */
NautilusIRExecutableRef nautilus_ir_graph_compile(NautilusIRGraphRef graph, const char* backend,
                                                  NautilusIROptionsRef options);

/* Native entry point of the compiled function @p name; cast it to the
 * function's C signature. NULL if there is no such function. */
void* nautilus_ir_executable_get_function(NautilusIRExecutableRef executable, const char* name);

void nautilus_ir_executable_dispose(NautilusIRExecutableRef executable);

#ifdef __cplusplus
}
#endif

#endif /* NAUTILUS_C_IR_H */
