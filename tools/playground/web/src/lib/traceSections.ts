// TraceModule::toString() (nautilus/src/nautilus/tracing/ExecutionTrace.cpp)
// emits every module function's trace under an UPPERCASED `NAME:` header
// line. This splits that blob into per-function sections so the UI can offer
// the same function navigation the graph view has.

export interface TraceSection {
	/** Header exactly as it appears in the trace (uppercased). */
	header: string;
	/** Pretty name if a case-insensitive match against IR names exists. */
	name: string;
	text: string;
}

const HEADER_RE = /^[A-Z0-9_]+:$/;

/**
 * Splits a multi-function trace dump into sections. `irFunctionNames` (from
 * the parsed IR stages, which preserve case) is used to map the uppercase
 * headers back to readable names.
 */
export function splitTraceSections(text: string, irFunctionNames: string[]): TraceSection[] {
	const byUpper = new Map(irFunctionNames.map((n) => [n.toUpperCase(), n]));
	const sections: TraceSection[] = [];
	let current: TraceSection | null = null;
	const lines = text.split('\n');
	const pending: string[] = [];

	const flush = () => {
		if (current) {
			current.text = pending.join('\n').replace(/\n+$/, '\n');
			sections.push(current);
		}
		pending.length = 0;
	};

	for (const line of lines) {
		if (HEADER_RE.test(line.trim()) && line === line.trimEnd()) {
			flush();
			const header = line.trim().slice(0, -1);
			current = { header, name: byUpper.get(header) ?? header, text: '' };
		} else if (current) {
			pending.push(line);
		}
	}
	flush();
	return sections;
}
