export const BACKENDS = ['mlir', 'cpp', 'bc', 'tbc', 'asmjit', 'cuda', 'metal'] as const;
export type Backend = (typeof BACKENDS)[number];
export const MLIR_DEBUG_SOURCE_MODES = ['mlir', 'nautilus-ir'] as const;
export type MlirDebugSourceMode = (typeof MLIR_DEBUG_SOURCE_MODES)[number];

export interface CompileOptions {
	enableLICM?: boolean;
	enableLocalCSE?: boolean;
	enableStrengthReduction?: boolean;
	enableDwarf?: boolean;
	mlirDebugSourceMode?: MlirDebugSourceMode;
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

/** One CompilationStatistics entry, as emitted by the runner's driver. */
export interface StatEntry {
	key: string;
	/** counter (integer) | duration (milliseconds) | text */
	type: 'counter' | 'duration' | 'text';
	value: number | string;
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
	statistics: StatEntry[];
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
	statistics?: StatEntry[];
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
