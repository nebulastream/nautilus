import { test } from 'node:test';
import assert from 'node:assert/strict';

import { parseNes, NES_PHASE_ORDER } from '../assets/nes-parser.js';

// Names exactly as .github/nebulastream/nes-bench.py `report` writes them.
const cases = [
	['mlir query nexmark q8 tumbling join: compilation', 'query_compilation', 'mlir', 'nexmark q8 tumbling join'],
	['tbc-jit query nexmark q8 tumbling join: execution', 'query_execution', 'tbc-jit', 'nexmark q8 tumbling join'],
	['tiered-asmjit-mlir query lrb sliding group avg: execution', 'query_execution', 'tiered-asmjit-mlir', 'lrb sliding group avg'],
	['cpp suite compilation: total', 'suite_compilation', 'cpp', 'total'],
	['asmjit suite compilation: median per pipeline', 'suite_compilation', 'asmjit', 'median per pipeline'],
	['mlir suite compilation phase: tracing.ms', 'suite_compilation', 'mlir', 'phase tracing.ms'],
	['mlir suite compilation phase: irPasses.totalMs', 'suite_compilation', 'mlir', 'phase irPasses.totalMs'],
	['tbc suite execution: geometric mean per query', 'suite_execution', 'tbc', 'geometric mean per query'],
	['tbc suite execution: operator/ geometric mean', 'suite_execution', 'tbc', 'operator/ geometric mean'],
];

for (const [name, phase, configuration, workload] of cases) {
	test(`parses ${name}`, () => {
		const p = parseNes(name);
		assert.equal(p.phase, phase);
		assert.equal(p.seriesKey, configuration);
		assert.equal(p.backend, configuration);
		assert.equal(p.workload, workload);
		assert.ok(NES_PHASE_ORDER.includes(p.phase));
	});
}

test('counts such as generated code size are not charted', () => {
	assert.equal(parseNes('mlir suite generated code: ir.operations').phase, 'other');
	assert.ok(!NES_PHASE_ORDER.includes('other'));
});

test('a query label may contain colons-free text with slashes and digits', () => {
	const p = parseNes('tbc-jit query a/b 2: compilation');
	assert.equal(p.workload, 'a/b 2');
	assert.equal(p.seriesKey, 'tbc-jit');
});
