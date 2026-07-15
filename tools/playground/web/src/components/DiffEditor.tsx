import { useEffect, useRef } from 'react';
import { monaco, registerLanguages } from '../monaco/languages';

interface DiffEditorProps {
	original: string;
	modified: string;
	language: string;
}

/** Read-only side-by-side Monaco diff, used for pass-snapshot comparisons. */
export function DiffEditor({ original, modified, language }: DiffEditorProps) {
	const containerRef = useRef<HTMLDivElement>(null);
	const editorRef = useRef<monaco.editor.IStandaloneDiffEditor | null>(null);

	useEffect(() => {
		registerLanguages();
		const editor = monaco.editor.createDiffEditor(containerRef.current!, {
			readOnly: true,
			automaticLayout: true,
			renderSideBySide: true,
			minimap: { enabled: false },
			scrollBeyondLastLine: false,
			fontSize: 13,
			renderOverviewRuler: false,
		});
		editorRef.current = editor;
		return () => {
			const model = editor.getModel();
			editor.dispose();
			model?.original.dispose();
			model?.modified.dispose();
			editorRef.current = null;
		};
	}, []);

	useEffect(() => {
		const editor = editorRef.current;
		if (!editor) {
			return;
		}
		const previous = editor.getModel();
		editor.setModel({
			original: monaco.editor.createModel(original, language),
			modified: monaco.editor.createModel(modified, language),
		});
		previous?.original.dispose();
		previous?.modified.dispose();
	}, [original, modified, language]);

	return <div ref={containerRef} className="editor-container" />;
}
