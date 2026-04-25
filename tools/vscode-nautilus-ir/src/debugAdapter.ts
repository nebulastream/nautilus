// Debug Adapter for Nautilus IR backed by gdb.
//
// Important context: a Nautilus IR file is never a self-contained executable.
// It is loaded by a C++ host program (e.g. an ExecutionTest binary, the demo
// JIT, or a NebulaStream worker) which JIT-compiles it via the Nautilus
// engine. To debug an IR file you therefore launch *the host program* under
// gdb, then rely on the Nautilus backend to emit source-line debug info that
// points back to the IR file. Once execution enters that JIT'd code, gdb
// resolves the addresses to FILE:LINE positions in the IR file and the
// editor's breakpoints / step / variables UI light up.
//
// Adapter responsibilities:
//   * Spawn `gdb --interpreter=mi2` on the host program (DAP `program`).
//   * Forward DAP `args` and `env` to gdb via -exec-arguments and the gdb env.
//   * Add the IR file's directory to gdb's source search path so file/line
//     events resolve cleanly.
//   * Map DAP setBreakpoints in an IR file -> gdb `-break-insert FILE:LINE`.
//     gdb will only resolve them after the JIT registers symbols, but it
//     handles "pending on future symbol load" automatically.
//   * Map next/stepIn/stepOut/continue to the corresponding -exec-* commands.
//   * Translate gdb `*stopped` events to DAP StoppedEvent.
//
// This adapter is inline — VS Code runs it inside the extension host via
// DebugAdapterInlineImplementation, so no extra adapter process is needed
// beyond gdb itself.

import * as vscode from 'vscode';
import { DebugSession, InitializedEvent, StoppedEvent, TerminatedEvent, OutputEvent, Thread, StackFrame, Source, Scope, Handles } from '@vscode/debugadapter';
import { DebugProtocol } from '@vscode/debugprotocol';
import * as path from 'path';
import * as fs from 'fs';
import { GdbMi, MiAsync, MiValue } from './gdbMi';

interface LaunchArgs extends DebugProtocol.LaunchRequestArguments {
	program: string;                  // C++ host executable
	args?: string[];                  // forwarded to the inferior
	irFile?: string;                  // optional Nautilus IR file of interest
	stopAtIrEntry?: boolean;          // breakpoint at first instruction of irFile
	stopOnEntry?: boolean;            // breakpoint at host main()
	gdbPath?: string;
	gdbArgs?: string[];
	sourceDirectories?: string[];
	cwd?: string;
	env?: Record<string, string>;
	trace?: boolean;
}

interface AttachArgs extends DebugProtocol.AttachRequestArguments {
	program: string;
	pid: number;
	irFile?: string;
	sourceDirectories?: string[];
	gdbPath?: string;
	gdbArgs?: string[];
	trace?: boolean;
}

const DEFAULT_THREAD_ID = 1;

export class GdbDescriptorFactory implements vscode.DebugAdapterDescriptorFactory {
	createDebugAdapterDescriptor(_session: vscode.DebugSession): vscode.ProviderResult<vscode.DebugAdapterDescriptor> {
		return new vscode.DebugAdapterInlineImplementation(new NautilusGdbSession());
	}
}

class NautilusGdbSession extends DebugSession {
	private gdb: GdbMi | undefined;
	private programPath = '';
	private trace = false;
	private breakpointIds = new Map<string, number[]>(); // file → gdb breakpoint ids
	private variableHandles = new Handles<'locals' | 'args' | 'registers'>();

	constructor() {
		super();
		this.setDebuggerLinesStartAt1(true);
		this.setDebuggerColumnsStartAt1(true);
	}

	protected initializeRequest(response: DebugProtocol.InitializeResponse, _args: DebugProtocol.InitializeRequestArguments): void {
		response.body = response.body ?? {};
		response.body.supportsConfigurationDoneRequest = true;
		response.body.supportsFunctionBreakpoints = true;
		response.body.supportsConditionalBreakpoints = true;
		response.body.supportsHitConditionalBreakpoints = true;
		response.body.supportsEvaluateForHovers = true;
		response.body.supportsStepBack = false;
		response.body.supportsSetVariable = true;
		response.body.supportsRestartRequest = true;
		response.body.supportsTerminateRequest = true;
		response.body.supportsLogPoints = true;
		response.body.supportsDisassembleRequest = true;
		response.body.supportsReadMemoryRequest = true;
		response.body.exceptionBreakpointFilters = [
			{ filter: 'signals', label: 'POSIX Signals', default: true },
		];
		this.sendResponse(response);
		this.sendEvent(new InitializedEvent());
	}

	protected async launchRequest(response: DebugProtocol.LaunchResponse, args: LaunchArgs): Promise<void> {
		try {
			this.trace = args.trace === true;
			this.programPath = args.program;
			const cfg = vscode.workspace.getConfiguration('nautilusIr');
			const gdbPath = args.gdbPath ?? cfg.get<string>('gdbPath', 'gdb') ?? 'gdb';
			const extraArgs = (args.gdbArgs ?? cfg.get<string[]>('gdbExtraArgs', []) ?? []).slice();
			const cwd = args.cwd ?? path.dirname(args.program);
			await this.startGdb(gdbPath, extraArgs, cwd, mergeEnv(args.env));

			// Load the host executable.
			this.sendOutput(`Loading host program: ${args.program}\n`);
			await this.gdb!.send(`file-exec-and-symbols ${quote(args.program)}`);

			// Tell gdb about all source-search directories. We always include the
			// host program's own directory; the IR file's directory ensures gdb
			// can resolve file/line events emitted by the JIT'd code.
			const dirs = new Set<string>();
			dirs.add(path.dirname(args.program));
			if (args.irFile) {
				dirs.add(path.dirname(args.irFile));
			}
			for (const d of args.sourceDirectories ?? []) {
				dirs.add(d);
			}
			for (const dir of dirs) {
				await this.safe(this.gdb!.send(`environment-directory -r ${quote(dir)}`));
			}

			// Forward arguments to the inferior. -exec-arguments takes a single
			// space-separated string (with quoting), so build it carefully.
			if (args.args && args.args.length > 0) {
				const argLine = args.args.map(quote).join(' ');
				await this.safe(this.gdb!.send(`exec-arguments ${argLine}`));
			}

			// Optional break at host main() — useful for debugging the host
			// before JIT compilation kicks in.
			if (args.stopOnEntry) {
				await this.safe(this.gdb!.send(`break-insert -t main`));
			}

			// Optional break at the first instruction of the IR file. The
			// breakpoint will be pending until the JIT registers symbols for
			// that file; gdb handles that via -break-insert -f.
			if (args.stopAtIrEntry !== false && args.irFile) {
				const entryLine = await detectIrEntryLine(args.irFile);
				if (entryLine > 0) {
					await this.safe(this.gdb!.send(`break-insert -f ${quote(args.irFile)}:${entryLine}`));
					this.sendOutput(`Set pending breakpoint at ${args.irFile}:${entryLine}\n`, 'console');
				}
			}

			await this.gdb!.send('exec-run');
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1000, `Launch failed: ${formatError(err)}`);
		}
	}

	protected async attachRequest(response: DebugProtocol.AttachResponse, args: AttachArgs): Promise<void> {
		try {
			this.trace = args.trace === true;
			this.programPath = args.program;
			const cfg = vscode.workspace.getConfiguration('nautilusIr');
			const gdbPath = args.gdbPath ?? cfg.get<string>('gdbPath', 'gdb') ?? 'gdb';
			const extraArgs = (args.gdbArgs ?? cfg.get<string[]>('gdbExtraArgs', []) ?? []).slice();
			await this.startGdb(gdbPath, extraArgs, path.dirname(args.program), process.env);
			await this.gdb!.send(`file-exec-and-symbols ${quote(args.program)}`);
			const dirs = new Set<string>();
			dirs.add(path.dirname(args.program));
			if (args.irFile) {
				dirs.add(path.dirname(args.irFile));
			}
			for (const d of args.sourceDirectories ?? []) {
				dirs.add(d);
			}
			for (const dir of dirs) {
				await this.safe(this.gdb!.send(`environment-directory -r ${quote(dir)}`));
			}
			await this.gdb!.send(`target-attach ${args.pid}`);
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1001, `Attach failed: ${formatError(err)}`);
		}
	}

	protected async configurationDoneRequest(response: DebugProtocol.ConfigurationDoneResponse): Promise<void> {
		this.sendResponse(response);
	}

	protected async setBreakPointsRequest(response: DebugProtocol.SetBreakpointsResponse, args: DebugProtocol.SetBreakpointsArguments): Promise<void> {
		try {
			const file = args.source.path ?? '';
			// Clear previous breakpoints for this file.
			const prev = this.breakpointIds.get(file) ?? [];
			for (const id of prev) {
				await this.safe(this.gdb!.send(`break-delete ${id}`));
			}
			const newIds: number[] = [];
			const verified: DebugProtocol.Breakpoint[] = [];
			for (const bp of args.breakpoints ?? []) {
				// Use -f so the breakpoint is accepted as pending while the JIT
				// has not yet registered symbols for the IR file.
				const spec = `-f ${quote(file)}:${bp.line}`;
				const condParts: string[] = [];
				if (bp.condition) {
					condParts.push(`-c ${quote(bp.condition)}`);
				}
				if (bp.hitCondition && /^\d+$/.test(bp.hitCondition.trim())) {
					condParts.push(`-i ${bp.hitCondition.trim()}`);
				}
				const cmd = `break-insert ${condParts.join(' ')} ${spec}`.trim();
				try {
					const result = await this.gdb!.send(cmd);
					const bkpt = (result.results as { bkpt?: { number?: string; line?: string; pending?: string } }).bkpt;
					const numStr = bkpt?.number;
					const id = numStr ? parseInt(numStr, 10) : NaN;
					if (!Number.isNaN(id)) {
						newIds.push(id);
					}
					const reportedLine = bkpt?.line ? parseInt(bkpt.line, 10) : bp.line;
					const isPending = !!bkpt?.pending;
					verified.push({
						verified: !isPending,
						line: reportedLine,
						message: isPending ? 'pending until JIT loads IR' : undefined,
					});
				} catch (err) {
					verified.push({ verified: false, line: bp.line, message: formatError(err) });
				}
			}
			this.breakpointIds.set(file, newIds);
			response.body = { breakpoints: verified };
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1010, `setBreakpoints failed: ${formatError(err)}`);
		}
	}

	protected threadsRequest(response: DebugProtocol.ThreadsResponse): void {
		response.body = { threads: [new Thread(DEFAULT_THREAD_ID, 'main')] };
		this.sendResponse(response);
	}

	protected async stackTraceRequest(response: DebugProtocol.StackTraceResponse, _args: DebugProtocol.StackTraceArguments): Promise<void> {
		try {
			const result = await this.gdb!.send('stack-list-frames');
			const stack = (result.results as { stack?: MiValue[] }).stack ?? [];
			const frames: StackFrame[] = [];
			let id = 1;
			for (const raw of stack) {
				const frame = raw as { level?: string; func?: string; file?: string; fullname?: string; line?: string };
				const file = frame.fullname ?? frame.file ?? this.programPath;
				const line = frame.line ? parseInt(frame.line, 10) : 0;
				frames.push(new StackFrame(
					id++,
					frame.func ?? `frame_${frame.level ?? '?'}`,
					new Source(path.basename(file), file),
					line,
					0,
				));
			}
			response.body = { stackFrames: frames, totalFrames: frames.length };
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1020, `stackTrace failed: ${formatError(err)}`);
		}
	}

	protected scopesRequest(response: DebugProtocol.ScopesResponse): void {
		response.body = {
			scopes: [
				new Scope('Locals', this.variableHandles.create('locals'), false),
				new Scope('Arguments', this.variableHandles.create('args'), false),
				new Scope('Registers', this.variableHandles.create('registers'), true),
			],
		};
		this.sendResponse(response);
	}

	protected async variablesRequest(response: DebugProtocol.VariablesResponse, args: DebugProtocol.VariablesArguments): Promise<void> {
		const kind = this.variableHandles.get(args.variablesReference);
		const variables: DebugProtocol.Variable[] = [];
		try {
			if (kind === 'locals' || kind === 'args') {
				const cmd = `stack-list-variables --simple-values`;
				const result = await this.gdb!.send(cmd);
				const list = (result.results as { variables?: MiValue[] }).variables ?? [];
				for (const raw of list) {
					const v = raw as { name?: string; value?: string; type?: string; arg?: string };
					const isArg = v.arg === '1';
					if ((kind === 'args') !== isArg) {
						continue;
					}
					variables.push({
						name: v.name ?? '?',
						value: v.value ?? '<opt>',
						type: v.type,
						variablesReference: 0,
					});
				}
			} else if (kind === 'registers') {
				const namesResult = await this.gdb!.send('data-list-register-names');
				const valuesResult = await this.gdb!.send('data-list-register-values --skip-unavailable x');
				const names = (namesResult.results as { 'register-names'?: MiValue[] })['register-names'] ?? [];
				const values = (valuesResult.results as { 'register-values'?: MiValue[] })['register-values'] ?? [];
				for (const rawV of values) {
					const v = rawV as { number?: string; value?: string };
					const idx = v.number ? parseInt(v.number, 10) : -1;
					const n = idx >= 0 ? names[idx] : undefined;
					variables.push({
						name: typeof n === 'string' ? n : `r${idx}`,
						value: v.value ?? '?',
						variablesReference: 0,
					});
				}
			}
			response.body = { variables };
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1030, `variables failed: ${formatError(err)}`);
		}
	}

	protected async continueRequest(response: DebugProtocol.ContinueResponse): Promise<void> {
		try {
			await this.gdb!.send('exec-continue');
			response.body = { allThreadsContinued: true };
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1040, formatError(err));
		}
	}

	protected async nextRequest(response: DebugProtocol.NextResponse): Promise<void> {
		await this.runStep(response, 'exec-next');
	}

	protected async stepInRequest(response: DebugProtocol.StepInResponse): Promise<void> {
		await this.runStep(response, 'exec-step');
	}

	protected async stepOutRequest(response: DebugProtocol.StepOutResponse): Promise<void> {
		await this.runStep(response, 'exec-finish');
	}

	protected async pauseRequest(response: DebugProtocol.PauseResponse): Promise<void> {
		try {
			await this.gdb!.send('exec-interrupt');
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1050, formatError(err));
		}
	}

	protected async evaluateRequest(response: DebugProtocol.EvaluateResponse, args: DebugProtocol.EvaluateArguments): Promise<void> {
		try {
			const result = await this.gdb!.send(`data-evaluate-expression ${quote(args.expression)}`);
			const value = (result.results as { value?: string }).value ?? '<no value>';
			response.body = { result: value, variablesReference: 0 };
			this.sendResponse(response);
		} catch (err) {
			response.body = { result: formatError(err), variablesReference: 0 };
			this.sendResponse(response);
		}
	}

	protected async setVariableRequest(response: DebugProtocol.SetVariableResponse, args: DebugProtocol.SetVariableArguments): Promise<void> {
		try {
			await this.gdb!.send(`gdb-set var ${args.name}=${args.value}`);
			response.body = { value: args.value };
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1060, formatError(err));
		}
	}

	protected async terminateRequest(response: DebugProtocol.TerminateResponse): Promise<void> {
		this.shutdown();
		this.sendResponse(response);
	}

	protected async disconnectRequest(response: DebugProtocol.DisconnectResponse): Promise<void> {
		this.shutdown();
		this.sendResponse(response);
	}

	protected async restartRequest(response: DebugProtocol.RestartResponse): Promise<void> {
		try {
			await this.gdb!.send('exec-run');
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1070, formatError(err));
		}
	}

	// ----- Internal helpers -----

	private async runStep(response: DebugProtocol.Response, command: string): Promise<void> {
		try {
			await this.gdb!.send(command);
			this.sendResponse(response);
		} catch (err) {
			this.sendErrorResponse(response, 1080, formatError(err));
		}
	}

	private async startGdb(gdbPath: string, extraArgs: string[], cwd: string, env: NodeJS.ProcessEnv): Promise<void> {
		this.gdb = new GdbMi(gdbPath, extraArgs);
		this.gdb.on('console', (text: string) => this.sendOutput(text, 'console'));
		this.gdb.on('output', (text: string) => this.sendOutput(text + '\n', 'stdout'));
		this.gdb.on('stderr', (text: string) => this.sendOutput(text, 'stderr'));
		this.gdb.on('exec', (evt: MiAsync) => this.handleExec(evt));
		this.gdb.on('notify', (evt: MiAsync) => {
			if (this.trace) {
				this.sendOutput(`[notify] ${evt.cls}\n`, 'console');
			}
		});
		this.gdb.on('exit', () => {
			this.sendEvent(new TerminatedEvent());
		});
		await this.gdb.start(cwd, env);
	}

	private handleExec(evt: MiAsync): void {
		if (evt.cls !== 'stopped') {
			return;
		}
		const reason = String(evt.results['reason'] ?? 'pause');
		let dapReason = 'pause';
		switch (reason) {
			case 'breakpoint-hit': dapReason = 'breakpoint'; break;
			case 'end-stepping-range':
			case 'function-finished': dapReason = 'step'; break;
			case 'signal-received': dapReason = 'exception'; break;
			case 'exited':
			case 'exited-normally':
			case 'exited-signalled':
				this.sendEvent(new TerminatedEvent());
				return;
		}
		const stopped = new StoppedEvent(dapReason, DEFAULT_THREAD_ID);
		(stopped.body as DebugProtocol.StoppedEvent['body']).allThreadsStopped = true;
		this.sendEvent(stopped);
	}

	private sendOutput(text: string, category: 'console' | 'stdout' | 'stderr' = 'stdout'): void {
		this.sendEvent(new OutputEvent(text, category));
	}

	private async safe<T>(p: Promise<T>): Promise<T | undefined> {
		try {
			return await p;
		} catch (err) {
			if (this.trace) {
				this.sendOutput(`[gdb-warn] ${formatError(err)}\n`, 'console');
			}
			return undefined;
		}
	}

	public shutdown(): void {
		if (this.gdb) {
			try {
				this.gdb.send('gdb-exit').catch(() => undefined);
			} catch {
				// ignore
			}
			this.gdb.dispose();
			this.gdb = undefined;
		}
	}
}

function quote(s: string): string {
	return `"${s.replace(/\\/g, '\\\\').replace(/"/g, '\\"')}"`;
}

function formatError(err: unknown): string {
	if (err instanceof Error) {
		return err.message;
	}
	return String(err);
}

function mergeEnv(custom: Record<string, string> | undefined): NodeJS.ProcessEnv {
	if (!custom) {
		return process.env;
	}
	return { ...process.env, ...custom };
}

// Locate the first executable instruction of an IR file — the line right
// after the entry block header (`Block_0(...):`). This is what the user
// almost always wants `stopAtIrEntry` to halt on.
async function detectIrEntryLine(file: string): Promise<number> {
	try {
		const text = await fs.promises.readFile(file, 'utf8');
		const lines = text.split(/\r?\n/);
		for (let i = 0; i < lines.length; i++) {
			if (/^\s*Block_0\s*\(/.test(lines[i])) {
				// Scan forward for the first non-blank, non-header line.
				for (let j = i + 1; j < lines.length; j++) {
					if (lines[j].trim() !== '') {
						return j + 1; // 1-based line number for gdb
					}
				}
				return i + 1;
			}
		}
	} catch {
		// fall through
	}
	return 0;
}
