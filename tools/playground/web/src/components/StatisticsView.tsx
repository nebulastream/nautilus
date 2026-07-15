import { useMemo } from 'react';
import type { StatEntry } from '../api';

// Value rendering mirrors CompilationStatistics::formatValue in the engine:
// durations as "X.XX ms", `.bytes` counters in B/KiB/MiB/GiB, plain counters
// with thousands separators, text verbatim.

function isByteKey(key: string): boolean {
	return key.endsWith('.bytes');
}

function formatBytes(bytes: number): string {
	const KiB = 1024;
	const MiB = 1024 * 1024;
	const GiB = 1024 * 1024 * 1024;
	const abs = Math.abs(bytes);
	if (abs >= GiB) {
		return `${(bytes / GiB).toFixed(2)} GiB`;
	}
	if (abs >= MiB) {
		return `${(bytes / MiB).toFixed(2)} MiB`;
	}
	if (abs >= KiB) {
		return `${(bytes / KiB).toFixed(2)} KiB`;
	}
	return `${bytes.toLocaleString('en-US')} B`;
}

function formatValue(entry: StatEntry): string {
	if (entry.type === 'duration' && typeof entry.value === 'number') {
		return `${entry.value.toFixed(2)} ms`;
	}
	if (entry.type === 'counter' && typeof entry.value === 'number') {
		return isByteKey(entry.key) ? formatBytes(entry.value) : entry.value.toLocaleString('en-US');
	}
	return String(entry.value);
}

interface StatGroup {
	name: string;
	entries: Array<StatEntry & { subkey: string }>;
}

/**
 * Grouped read-only table of the engine's compilation statistics. Groups are
 * the key prefix before the first dot (matching formatReport's layout), in
 * insertion order; `total*` subkeys sort first within a group.
 */
export function StatisticsView({ statistics }: { statistics: StatEntry[] }) {
	const groups = useMemo(() => {
		const byName = new Map<string, StatGroup>();
		const ordered: StatGroup[] = [];
		for (const entry of statistics) {
			const dot = entry.key.indexOf('.');
			const name = dot === -1 ? 'general' : entry.key.slice(0, dot);
			const subkey = dot === -1 ? entry.key : entry.key.slice(dot + 1);
			let group = byName.get(name);
			if (!group) {
				group = { name, entries: [] };
				byName.set(name, group);
				ordered.push(group);
			}
			group.entries.push({ ...entry, subkey });
		}
		for (const group of ordered) {
			const isTotal = (s: string) => s === 'totalMs' || s === 'total.ms' || s === 'total';
			group.entries.sort((a, b) => Number(isTotal(b.subkey)) - Number(isTotal(a.subkey)));
		}
		return ordered;
	}, [statistics]);

	if (statistics.length === 0) {
		return <div className="stats-empty">No statistics were recorded for this compile.</div>;
	}

	return (
		<div className="stats-view">
			{groups.map((group) => (
				<section key={group.name} className="stats-group">
					<h3 className="stats-group-title">{group.name}</h3>
					<table className="stats-table">
						<tbody>
							{group.entries.map((entry) => (
								<tr key={entry.key}>
									<td className="stats-key" title={entry.key}>
										{entry.subkey}
									</td>
									<td className="stats-value">{formatValue(entry)}</td>
								</tr>
							))}
						</tbody>
					</table>
				</section>
			))}
		</div>
	);
}
