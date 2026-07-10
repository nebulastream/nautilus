export const BACKENDS = ['mlir', 'cpp', 'bc', 'tbc', 'asmjit'] as const;
export type Backend = (typeof BACKENDS)[number];

export interface CompileOptions {
	enableLICM?: boolean;
	enableLocalCSE?: boolean;
	enableStrengthReduction?: boolean;
	maxPipelineIterations?: number;
}

export interface CompileRequest {
	source: string;
	backend: Backend;
	options?: CompileOptions;
}

/** One pipeline-stage artifact, as delivered to the frontend. */
export interface Stage {
	key: string;
	title: string;
	/** frontend | pass | graph | backend | other */
	phase: string;
	/** Monaco language id the frontend renders this stage with. */
	lang: string;
	order: number;
	text: string;
	truncated: boolean;
}

export type FailurePhase = 'compile' | 'link' | 'trace' | 'pipeline' | 'timeout' | 'oom' | 'internal';

export interface Diagnostics {
	phase: FailurePhase | null;
	compilerStderr: string;
	linkStderr: string;
	runStderr: string;
	runStdout: string;
	/** Pipeline error text from the driver manifest, if any. */
	pipelineError: string;
}

export interface CompileResult {
	status: 'done' | 'failed';
	backend: Backend;
	stages: Stage[];
	diagnostics: Diagnostics;
	timings: {
		queueMs: number;
		sandboxMs: number;
		totalMs: number;
	};
}

export type JobStatus = 'queued' | 'running' | 'done' | 'failed';

export interface JobView {
	status: JobStatus;
	queuePosition?: number;
	result?: CompileResult;
	error?: string;
}

/** Raw manifest.json written by the runner's driver. */
export interface RunnerManifest {
	schema: number;
	backend: string;
	error?: string;
	stages: Array<{
		key: string;
		title: string;
		phase: string;
		ext: string;
		order: number;
		text: string;
		truncated: boolean;
	}>;
}
