import type { Diagnostics as DiagnosticsData } from '../api';

const PHASE_MESSAGES: Record<string, string> = {
	compile: 'Your code failed to compile.',
	link: 'Linking failed — this is likely a playground bug, not yours.',
	trace: 'The tracing run was killed — most likely an infinite loop or a crash in your code.',
	pipeline: 'The Nautilus compilation pipeline reported an error.',
	timeout: 'The job exceeded its time limit.',
	oom: 'The job exceeded its memory limit.',
	internal: 'Internal playground error.',
};

export function DiagnosticsPanel({ diagnostics }: { diagnostics: DiagnosticsData }) {
	if (!diagnostics.phase) {
		return null;
	}
	const detail =
		diagnostics.phase === 'compile'
			? diagnostics.compilerStderr
			: diagnostics.phase === 'link'
				? diagnostics.linkStderr
				: diagnostics.pipelineError || diagnostics.runStderr || diagnostics.compilerStderr;

	return (
		<div className="diagnostics">
			<div className="diagnostics-headline">{PHASE_MESSAGES[diagnostics.phase] ?? 'The job failed.'}</div>
			{detail && <pre className="diagnostics-detail">{detail}</pre>}
		</div>
	);
}

/** Parses `user_module.cpp:LINE:COL: error|warning: message` clang lines. */
export function markersFromClangStderr(stderr: string): Array<{ line: number; column: number; message: string; severity: 'error' | 'warning' }> {
	const markers: Array<{ line: number; column: number; message: string; severity: 'error' | 'warning' }> = [];
	const re = /user_module\.cpp:(\d+):(\d+):\s+(error|warning):\s+(.*)/g;
	let match: RegExpExecArray | null;
	while ((match = re.exec(stderr)) !== null) {
		markers.push({
			line: Number.parseInt(match[1], 10),
			column: Number.parseInt(match[2], 10),
			message: match[4],
			severity: match[3] as 'error' | 'warning',
		});
	}
	return markers;
}
