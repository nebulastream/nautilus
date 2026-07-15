import { createHash, randomUUID } from 'node:crypto';
import { EventEmitter } from 'node:events';
import { config } from '../config.js';
import { runInSandbox } from '../sandbox/dockerRunner.js';
import { assembleResult } from '../result/assemble.js';
import type { Backend, CompileOptions, CompileResult, JobStatus, JobView } from '../types.js';

interface Job {
	id: string;
	status: JobStatus;
	backend: Backend;
	options: CompileOptions;
	source: string;
	cacheKey: string;
	createdAt: number;
	startedAt?: number;
	result?: CompileResult;
	error?: string;
	events: EventEmitter;
}

/** Insertion-ordered Map used as an LRU: re-insert on hit, evict oldest. */
const resultCache = new Map<string, CompileResult>();
const jobs = new Map<string, Job>();
const pending: Job[] = [];
let runningCount = 0;

export function cacheKeyFor(source: string, backend: Backend, options: CompileOptions): string {
	return createHash('sha256').update(JSON.stringify({ source, backend, options })).digest('hex');
}

export function cachedResult(cacheKey: string): CompileResult | undefined {
	const hit = resultCache.get(cacheKey);
	if (hit) {
		resultCache.delete(cacheKey);
		resultCache.set(cacheKey, hit);
	}
	return hit;
}

export function queueDepth(): number {
	return pending.length;
}

export class QueueFullError extends Error {}

export function submitJob(source: string, backend: Backend, options: CompileOptions): Job {
	if (pending.length >= config.queueLimit) {
		throw new QueueFullError('compile queue is full, try again shortly');
	}
	const job: Job = {
		id: randomUUID(),
		status: 'queued',
		backend,
		options,
		source,
		cacheKey: cacheKeyFor(source, backend, options),
		createdAt: Date.now(),
		events: new EventEmitter(),
	};
	jobs.set(job.id, job);
	pending.push(job);
	setTimeout(() => jobs.delete(job.id), config.jobTtlMs).unref();
	void pump();
	return job;
}

export function getJob(id: string): JobView | undefined {
	const job = jobs.get(id);
	if (!job) {
		return undefined;
	}
	return viewOf(job);
}

export function subscribe(id: string, listener: (view: JobView) => void): (() => void) | undefined {
	const job = jobs.get(id);
	if (!job) {
		return undefined;
	}
	const handler = () => listener(viewOf(job));
	job.events.on('update', handler);
	return () => job.events.off('update', handler);
}

function viewOf(job: Job): JobView {
	const view: JobView = { status: job.status };
	if (job.status === 'queued') {
		const position = pending.indexOf(job);
		view.queuePosition = position >= 0 ? position + 1 : undefined;
	}
	if (job.result) {
		view.result = job.result;
	}
	if (job.error) {
		view.error = job.error;
	}
	return view;
}

function transition(job: Job, status: JobStatus): void {
	job.status = status;
	job.events.emit('update');
}

async function pump(): Promise<void> {
	while (runningCount < config.maxConcurrency && pending.length > 0) {
		const job = pending.shift();
		if (!job) {
			return;
		}
		runningCount += 1;
		void execute(job).finally(() => {
			runningCount -= 1;
			void pump();
		});
	}
}

async function execute(job: Job): Promise<void> {
	// An identical job may have completed while this one sat in the queue.
	const cached = cachedResult(job.cacheKey);
	if (cached) {
		job.result = cached;
		transition(job, cached.status);
		return;
	}

	job.startedAt = Date.now();
	transition(job, 'running');
	const queueMs = job.startedAt - job.createdAt;

	try {
		const outcome = await runInSandbox(job.id, job.source, job.backend, job.options);
		const sandboxMs = Date.now() - job.startedAt;
		job.result = await assembleResult(outcome, job.backend, queueMs, sandboxMs);
		await outcome.cleanup();

		resultCache.set(job.cacheKey, job.result);
		while (resultCache.size > config.cacheMaxEntries) {
			const oldest = resultCache.keys().next().value;
			if (oldest === undefined) {
				break;
			}
			resultCache.delete(oldest);
		}
		transition(job, job.result.status);
	} catch (error) {
		job.error = error instanceof Error ? error.message : String(error);
		transition(job, 'failed');
	}
}
