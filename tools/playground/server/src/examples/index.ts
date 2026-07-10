import { readdir, readFile } from 'node:fs/promises';
import path from 'node:path';
import { config } from '../config.js';

export interface Example {
	id: string;
	title: string;
	description: string;
	source: string;
}

let cache: Example[] | null = null;

/**
 * Loads the curated gallery from `examples/*.cpp`. Each file starts with
 * `// title: ...` and `// description: ...` header comments, which are
 * stripped from the source shown in the editor.
 */
export async function loadExamples(): Promise<Example[]> {
	if (cache) {
		return cache;
	}
	const examples: Example[] = [];
	let files: string[] = [];
	try {
		files = (await readdir(config.examplesDir)).filter((f) => f.endsWith('.cpp')).sort();
	} catch {
		return [];
	}
	for (const file of files) {
		const raw = await readFile(path.join(config.examplesDir, file), 'utf8');
		const lines = raw.split('\n');
		let title = path.basename(file, '.cpp');
		let description = '';
		let bodyStart = 0;
		for (const [i, line] of lines.entries()) {
			const titleMatch = line.match(/^\/\/ title:\s*(.*)$/);
			const descMatch = line.match(/^\/\/ description:\s*(.*)$/);
			if (titleMatch) {
				title = titleMatch[1];
				bodyStart = i + 1;
			} else if (descMatch) {
				description = descMatch[1];
				bodyStart = i + 1;
			} else {
				break;
			}
		}
		examples.push({
			id: path.basename(file, '.cpp'),
			title,
			description,
			source: lines.slice(bodyStart).join('\n').replace(/^\n+/, ''),
		});
	}
	cache = examples;
	return examples;
}
