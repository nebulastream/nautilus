import { useEffect, useRef } from 'react';
import { monaco, registerLanguages } from '../monaco/languages';

export interface EditorMarker {
	line: number;
	column: number;
	message: string;
	severity: 'error' | 'warning';
}

interface EditorProps {
	value: string;
	onChange?: (value: string) => void;
	language: string;
	readOnly?: boolean;
	markers?: EditorMarker[];
	/** Invoked on Ctrl/Cmd+Enter inside the editor. */
	onSubmit?: () => void;
	wordWrap?: boolean;
}

/** Thin React wrapper around a bundled Monaco editor instance. */
export function Editor({ value, onChange, language, readOnly = false, markers = [], onSubmit, wordWrap = false }: EditorProps) {
	const containerRef = useRef<HTMLDivElement>(null);
	const editorRef = useRef<monaco.editor.IStandaloneCodeEditor | null>(null);
	const onChangeRef = useRef(onChange);
	onChangeRef.current = onChange;
	const onSubmitRef = useRef(onSubmit);
	onSubmitRef.current = onSubmit;

	useEffect(() => {
		registerLanguages();
		const editor = monaco.editor.create(containerRef.current!, {
			value,
			language,
			readOnly,
			automaticLayout: true,
			minimap: { enabled: false },
			scrollBeyondLastLine: false,
			fontSize: 13,
			tabSize: 4,
			renderWhitespace: 'none',
			wordWrap: wordWrap ? 'on' : 'off',
		});
		editor.onDidChangeModelContent(() => {
			onChangeRef.current?.(editor.getValue());
		});
		editor.addCommand(monaco.KeyMod.CtrlCmd | monaco.KeyCode.Enter, () => {
			onSubmitRef.current?.();
		});
		editorRef.current = editor;
		return () => {
			editor.dispose();
			editorRef.current = null;
		};
		// The editor instance is created once; value/language/markers are
		// synchronized by the effects below.
		// eslint-disable-next-line react-hooks/exhaustive-deps
	}, []);

	useEffect(() => {
		const editor = editorRef.current;
		if (editor && editor.getValue() !== value) {
			editor.setValue(value);
		}
	}, [value]);

	useEffect(() => {
		const model = editorRef.current?.getModel();
		if (model) {
			monaco.editor.setModelLanguage(model, language);
		}
	}, [language]);

	useEffect(() => {
		const model = editorRef.current?.getModel();
		if (!model) {
			return;
		}
		monaco.editor.setModelMarkers(
			model,
			'playground',
			markers.map((m) => ({
				startLineNumber: m.line,
				startColumn: m.column,
				endLineNumber: m.line,
				endColumn: m.column + 1,
				message: m.message,
				severity: m.severity === 'error' ? monaco.MarkerSeverity.Error : monaco.MarkerSeverity.Warning,
			})),
		);
	}, [markers]);

	return <div ref={containerRef} className="editor-container" />;
}
