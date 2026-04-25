// Synthesize a Mermaid `flowchart TD` from a parsed Nautilus IR function.
//
// The output is consumed by the extension's graph webview. Each block becomes
// one node; edges are derived from block terminators (`return` / `br` / `if`).
// Block names are reused verbatim as Mermaid node ids — the parser already
// guarantees they match `/Block_\d+/`, which is a valid Mermaid identifier.
//
// The webview renders Mermaid with `securityLevel: 'strict'` and
// `htmlLabels: false`, so labels are SVG <text> and DOMPurify-sanitised.
// Click handlers are wired up by the webview after rendering rather than via
// a Mermaid `click ... call ...` directive (which would require `'loose'`).

import { IrFunction, ParsedIr } from './parser';

export interface CfgGraph {
	mermaid: string;
	functionName: string;
	// Block names rendered in this graph, in declaration order. The webview
	// uses this list (validated against `/^Block_\d+$/`) to attach click
	// handlers to the rendered SVG nodes.
	blocks: string[];
}

export function cfgFor(ir: ParsedIr, functionName: string): CfgGraph | undefined {
	const fn = ir.functionByName.get(functionName);
	if (!fn) {
		return undefined;
	}
	return buildGraph(fn);
}

function buildGraph(fn: IrFunction): CfgGraph {
	const lines: string[] = [];
	lines.push('flowchart TD');

	if (fn.blocks.length === 0) {
		lines.push(`    empty["${quoteLabel(fn.name)}: (no blocks)"]`);
		return { mermaid: lines.join('\n'), functionName: fn.name, blocks: [] };
	}

	const entryName = fn.blocks[0].name;

	// Nodes. Shape conveys the terminator: rectangle for normal, stadium
	// for return, rhombus for conditional `if`.
	for (const block of fn.blocks) {
		const id = block.name;
		const label = renderLabel(block.name, block.args, block.instructions, block.terminator);
		let shape: string;
		if (block.terminator === 'return') {
			shape = `${id}(["${label}"])`;
		} else if (block.terminator === 'if') {
			shape = `${id}{{"${label}"}}`;
		} else {
			shape = `${id}["${label}"]`;
		}
		lines.push(`    ${shape}`);
	}

	// Edges. The parser preserves successor order on conditional branches
	// (true branch first, then false), matching the textual
	// `if $cond ? Block_T : Block_F :void` form.
	for (const block of fn.blocks) {
		if (block.terminator === 'if' && block.successors.length === 2) {
			const [t, f] = block.successors;
			lines.push(`    ${block.name} -- "true" --> ${t}`);
			lines.push(`    ${block.name} -- "false" --> ${f}`);
		} else {
			for (const succ of block.successors) {
				lines.push(`    ${block.name} --> ${succ}`);
			}
		}
	}

	// Styling. Colors mirror the palette of the C++ Mermaid dumper in
	// `nautilus/src/nautilus/compiler/ir/util/GraphVizUtil.cpp` so the
	// extension's view feels familiar to anyone reading those dumps.
	lines.push('    classDef entry stroke:#3cb4a4,stroke-width:2px,fill:#d7ede7,color:#000');
	lines.push('    classDef exit stroke:#e98693,stroke-width:2px,fill:#fde8ec,color:#000');
	lines.push('    classDef cond stroke:#3cb4a4,stroke-width:1px,fill:#fff,color:#000');
	lines.push('    classDef cursor stroke:#da2d4f,stroke-width:3px,fill:#fff7f9,color:#000');

	lines.push(`    class ${entryName} entry`);
	for (const block of fn.blocks) {
		if (block.terminator === 'return') {
			lines.push(`    class ${block.name} exit`);
		} else if (block.terminator === 'if') {
			lines.push(`    class ${block.name} cond`);
		}
	}

	return {
		mermaid: lines.join('\n'),
		functionName: fn.name,
		blocks: fn.blocks.map(b => b.name),
	};
}

function renderLabel(
	name: string,
	args: { name: string; type: string }[],
	instructions: number,
	terminator: string,
): string {
	const sig = args.length === 0
		? name
		: `${name}(${args.map(a => `${a.name}:${a.type}`).join(', ')})`;
	const insnWord = instructions === 1 ? 'insn' : 'insns';
	// Single-line plain-text label. Mermaid (strict mode, no htmlLabels)
	// escapes any markup automatically; we only need to neutralise the
	// closing-quote character so the label string stays well-formed.
	return quoteLabel(`${sig} | ${instructions} ${insnWord} · ${terminator}`);
}

function quoteLabel(s: string): string {
	return s.replace(/"/g, '&quot;');
}
