import * as vscode from 'vscode';
import * as path from 'path';
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
			statusItem.text = `$(symbol-method) ${block.functionName}::${block.name}  ⟶  ${block.successors.join(', ') || block.terminator}`;
			statusItem.tooltip = new vscode.MarkdownString(
				`**${block.functionName}::${block.name}**\n\nPredecessors: ${block.predecessors.join(', ') || '*(none)*'}\n\nSuccessors: ${block.successors.join(', ') || '*(none)*'}`,
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
	const items: (vscode.QuickPickItem & { fn: string; block: string })[] = ir.blocks.map(b => ({
		label: `${b.functionName}::${b.name}`,
		description: `${b.args.length} args, ${b.instructions} insns`,
		detail: `→ ${b.successors.length ? b.successors.join(', ') : b.terminator}`,
		fn: b.functionName,
		block: b.name,
	}));
	const pick = await vscode.window.showQuickPick(items, {
		placeHolder: 'Jump to block...',
		matchOnDescription: true,
		matchOnDetail: true,
	});
	if (!pick) {
		return;
	}
	const fn = ir.functionByName.get(pick.fn);
	const target = fn?.blockByName.get(pick.block);
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
	const totalInsns = ir.blocks.reduce((sum, b) => sum + b.instructions, 0);
	const totalDefs = ir.functions.reduce((sum, fn) => sum + fn.definitions.size, 0);
	const unreachable: string[] = [];
	for (const fn of ir.functions) {
		const entry = fn.blocks[0];
		for (const block of fn.blocks) {
			if (block !== entry && block.predecessors.length === 0) {
				unreachable.push(`${fn.name}::${block.name}`);
			}
		}
	}
	vscode.window.showInformationMessage(
		`Nautilus IR: ${ir.functions.length} functions · ${ir.blocks.length} blocks · ${totalInsns} instructions · ${totalDefs} SSA defs · ${unreachable.length} unreachable`,
	);
}

function showBlockInfoCommand(uri: vscode.Uri, blockName: string): void {
	vscode.workspace.openTextDocument(uri).then(doc => {
		const ir = irFor(doc);
		const block = ir.blocks.find(b => b.name === blockName);
		if (!block) {
			return;
		}
		vscode.window.showInformationMessage(
			`${block.functionName}::${block.name}: preds=[${block.predecessors.join(', ')}] succs=[${block.successors.join(', ')}] terminator=${block.terminator}`,
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
	// A Nautilus IR file is loaded by a C++ host program. Ask the user which
	// host binary to attach gdb to, defaulting to a remembered choice.
	const irPath = editor.document.uri.fsPath;
	const folder = vscode.workspace.getWorkspaceFolder(editor.document.uri);
	const memento = vscode.workspace.getConfiguration('nautilusIr');
	const remembered = memento.get<string>('lastHostProgram', '');
	const picked = await vscode.window.showOpenDialog({
		title: 'Select the C++ host executable that loads this Nautilus IR',
		canSelectFiles: true,
		canSelectFolders: false,
		canSelectMany: false,
		defaultUri: remembered ? vscode.Uri.file(remembered) : folder?.uri,
		openLabel: 'Debug',
	});
	if (!picked || picked.length === 0) {
		return;
	}
	const hostProgram = picked[0].fsPath;
	await memento.update('lastHostProgram', hostProgram, vscode.ConfigurationTarget.Workspace);

	await vscode.debug.startDebugging(folder, {
		type: 'nautilus-ir-gdb',
		request: 'launch',
		name: 'Debug Nautilus IR via host program (gdb)',
		program: hostProgram,
		args: [],
		irFile: irPath,
		stopAtIrEntry: true,
		cwd: folder?.uri.fsPath ?? path.dirname(hostProgram),
	});
}

// ----- Lint -----

function runLint(doc: vscode.TextDocument): vscode.Diagnostic[] {
	const ir = irFor(doc);
	const out: vscode.Diagnostic[] = [];

	// SSA names are scoped per function — `$3` in `add()` is unrelated to
	// `$3` in `execute()`. Validate each function in isolation.
	for (const fn of ir.functions) {
		const known = new Set<string>(fn.definitions.keys());
		for (const block of fn.blocks) {
			for (const arg of block.args) {
				known.add(arg.name);
			}
		}
		// Undefined SSA references inside this function.
		for (const [name, refs] of ir.references) {
			if (known.has(name)) {
				continue;
			}
			for (const r of refs) {
				if (r.functionName !== fn.name) {
					continue;
				}
				out.push(new vscode.Diagnostic(
					new vscode.Range(r.line, r.character, r.line, r.character + name.length),
					`Undefined SSA value ${name} in ${fn.name}().`,
					vscode.DiagnosticSeverity.Warning,
				));
			}
		}
		// Dangling branch targets.
		for (const block of fn.blocks) {
			for (const succ of block.successors) {
				if (!fn.blockByName.has(succ)) {
					out.push(new vscode.Diagnostic(
						block.headerRange,
						`Branch in ${fn.name}() targets unknown block ${succ}.`,
						vscode.DiagnosticSeverity.Warning,
					));
				}
			}
			if (block.instructions > 0 && block.terminator === 'unknown') {
				out.push(new vscode.Diagnostic(
					block.headerRange,
					`Block ${block.name} in ${fn.name}() has no return/branch terminator.`,
					vscode.DiagnosticSeverity.Information,
				));
			}
		}
	}

	return out;
}
