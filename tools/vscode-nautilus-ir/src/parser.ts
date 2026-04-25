// Lightweight, regex-based parser for Nautilus IR (the dialect dumped into
// `nautilus/test/data/*/ir/*.trace`).
//
// Grammar reference (informally):
//
//   module      ::= 'NautilusIr' '{' function* '}' '//NESIR'?
//   function    ::= IDENT '(' ')' '{' block+ '}'
//   block       ::= 'Block_' INT '(' arg-list? ')' ':' statement*
//   arg-list    ::= typed-ssa (',' typed-ssa)*
//   typed-ssa   ::= '$' INT ':' TYPE
//   statement   ::= def | bare-decl | branch | conditional | return | store | void-call
//   def         ::= '$' INT '=' rhs ':' TYPE
//   bare-decl   ::= '$' INT ':' TYPE
//   rhs         ::= constant | unary | binary | cast | load | call
//   unary       ::= ('!' | '~') '$' INT
//   binary      ::= '$' INT BIN-OP ('$' INT | INT)
//   cast        ::= '$' INT 'cast_to' TYPE
//   load        ::= 'load' '(' '$' INT ')'
//   call        ::= 'func_*' '(' arg* ')'
//   branch      ::= 'br' BLOCK '(' ssa-list? ')' ':' 'void'
//   conditional ::= 'if' '$' INT '?' BLOCK '(' ssa-list? ')' ':' BLOCK '(' ssa-list? ')' ':' 'void'
//   return      ::= 'return' ('(' '$' INT ')')? ':' TYPE
//   store       ::= 'store' '(' '$' INT ',' '$' INT ')' ':' 'void'
//   void-call   ::= 'func_*' '(' arg* ')' ':' 'void'
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
	instructions: number;
	terminator: Terminator;
}

export interface IrFunction {
	name: string;
	headerLine: number;
	headerRange: vscode.Range;
	bodyRange: vscode.Range;
	blocks: Block[];
	blockByName: Map<string, Block>;
	definitions: Map<string, SsaDefinition>;
}

export interface ParsedIr {
	moduleRange: vscode.Range | undefined;
	functions: IrFunction[];
	functionByName: Map<string, IrFunction>;
	// Convenience: aggregate views across the whole module.
	blocks: Block[];
	blockByName: Map<string, Block>;
	definitions: Map<string, SsaDefinition>;
	references: Map<string, SsaReference[]>;
}

const FUNCTION_HEADER_RE = /^\s*([A-Za-z_][A-Za-z0-9_]*)\s*\(\s*\)\s*\{\s*$/;
const BLOCK_HEADER_RE = /^\s*(Block_\d+)\s*\(([^)]*)\)\s*:\s*$/;
const SSA_DEF_RE = /^\s*(\$\d+)\s*=\s*(.+?)\s*$/;
const BARE_DECL_RE = /^\s*(\$\d+)\s*:\s*([a-z0-9]+)\s*$/;
const SSA_REF_RE = /\$\d+/g;
const BLOCK_REF_RE = /\bBlock_\d+\b/g;
const TYPE_TAIL_RE = /:\s*(i8|i16|i32|i64|ui8|ui16|ui32|ui64|f32|f64|bool|ptr|void)\s*$/;
const PRIMITIVE_TYPES = new Set([
	'i8', 'i16', 'i32', 'i64',
	'ui8', 'ui16', 'ui32', 'ui64',
	'f32', 'f64', 'bool', 'ptr', 'void',
]);

const MODULE_OPEN_RE = /^\s*NautilusIr\s*\{\s*$/;

export function parse(document: vscode.TextDocument): ParsedIr {
	const ir: ParsedIr = {
		moduleRange: undefined,
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

	for (let i = 0; i < lineCount; i++) {
		const text = document.lineAt(i).text;
		const commentIdx = text.indexOf('//');
		const code = commentIdx >= 0 ? text.slice(0, commentIdx) : text;
		const trimmed = code.trim();

		if (MODULE_OPEN_RE.test(text)) {
			moduleStart = i;
			continue;
		}

		// Function header. Distinguish from block headers (Block_N) and
		// from the literal `NautilusIr {` line by the regexes above.
		const fnMatch = code.match(FUNCTION_HEADER_RE);
		if (fnMatch && fnMatch[1] !== 'NautilusIr') {
			finalizeBlock(currentBlock, i - 1, document);
			finalizeFunction(currentFn, i - 1, document);
			currentFn = {
				name: fnMatch[1],
				headerLine: i,
				headerRange: new vscode.Range(i, 0, i, text.length),
				bodyRange: new vscode.Range(i, 0, i, text.length),
				blocks: [],
				blockByName: new Map(),
				definitions: new Map(),
			};
			ir.functions.push(currentFn);
			ir.functionByName.set(currentFn.name, currentFn);
			currentBlock = undefined;
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
			finalizeFunction(currentFn, i - 1, document);
			currentFn = undefined;
			continue;
		}

		// Module-end brace + comment ("} //NESIR") was already split above; ignore.
		if (trimmed === '' || trimmed === '{') {
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
			const scanStart = Math.max(lhsEnd, 0);
			const scanText = text.slice(scanStart);
			let m: RegExpExecArray | null;
			SSA_REF_RE.lastIndex = 0;
			while ((m = SSA_REF_RE.exec(scanText)) !== null) {
				const refName = m[0];
				const refChar = scanStart + m.index;
				const refs = ir.references.get(refName) ?? [];
				refs.push({ name: refName, line: i, character: refChar, functionName: currentFn.name });
				ir.references.set(refName, refs);
			}

			// Successor block references (only collect for terminator-bearing lines).
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
