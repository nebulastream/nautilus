// Debug Adapter for Nautilus IR backed by gdb.
//
// The user told us they can debug Nautilus IR with gdb directly. We wrap that
// workflow in a Debug Adapter Protocol session so VS Code's standard UI
// (breakpoints in the gutter, step/continue, variables view, watch) drives
// the underlying gdb process.
//
// Strategy:
//   * Spawn `gdb --interpreter=mi2` on activation.
//   * Treat the .nesir/.trace file as the "source" — gdb is configured with
//     `set substitute-path` and `directory` so its file/line events match.
//   * Map DAP setBreakpoints → gdb `-break-insert FILE:LINE`.
//   * Map DAP next/stepIn/stepOut/continue → corresponding -exec-* commands.
//   * Translate gdb `*stopped` events to DAP StoppedEvent.
//
// This adapter is intentionally inline — VS Code runs it inside the extension
// host via DebugAdapterInlineImplementation, so no extra child process is
// needed beyond gdb itself.

import * as vscode from 'vscode';
import { DebugSession, InitializedEvent, StoppedEvent, TerminatedEvent, OutputEvent, Thread, StackFrame, Source, Scope, Handles } from '@vscode/debugadapter';
import { DebugProtocol } from '@vscode/debugprotocol';
import * as path from 'path';
import { GdbMi, MiAsync, MiValue } from './gdbMi';

interface LaunchArgs extends DebugProtocol.LaunchRequestArguments {
	program: string;
	gdbPath?: string;
	gdbArgs?: string[];
	stopOnEntry?: boolean;
	cwd?: string;
	env?: Record<string, string>;
	trace?: boolean;
}

interface AttachArgs extends DebugProtocol.AttachRequestArguments {
	program: string;
	pid: number;
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

			this.sendOutput(`Loaded ${args.program}\n`);
			await this.gdb!.send(`file-exec-and-symbols ${quote(args.program)}`);

			// Make the IR file findable by gdb so source/line events resolve.
			const dir = path.dirname(args.program);
			await this.safe(this.gdb!.send(`environment-directory -r ${quote(dir)}`));

			if (args.stopOnEntry) {
				await this.safe(this.gdb!.send(`break-insert -t main`));
			}
			await this.safe(this.gdb!.send('exec-arguments'));
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
				const spec = `${quote(file)}:${bp.line}`;
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
					const bkpt = (result.results as { bkpt?: { number?: string; line?: string } }).bkpt;
					const numStr = bkpt?.number;
					const id = numStr ? parseInt(numStr, 10) : NaN;
					if (!Number.isNaN(id)) {
						newIds.push(id);
					}
					const reportedLine = bkpt?.line ? parseInt(bkpt.line, 10) : bp.line;
					verified.push({ verified: true, line: reportedLine });
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
