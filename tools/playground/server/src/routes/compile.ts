import type { FastifyInstance } from 'fastify';
import { z } from 'zod';
import { config } from '../config.js';
import { BACKENDS } from '../types.js';
import { allowRequest } from '../rateLimit.js';
import { cachedResult, cacheKeyFor, getJob, QueueFullError, submitJob, subscribe } from '../jobs/queue.js';
import type { Example } from '../examples/index.js';

const compileSchema = z.object({
	source: z.string().min(1),
	backend: z.enum(BACKENDS),
	options: z
		.object({
			enableLICM: z.boolean().optional(),
			enableLocalCSE: z.boolean().optional(),
			enableStrengthReduction: z.boolean().optional(),
			maxPipelineIterations: z.number().int().min(1).max(8).optional(),
		})
		.optional(),
});

export function registerRoutes(app: FastifyInstance, examples: Example[]): void {
	app.post('/api/compile', async (request, reply) => {
		const parsed = compileSchema.safeParse(request.body);
		if (!parsed.success) {
			return reply.code(400).send({ error: parsed.error.issues[0]?.message ?? 'invalid request' });
		}
		const { source, backend } = parsed.data;
		const options = parsed.data.options ?? {};

		if (Buffer.byteLength(source, 'utf8') > config.sourceMaxBytes) {
			return reply.code(413).send({ error: `source exceeds ${config.sourceMaxBytes} bytes` });
		}

		// Cache hits are free: don't spend the rate budget or a queue slot.
		const cached = cachedResult(cacheKeyFor(source, backend, options));
		if (cached) {
			const job = submitJobFromCache();
			return reply.code(200).send({ jobId: job, cached: true, result: cached });
		}

		if (!allowRequest(request.ip)) {
			return reply.code(429).send({ error: 'rate limit exceeded, slow down' });
		}

		try {
			const job = submitJob(source, backend, options);
			return await reply.code(202).send({ jobId: job.id });
		} catch (error) {
			if (error instanceof QueueFullError) {
				return reply.code(429).send({ error: error.message });
			}
			throw error;
		}
	});

	app.get<{ Params: { id: string } }>('/api/jobs/:id', async (request, reply) => {
		const view = getJob(request.params.id);
		if (!view) {
			return reply.code(404).send({ error: 'unknown or expired job' });
		}
		return reply.send(view);
	});

	// Server-sent events: one status snapshot immediately, then one per
	// transition; closes after the terminal event.
	app.get<{ Params: { id: string } }>('/api/jobs/:id/events', (request, reply) => {
		const initial = getJob(request.params.id);
		if (!initial) {
			reply.code(404).send({ error: 'unknown or expired job' });
			return;
		}
		reply.raw.writeHead(200, {
			'Content-Type': 'text/event-stream',
			'Cache-Control': 'no-cache',
			Connection: 'keep-alive',
		});
		const send = (view: ReturnType<typeof getJob>) => {
			reply.raw.write(`data: ${JSON.stringify(view)}\n\n`);
			if (view && (view.status === 'done' || view.status === 'failed')) {
				unsubscribe?.();
				reply.raw.end();
			}
		};
		const unsubscribe = subscribe(request.params.id, send);
		request.raw.on('close', () => unsubscribe?.());
		send(initial);
	});

	// Served from memory — the gallery is loaded once at startup.
	app.get('/api/examples', async () => examples);

	app.get('/api/meta', async () => ({
		backends: BACKENDS,
		passOptions: ['enableLICM', 'enableLocalCSE', 'enableStrengthReduction'],
		limits: {
			sourceMaxBytes: config.sourceMaxBytes,
			maxPipelineIterations: 8,
		},
	}));
}

/**
 * Cache hits return the result inline; the fake job id keeps the response
 * shape uniform for clients that always follow up with a job lookup.
 */
function submitJobFromCache(): string {
	return 'cached';
}
