const { addDynamicIconSelectors } = require('@iconify/tailwind');

/** @type {import('tailwindcss').Config} */
module.exports = {
	content: ['./public/*.html'],
	plugins: [
		// Iconify plugin
		addDynamicIconSelectors(),
	],
};