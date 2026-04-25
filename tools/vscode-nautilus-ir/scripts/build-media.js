// Copy the UMD bundles for the webview's runtime dependencies into
// `media/vendor/` so they can be loaded under the webview's strict CSP
// without needing `node_modules` at runtime. The copies are committed
// into the packaged .vsix via .vscodeignore (which only excludes `src/`,
// not `media/`).

'use strict';

const fs = require('fs');
const path = require('path');

const root = path.resolve(__dirname, '..');
const dest = path.join(root, 'media', 'vendor');
fs.mkdirSync(dest, { recursive: true });

// [source, target-name]
const bundles = [
	['mermaid/dist/mermaid.min.js', 'mermaid.min.js'],
	['svg-pan-zoom/dist/svg-pan-zoom.min.js', 'svg-pan-zoom.min.js'],
];

let failed = false;
for (const [pkgPath, targetName] of bundles) {
	const src = path.join(root, 'node_modules', pkgPath);
	const dst = path.join(dest, targetName);
	if (!fs.existsSync(src)) {
		console.error(`build-media: missing ${src} (run \`npm install\` first?)`);
		failed = true;
		continue;
	}
	fs.copyFileSync(src, dst);
	console.log(`build-media: ${pkgPath} -> ${path.relative(root, dst)}`);
}

process.exit(failed ? 1 : 0);
