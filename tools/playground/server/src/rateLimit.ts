import { config } from './config.js';

const WINDOW_MS = 60_000;
const hits = new Map<string, number[]>();

/**
 * Sliding-window per-IP rate limit for compile submissions. Read endpoints
 * (job polling, examples) are not limited.
 */
export function allowRequest(ip: string): boolean {
	const now = Date.now();
	const windowStart = now - WINDOW_MS;
	const timestamps = (hits.get(ip) ?? []).filter((t) => t > windowStart);
	if (timestamps.length >= config.rateLimitPerMinute) {
		hits.set(ip, timestamps);
		return false;
	}
	timestamps.push(now);
	hits.set(ip, timestamps);
	return true;
}

// Periodically drop idle IPs so the map cannot grow without bound.
setInterval(() => {
	const windowStart = Date.now() - WINDOW_MS;
	for (const [ip, timestamps] of hits) {
		const alive = timestamps.filter((t) => t > windowStart);
		if (alive.length === 0) {
			hits.delete(ip);
		} else {
			hits.set(ip, alive);
		}
	}
}, WINDOW_MS).unref();
