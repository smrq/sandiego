#!/usr/bin/env node

const fs = require('fs');

const keySpacing = 19.05;

const keys = translateAll([
	...keyColumn(keySpacing * -3, -24, [1, 1.5, 1]),
	...keyColumn(keySpacing * -2, -24, [1, 1, 1, 1]),
	...keyColumn(keySpacing * -1, -24, [1, 1, 1, 1]),
	...keyColumn(keySpacing *  0, -12, [1, 1, 1, 1]),
	...rotateAll(
		keyColumn(keySpacing * 1.025, -14.5, [1, 1, 1, 1]),
		-2,
		{ x: 0, y: 0 }
	),
	...rotateAll([
		...keyColumn(keySpacing * 2.055, -23, [1, 1, 1, 1]),
		...keyColumn(keySpacing * 3.055, -26, [1, 1, 1, 1])
	], -4, { x: 0, y: 0 }),
	rotate(key({
		x: keySpacing * 2.555,
		y: -53,
		w: 2
	}), -4, { x: 0, y: 0 }),
	...translateAll(
		keyArc(5, 45.25, 30, 1),
		keySpacing * -1.75,
		-24 - keySpacing * 0.75
	),
	translate(
		rotate(key({ x: 0, y: -12, w: 2 }), 30, { x: 0, y: 0 }),
		keySpacing * -1.75,
		-45.25 - 24 - keySpacing * 0.75
	),
], 93.5, 103);

fs.writeFileSync('./layout.scad', formatScad(keys));

function key(opts) {
	return Object.assign({ w: 1, h: 1, rotation: 0 }, opts);
}

function translate(key, x, y) {
	return Object.assign({}, key, { x: key.x + x, y: key.y + y });
}

function translateAll(keys, x, y) {
	return keys.map(key => translate(key, x, y));
}

function rotate(key, deg, origin) {
	const cosAngle = Math.cos(degToRad(deg));
	const sinAngle = Math.sin(degToRad(deg));

	return Object.assign({}, key, {
		x: cosAngle * (key.x - origin.x) -
			sinAngle * (key.y - origin.y) +
			origin.x,

		y: sinAngle * (key.x - origin.x) +
			cosAngle * (key.y - origin.y) +
			origin.y,

		rotation: key.rotation + deg
	});
}

function rotateAll(keys, deg, origin) {
	return keys.map(key => rotate(key, deg, origin));
}

function keyColumn(x, y, heights) {
	return heights.map(h => {
		const result = key({ x, y: y + keySpacing * (h * 0.5), h })
		y += keySpacing * h;
		return result;
	});
}

function keyArc(count, arclength, deg, h) {
	return [...Array(count)].map((_, i) => {
		return rotate(
			key({ x: 0, y: 0, h }),
			(i - 1) * deg,
			{ x: 0, y: -arclength }
		);
	});
}

function degToRad(deg) {
	return deg / 180 * Math.PI;
}

function formatScad(keys) {
	return `keys=[
${
	keys.map(key => `\t[${key.x}, ${key.y}, ${key.rotation}, ${key.w}, ${key.h}]`)
		.join(',\n')
}
];

function key_xy(key) = [key[0], key[1]];
function key_rotation(key) = key[2];
function key_w(key) = key[3];
function key_h(key) = key[4];
`;
}
