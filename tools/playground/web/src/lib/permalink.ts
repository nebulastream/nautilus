// Shareable permalinks: the full editor state (source, backend, pass
// options) is LZ-compressed into the URL fragment as `#code=...`. The
// fragment never reaches the server, so shared snippets stay client-side
// until the recipient compiles.

import { compressToEncodedURIComponent, decompressFromEncodedURIComponent } from 'lz-string';
import { BACKENDS, type Backend, type CompileOptions } from '../api';

export interface ShareState {
	source: string;
	backend: Backend;
	options: CompileOptions;
}

export function encodeShareHash(state: ShareState): string {
	return `#code=${compressToEncodedURIComponent(JSON.stringify(state))}`;
}

export function decodeShareHash(hash: string): ShareState | null {
	const match = hash.match(/#code=([A-Za-z0-9+\-$_]+)/);
	if (!match) {
		return null;
	}
	try {
		const json = decompressFromEncodedURIComponent(match[1]);
		if (!json) {
			return null;
		}
		const parsed = JSON.parse(json) as Partial<ShareState>;
		if (typeof parsed.source !== 'string' || parsed.source.length === 0) {
			return null;
		}
		return {
			source: parsed.source,
			backend: BACKENDS.includes(parsed.backend as Backend) ? (parsed.backend as Backend) : 'mlir',
			options: typeof parsed.options === 'object' && parsed.options !== null ? parsed.options : {},
		};
	} catch {
		return null;
	}
}
