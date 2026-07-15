import { test } from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

import { parse, PHASE_ORDER } from '../assets/parser.js';
import { toNs, pickDisplayUnit, formatNs, formatPercent } from '../assets/format.js';

const here = dirname(fileURLToPath(import.meta.url));
const fixtureNames = readFileSync(join(here, 'names-fixture.txt'), 'utf8').split('\n').filter(Boolean);

test('all published benchmark names parse into a known phase', () => {
	assert.ok(fixtureNames.length > 150, `fixture looks truncated: ${fixtureNames.length} names`);
	for (const name of fixtureNames) {
		const p = parse(name);
		assert.notEqual(p.phase, 'other', `unparsed name: ${name}`);
		assert.ok(PHASE_ORDER.includes(p.phase), `unknown phase for ${name}`);
		assert.ok(p.workload.length > 0, `empty workload for ${name}`);
	}
});

const cases = [
	['trace_fibonacci', { phase: 'trace', backend: null, workload: 'fibonacci', variant: 'exception-based tracing' }],
	['completing_trace_gcd', { phase: 'trace', backend: null, workload: 'gcd', variant: 'lazy tracing' }],
	['ssa_deeplyNestedIfElse', { phase: 'ssa', backend: null, workload: 'deeplyNestedIfElse', variant: 'default' }],
	['ir_chainedIf100', { phase: 'ir', backend: null, workload: 'chainedIf100', variant: 'default' }],
	['comp_mlir_staticLoop', { phase: 'comp', backend: 'mlir', workload: 'staticLoop', variant: 'default' }],
	['comp_asmjit_add', { phase: 'comp', backend: 'asmjit', workload: 'add', variant: 'default' }],
	['exec_mlir_sum', { phase: 'exec', backend: 'mlir', workload: 'sum', variant: 'default' }],
	['exec_bc_add_threaded_noReuse', { phase: 'exec', backend: 'bc', workload: 'add', variant: 'threaded_noReuse' }],
	['exec_bc_sum_passesOn', { phase: 'exec', backend: 'bc', workload: 'sum', variant: 'passesOn' }],
	['exec_bc_fibonacci_switch', { phase: 'exec', backend: 'bc', workload: 'fibonacci', variant: 'switch' }],
	['exec_interpreted_addOne', { phase: 'exec', backend: 'interpreted', workload: 'addOne', variant: 'default' }],
	['exec_tbc_sum_passesOff', { phase: 'exec', backend: 'tbc', workload: 'sum', variant: 'passesOff' }],
	// Present in benchmark sources but not yet in published data (capable-runner only):
	['exec_tbc_internalCall_jit', { phase: 'exec', backend: 'tbc', workload: 'internalCall', variant: 'jit' }],
	['exec_asmjit_add_allOpts', { phase: 'exec', backend: 'asmjit', workload: 'add', variant: 'allOpts' }],
	['exec_bc_add_threaded_allOpts', { phase: 'exec', backend: 'bc', workload: 'add', variant: 'threaded_allOpts' }],
	['single_compile_mlir_sumLoop', { phase: 'single_compile', backend: 'mlir', workload: 'sumLoop', variant: 'default' }],
	['tiered_twotier_addOne', { phase: 'tiered', backend: 'twotier', workload: 'addOne', variant: 'default' }],
	['tiered_singletier_sumLoop', { phase: 'tiered', backend: 'singletier', workload: 'sumLoop', variant: 'default' }],
	['e2e_tiered_bc_to_mlir', { phase: 'e2e', backend: null, workload: 'tiered_bc_to_mlir', variant: 'default' }],
	['e2e_single_mlir', { phase: 'e2e', backend: null, workload: 'single_mlir', variant: 'default' }],
];

for (const [name, expected] of cases) {
	test(`parse(${name})`, () => {
		const p = parse(name);
		for (const [k, v] of Object.entries(expected)) {
			assert.equal(p[k], v, `${name}.${k}`);
		}
	});
}

test('series keys separate variations within a workload', () => {
	assert.equal(parse('trace_add').seriesKey, 'exception-based tracing');
	assert.equal(parse('completing_trace_add').seriesKey, 'lazy tracing');
	assert.equal(parse('comp_mlir_add').seriesKey, 'mlir');
	assert.equal(parse('exec_bc_add_threaded_reuse').seriesKey, 'bc / threaded_reuse');
	assert.equal(parse('ir_add').seriesKey, 'value');
});

test('unknown names degrade to phase "other" without throwing', () => {
	for (const junk of ['foo_bar', 'exec_unknownBackend_add', '', 'comp']) {
		const p = parse(junk);
		assert.equal(p.phase, 'other');
	}
});

test('unit conversion and display units', () => {
	assert.equal(toNs(544.697, 'ns'), 544.697);
	assert.equal(toNs(1.17054, 'us'), 1170.54);
	assert.equal(toNs(2.5, 'ms'), 2.5e6);
	assert.equal(toNs(1, 'bogus'), null);
	assert.equal(pickDisplayUnit(500).label, 'ns');
	assert.equal(pickDisplayUnit(1170.54).label, 'µs');
	assert.equal(pickDisplayUnit(2.5e6).label, 'ms');
	assert.equal(formatNs(1170.54), '1.171 µs');
	assert.equal(formatPercent(4.267), '+4.3%');
	assert.equal(formatPercent(-12.5), '-12.5%');
	assert.equal(formatNs(null), '—');
});
