// Language features wired up against the lightweight Nautilus IR parser.
//
// All providers cache the most recent parse per document version to avoid
// re-parsing on every cursor movement.

import * as vscode from 'vscode';
import { parse, ParsedIr, ssaAt, blockAt, Block } from './parser';

const cache = new WeakMap<vscode.TextDocument, { version: number; ir: ParsedIr }>();

export function irFor(document: vscode.TextDocument): ParsedIr {
	const cached = cache.get(document);
	if (cached && cached.version === document.version) {
		return cached.ir;
	}
	const ir = parse(document);
	cache.set(document, { version: document.version, ir });
	return ir;
}

// ----- DocumentSymbolProvider: blocks become symbols, defs become children -----

export class IrSymbolProvider implements vscode.DocumentSymbolProvider {
	provideDocumentSymbols(document: vscode.TextDocument): vscode.DocumentSymbol[] {
		const ir = irFor(document);
		const symbols: vscode.DocumentSymbol[] = [];
		for (const block of ir.blocks) {
			const argSummary = block.args.map(a => `${a.name}:${a.type}`).join(', ');
			const blockSym = new vscode.DocumentSymbol(
				block.name,
				`(${argSummary})  → ${block.successors.length ? block.successors.join(', ') : block.terminator}`,
				vscode.SymbolKind.Function,
				block.bodyRange,
				block.headerRange,
			);
			for (const [name, def] of ir.definitions) {
				if (def.line >= block.headerLine && def.line <= block.bodyRange.end.line) {
					const defRange = new vscode.Range(def.line, def.character, def.line, def.character + name.length);
					blockSym.children.push(new vscode.DocumentSymbol(
						name,
						def.type ? `:${def.type}` : '',
						vscode.SymbolKind.Variable,
						defRange,
						defRange,
					));
				}
			}
			symbols.push(blockSym);
		}
		return symbols;
	}
}

// ----- DefinitionProvider: jump to SSA def or block header -----

export class IrDefinitionProvider implements vscode.DefinitionProvider {
	provideDefinition(document: vscode.TextDocument, position: vscode.Position): vscode.Definition | undefined {
		const ir = irFor(document);
		const ssa = ssaAt(document, position);
		if (ssa) {
			const def = ir.definitions.get(ssa.name);
			if (def) {
				const range = new vscode.Range(def.line, def.character, def.line, def.character + ssa.name.length);
				return new vscode.Location(document.uri, range);
			}
			// Could be a block argument; jump to the block header it appears on.
			for (const block of ir.blocks) {
				if (block.args.some(a => a.name === ssa.name)) {
					return new vscode.Location(document.uri, block.headerRange);
				}
			}
		}
		const block = blockAt(document, position);
		if (block) {
			const target = ir.blockByName.get(block.name);
			if (target) {
				return new vscode.Location(document.uri, target.headerRange);
			}
		}
		return undefined;
	}
}

// ----- ReferenceProvider: find every use of an SSA value or block -----

export class IrReferenceProvider implements vscode.ReferenceProvider {
	provideReferences(document: vscode.TextDocument, position: vscode.Position, _ctx: vscode.ReferenceContext): vscode.Location[] {
		const ir = irFor(document);
		const ssa = ssaAt(document, position);
		if (ssa) {
			const refs = ir.references.get(ssa.name) ?? [];
			const locations = refs.map(r => new vscode.Location(
				document.uri,
				new vscode.Range(r.line, r.character, r.line, r.character + ssa.name.length),
			));
			const def = ir.definitions.get(ssa.name);
			if (def) {
				locations.unshift(new vscode.Location(
					document.uri,
					new vscode.Range(def.line, def.character, def.line, def.character + ssa.name.length),
				));
			}
			return locations;
		}
		const block = blockAt(document, position);
		if (block) {
			const target = ir.blockByName.get(block.name);
			const out: vscode.Location[] = target ? [new vscode.Location(document.uri, target.headerRange)] : [];
			// Scan every line for textual references to that block name.
			const re = new RegExp(`\\b${block.name}\\b`, 'g');
			for (let i = 0; i < document.lineCount; i++) {
				const text = document.lineAt(i).text;
				let m: RegExpExecArray | null;
				while ((m = re.exec(text)) !== null) {
					if (target && i === target.headerLine) {
						continue;
					}
					out.push(new vscode.Location(
						document.uri,
						new vscode.Range(i, m.index, i, m.index + block.name.length),
					));
				}
			}
			return out;
		}
		return [];
	}
}

// ----- HoverProvider: explain SSA values, blocks, types -----

export class IrHoverProvider implements vscode.HoverProvider {
	provideHover(document: vscode.TextDocument, position: vscode.Position): vscode.Hover | undefined {
		const ir = irFor(document);
		const ssa = ssaAt(document, position);
		if (ssa) {
			return new vscode.Hover(this.ssaHover(ir, ssa.name), ssa.range);
		}
		const block = blockAt(document, position);
		if (block) {
			const target = ir.blockByName.get(block.name);
			if (target) {
				return new vscode.Hover(this.blockHover(target), block.range);
			}
		}
		const wordRange = document.getWordRangeAtPosition(position, /\b(i8|i16|i32|i64|ui8|ui16|ui32|ui64|f32|f64|bool|ptr|void)\b/);
		if (wordRange) {
			const word = document.getText(wordRange);
			return new vscode.Hover(typeDoc(word), wordRange);
		}
		const opRange = document.getWordRangeAtPosition(position, /\b(cast_to|load|store|return|br|if|and|or|xor|not|func_\*|alloca)\b/);
		if (opRange) {
			const op = document.getText(opRange);
			return new vscode.Hover(opDoc(op), opRange);
		}
		return undefined;
	}

	private ssaHover(ir: ParsedIr, name: string): vscode.MarkdownString {
		const md = new vscode.MarkdownString();
		md.isTrusted = false;
		md.supportHtml = false;
		const def = ir.definitions.get(name);
		if (def) {
			md.appendMarkdown(`**${name}** *(SSA value)*\n\n`);
			md.appendCodeblock(def.rhs, 'nautilus-ir');
			md.appendMarkdown(`\nDefined on line **${def.line + 1}**`);
			if (def.type) {
				md.appendMarkdown(` with type \`${def.type}\``);
			}
			const refs = ir.references.get(name) ?? [];
			md.appendMarkdown(`\n\n${refs.length} use(s).`);
			return md;
		}
		// Block argument lookup.
		for (const block of ir.blocks) {
			const arg = block.args.find(a => a.name === name);
			if (arg) {
				md.appendMarkdown(`**${name}** *(block argument of \`${block.name}\`)*\n\n`);
				md.appendMarkdown(`Type: \`${arg.type}\``);
				return md;
			}
		}
		md.appendMarkdown(`**${name}** — unknown SSA value`);
		return md;
	}

	private blockHover(block: Block): vscode.MarkdownString {
		const md = new vscode.MarkdownString();
		md.appendMarkdown(`**${block.name}** *(basic block)*\n\n`);
		const argSummary = block.args.length === 0
			? '*(no arguments)*'
			: block.args.map(a => `\`${a.name}: ${a.type}\``).join(', ');
		md.appendMarkdown(`Arguments: ${argSummary}\n\n`);
		md.appendMarkdown(`Instructions: ${block.instructions}\n\n`);
		md.appendMarkdown(`Terminator: \`${block.terminator}\`\n\n`);
		md.appendMarkdown(`Predecessors: ${formatList(block.predecessors)}\n\n`);
		md.appendMarkdown(`Successors: ${formatList(block.successors)}`);
		return md;
	}
}

function formatList(items: string[]): string {
	if (items.length === 0) {
		return '*(none)*';
	}
	return items.map(i => `\`${i}\``).join(', ');
}

function typeDoc(t: string): vscode.MarkdownString {
	const md = new vscode.MarkdownString();
	md.appendMarkdown(`**${t}** — Nautilus IR primitive type\n\n`);
	const docs: Record<string, string> = {
		i8: 'Signed 8-bit integer.',
		i16: 'Signed 16-bit integer.',
		i32: 'Signed 32-bit integer.',
		i64: 'Signed 64-bit integer.',
		ui8: 'Unsigned 8-bit integer.',
		ui16: 'Unsigned 16-bit integer.',
		ui32: 'Unsigned 32-bit integer.',
		ui64: 'Unsigned 64-bit integer.',
		f32: 'Single-precision (32-bit) IEEE-754 floating point.',
		f64: 'Double-precision (64-bit) IEEE-754 floating point.',
		bool: 'Boolean value (true/false).',
		ptr: 'Opaque pointer to an arbitrary memory address.',
		void: 'No value (used for store/return).',
	};
	md.appendMarkdown(docs[t] ?? '');
	return md;
}

function opDoc(op: string): vscode.MarkdownString {
	const md = new vscode.MarkdownString();
	const docs: Record<string, string> = {
		cast_to: 'Type conversion. `$x cast_to TYPE :TYPE` reinterprets `$x` as `TYPE`.',
		load: 'Read a value from a pointer: `$y = load($ptr) :TYPE`.',
		store: 'Write a value to a pointer: `store($value, $ptr) :void`.',
		return: 'Terminator that exits `execute()`. May return zero or one value.',
		br: 'Unconditional branch: `br Block_N($args) :void`.',
		if: 'Conditional branch: `if $cond ? Block_T($args) : Block_F($args) :void`.',
		and: 'Logical/bitwise conjunction.',
		or: 'Logical/bitwise disjunction.',
		xor: 'Logical/bitwise exclusive-or.',
		not: 'Logical negation.',
		'func_*': 'Indirect call through a function pointer resolved by the JIT runtime.',
		alloca: 'Allocate a stack slot of the given type.',
	};
	md.appendMarkdown(`**${op}** — Nautilus IR operation\n\n${docs[op] ?? ''}`);
	return md;
}

// ----- DocumentHighlightProvider: highlight all uses of the SSA under cursor -----

export class IrHighlightProvider implements vscode.DocumentHighlightProvider {
	provideDocumentHighlights(document: vscode.TextDocument, position: vscode.Position): vscode.DocumentHighlight[] {
		const ir = irFor(document);
		const ssa = ssaAt(document, position);
		if (ssa) {
			const out: vscode.DocumentHighlight[] = [];
			const def = ir.definitions.get(ssa.name);
			if (def) {
				out.push(new vscode.DocumentHighlight(
					new vscode.Range(def.line, def.character, def.line, def.character + ssa.name.length),
					vscode.DocumentHighlightKind.Write,
				));
			}
			const refs = ir.references.get(ssa.name) ?? [];
			for (const r of refs) {
				out.push(new vscode.DocumentHighlight(
					new vscode.Range(r.line, r.character, r.line, r.character + ssa.name.length),
					vscode.DocumentHighlightKind.Read,
				));
			}
			return out;
		}
		const block = blockAt(document, position);
		if (block) {
			const re = new RegExp(`\\b${block.name}\\b`, 'g');
			const out: vscode.DocumentHighlight[] = [];
			for (let i = 0; i < document.lineCount; i++) {
				const text = document.lineAt(i).text;
				let m: RegExpExecArray | null;
				while ((m = re.exec(text)) !== null) {
					out.push(new vscode.DocumentHighlight(
						new vscode.Range(i, m.index, i, m.index + block.name.length),
						vscode.DocumentHighlightKind.Read,
					));
				}
			}
			return out;
		}
		return [];
	}
}

// ----- FoldingRangeProvider: fold blocks and the wrapper module -----

export class IrFoldingProvider implements vscode.FoldingRangeProvider {
	provideFoldingRanges(document: vscode.TextDocument): vscode.FoldingRange[] {
		const ir = irFor(document);
		const ranges: vscode.FoldingRange[] = [];
		if (ir.moduleRange) {
			ranges.push(new vscode.FoldingRange(ir.moduleRange.start.line, ir.moduleRange.end.line, vscode.FoldingRangeKind.Region));
		}
		if (ir.executeRange) {
			ranges.push(new vscode.FoldingRange(ir.executeRange.start.line, ir.executeRange.end.line, vscode.FoldingRangeKind.Region));
		}
		for (const block of ir.blocks) {
			if (block.bodyRange.end.line > block.headerLine) {
				ranges.push(new vscode.FoldingRange(block.headerLine, block.bodyRange.end.line, vscode.FoldingRangeKind.Region));
			}
		}
		return ranges;
	}
}

// ----- RenameProvider: rename SSA values consistently in the file -----

export class IrRenameProvider implements vscode.RenameProvider {
	prepareRename(document: vscode.TextDocument, position: vscode.Position): vscode.Range {
		const ssa = ssaAt(document, position);
		if (!ssa) {
			throw new Error('Only $N SSA values can be renamed.');
		}
		return ssa.range;
	}

	provideRenameEdits(document: vscode.TextDocument, position: vscode.Position, newName: string): vscode.WorkspaceEdit {
		if (!/^\$\d+$/.test(newName)) {
			throw new Error('New name must match pattern $N (e.g. $42).');
		}
		const ir = irFor(document);
		const ssa = ssaAt(document, position);
		if (!ssa) {
			throw new Error('Cursor is not on an SSA value.');
		}
		const edit = new vscode.WorkspaceEdit();
		const def = ir.definitions.get(ssa.name);
		if (def) {
			edit.replace(document.uri, new vscode.Range(def.line, def.character, def.line, def.character + ssa.name.length), newName);
		}
		const refs = ir.references.get(ssa.name) ?? [];
		for (const r of refs) {
			edit.replace(document.uri, new vscode.Range(r.line, r.character, r.line, r.character + ssa.name.length), newName);
		}
		// Also rewrite occurrences in block headers (block arguments).
		for (const block of ir.blocks) {
			const headerText = document.lineAt(block.headerLine).text;
			const re = new RegExp(`\\${ssa.name}\\b`, 'g');
			let m: RegExpExecArray | null;
			while ((m = re.exec(headerText)) !== null) {
				edit.replace(document.uri, new vscode.Range(block.headerLine, m.index, block.headerLine, m.index + ssa.name.length), newName);
			}
		}
		return edit;
	}
}

// ----- CodeLensProvider: show predecessor/successor counts above each block -----

export class IrCodeLensProvider implements vscode.CodeLensProvider {
	provideCodeLenses(document: vscode.TextDocument): vscode.CodeLens[] {
		const config = vscode.workspace.getConfiguration('nautilusIr');
		if (!config.get<boolean>('codeLens', true)) {
			return [];
		}
		const ir = irFor(document);
		const lenses: vscode.CodeLens[] = [];
		for (const block of ir.blocks) {
			const title = `${block.predecessors.length} preds · ${block.successors.length} succs · ${block.instructions} insn`;
			lenses.push(new vscode.CodeLens(block.headerRange, {
				title,
				command: 'nautilus-ir.showBlockInfo',
				arguments: [document.uri, block.name],
			}));
		}
		return lenses;
	}
}
