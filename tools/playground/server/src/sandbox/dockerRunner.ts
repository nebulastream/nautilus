import { spawn } from 'node:child_process';
import { mkdir, rm, writeFile } from 'node:fs/promises';
import path from 'node:path';
import { config } from '../config.js';
import type { CompileOptions, Backend } from '../types.js';

export interface SandboxOutcome {
	/** Exit code of the container (null if it was hard-killed). */
	exitCode: number | null;
	timedOut: boolean;
	/** Directory containing the runner's /out artifacts. */
	outDir: string;
	/** Call after the result has been assembled to remove the job dir. */
	cleanup: () => Promise<void>;
}

/**
 * Runs one compile job in an ephemeral, fully isolated runner container.
 * Both the clang compile of the user source and the tracing execution are
 * adversarial, so everything happens behind: no network, read-only rootfs,
 * tmpfs workdir, memory/cpu/pids caps, no capabilities, non-root user, and a
 * hard wall-clock kill on top of the in-container timeouts.
 */
export async function runInSandbox(jobId: string, source: string, backend: Backend, options: CompileOptions): Promise<SandboxOutcome> {
	const jobDir = path.join(config.jobRoot, jobId);
	const inDir = path.join(jobDir, 'in');
	const outDir = path.join(jobDir, 'out');
	await mkdir(inDir, { recursive: true });
	await mkdir(outDir, { recursive: true });
	await writeFile(path.join(inDir, 'user_module.cpp'), source, 'utf8');

	const containerName = `pg-${jobId}`;
	const args = [
		'run',
		'--rm',
		'--name', containerName,
		'--network=none',
		'--read-only',
		// Explicit mode: with the containerd image store (Docker Desktop default)
		// the tmpfs does not inherit /work's image ownership and comes up
		// root:0755, unwritable for the non-root runner user.
		'--tmpfs', '/work:rw,exec,size=512m,mode=1777',
		'-v', `${inDir}:/in:ro`,
		'-v', `${outDir}:/out:rw`,
		'--memory=2g',
		'--memory-swap=2g',
		'--cpus=2',
		'--pids-limit=128',
		'--cap-drop=ALL',
		'--security-opt=no-new-privileges',
		'--ulimit', 'fsize=268435456',
		'--ulimit', 'core=0',
		'--ulimit', 'nofile=256',
		'-e', `PG_BACKEND=${backend}`,
	];
	if (options.enableLICM) {
		args.push('-e', 'PG_ENABLE_LICM=1');
	}
	if (options.enableLocalCSE) {
		args.push('-e', 'PG_ENABLE_LOCAL_CSE=1');
	}
	if (options.enableStrengthReduction) {
		args.push('-e', 'PG_ENABLE_STRENGTH_REDUCTION=1');
	}
	if (options.maxPipelineIterations !== undefined) {
		args.push('-e', `PG_MAX_ITERATIONS=${options.maxPipelineIterations}`);
	}
	args.push(config.runnerImage);

	const outcome = await new Promise<{ exitCode: number | null; timedOut: boolean }>((resolve) => {
		const child = spawn(config.sandboxCmd, args, { stdio: 'ignore' });
		let timedOut = false;
		const killTimer = setTimeout(() => {
			timedOut = true;
			// `--rm` removes the container once the kill lands.
			spawn(config.sandboxCmd, ['kill', containerName], { stdio: 'ignore' });
			child.kill('SIGKILL');
		}, config.containerTimeoutMs);
		child.on('error', () => {
			clearTimeout(killTimer);
			resolve({ exitCode: null, timedOut });
		});
		child.on('close', (code) => {
			clearTimeout(killTimer);
			resolve({ exitCode: code, timedOut });
		});
	});

	return {
		...outcome,
		outDir,
		cleanup: async () => {
			await rm(jobDir, { recursive: true, force: true });
		},
	};
}
