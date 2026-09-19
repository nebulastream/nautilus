// Lightweight, regex-based parser for Nautilus IR (the dialect dumped into
// `nautilus/test/data/*/ir/*.nautilus`).
//
// Grammar reference (informally):
//
//   module      ::= 'nautilus' '{' declaration* function* '}' '//nautilus'?
//   declaration ::= 'declare' ('external' | 'intrinsic') '#' INT CALLEE
//                   '(' type-list? ')' ':' TYPE attributes?
//   function    ::= IDENT '(' arg-list? ')' (':' TYPE)? attributes?
//                   '{' block+ exception-region? '}'
//   attributes  ::= '[' ATTR (',' ATTR)* ']'      // readnone | readonly |
//                                                 // writeonly | willreturn |
//                                                 // nounwind
//   block       ::= 'Block_' INT '(' arg-list? ')' ':' statement*
//   arg-list    ::= typed-ssa (',' typed-ssa)*
//   typed-ssa   ::= '$' INT ':' TYPE
//   statement   ::= def | bare-decl | branch | conditional | return | store | void-call
//   def         ::= '$' INT '=' rhs ':' TYPE pad-link?
//   bare-decl   ::= '$' INT ':' TYPE
//   rhs         ::= constant | unary | binary | cast | load | alloca
//                 | func-addr | call
//   unary       ::= ('!' | '~') '$' INT
//   binary      ::= '$' INT BIN-OP ('$' INT | INT)
//   cast        ::= '$' INT 'cast_to' TYPE
//   load        ::= 'load' '(' '$' INT ')'
//   alloca      ::= 'alloca' '[' INT ']'          // index into the alloca table
//   func-addr   ::= 'func_addr' CALLEE
//   call        ::= 'call' (CALLEE | '$' INT) '(' arg* ')'
//   branch      ::= 'br' BLOCK '(' ssa-list? ')' ':' 'void'
//   conditional ::= 'if' '$' INT '?' BLOCK '(' ssa-list? ')' ':' BLOCK '(' ssa-list? ')' ':' 'void'
//   return      ::= 'return' ('(' '$' INT ')')? ':' TYPE
//   store       ::= 'store' '(' '$' INT ',' '$' INT ')' ':' 'void'
//   void-call   ::= 'call' (CALLEE | '$' INT) '(' arg* ')' ':' 'void' pad-link?
//   pad-link    ::= '->' 'pad_' INT
//
//   exception-region ::= 'exception_region:' pad+
//   pad              ::= 'pad_' INT ':' statement*
//
// A CALLEE is either an internal function's own name (`add`), or the withheld
// spelling of a native one suffixed with its function-table id (`func_*#3`);
// `IRGraph.cpp` decides which. An operation line may carry a trailing
// `; at file:line (function)` source-location comment when the dump was taken
// with `dump.sourceLocations` on, with `; inlined from ...` continuation lines
// under it.
//
// Function names are arbitrary identifiers (the most common is `execute`,
// but multi-function modules also contain `add`, `inner`, `outer`,
// `sumFields`, `loopHelper`, etc.).

import * as vscode from 'vscode';

export interface SsaDefinition {
	name: string;
	line: number;
	character: number;
	rhs: string;             // empty string for bare-typed declarations
	type: string | undefined;
	functionName: string;    // owning function
}

export interface SsaReference {
	name: string;
	line: number;
	character: number;
	functionName: string;
}

export interface BlockArgument {
	name: string;
	type: string;
}

export type Terminator = 'return' | 'br' | 'if' | 'unknown';

export interface Block {
	name: string;
	functionName: string;
	headerLine: number;
	headerRange: vscode.Range;
	bodyRange: vscode.Range;
	args: BlockArgument[];
	successors: string[];
	predecessors: string[];
	/**
	 * Landing pads (`pad_0`, …) this block's calls unwind to, from their
	 * `-> pad_N` links. Not successors: control only reaches them when a call
	 * throws, so they stay off the normal-path edges.
	 */
	padLinks: string[];
	instructions: number;
	terminator: Terminator;
}

/**
 * One landing pad from a function's `exception_region:` section — the cleanup
 * code an unwinding call branches to, emitted by ExceptionRegionPreparationPass.
 * Pads are not basic blocks: they sit outside the CFG and are reached by the
 * `-> pad_N` link on a call rather than by a terminator, so they are tracked
 * separately and never appear in `blocks`.
 */
export interface LandingPad {
	name: string;                          // `pad_0`, `pad_1`, …
	functionName: string;
	headerLine: number;
	headerRange: vscode.Range;
	bodyRange: vscode.Range;
	instructions: number;
}

export interface IrFunction {
	name: string;
	args: BlockArgument[];                 // typed parameters from the header
	returnType: string | undefined;        // primitive type after `):` (undefined if absent)
	attributes: string[];                  // `[readnone, nounwind]` suffix, empty when absent
	headerLine: number;
	headerRange: vscode.Range;
	bodyRange: vscode.Range;
	blocks: Block[];
	blockByName: Map<string, Block>;
	definitions: Map<string, SsaDefinition>;
	pads: LandingPad[];
}

/** One entry of the module's `declare` region: a callee defined elsewhere. */
export interface Declaration {
	linkage: 'external' | 'intrinsic';
	id: number;                            // function-table id, as printed after `#`
	callee: string;                        // `func_*` unless address logging is on
	line: number;
	attributes: string[];
}

export interface ParsedIr {
	moduleRange: vscode.Range | undefined;
	declarations: Declaration[];
	functions: IrFunction[];
	functionByName: Map<string, IrFunction>;
	// Convenience: aggregate views across the whole module.
	blocks: Block[];
	blockByName: Map<string, Block>;
	definitions: Map<string, SsaDefinition>;
	references: Map<string, SsaReference[]>;
}

// Matches "name(arg-list?) :returnType? [attrs]? {". The arg list, the return
// type and the attribute suffix are all optional: the first two to support
// legacy dumps, the third because FunctionAttributeInferencePass only prints
// what it could prove beyond the pessimistic default.
const FUNCTION_HEADER_RE =
	/^\s*([A-Za-z_][A-Za-z0-9_]*)\s*\(([^)]*)\)\s*(?::\s*([a-z0-9]+))?\s*(?:\[([^\]]*)\])?\s*\{\s*$/;
// The module's declaration region: one line per callee defined outside it.
const DECLARE_RE =
	/^\s*declare\s+(external|intrinsic)\s+#(\d+)\s+(\S+?)\s*\([^)]*\)\s*:\s*[a-z0-9]+\s*(?:\[([^\]]*)\])?\s*$/;
const BLOCK_HEADER_RE = /^\s*(Block_\d+)\s*\(([^)]*)\)\s*:\s*$/;
// Opens a function's landing-pad section; every `pad_N:` until the closing
// brace belongs to it.
const EXCEPTION_REGION_RE = /^\s*exception_region:\s*$/;
const PAD_HEADER_RE = /^\s*(pad_\d+):\s*$/;
// The `-> pad_N` link an unwinding call carries after its type suffix.
const PAD_LINK_RE = /->\s*(pad_\d+)\s*$/;
const SSA_DEF_RE = /^\s*(\$\d+)\s*=\s*(.+?)\s*$/;
const BARE_DECL_RE = /^\s*(\$\d+)\s*:\s*([a-z0-9]+)\s*$/;
const SSA_REF_RE = /\$\d+/g;
const BLOCK_REF_RE = /\bBlock_\d+\b/g;
// The type suffix ends the operation, except for the `-> pad_N` link an
// unwinding call carries after it.
const TYPE_TAIL_RE =
	/:\s*(i8|i16|i32|i64|ui8|ui16|ui32|ui64|f32|f64|bool|ptr|void)\s*(?:->\s*pad_\d+\s*)?$/;
const PRIMITIVE_TYPES = new Set([
	'i8', 'i16', 'i32', 'i64',
	'ui8', 'ui16', 'ui32', 'ui64',
	'f32', 'f64', 'bool', 'ptr', 'void',
]);

const MODULE_OPEN_RE = /^\s*nautilus\s*\{\s*$/;
const MODULE_KEYWORD = 'nautilus';

export function parse(document: vscode.TextDocument): ParsedIr {
	const ir: ParsedIr = {
		moduleRange: undefined,
		declarations: [],
		functions: [],
		functionByName: new Map(),
		blocks: [],
		blockByName: new Map(),
		definitions: new Map(),
		references: new Map(),
	};

	const lineCount = document.lineCount;
	let moduleStart = -1;
	let currentFn: IrFunction | undefined;
	let currentBlock: Block | undefined;
	// Set once `exception_region:` opens; the pads that follow are collected
	// on the function instead of being folded into the block above them.
	let currentPad: LandingPad | undefined;
	let inExceptionRegion = false;

	for (let i = 0; i < lineCount; i++) {
		const text = document.lineAt(i).text;
		const code = stripComment(text);
		const trimmed = code.trim();

		if (MODULE_OPEN_RE.test(text)) {
			moduleStart = i;
			continue;
		}

		// Declaration region: callees defined outside this module. Recorded,
		// but never a function header — they carry no body.
		const declMatch = code.match(DECLARE_RE);
		if (declMatch && currentFn === undefined) {
			ir.declarations.push({
				linkage: declMatch[1] as 'external' | 'intrinsic',
				id: Number.parseInt(declMatch[2], 10),
				callee: declMatch[3],
				line: i,
				attributes: splitAttributes(declMatch[4]),
			});
			continue;
		}

		// Function header. Distinguish from block headers (Block_N) and
		// from the literal `nautilus {` module-open line.
		const fnMatch = code.match(FUNCTION_HEADER_RE);
		if (fnMatch && fnMatch[1] !== MODULE_KEYWORD) {
			finalizeBlock(currentBlock, i - 1, document);
			finalizePad(currentPad, i - 1, document);
			finalizeFunction(currentFn, i - 1, document);
			const argList = fnMatch[2].trim();
			const args: BlockArgument[] = argList === ''
				? []
				: argList.split(',').map(rawArgToken);
			const returnType = fnMatch[3] && PRIMITIVE_TYPES.has(fnMatch[3]) ? fnMatch[3] : undefined;
			currentFn = {
				name: fnMatch[1],
				args,
				returnType,
				attributes: splitAttributes(fnMatch[4]),
				headerLine: i,
				headerRange: new vscode.Range(i, 0, i, text.length),
				bodyRange: new vscode.Range(i, 0, i, text.length),
				blocks: [],
				blockByName: new Map(),
				definitions: new Map(),
				pads: [],
			};
			ir.functions.push(currentFn);
			ir.functionByName.set(currentFn.name, currentFn);
			currentBlock = undefined;
			currentPad = undefined;
			inExceptionRegion = false;
			continue;
		}

		// `exception_region:` closes the last basic block: everything below it
		// is landing-pad cleanup, reached by a call's `-> pad_N` link rather
		// than by any terminator.
		if (currentFn && EXCEPTION_REGION_RE.test(code)) {
			finalizeBlock(currentBlock, i - 1, document);
			currentBlock = undefined;
			inExceptionRegion = true;
			continue;
		}

		const padMatch = code.match(PAD_HEADER_RE);
		if (padMatch && currentFn && inExceptionRegion) {
			finalizePad(currentPad, i - 1, document);
			currentPad = {
				name: padMatch[1],
				functionName: currentFn.name,
				headerLine: i,
				headerRange: new vscode.Range(i, 0, i, text.length),
				bodyRange: new vscode.Range(i, 0, i, text.length),
				instructions: 0,
			};
			currentFn.pads.push(currentPad);
			continue;
		}

		// Block header.
		const headerMatch = code.match(BLOCK_HEADER_RE);
		if (headerMatch && currentFn) {
			finalizeBlock(currentBlock, i - 1, document);
			const name = headerMatch[1];
			const argList = headerMatch[2].trim();
			const args: BlockArgument[] = argList === ''
				? []
				: argList.split(',').map(rawArgToken);
			currentBlock = {
				name,
				functionName: currentFn.name,
				headerLine: i,
				headerRange: new vscode.Range(i, 0, i, text.length),
				bodyRange: new vscode.Range(i, 0, i, text.length),
				args,
				successors: [],
				predecessors: [],
				padLinks: [],
				instructions: 0,
				terminator: 'unknown',
			};
			currentFn.blocks.push(currentBlock);
			currentFn.blockByName.set(name, currentBlock);
			ir.blocks.push(currentBlock);
			// Aggregate by `function::block` to keep names unique across functions
			// while still supporting plain `Block_0` lookups for the active function.
			ir.blockByName.set(`${currentFn.name}::${name}`, currentBlock);
			if (!ir.blockByName.has(name)) {
				ir.blockByName.set(name, currentBlock);
			}
			continue;
		}

		// Function-end brace.
		if (trimmed === '}') {
			finalizeBlock(currentBlock, i - 1, document);
			currentBlock = undefined;
			finalizePad(currentPad, i - 1, document);
			currentPad = undefined;
			inExceptionRegion = false;
			finalizeFunction(currentFn, i - 1, document);
			currentFn = undefined;
			continue;
		}

		// Module-end brace + comment ("} //NESIR") was already split above; ignore.
		if (trimmed === '' || trimmed === '{') {
			continue;
		}

		// Inside a landing pad: the destructor calls it runs are real uses of
		// the function's SSA values, so they are indexed like any other
		// operand — but they define nothing and branch nowhere.
		if (inExceptionRegion) {
			if (currentPad && currentFn) {
				currentPad.instructions++;
				collectSsaReferences(ir, currentFn.name, text, 0, i);
				currentPad.bodyRange = new vscode.Range(currentPad.headerLine, 0, i, text.length);
			}
			continue;
		}

		// Inside a block: capture defs, bare declarations, and references.
		if (currentBlock && currentFn) {
			currentBlock.instructions++;

			// Bare typed declaration: `$N :type`
			const bareMatch = code.match(BARE_DECL_RE);
			if (bareMatch && PRIMITIVE_TYPES.has(bareMatch[2])) {
				const name = bareMatch[1];
				const def: SsaDefinition = {
					name,
					line: i,
					character: text.indexOf(name),
					rhs: '',
					type: bareMatch[2],
					functionName: currentFn.name,
				};
				registerDef(ir, currentFn, def);
				continue;
			}

			// Standard def: `$N = rhs :type`
			const defMatch = code.match(SSA_DEF_RE);
			if (defMatch) {
				const name = defMatch[1];
				const rhs = defMatch[2];
				const typeMatch = rhs.match(TYPE_TAIL_RE);
				const def: SsaDefinition = {
					name,
					line: i,
					character: text.indexOf(name),
					rhs,
					type: typeMatch?.[1],
					functionName: currentFn.name,
				};
				registerDef(ir, currentFn, def);
			}

			// Determine terminator.
			if (/^\s*return\b/.test(code)) {
				currentBlock.terminator = 'return';
			} else if (/^\s*if\b/.test(code)) {
				currentBlock.terminator = 'if';
			} else if (/^\s*br\b/.test(code)) {
				currentBlock.terminator = 'br';
			}

			// SSA references on the line (excluding the LHS of a def).
			const lhsEnd = defMatch ? text.indexOf('=') + 1 : 0;
			collectSsaReferences(ir, currentFn.name, text, Math.max(lhsEnd, 0), i);

			// Unwind edge: the pad this call branches to when it throws.
			const padLink = code.match(PAD_LINK_RE);
			if (padLink && !currentBlock.padLinks.includes(padLink[1])) {
				currentBlock.padLinks.push(padLink[1]);
			}

			// Successor block references (only collect for terminator-bearing lines).
			let m: RegExpExecArray | null;
			BLOCK_REF_RE.lastIndex = 0;
			while ((m = BLOCK_REF_RE.exec(code)) !== null) {
				const target = m[0];
				if (target !== currentBlock.name && !currentBlock.successors.includes(target)) {
					currentBlock.successors.push(target);
				}
			}

			currentBlock.bodyRange = new vscode.Range(
				currentBlock.headerLine,
				0,
				i,
				text.length,
			);
		}
	}

	finalizeBlock(currentBlock, lineCount - 1, document);
	finalizePad(currentPad, lineCount - 1, document);
	finalizeFunction(currentFn, lineCount - 1, document);

	// Compute predecessors per function (control flow does not cross function
	// boundaries — each function has its own Block_0).
	for (const fn of ir.functions) {
		for (const block of fn.blocks) {
			for (const succ of block.successors) {
				const target = fn.blockByName.get(succ);
				if (target && !target.predecessors.includes(block.name)) {
					target.predecessors.push(block.name);
				}
			}
		}
	}

	if (moduleStart >= 0) {
		ir.moduleRange = new vscode.Range(moduleStart, 0, lineCount - 1, 0);
	}

	return ir;
}

/**
 * Drops a trailing comment. `//` opens the module-end marker; `;` opens the
 * source-location trailer `dump.sourceLocations` appends to an operation (and
 * the `; inlined from ...` continuation lines under it, which strip to
 * nothing and are then skipped as blank).
 */
function stripComment(text: string): string {
	const slash = text.indexOf('//');
	const semi = text.indexOf(';');
	const cut = slash < 0 ? semi : semi < 0 ? slash : Math.min(slash, semi);
	return cut < 0 ? text : text.slice(0, cut);
}

/** `readnone, willreturn, nounwind` → the individual attribute names. */
function splitAttributes(raw: string | undefined): string[] {
	if (!raw) {
		return [];
	}
	return raw.split(',').map(a => a.trim()).filter(a => a !== '');
}

/** Indexes every `$N` on `text` at or after `scanStart` as a use. */
function collectSsaReferences(
	ir: ParsedIr,
	functionName: string,
	text: string,
	scanStart: number,
	line: number,
): void {
	const scanText = text.slice(scanStart);
	SSA_REF_RE.lastIndex = 0;
	let m: RegExpExecArray | null;
	while ((m = SSA_REF_RE.exec(scanText)) !== null) {
		const refName = m[0];
		const refs = ir.references.get(refName) ?? [];
		refs.push({ name: refName, line, character: scanStart + m.index, functionName });
		ir.references.set(refName, refs);
	}
}

function registerDef(ir: ParsedIr, fn: IrFunction, def: SsaDefinition): void {
	fn.definitions.set(def.name, def);
	// The module-level definitions map keys SSA names to the *first* observed
	// definition. This is good enough for navigation in single-function
	// modules; multi-function modules still get scoped lookups via the
	// per-function map.
	if (!ir.definitions.has(def.name)) {
		ir.definitions.set(def.name, def);
	}
}

function finalizeBlock(block: Block | undefined, endLine: number, document: vscode.TextDocument): void {
	if (!block) {
		return;
	}
	const lastLineIdx = Math.max(block.headerLine, endLine);
	const last = document.lineAt(Math.min(lastLineIdx, document.lineCount - 1));
	block.bodyRange = new vscode.Range(block.headerLine, 0, last.lineNumber, last.text.length);
}

function finalizePad(pad: LandingPad | undefined, endLine: number, document: vscode.TextDocument): void {
	if (!pad) {
		return;
	}
	const lastLineIdx = Math.max(pad.headerLine, endLine);
	const last = document.lineAt(Math.min(lastLineIdx, document.lineCount - 1));
	pad.bodyRange = new vscode.Range(pad.headerLine, 0, last.lineNumber, last.text.length);
}

function finalizeFunction(fn: IrFunction | undefined, endLine: number, document: vscode.TextDocument): void {
	if (!fn) {
		return;
	}
	const last = document.lineAt(Math.min(Math.max(fn.headerLine, endLine), document.lineCount - 1));
	fn.bodyRange = new vscode.Range(fn.headerLine, 0, last.lineNumber, last.text.length);
}

function rawArgToken(raw: string): BlockArgument {
	const parts = raw.split(':').map(s => s.trim());
	return {
		name: parts[0] ?? '',
		type: parts[1] ?? '',
	};
}

// Find the function that owns a given line.
export function functionAt(ir: ParsedIr, line: number): IrFunction | undefined {
	return ir.functions.find(fn => line >= fn.headerLine && line <= fn.bodyRange.end.line);
}

// Find the block that owns a given line.
export function blockAtLine(ir: ParsedIr, line: number): Block | undefined {
	return ir.blocks.find(b => line >= b.headerLine && line <= b.bodyRange.end.line);
}

// Extract the SSA name that the cursor is currently positioned on.
export function ssaAt(document: vscode.TextDocument, position: vscode.Position): { name: string; range: vscode.Range } | undefined {
	const line = document.lineAt(position.line).text;
	const re = /\$\d+/g;
	let m: RegExpExecArray | null;
	while ((m = re.exec(line)) !== null) {
		const start = m.index;
		const end = start + m[0].length;
		if (position.character >= start && position.character <= end) {
			return {
				name: m[0],
				range: new vscode.Range(position.line, start, position.line, end),
			};
		}
	}
	return undefined;
}

// Extract the block name (Block_N) the cursor is on.
export function blockAt(document: vscode.TextDocument, position: vscode.Position): { name: string; range: vscode.Range } | undefined {
	const line = document.lineAt(position.line).text;
	const re = /\bBlock_\d+\b/g;
	let m: RegExpExecArray | null;
	while ((m = re.exec(line)) !== null) {
		const start = m.index;
		const end = start + m[0].length;
		if (position.character >= start && position.character <= end) {
			return {
				name: m[0],
				range: new vscode.Range(position.line, start, position.line, end),
			};
		}
	}
	return undefined;
}
