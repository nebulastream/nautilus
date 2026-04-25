import * as vscode from 'vscode';
import {
	IrSymbolProvider,
	IrDefinitionProvider,
	IrReferenceProvider,
	IrHoverProvider,
	IrHighlightProvider,
	IrFoldingProvider,
	IrRenameProvider,
	IrCodeLensProvider,
	irFor,
} from './features';
import { GdbDescriptorFactory } from './debugAdapter';

const SELECTOR: vscode.DocumentSelector = { language: 'nautilus-ir', scheme: 'file' };

export function activate(context: vscode.ExtensionContext): void {
	context.subscriptions.push(
		vscode.languages.registerDocumentSymbolProvider(SELECTOR, new IrSymbolProvider()),
		vscode.languages.registerDefinitionProvider(SELECTOR, new IrDefinitionProvider()),
		vscode.languages.registerReferenceProvider(SELECTOR, new IrReferenceProvider()),
		vscode.languages.registerHoverProvider(SELECTOR, new IrHoverProvider()),
		vscode.languages.registerDocumentHighlightProvider(SELECTOR, new IrHighlightProvider()),
		vscode.languages.registerFoldingRangeProvider(SELECTOR, new IrFoldingProvider()),
		vscode.languages.registerRenameProvider(SELECTOR, new IrRenameProvider()),
		vscode.languages.registerCodeLensProvider(SELECTOR, new IrCodeLensProvider()),
	);

	// Diagnostics: light static checks (undefined SSA references, dangling branches).
	const diagnostics = vscode.languages.createDiagnosticCollection('nautilus-ir');
	context.subscriptions.push(diagnostics);

	const refresh = (doc: vscode.TextDocument) => {
		if (doc.languageId !== 'nautilus-ir') {
			return;
		}
		diagnostics.set(doc.uri, runLint(doc));
	};

	vscode.workspace.textDocuments.forEach(refresh);
	context.subscriptions.push(
		vscode.workspace.onDidOpenTextDocument(refresh),
		vscode.workspace.onDidChangeTextDocument(e => refresh(e.document)),
		vscode.workspace.onDidCloseTextDocument(d => diagnostics.delete(d.uri)),
	);

	// Commands.
	context.subscriptions.push(
		vscode.commands.registerCommand('nautilus-ir.gotoBlock', gotoBlockCommand),
		vscode.commands.registerCommand('nautilus-ir.dumpInfo', dumpInfoCommand),
		vscode.commands.registerCommand('nautilus-ir.showBlockInfo', showBlockInfoCommand),
		vscode.commands.registerCommand('nautilus-ir.toggleFollowDefinitions', toggleFollowDefinitionsCommand),
		vscode.commands.registerCommand('nautilus-ir.debugWithGdb', debugWithGdbCommand),
	);

	// Debug adapter (inline implementation, no separate process).
	context.subscriptions.push(
		vscode.debug.registerDebugAdapterDescriptorFactory('nautilus-ir-gdb', new GdbDescriptorFactory()),
	);

	// Status bar item: show current block when cursor is inside one.
	const statusItem = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 100);
	statusItem.command = 'nautilus-ir.gotoBlock';
	context.subscriptions.push(statusItem);

	const updateStatus = () => {
		const editor = vscode.window.activeTextEditor;
		if (!editor || editor.document.languageId !== 'nautilus-ir') {
			statusItem.hide();
			return;
		}
		const ir = irFor(editor.document);
		const line = editor.selection.active.line;
		const block = ir.blocks.find(b => line >= b.headerLine && line <= b.bodyRange.end.line);
		if (block) {
			statusItem.text = `$(symbol-method) ${block.name}  ⟶  ${block.successors.join(', ') || block.terminator}`;
			statusItem.tooltip = new vscode.MarkdownString(
				`**${block.name}**\n\nPredecessors: ${block.predecessors.join(', ') || '*(none)*'}\n\nSuccessors: ${block.successors.join(', ') || '*(none)*'}`,
			);
			statusItem.show();
		} else {
			statusItem.hide();
		}
	};
	context.subscriptions.push(
		vscode.window.onDidChangeActiveTextEditor(updateStatus),
		vscode.window.onDidChangeTextEditorSelection(updateStatus),
	);
	updateStatus();
}

export function deactivate(): void {
	// Nothing to clean up — all subscriptions are handled by VS Code.
}

// ----- Commands -----

async function gotoBlockCommand(): Promise<void> {
	const editor = vscode.window.activeTextEditor;
	if (!editor || editor.document.languageId !== 'nautilus-ir') {
		return;
	}
	const ir = irFor(editor.document);
	const items: vscode.QuickPickItem[] = ir.blocks.map(b => ({
		label: b.name,
		description: `${b.args.length} args, ${b.instructions} insns`,
		detail: `→ ${b.successors.length ? b.successors.join(', ') : b.terminator}`,
	}));
	const pick = await vscode.window.showQuickPick(items, {
		placeHolder: 'Jump to block...',
		matchOnDescription: true,
		matchOnDetail: true,
	});
	if (!pick) {
		return;
	}
	const target = ir.blockByName.get(pick.label);
	if (target) {
		const pos = new vscode.Position(target.headerLine, 0);
		editor.selection = new vscode.Selection(pos, pos);
		editor.revealRange(target.headerRange, vscode.TextEditorRevealType.InCenter);
	}
}

function dumpInfoCommand(): void {
	const editor = vscode.window.activeTextEditor;
	if (!editor || editor.document.languageId !== 'nautilus-ir') {
		return;
	}
	const ir = irFor(editor.document);
	const total = ir.blocks.reduce((sum, b) => sum + b.instructions, 0);
	const unreachable = ir.blocks.filter(b => b.predecessors.length === 0 && b !== ir.blocks[0]);
	vscode.window.showInformationMessage(
		`Nautilus IR: ${ir.blocks.length} blocks · ${total} instructions · ${ir.definitions.size} SSA defs · ${unreachable.length} unreachable`,
	);
}

function showBlockInfoCommand(uri: vscode.Uri, blockName: string): void {
	vscode.workspace.openTextDocument(uri).then(doc => {
		const ir = irFor(doc);
		const block = ir.blockByName.get(blockName);
		if (!block) {
			return;
		}
		vscode.window.showInformationMessage(
			`${block.name}: preds=[${block.predecessors.join(', ')}] succs=[${block.successors.join(', ')}] terminator=${block.terminator}`,
		);
	});
}

function toggleFollowDefinitionsCommand(): void {
	const config = vscode.workspace.getConfiguration('nautilusIr');
	const current = config.get<boolean>('highlightDefinitions', true);
	config.update('highlightDefinitions', !current, vscode.ConfigurationTarget.Global);
	vscode.window.showInformationMessage(`Nautilus IR: definition highlighting ${!current ? 'enabled' : 'disabled'}.`);
}

async function debugWithGdbCommand(): Promise<void> {
	const editor = vscode.window.activeTextEditor;
	if (!editor || editor.document.languageId !== 'nautilus-ir') {
		vscode.window.showWarningMessage('Open a Nautilus IR file before launching the gdb debugger.');
		return;
	}
	const folder = vscode.workspace.getWorkspaceFolder(editor.document.uri);
	await vscode.debug.startDebugging(folder, {
		type: 'nautilus-ir-gdb',
		request: 'launch',
		name: 'Debug Nautilus IR (gdb)',
		program: editor.document.uri.fsPath,
		stopOnEntry: true,
	});
}

// ----- Lint -----

function runLint(doc: vscode.TextDocument): vscode.Diagnostic[] {
	const ir = irFor(doc);
	const out: vscode.Diagnostic[] = [];

	// Block argument names that shadow defs are fine (block args ARE defs); collect the set.
	const knownNames = new Set<string>(ir.definitions.keys());
	for (const block of ir.blocks) {
		for (const arg of block.args) {
			knownNames.add(arg.name);
		}
	}

	// 1. Undefined SSA references.
	for (const [name, refs] of ir.references) {
		if (knownNames.has(name)) {
			continue;
		}
		for (const r of refs) {
			out.push(new vscode.Diagnostic(
				new vscode.Range(r.line, r.character, r.line, r.character + name.length),
				`Undefined SSA value ${name}.`,
				vscode.DiagnosticSeverity.Warning,
			));
		}
	}

	// 2. Branches to unknown blocks.
	for (const block of ir.blocks) {
		for (const succ of block.successors) {
			if (!ir.blockByName.has(succ)) {
				out.push(new vscode.Diagnostic(
					block.bodyRange,
					`Branch targets unknown block ${succ}.`,
					vscode.DiagnosticSeverity.Warning,
				));
			}
		}
	}

	// 3. Blocks without a recognizable terminator.
	for (const block of ir.blocks) {
		if (block.instructions > 0 && block.terminator === 'unknown') {
			out.push(new vscode.Diagnostic(
				block.headerRange,
				`Block ${block.name} has no return/branch terminator.`,
				vscode.DiagnosticSeverity.Information,
			));
		}
	}

	return out;
}
