// Monaco setup: bundles the editor worker locally (no CDN — the playground
// runs behind a strict CSP) and registers Monarch grammars for the Nautilus
// pipeline dialects. The nautilus-ir grammar mirrors the TextMate grammar in
// tools/vscode-nautilus-ir/syntaxes/nautilus-ir.tmLanguage.json.

import * as monaco from 'monaco-editor';
import EditorWorker from 'monaco-editor/esm/vs/editor/editor.worker?worker';

self.MonacoEnvironment = {
	getWorker: () => new EditorWorker(),
};

const IR_TYPES = ['i8', 'i16', 'i32', 'i64', 'ui8', 'ui16', 'ui32', 'ui64', 'f32', 'f64', 'bool', 'ptr', 'void'];

let registered = false;

export function registerLanguages(): void {
	if (registered) {
		return;
	}
	registered = true;

	// --- Nautilus IR (.nautilus / .ir dumps) ---
	monaco.languages.register({ id: 'nautilus-ir' });
	monaco.languages.setMonarchTokensProvider('nautilus-ir', {
		defaultToken: '',
		tokenizer: {
			root: [
				[/\/\/.*$/, 'comment'],
				[/\b(nautilus|br|if|return|store|load|cast_to)\b/, 'keyword'],
				[new RegExp(`\\b(${IR_TYPES.join('|')})\\b`), 'type'],
				[/\bBlock_\d+\b/, 'type.identifier'],
				[/\$\d+/, 'variable.name'],
				[/\bfunc_[A-Za-z0-9_]*\b/, 'entity.name.function'],
				[/-?\d+\.\d+/, 'number.float'],
				[/-?\d+/, 'number'],
				[/[=?:(){},]/, 'delimiter'],
				[/[A-Za-z_][A-Za-z0-9_]*(?=\s*\()/, 'entity.name.function'],
			],
		},
	});

	// --- Execution trace (.trace dumps) ---
	monaco.languages.register({ id: 'nautilus-trace' });
	monaco.languages.setMonarchTokensProvider('nautilus-trace', {
		defaultToken: '',
		tokenizer: {
			root: [
				[/\/\/.*$/, 'comment'],
				[/^\s*B\d+\b/, 'type.identifier'],
				[/\bBlock_?\d+\b/, 'type.identifier'],
				[
					/\b(JMP|CMP|RETURN|ASSIGN|CONST|ADD|SUB|MUL|DIV|MOD|EQ|NEQ|LT|LTE|GT|GTE|AND|OR|NOT|NEGATE|LOAD|STORE|CALL|CAST)\b/,
					'keyword',
				],
				[new RegExp(`\\b(${IR_TYPES.join('|')})\\b`), 'type'],
				[/\$\d+|T\d+/, 'variable.name'],
				[/-?\d+\.\d+/, 'number.float'],
				[/-?\d+/, 'number'],
			],
		},
	});

	// --- MLIR ---
	monaco.languages.register({ id: 'mlir' });
	monaco.languages.setMonarchTokensProvider('mlir', {
		defaultToken: '',
		tokenizer: {
			root: [
				[/\/\/.*$/, 'comment'],
				[/%[A-Za-z0-9_#.]+/, 'variable.name'],
				[/\^[A-Za-z0-9_]+/, 'type.identifier'],
				[/![A-Za-z0-9_.]+/, 'type'],
				[/@[A-Za-z0-9_]+/, 'entity.name.function'],
				[/\b(func|module|return|arith|llvm|cf|memref|scf|index|loc)\b/, 'keyword'],
				[/\b(i\d+|f16|f32|f64|index)\b/, 'type'],
				[/"[^"]*"/, 'string'],
				[/-?\d+\.\d+/, 'number.float'],
				[/-?\d+/, 'number'],
			],
		},
	});

	// --- LLVM IR ---
	monaco.languages.register({ id: 'llvm-ir' });
	monaco.languages.setMonarchTokensProvider('llvm-ir', {
		defaultToken: '',
		tokenizer: {
			root: [
				[/;.*$/, 'comment'],
				[/%[A-Za-z0-9_.]+/, 'variable.name'],
				[/@[A-Za-z0-9_.]+/, 'entity.name.function'],
				[
					/\b(define|declare|ret|br|icmp|fcmp|add|sub|mul|sdiv|udiv|srem|urem|and|or|xor|shl|lshr|ashr|load|store|getelementptr|call|phi|select|alloca|zext|sext|trunc|bitcast|inttoptr|ptrtoint|label|to|align|nuw|nsw)\b/,
					'keyword',
				],
				[/\b(i\d+|half|float|double|ptr|void)\b/, 'type'],
				[/"[^"]*"/, 'string'],
				[/-?\d+\.\d+(e[+-]?\d+)?/, 'number.float'],
				[/-?\d+/, 'number'],
			],
		},
	});

	// --- x86/ARM assembly listings (asmjit backend) ---
	monaco.languages.register({ id: 'asm' });
	monaco.languages.setMonarchTokensProvider('asm', {
		defaultToken: '',
		tokenizer: {
			root: [
				[/[;#].*$/, 'comment'],
				[/^\s*[A-Za-z_.][A-Za-z0-9_.]*:/, 'type.identifier'],
				[/^\s*[a-z][a-z0-9.]*\b/, 'keyword'],
				[/%?\b(r[a-z0-9]+|e[a-z]{2}|[abcd][lhx]|x\d+|w\d+|sp|lr|pc)\b/, 'variable.name'],
				[/\$?-?0x[0-9a-fA-F]+|\$?-?\d+/, 'number'],
			],
		},
	});

	// --- Graphviz dot (engine's dump.graph output) ---
	monaco.languages.register({ id: 'graphviz' });
	monaco.languages.setMonarchTokensProvider('graphviz', {
		defaultToken: '',
		tokenizer: {
			root: [
				[/\/\/.*$/, 'comment'],
				[/\b(digraph|graph|subgraph|node|edge|label|shape|color|style)\b/, 'keyword'],
				[/->|--/, 'delimiter'],
				[/"[^"]*"/, 'string'],
				[/[{}[\];=]/, 'delimiter'],
			],
		},
	});
}

export { monaco };
