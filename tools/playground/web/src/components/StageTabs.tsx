import type { Stage } from '../api';

interface StageTabsProps {
	stages: Stage[];
	/** 'graph' is a synthetic tab rendered client-side from the IR stages. */
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
 * synthetic Graph tab. The engine's own graph/mermaid dumps are folded into
 * the Graph experience, so they are hidden from the text tabs.
 */
export function StageTabs({ stages, activeKey, onSelect }: StageTabsProps) {
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
			<div className="stage-group">
				<span className="stage-group-label">Graph</span>
				<button
					className={activeKey === 'graph' ? 'stage-tab active' : 'stage-tab'}
					onClick={() => onSelect('graph')}
				>
					CFG
				</button>
			</div>
		</nav>
	);
}
