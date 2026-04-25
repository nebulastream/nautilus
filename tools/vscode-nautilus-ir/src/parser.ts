// Lightweight, regex-based parser for Nautilus IR.
//
// Nautilus IR is a small, line-oriented format. We don't build a full AST
// here, just enough structure (blocks, SSA defs, references, branches) to
// power navigation, hover, and folding features.

import * as vscode from 'vscode';

export interface SsaDefinition {
	name: string;            // e.g. "$3"
	line: number;            // zero-based line index of the definition
	character: number;       // column of the leading $
	rhs: string;             // raw right-hand side text (after `=`)
	type: string | undefined;// trailing :type if present
}

export interface SsaReference {
	name: string;
	line: number;
	character: number;
}

export interface BlockArgument {
	name: string;
	type: string;
}

export interface Block {
	name: string;             // e.g. "Block_5"
	headerLine: number;       // zero-based
	headerRange: vscode.Range;
	bodyRange: vscode.Range;  // header line through last instruction
	args: BlockArgument[];
	successors: string[];     // referenced block names from terminator(s)
	predecessors: string[];   // populated in a second pass
	instructions: number;     // count of non-blank, non-header lines
	terminator: 'return' | 'br' | 'if' | 'unknown';
}

export interface ParsedIr {
	blocks: Block[];
	blockByName: Map<string, Block>;
	definitions: Map<string, SsaDefinition>;       // by name
	references: Map<string, SsaReference[]>;       // by name
	moduleRange: vscode.Range | undefined;         // NautilusIr {...}
	executeRange: vscode.Range | undefined;        // execute() {...}
}

const BLOCK_HEADER_RE = /^\s*(Block_\d+|B\d+)\s*\(([^)]*)\)\s*:\s*$/;
const SSA_DEF_RE = /^\s*(\$\d+)\s*=\s*(.+?)\s*$/;
const SSA_REF_RE = /\$\d+/g;
const BLOCK_REF_RE = /\b(Block_\d+|B\d+)\b/g;
const TYPE_TAIL_RE = /:\s*(i8|i16|i32|i64|ui8|ui16|ui32|ui64|f32|f64|bool|ptr|void)\s*$/;

export function parse(document: vscode.TextDocument): ParsedIr {
	const result: ParsedIr = {
		blocks: [],
		blockByName: new Map(),
		definitions: new Map(),
		references: new Map(),
		moduleRange: undefined,
		executeRange: undefined,
	};

	const lineCount = document.lineCount;
	let currentBlock: Block | undefined;

	let moduleStart = -1;
	let executeStart = -1;

	for (let i = 0; i < lineCount; i++) {
		const text = document.lineAt(i).text;
		const trimmed = text.trim();

		// Strip line comments before further analysis.
		const commentIdx = text.indexOf('//');
		const code = commentIdx >= 0 ? text.slice(0, commentIdx) : text;

		if (trimmed.startsWith('NautilusIr')) {
			moduleStart = i;
			continue;
		}
		if (/^execute\s*\(\s*\)\s*\{/.test(trimmed)) {
			executeStart = i;
			continue;
		}

		// Block header.
		const headerMatch = code.match(BLOCK_HEADER_RE);
		if (headerMatch) {
			finalizeBlock(currentBlock, i - 1, document);
			const name = headerMatch[1];
			const argList = headerMatch[2].trim();
			const args: BlockArgument[] = argList === ''
				? []
				: argList.split(',').map(rawArgToken);
			currentBlock = {
				name,
				headerLine: i,
				headerRange: new vscode.Range(i, 0, i, text.length),
				bodyRange: new vscode.Range(i, 0, i, text.length),
				args,
				successors: [],
				predecessors: [],
				instructions: 0,
				terminator: 'unknown',
			};
			result.blocks.push(currentBlock);
			result.blockByName.set(name, currentBlock);
			continue;
		}

		// Skip blank and brace-only lines.
		if (trimmed === '' || trimmed === '{' || trimmed === '}' || trimmed === '} //NESIR') {
			continue;
		}

		// Inside a block: capture defs and references.
		if (currentBlock) {
			currentBlock.instructions++;

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
				};
				result.definitions.set(name, def);
			}

			// Determine terminator.
			if (/^\s*return\b/.test(code)) {
				currentBlock.terminator = 'return';
			} else if (/^\s*if\b/.test(code) || /^\s*CMP\b/.test(code)) {
				currentBlock.terminator = 'if';
			} else if (/^\s*br\b/.test(code) || /^\s*JMP\b/.test(code)) {
				currentBlock.terminator = 'br';
			}

			// Collect SSA references on the line (excluding the LHS def).
			const lhsEnd = defMatch ? text.indexOf('=') + 1 : 0;
			const scanStart = Math.max(lhsEnd, 0);
			const scanText = text.slice(scanStart);
			let m: RegExpExecArray | null;
			SSA_REF_RE.lastIndex = 0;
			while ((m = SSA_REF_RE.exec(scanText)) !== null) {
				const refName = m[0];
				const refChar = scanStart + m.index;
				const refs = result.references.get(refName) ?? [];
				refs.push({ name: refName, line: i, character: refChar });
				result.references.set(refName, refs);
			}

			// Collect successor block references.
			BLOCK_REF_RE.lastIndex = 0;
			while ((m = BLOCK_REF_RE.exec(code)) !== null) {
				const target = m[1];
				if (target !== currentBlock.name && !currentBlock.successors.includes(target)) {
					currentBlock.successors.push(target);
				}
			}

			// Extend the body range.
			currentBlock.bodyRange = new vscode.Range(
				currentBlock.headerLine,
				0,
				i,
				text.length,
			);
		}
	}

	finalizeBlock(currentBlock, lineCount - 1, document);

	// Compute predecessors.
	for (const block of result.blocks) {
		for (const succ of block.successors) {
			const target = result.blockByName.get(succ);
			if (target && !target.predecessors.includes(block.name)) {
				target.predecessors.push(block.name);
			}
		}
	}

	if (moduleStart >= 0) {
		result.moduleRange = new vscode.Range(moduleStart, 0, lineCount - 1, 0);
	}
	if (executeStart >= 0) {
		result.executeRange = new vscode.Range(executeStart, 0, lineCount - 1, 0);
	}

	return result;
}

function finalizeBlock(block: Block | undefined, endLine: number, document: vscode.TextDocument): void {
	if (!block) {
		return;
	}
	const lastLineIdx = Math.max(block.headerLine, endLine);
	const lastLine = document.lineAt(Math.min(lastLineIdx, document.lineCount - 1));
	block.bodyRange = new vscode.Range(block.headerLine, 0, lastLine.lineNumber, lastLine.text.length);
}

function rawArgToken(raw: string): BlockArgument {
	const parts = raw.split(':').map(s => s.trim());
	return {
		name: parts[0] ?? '',
		type: parts[1] ?? '',
	};
}

// Extract the SSA name that the cursor is currently positioned on, if any.
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

// Extract the block name (Block_N or BN) the cursor is on, if any.
export function blockAt(document: vscode.TextDocument, position: vscode.Position): { name: string; range: vscode.Range } | undefined {
	const line = document.lineAt(position.line).text;
	const re = /\b(Block_\d+|B\d+)\b/g;
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
