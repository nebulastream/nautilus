// Unit handling and small formatting helpers.
//
// Benchmark values arrive with per-bench units (catch2 auto-scales: ns/us/ms),
// and the same logical series can cross a unit boundary between commits, so
// all math happens in nanoseconds and only display picks a unit.
//
// The `range` field ("± 32.6585") is NOT convertible: catch2 scales mean and
// std-dev independently and github-action-benchmark drops the std-dev unit,
// so ranges are shown verbatim and never used in calculations.

const UNIT_TO_NS = {
	ns: 1,
	us: 1e3,
	µs: 1e3,
	ms: 1e6,
	s: 1e9,
};

export function toNs(value, unit) {
	const factor = UNIT_TO_NS[unit];
	if (factor === undefined || typeof value !== 'number' || !isFinite(value)) {
		return null;
	}
	return value * factor;
}

// Pick the largest unit in which the magnitude stays >= 1.
export function pickDisplayUnit(maxNs) {
	if (!isFinite(maxNs) || maxNs < 1e3) {
		return { label: 'ns', div: 1 };
	}
	if (maxNs < 1e6) {
		return { label: 'µs', div: 1e3 };
	}
	if (maxNs < 1e9) {
		return { label: 'ms', div: 1e6 };
	}
	return { label: 's', div: 1e9 };
}

export function formatValue(v) {
	if (v === null || v === undefined || !isFinite(v)) {
		return '—';
	}
	if (v >= 100) {
		return v.toFixed(1);
	}
	if (v >= 10) {
		return v.toFixed(2);
	}
	return v.toFixed(3);
}

// Auto-scaled "1.23 µs" style rendering of a nanosecond quantity.
export function formatNs(ns) {
	if (ns === null || ns === undefined || !isFinite(ns)) {
		return '—';
	}
	const unit = pickDisplayUnit(Math.abs(ns));
	return `${formatValue(ns / unit.div)} ${unit.label}`;
}

export function formatPercent(p) {
	if (p === null || p === undefined || !isFinite(p)) {
		return '—';
	}
	const sign = p > 0 ? '+' : '';
	return `${sign}${p.toFixed(1)}%`;
}

export function shortHash(id) {
	return (id || '').slice(0, 7);
}

export function firstLine(message) {
	return (message || '').split('\n')[0];
}

export function formatDate(timestamp) {
	const d = new Date(timestamp);
	if (isNaN(d.getTime())) {
		return '';
	}
	return d.toISOString().slice(0, 10);
}
