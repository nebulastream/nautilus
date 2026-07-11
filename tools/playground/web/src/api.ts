// Client-side mirror of the server API types (tools/playground/server/src/types.ts).

export const BACKENDS = ['mlir', 'cpp', 'bc', 'tbc', 'asmjit', 'cuda', 'metal'] as const;
export type Backend = (typeof BACKENDS)[number];

export interface CompileOptions {
	enableLICM?: boolean;
	enableLocalCSE?: boolean;
	enableStrengthReduction?: boolean;
	maxPipelineIterations?: number;
}

export interface Stage {
	key: string;
	title: string;
	phase: string;
	lang: string;
	order: number;
	text: string;
	truncated: boolean;
}

export interface Diagnostics {
	phase: string | null;
	compilerStderr: string;
	linkStderr: string;
	runStderr: string;
	runStdout: string;
	pipelineError: string;
}

/** One CompilationStatistics entry recorded by the engine during the compile. */
export interface StatEntry {
	key: string;
	/** counter (integer) | duration (milliseconds) | text */
	type: 'counter' | 'duration' | 'text';
	value: number | string;
}

export interface CompileResult {
	status: 'done' | 'failed';
	backend: Backend;
	stages: Stage[];
	statistics?: StatEntry[];
	diagnostics: Diagnostics;
	timings: { queueMs: number; sandboxMs: number; totalMs: number };
}

export interface JobView {
	status: 'queued' | 'running' | 'done' | 'failed';
	queuePosition?: number;
	result?: CompileResult;
	error?: string;
}

export interface Example {
	id: string;
	title: string;
	description: string;
	source: string;
}

export async function fetchExamples(): Promise<Example[]> {
	const response = await fetch('/api/examples');
	if (!response.ok) {
		return [];
	}
	return (await response.json()) as Example[];
}

export class ApiError extends Error {}

/**
 * Submits a compile job and resolves with the terminal JobView. Progress
 * transitions (queued/running) are reported through `onProgress`. Prefers
 * SSE; falls back to 1s polling if the stream errors.
 */
export async function compile(
	source: string,
	backend: Backend,
	options: CompileOptions,
	onProgress: (view: JobView) => void,
): Promise<JobView> {
	const response = await fetch('/api/compile', {
		method: 'POST',
		headers: { 'Content-Type': 'application/json' },
		body: JSON.stringify({ source, backend, options }),
	});
	const body = (await response.json()) as { jobId?: string; cached?: boolean; result?: CompileResult; error?: string };
	if (!response.ok) {
		throw new ApiError(body.error ?? `request failed (${response.status})`);
	}
	if (body.cached && body.result) {
		return { status: body.result.status, result: body.result };
	}
	if (!body.jobId) {
		throw new ApiError('malformed server response');
	}
	return waitForJob(body.jobId, onProgress);
}

function waitForJob(jobId: string, onProgress: (view: JobView) => void): Promise<JobView> {
	return new Promise((resolve, reject) => {
		let settled = false;
		const settle = (view: JobView) => {
			if (!settled) {
				settled = true;
				resolve(view);
			}
		};

		const poll = async () => {
			try {
				while (!settled) {
					const response = await fetch(`/api/jobs/${jobId}`);
					if (!response.ok) {
						throw new ApiError('job expired');
					}
					const view = (await response.json()) as JobView;
					onProgress(view);
					if (view.status === 'done' || view.status === 'failed') {
						settle(view);
						return;
					}
					await new Promise((r) => setTimeout(r, 1000));
				}
			} catch (error) {
				if (!settled) {
					settled = true;
					reject(error);
				}
			}
		};

		const events = new EventSource(`/api/jobs/${jobId}/events`);
		events.onmessage = (event) => {
			const view = JSON.parse(event.data as string) as JobView;
			onProgress(view);
			if (view.status === 'done' || view.status === 'failed') {
				events.close();
				settle(view);
			}
		};
		events.onerror = () => {
			events.close();
			if (!settled) {
				void poll();
			}
		};
	});
}
