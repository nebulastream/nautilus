import os from 'node:os';
import path from 'node:path';

function intEnv(name: string, fallback: number): number {
	const raw = process.env[name];
	const value = raw === undefined ? NaN : Number.parseInt(raw, 10);
	return Number.isFinite(value) ? value : fallback;
}

export const config = {
	port: intEnv('PORT', 8080),
	host: process.env.HOST ?? '0.0.0.0',
	/** Origin allowed by CORS; same-origin serving makes this mostly moot. */
	corsOrigin: process.env.CORS_ORIGIN ?? true,

	/**
	 * Command used to launch the sandbox. `docker` by default; set to
	 * `podman` (rootless) or a wrapper script that adds --runtime=runsc
	 * (gVisor) for hardened deployments.
	 */
	sandboxCmd: process.env.SANDBOX_CMD ?? 'docker',
	runnerImage: process.env.RUNNER_IMAGE ?? 'nautilus-playground-runner',
	/** Hard wall-clock kill for the whole container. */
	containerTimeoutMs: intEnv('CONTAINER_TIMEOUT_MS', 120_000),
	jobRoot: process.env.JOB_ROOT ?? path.join(os.tmpdir(), 'nautilus-playground-jobs'),

	maxConcurrency: intEnv('MAX_CONCURRENCY', Math.max(1, Math.min(2, Math.floor(os.cpus().length / 2)))),
	queueLimit: intEnv('QUEUE_LIMIT', 20),
	sourceMaxBytes: intEnv('SOURCE_MAX_BYTES', 131_072),
	rateLimitPerMinute: intEnv('RATE_LIMIT_PER_MINUTE', 10),
	jobTtlMs: intEnv('JOB_TTL_MS', 15 * 60_000),
	cacheMaxEntries: intEnv('CACHE_MAX_ENTRIES', 200),

	/** Directory of curated example sources served by /api/examples. */
	examplesDir: process.env.EXAMPLES_DIR ?? path.join(import.meta.dirname, 'examples'),
	/** Built web app; served statically when present. */
	webDist: process.env.WEB_DIST ?? path.join(import.meta.dirname, '..', '..', 'web', 'dist'),
};
