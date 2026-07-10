import { readFile } from 'node:fs/promises';
import path from 'node:path';
import type { Backend, CompileResult, Diagnostics, FailurePhase, RunnerManifest, Stage } from '../types.js';
import type { SandboxOutcome } from '../sandbox/dockerRunner.js';

/** Maps a stage to the Monaco language id the frontend highlights it with. */
function langFor(key: string, ext: string): string {
	if (ext === 'trace') {
		return 'nautilus-trace';
	}
	if (ext === 'nautilus' || ext === 'ir') {
		return 'nautilus-ir';
	}
	if (ext === 'mermaid') {
		return 'mermaid';
	}
	if (ext === 'dot') {
		return 'graphviz';
	}
	if (ext === 'mlir') {
		return 'mlir';
	}
	if (ext === 'll') {
		return 'llvm-ir';
	}
	if (ext === 'c' || key === 'after_c_generation' || key === 'after_cpp_generation') {
		return 'cpp';
	}
	if (ext === 'asm') {
		return 'asm';
	}
	return 'plaintext';
}

async function readIfExists(file: string, maxBytes = 128 * 1024): Promise<string> {
	try {
		const text = await readFile(file, 'utf8');
		return text.length > maxBytes ? `${text.slice(0, maxBytes)}\n… (truncated)` : text;
	} catch {
		return '';
	}
}

function failurePhaseFor(outcome: SandboxOutcome): FailurePhase | null {
	if (outcome.timedOut) {
		return 'timeout';
	}
	switch (outcome.exitCode) {
		case 0:
			return null;
		case 10:
			return 'compile';
		case 11:
			return 'link';
		case 12:
			return 'trace';
		case 13:
			return 'pipeline';
		case 137: // SIGKILL: cgroup OOM kill or the orchestrator's hard kill
			return 'oom';
		default:
			return 'internal';
	}
}

/**
 * Merges the runner's /out directory (manifest.json + captured stderr/stdout
 * files) and the sandbox exit status into the API result. A `pipeline`
 * failure (driver exit 13) still carries the stages dumped before the
 * failure, so partial results stay inspectable in the UI.
 */
export async function assembleResult(outcome: SandboxOutcome, backend: Backend, queueMs: number, sandboxMs: number): Promise<CompileResult> {
	const phase = failurePhaseFor(outcome);

	let manifest: RunnerManifest | null = null;
	try {
		manifest = JSON.parse(await readFile(path.join(outcome.outDir, 'manifest.json'), 'utf8')) as RunnerManifest;
	} catch {
		manifest = null;
	}

	const stages: Stage[] = (manifest?.stages ?? []).map((stage) => ({
		key: stage.key,
		title: stage.title,
		phase: stage.phase,
		lang: langFor(stage.key, stage.ext),
		order: stage.order,
		text: stage.text,
		truncated: stage.truncated,
	}));

	const diagnostics: Diagnostics = {
		phase,
		compilerStderr: await readIfExists(path.join(outcome.outDir, 'compile_stderr.txt')),
		linkStderr: await readIfExists(path.join(outcome.outDir, 'link_stderr.txt')),
		runStderr: await readIfExists(path.join(outcome.outDir, 'run_stderr.txt')),
		runStdout: await readIfExists(path.join(outcome.outDir, 'run_stdout.txt')),
		pipelineError: manifest?.error ?? '',
	};

	return {
		status: phase === null ? 'done' : 'failed',
		backend,
		stages,
		diagnostics,
		timings: {
			queueMs: Math.round(queueMs),
			sandboxMs: Math.round(sandboxMs),
			totalMs: Math.round(queueMs + sandboxMs),
		},
	};
}
