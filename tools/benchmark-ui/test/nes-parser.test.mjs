import { test } from 'node:test';
import assert from 'node:assert/strict';

import { parseNes, NES_PHASE_ORDER, splitExtra, passStage, PASS_STAGES } from '../assets/nes-parser.js';

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

test('splitExtra separates the detail from the per-pass breakdown', () => {
	const { detail, passes } = splitExtra(
		'benchmark_small/Nexmark:1, 1 pipelines, 5 rounds\npasses tracing=4.12 irPasses.noThrowInference=0.00987 jit.compile=29.2 other=1.21',
	);
	assert.equal(detail, 'benchmark_small/Nexmark:1, 1 pipelines, 5 rounds');
	assert.deepEqual(passes, [
		{ name: 'tracing', ms: 4.12 },
		{ name: 'irPasses.noThrowInference', ms: 0.00987 },
		{ name: 'jit.compile', ms: 29.2 },
		{ name: 'other', ms: 1.21 },
	]);
});

test('splitExtra without a breakdown keeps the detail', () => {
	assert.deepEqual(splitExtra('Nexmark:1, 1 pipelines, 5 rounds'), { detail: 'Nexmark:1, 1 pipelines, 5 rounds', passes: null });
	assert.deepEqual(splitExtra(undefined), { detail: '', passes: null });
});

test('every pass nes-bench.py reports falls into its stage', () => {
	const stages = {
		tracing: 'tracing',
		ssaCreation: 'ssa',
		irGeneration: 'ir',
		'irPasses.exceptionRegionPreparation': 'irPasses',
		'mlir.loweringFromIR': 'lowering',
		'cpp.loweringFromIR': 'lowering',
		'mlir.pipeline': 'optimization',
		'llvm.optimizerBuild': 'optimization',
		'jit.compile': 'codegen',
		'asmjit.compile': 'codegen',
		'cpp.compile': 'codegen',
		backend: 'codegen',
		other: 'other',
	};
	for (const [name, stage] of Object.entries(stages)) {
		assert.equal(passStage(name), stage, name);
	}
	assert.equal(PASS_STAGES.length, 8);
});
