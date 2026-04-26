// Webview panel that renders the control-flow graph of the active Nautilus
// IR file using Mermaid. The graph is synthesized client-side from the IR
// text — there is no dependency on Nautilus's `dump.graph` option.

import * as vscode from 'vscode';
import { irFor } from './features';
import { cfgFor } from './cfgFromParsedIr';
import { ParsedIr, blockAtLine, functionAt } from './parser';

const RENDER_DEBOUNCE_MS = 150;

// Wire-format messages exchanged with `media/graph.js`.
type ExtensionToWebview =
	| { type: 'render'; mermaid: string; functions: string[]; activeFunction: string; blocks: string[]; currentBlock: string | null }
	| { type: 'highlight'; block: string | null };

type WebviewToExtension =
	| { type: 'ready' }
	| { type: 'revealBlock'; block: string }
	| { type: 'selectFunction'; name: string }
	| { type: 'log'; level: 'info' | 'warn' | 'error'; message: string };

export class GraphPanel {
	private static current: GraphPanel | undefined;

	public static createOrShow(extensionUri: vscode.Uri): void {
		const editor = vscode.window.activeTextEditor;
		if (!editor || editor.document.languageId !== 'nautilus-ir') {
			vscode.window.showInformationMessage(
				'Open a Nautilus IR (.nautilus) file before opening the graph view.',
			);
			return;
		}

		if (GraphPanel.current) {
			GraphPanel.current.panel.reveal(vscode.ViewColumn.Beside, true);
			GraphPanel.current.linkTo(editor.document);
			return;
		}

		const panel = vscode.window.createWebviewPanel(
			'nautilus-ir.graph',
			titleFor(editor.document),
			{ viewColumn: vscode.ViewColumn.Beside, preserveFocus: true },
			{
				enableScripts: true,
				retainContextWhenHidden: true,
				localResourceRoots: [vscode.Uri.joinPath(extensionUri, 'media')],
			},
		);

		GraphPanel.current = new GraphPanel(panel, extensionUri, editor.document);
	}

	private readonly disposables: vscode.Disposable[] = [];
	private linkedDoc: vscode.TextDocument;
	private activeFunction: string | undefined;
	private renderTimer: NodeJS.Timeout | undefined;
	private webviewReady = false;
	private pendingRender: Extract<ExtensionToWebview, { type: 'render' }> | undefined;
	private lastRenderJson: string | undefined;
	private lastHighlightedBlock: string | null | undefined;

	private constructor(
		private readonly panel: vscode.WebviewPanel,
		private readonly extensionUri: vscode.Uri,
		initialDoc: vscode.TextDocument,
	) {
		this.linkedDoc = initialDoc;
		this.panel.webview.html = this.htmlFor(this.panel.webview);

		this.disposables.push(
			this.panel.onDidDispose(() => this.dispose()),
			this.panel.webview.onDidReceiveMessage(msg => this.handleMessage(msg as WebviewToExtension)),
			vscode.workspace.onDidChangeTextDocument(e => {
				if (e.document === this.linkedDoc) {
					this.scheduleRender();
				}
			}),
			vscode.window.onDidChangeActiveTextEditor(editor => {
				if (editor && editor.document.languageId === 'nautilus-ir' && editor.document !== this.linkedDoc) {
					this.linkTo(editor.document);
				}
			}),
			vscode.window.onDidChangeTextEditorSelection(e => {
				if (e.textEditor.document !== this.linkedDoc) {
					return;
				}
				this.postCurrentBlockHighlight(e.textEditor.selection.active.line);
			}),
		);
	}

	public linkTo(document: vscode.TextDocument): void {
		this.linkedDoc = document;
		this.panel.title = titleFor(document);
		this.activeFunction = undefined;
		this.lastRenderJson = undefined; // new doc → never dedup against old payload
		this.lastHighlightedBlock = undefined;
		this.scheduleRender(/* immediate */ true);
	}

	private scheduleRender(immediate = false): void {
		if (this.renderTimer) {
			clearTimeout(this.renderTimer);
			this.renderTimer = undefined;
		}
		const fire = () => {
			this.renderTimer = undefined;
			this.render();
		};
		if (immediate) {
			fire();
		} else {
			this.renderTimer = setTimeout(fire, RENDER_DEBOUNCE_MS);
		}
	}

	private render(): void {
		const ir = irFor(this.linkedDoc);
		const payload = this.buildRenderPayload(ir);
		if (!payload) {
			return;
		}
		const json = JSON.stringify(payload);
		if (json === this.lastRenderJson) {
			return; // edits that don't change the rendered graph stay invisible
		}
		this.lastRenderJson = json;
		this.lastHighlightedBlock = payload.currentBlock;
		this.pendingRender = payload;
		if (this.webviewReady) {
			void this.panel.webview.postMessage(payload);
		}
	}

	private buildRenderPayload(ir: ParsedIr): Extract<ExtensionToWebview, { type: 'render' }> | undefined {
		if (ir.functions.length === 0) {
			return {
				type: 'render',
				mermaid: 'flowchart TD\n    empty["(no functions found)"]',
				functions: [],
				activeFunction: '',
				blocks: [],
				currentBlock: null,
			};
		}

		const fnNames = ir.functions.map(f => f.name);
		if (!this.activeFunction || !fnNames.includes(this.activeFunction)) {
			this.activeFunction = this.functionAtCursor(ir) ?? fnNames[0];
		}
		const graph = cfgFor(ir, this.activeFunction);
		if (!graph) {
			return undefined;
		}
		return {
			type: 'render',
			mermaid: graph.mermaid,
			functions: fnNames,
			activeFunction: graph.functionName,
			blocks: graph.blocks,
			currentBlock: this.currentBlockName(ir),
		};
	}

	private cursorLine(): number | undefined {
		const editor = vscode.window.visibleTextEditors.find(e => e.document === this.linkedDoc);
		return editor?.selection.active.line;
	}

	private functionAtCursor(ir: ParsedIr): string | undefined {
		const line = this.cursorLine();
		return line === undefined ? undefined : functionAt(ir, line)?.name;
	}

	private currentBlockName(ir: ParsedIr): string | null {
		const line = this.cursorLine();
		if (line === undefined) {
			return null;
		}
		const block = blockAtLine(ir, line);
		return block && block.functionName === this.activeFunction ? block.name : null;
	}

	private postCurrentBlockHighlight(line: number): void {
		if (!this.webviewReady) {
			return;
		}
		const ir = irFor(this.linkedDoc);
		const fn = functionAt(ir, line);
		// Cursor moved into a different function → switch the rendered graph.
		if (fn && fn.name !== this.activeFunction) {
			this.activeFunction = fn.name;
			this.scheduleRender(/* immediate */ true);
			return;
		}
		const block = blockAtLine(ir, line);
		const name = block && block.functionName === this.activeFunction ? block.name : null;
		if (name === this.lastHighlightedBlock) {
			return; // no-op: cursor moved within the same block
		}
		this.lastHighlightedBlock = name;
		const payload: ExtensionToWebview = { type: 'highlight', block: name };
		void this.panel.webview.postMessage(payload);
	}

	private handleMessage(msg: WebviewToExtension): void {
		switch (msg.type) {
		case 'ready':
			this.webviewReady = true;
			if (!this.pendingRender) {
				this.pendingRender = this.buildRenderPayload(irFor(this.linkedDoc));
			}
			if (this.pendingRender) {
				void this.panel.webview.postMessage(this.pendingRender);
			}
			return;
		case 'revealBlock':
			void this.revealBlock(msg.block);
			return;
		case 'selectFunction':
			this.activeFunction = msg.name;
			this.scheduleRender(/* immediate */ true);
			return;
		case 'log':
			if (msg.level === 'error') {
				vscode.window.showErrorMessage(`Nautilus IR graph: ${msg.message}`);
			}
			return;
		}
	}

	private async revealBlock(blockName: string): Promise<void> {
		if (!blockName) {
			return;
		}
		const editor = await vscode.window.showTextDocument(this.linkedDoc, {
			viewColumn: vscode.ViewColumn.One,
			preserveFocus: false,
		});
		const ir = irFor(this.linkedDoc);
		const block = ir.functionByName.get(this.activeFunction ?? '')?.blockByName.get(blockName)
			?? ir.blockByName.get(blockName);
		if (!block) {
			return;
		}
		const pos = new vscode.Position(block.headerLine, 0);
		editor.selection = new vscode.Selection(pos, pos);
		editor.revealRange(block.headerRange, vscode.TextEditorRevealType.InCenter);
	}

	private htmlFor(webview: vscode.Webview): string {
		const mediaRoot = vscode.Uri.joinPath(this.extensionUri, 'media');
		const mermaidUri = webview.asWebviewUri(vscode.Uri.joinPath(mediaRoot, 'vendor', 'mermaid.min.js'));
		const panZoomUri = webview.asWebviewUri(vscode.Uri.joinPath(mediaRoot, 'vendor', 'svg-pan-zoom.min.js'));
		const purifyUri = webview.asWebviewUri(vscode.Uri.joinPath(mediaRoot, 'vendor', 'purify.min.js'));
		const scriptUri = webview.asWebviewUri(vscode.Uri.joinPath(mediaRoot, 'graph.js'));
		const styleUri = webview.asWebviewUri(vscode.Uri.joinPath(mediaRoot, 'graph.css'));
		const nonce = randomNonce();
		const csp = [
			`default-src 'none'`,
			`img-src ${webview.cspSource} data:`,
			`style-src ${webview.cspSource} 'unsafe-inline'`,
			`font-src ${webview.cspSource}`,
			`script-src ${webview.cspSource} 'nonce-${nonce}'`,
		].join('; ');

		return `<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta http-equiv="Content-Security-Policy" content="${csp}">
<link rel="stylesheet" href="${styleUri}">
<title>Nautilus IR Graph</title>
</head>
<body>
<div id="toolbar">
	<label for="function-select">Function</label>
	<select id="function-select"></select>
	<button id="fit-btn" title="Fit to window">Fit</button>
	<button id="reset-btn" title="Reset zoom">Reset</button>
	<span id="status"></span>
</div>
<div id="graph-host">
	<div id="graph-container"></div>
</div>
<script nonce="${nonce}" src="${purifyUri}"></script>
<script nonce="${nonce}" src="${mermaidUri}"></script>
<script nonce="${nonce}" src="${panZoomUri}"></script>
<script nonce="${nonce}" src="${scriptUri}"></script>
</body>
</html>`;
	}

	private dispose(): void {
		GraphPanel.current = undefined;
		if (this.renderTimer) {
			clearTimeout(this.renderTimer);
		}
		while (this.disposables.length) {
			const d = this.disposables.pop();
			try { d?.dispose(); } catch { /* ignore */ }
		}
	}
}

function titleFor(document: vscode.TextDocument): string {
	return `Nautilus IR Graph — ${document.fileName.split(/[\\/]/).pop() ?? ''}`;
}

function randomNonce(): string {
	const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
	let s = '';
	for (let i = 0; i < 32; i++) {
		s += chars.charAt(Math.floor(Math.random() * chars.length));
	}
	return s;
}
