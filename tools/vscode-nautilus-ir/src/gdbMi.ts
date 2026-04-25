// Minimal GDB/MI parser & driver.
//
// We only implement the subset needed for stepping through Nautilus IR files:
// - send commands with a token, wait for the matching `^done`/`^error`/`^running`
// - parse async records (`*stopped`, `=thread-group-*`, `~"..."` console output)
//
// This is intentionally compact; it is NOT a general-purpose MI parser.

import { ChildProcess, spawn } from 'child_process';
import { EventEmitter } from 'events';

export interface MiResult {
	cls: 'done' | 'running' | 'connected' | 'error' | 'exit';
	results: Record<string, MiValue>;
	raw: string;
}

export type MiValue = string | MiValue[] | { [key: string]: MiValue };

export interface MiAsync {
	cls: string; // e.g. "stopped", "thread-group-added"
	results: Record<string, MiValue>;
	raw: string;
}

interface PendingCommand {
	resolve: (r: MiResult) => void;
	reject: (err: Error) => void;
	command: string;
}

export class GdbMi extends EventEmitter {
	private proc: ChildProcess | undefined;
	private tokenCounter = 1;
	private pending = new Map<number, PendingCommand>();
	private buffer = '';
	private exited = false;

	constructor(private gdbPath: string, private extraArgs: string[]) {
		super();
	}

	start(cwd: string, env: NodeJS.ProcessEnv): Promise<void> {
		return new Promise((resolve, reject) => {
			const args = ['--interpreter=mi2', '--quiet', ...this.extraArgs];
			const proc = spawn(this.gdbPath, args, { cwd, env, stdio: ['pipe', 'pipe', 'pipe'] });
			this.proc = proc;
			proc.stdout.setEncoding('utf8');
			proc.stderr.setEncoding('utf8');
			proc.stdout.on('data', chunk => this.consume(chunk));
			proc.stderr.on('data', chunk => this.emit('stderr', chunk));
			proc.on('error', err => {
				this.exited = true;
				this.failAll(err);
				reject(err);
				this.emit('exit', -1);
			});
			proc.on('exit', code => {
				this.exited = true;
				this.failAll(new Error(`gdb exited with code ${code}`));
				this.emit('exit', code ?? 0);
			});
			// Wait for the initial "(gdb)" prompt before resolving.
			const onReady = () => {
				this.removeListener('ready', onReady);
				resolve();
			};
			this.once('ready', onReady);
			// Bound the wait so a missing gdb doesn't hang the session.
			setTimeout(() => {
				if (!this.exited && this.listenerCount('ready') > 0) {
					this.removeListener('ready', onReady);
					resolve(); // resolve anyway; later commands will fail clearly
				}
			}, 5_000);
		});
	}

	dispose(): void {
		if (this.proc && !this.exited) {
			try {
				this.proc.kill('SIGTERM');
			} catch {
				// ignore
			}
		}
	}

	send(command: string): Promise<MiResult> {
		if (!this.proc || this.exited) {
			return Promise.reject(new Error('gdb is not running'));
		}
		return new Promise((resolve, reject) => {
			const token = this.tokenCounter++;
			this.pending.set(token, { resolve, reject, command });
			this.proc!.stdin!.write(`${token}-${command}\n`);
		});
	}

	private consume(chunk: string): void {
		this.buffer += chunk;
		let idx: number;
		while ((idx = this.buffer.indexOf('\n')) >= 0) {
			const line = this.buffer.slice(0, idx).replace(/\r$/, '');
			this.buffer = this.buffer.slice(idx + 1);
			this.handleLine(line);
		}
	}

	private handleLine(line: string): void {
		if (line === '(gdb)' || line === '(gdb) ') {
			this.emit('ready');
			return;
		}
		// Console / log / target stream records.
		if (line.startsWith('~') || line.startsWith('@') || line.startsWith('&')) {
			const text = parseCString(line.slice(1));
			this.emit('console', text);
			return;
		}
		// Async record: *stopped, =thread-group-added, +download...
		if (line.startsWith('*') || line.startsWith('=') || line.startsWith('+')) {
			const sigil = line[0];
			const body = line.slice(1);
			const commaIdx = body.indexOf(',');
			const cls = commaIdx >= 0 ? body.slice(0, commaIdx) : body;
			const rest = commaIdx >= 0 ? body.slice(commaIdx + 1) : '';
			const results = rest ? parseResults(rest) : {};
			const evt: MiAsync = { cls, results, raw: line };
			if (sigil === '*') {
				this.emit('exec', evt);
			} else if (sigil === '=') {
				this.emit('notify', evt);
			} else {
				this.emit('status', evt);
			}
			return;
		}
		// Result record: TOKEN^class[,results]
		const resultMatch = /^(\d+)\^([a-z]+)(?:,(.*))?$/.exec(line);
		if (resultMatch) {
			const token = parseInt(resultMatch[1], 10);
			const cls = resultMatch[2] as MiResult['cls'];
			const results = resultMatch[3] ? parseResults(resultMatch[3]) : {};
			const pending = this.pending.get(token);
			this.pending.delete(token);
			if (pending) {
				if (cls === 'error') {
					const msg = typeof results.msg === 'string' ? results.msg : 'gdb error';
					pending.reject(new Error(`${pending.command}: ${msg}`));
				} else {
					pending.resolve({ cls, results, raw: line });
				}
			}
			return;
		}
		// Anything else: treat as raw target output.
		if (line.length > 0) {
			this.emit('output', line);
		}
	}

	private failAll(err: Error): void {
		for (const p of this.pending.values()) {
			p.reject(err);
		}
		this.pending.clear();
	}
}

// ----- Result parsing -----

function parseResults(input: string): Record<string, MiValue> {
	const reader = new Reader(input);
	const out: Record<string, MiValue> = {};
	while (!reader.eof()) {
		const key = reader.readIdentifier();
		reader.expect('=');
		out[key] = reader.readValue();
		if (reader.peek() === ',') {
			reader.advance();
		}
	}
	return out;
}

class Reader {
	private pos = 0;
	constructor(private text: string) {}

	eof(): boolean {
		return this.pos >= this.text.length;
	}

	peek(): string {
		return this.text[this.pos] ?? '';
	}

	advance(): string {
		return this.text[this.pos++] ?? '';
	}

	expect(ch: string): void {
		if (this.advance() !== ch) {
			throw new Error(`expected '${ch}' at ${this.pos}`);
		}
	}

	readIdentifier(): string {
		const start = this.pos;
		while (!this.eof() && /[A-Za-z0-9_-]/.test(this.peek())) {
			this.pos++;
		}
		return this.text.slice(start, this.pos);
	}

	readValue(): MiValue {
		const ch = this.peek();
		if (ch === '"') {
			return this.readString();
		}
		if (ch === '{') {
			return this.readTuple();
		}
		if (ch === '[') {
			return this.readList();
		}
		// Bareword fallback.
		return this.readIdentifier();
	}

	readString(): string {
		this.expect('"');
		let out = '';
		while (!this.eof()) {
			const c = this.advance();
			if (c === '"') {
				return out;
			}
			if (c === '\\') {
				const n = this.advance();
				switch (n) {
					case 'n': out += '\n'; break;
					case 't': out += '\t'; break;
					case 'r': out += '\r'; break;
					case '\\': out += '\\'; break;
					case '"': out += '"'; break;
					default: out += n;
				}
			} else {
				out += c;
			}
		}
		return out;
	}

	readTuple(): { [key: string]: MiValue } {
		this.expect('{');
		const out: { [key: string]: MiValue } = {};
		if (this.peek() === '}') {
			this.advance();
			return out;
		}
		while (true) {
			const key = this.readIdentifier();
			this.expect('=');
			out[key] = this.readValue();
			const sep = this.advance();
			if (sep === '}') {
				return out;
			}
			if (sep !== ',') {
				throw new Error(`expected ',' or '}' at ${this.pos}`);
			}
		}
	}

	readList(): MiValue[] {
		this.expect('[');
		const out: MiValue[] = [];
		if (this.peek() === ']') {
			this.advance();
			return out;
		}
		while (true) {
			// Lists may contain bare values OR key=value pairs (we drop the key).
			const beforeKey = this.pos;
			let value: MiValue;
			if (/[A-Za-z_]/.test(this.peek())) {
				this.readIdentifier();
				if (this.peek() === '=') {
					this.advance();
					value = this.readValue();
				} else {
					this.pos = beforeKey;
					value = this.readValue();
				}
			} else {
				value = this.readValue();
			}
			out.push(value);
			const sep = this.advance();
			if (sep === ']') {
				return out;
			}
			if (sep !== ',') {
				throw new Error(`expected ',' or ']' at ${this.pos}`);
			}
		}
	}
}

function parseCString(input: string): string {
	const reader = new Reader(input);
	if (reader.peek() === '"') {
		return reader.readString();
	}
	return input;
}
