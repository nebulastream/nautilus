import Fastify from 'fastify';
import cors from '@fastify/cors';
import fastifyStatic from '@fastify/static';
import { existsSync } from 'node:fs';
import { mkdir } from 'node:fs/promises';
import { config } from './config.js';
import { registerRoutes } from './routes/compile.js';

async function main(): Promise<void> {
	await mkdir(config.jobRoot, { recursive: true });

	const app = Fastify({
		logger: true,
		bodyLimit: config.sourceMaxBytes + 4096,
		// Respect X-Forwarded-For only when deployed behind the reverse proxy.
		trustProxy: process.env.TRUST_PROXY === '1',
	});

	await app.register(cors, { origin: config.corsOrigin });

	if (existsSync(config.webDist)) {
		await app.register(fastifyStatic, { root: config.webDist });
		app.setNotFoundHandler((request, reply) => {
			// SPA fallback for non-API routes.
			if (!request.url.startsWith('/api/')) {
				return reply.sendFile('index.html');
			}
			return reply.code(404).send({ error: 'not found' });
		});
	}

	registerRoutes(app);

	await app.listen({ port: config.port, host: config.host });
}

main().catch((error) => {
	console.error(error);
	process.exit(1);
});
