// Minimal browser shim for the `vscode` module, sufficient for the parts of
// the vscode-nautilus-ir extension reused here (parser.ts and
// cfgFromParsedIr.ts): Position, Range, and a read-only TextDocument over a
// plain string. Wired up via the `vscode` alias in vite.config.ts and the
// matching tsconfig `paths` entry.

export class Position {
	constructor(
		public readonly line: number,
		public readonly character: number,
	) {}
}

export class Range {
	public readonly start: Position;
	public readonly end: Position;

	constructor(startLine: number, startCharacter: number, endLine: number, endCharacter: number) {
		this.start = new Position(startLine, startCharacter);
		this.end = new Position(endLine, endCharacter);
	}
}

export interface TextLine {
	readonly lineNumber: number;
	readonly text: string;
}

export interface TextDocument {
	readonly lineCount: number;
	lineAt(line: number): TextLine;
	getText(): string;
}

export function makeTextDocument(text: string): TextDocument {
	const lines = text.split(/\r?\n/);
	return {
		lineCount: lines.length,
		lineAt(line: number): TextLine {
			return { lineNumber: line, text: lines[line] ?? '' };
		},
		getText(): string {
			return text;
		},
	};
}
