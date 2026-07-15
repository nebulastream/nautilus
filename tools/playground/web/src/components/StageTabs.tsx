import type { Stage } from '../api';

interface StageTabsProps {
	stages: Stage[];
	/** Whether the result carries compilation statistics ('statistics' is a synthetic client-side tab). */
	hasStatistics: boolean;
	activeKey: string;
	onSelect: (key: string) => void;
}

const PHASE_LABELS: Record<string, string> = {
	frontend: 'Pipeline',
	pass: 'Optimization passes',
	backend: 'Backend',
	other: 'Other',
};

/**
 * Pipeline stepper: stages grouped by phase in pipeline order, plus the
 * synthetic Statistics tab. The engine's own graph/mermaid dumps are hidden —
 * every IR stage offers its own Text|CFG toggle instead.
 */
export function StageTabs({ stages, hasStatistics, activeKey, onSelect }: StageTabsProps) {
	const groups = new Map<string, Stage[]>();
	for (const stage of stages) {
		if (stage.phase === 'graph') {
			continue;
		}
		const list = groups.get(stage.phase) ?? [];
		list.push(stage);
		groups.set(stage.phase, list);
	}

	return (
		<nav className="stage-tabs">
			{hasStatistics && (
				<div className="stage-group">
					<span className="stage-group-label">Compilation</span>
					<button
						className={activeKey === 'statistics' ? 'stage-tab active' : 'stage-tab'}
						onClick={() => onSelect('statistics')}
					>
						Statistics
					</button>
				</div>
			)}
			{[...groups.entries()].map(([phase, phaseStages]) => (
				<div key={phase} className="stage-group">
					<span className="stage-group-label">{PHASE_LABELS[phase] ?? phase}</span>
					{phaseStages.map((stage) => (
						<button
							key={stage.key}
							className={stage.key === activeKey ? 'stage-tab active' : 'stage-tab'}
							title={stage.phase === 'pass' ? `${stage.title} (last iteration that changed the IR)` : stage.title}
							onClick={() => onSelect(stage.key)}
						>
							{stage.title}
						</button>
					))}
				</div>
			))}
		</nav>
	);
}
