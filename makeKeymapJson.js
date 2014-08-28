Array.prototype.zip = function zip() {
	var arrays = this;
	return arrays[0].map(function (_, i) {
		return arrays.map(function (array) {
			return array[i];
		});
	});
};

var labels = {
	GRAVE: '~\n`',
	ESC: 'Esc\n',
	INSERT: 'Ins\n',
	DELETE: 'Del\n',
	HOME: 'Home\n',
	END: 'End\n',
	PAGEUP: 'PgUp\n',
	PAGEDOWN: 'PgDn\n',
	BACKSPACE: 'Bksp\n',
	SPACE: 'Space\n',
	LEFT: '←\n',
	DOWN: '↓\n',
	UP: '↑\n',
	RIGHT: '→\n',
	CAPSLOCK: '\nCapsLk',
	SCROLLLOCK: '\nScrLk',
	NUMLOCK: '\nNumLk',
	COMMA: '<\n,',
	PERIOD: '>\n.',
	SLASH: '?\n/',
	BACKSLASH: '|\n\\',
	QUOTE: '"\n\'',
	SEMICOLON: ':\n;',
	DASH: '_\n-',
	EQUAL: '+\n=',
	LBRACKET: '{\n[',
	RBRACKET: '}\n]',
	TAB: 'Tab\n',
	RETURN: 'Retn\n',
	PRINTSCREEN_SYSRQ: '\nPrSc',
	PAUSE_BREAK: '\nPause',
	'1': '!\n1',
	'2': '@\n2',
	'3': '#\n3',
	'4': '$\n4',
	'5': '%\n5',
	'6': '^\n6',
	'7': '&\n7',
	'8': '*\n8',
	'9': '(\n9',
	'0': ')\n0'
};
for (var c = 'A'; c <= 'Z'; c = String.fromCharCode(c.charCodeAt(0) + 1)) {
	labels[c] = c + '\n';
}


var cols = [
	{x: 0.5, y: 0.875},
	{x: 1.5, y: 0.875},
	{x: 2.5, y: 0.375},
	{x: 3.5, y: 0.000},
	{x: 4.5, y: 0.625},
	{x: 5.5, y: 0.750},
	{x: 6.5, y: 0.250},
	{x: 12, y: 0.250},
	{x: 13, y: 0.750},
	{x: 14, y: 0.625},
	{x: 15, y: 0.000},
	{x: 16, y: 0.375},
	{x: 17, y: 0.875},
	{x: 18, y: 0.875}
];


var mainCode = require('fs').readFileSync('src/Main.ino');
var keymap = /keyMap\[LAYERS\]\[ROWS\]\[COLUMNS\] = {([^\}]+)}/
	.exec(mainCode)[1]
	.split('KEYMAP')
	.slice(1)
	.map(function (code) {
		return code.replace(/[^A-Za-z0-9_]/g, ' ')
			.trim()
			.split(/\s+/);
	})
	.zip();
var keyLabels = keymap.map(function (keyLayers) {
	return keyLayers.map(function (l) { return l === 'NULL' ? '' : l })
		.map(function (l) { return labels[l] || l + '\n'; })
		.join('\n');
});

function keyCoord(row, col, highlight, width, height, rotation, ry) {
	var pos = { x: cols[col].x, y: cols[col].y + row, c: highlight ? '#cb2f2a' : '#2b2c2e', t: '#ffffff' };
	if (width != null) pos.w = width;
	if (height != null) pos.h = height;
	if (rotation != null && ry != null) {
		pos.r = rotation;
		pos.rx = cols[col].x + (width || 1) / 2;
		pos.ry = cols[col].y + row + ry;
	}
	return pos;
}

var keyCoordinates = [
	keyCoord(0,0,false), keyCoord(0,1,false), keyCoord(0,2,false), keyCoord(0,3,false), keyCoord(0,4,false), keyCoord(0,5,false),
	keyCoord(0,8,false), keyCoord(0,9,false), keyCoord(0,10,false), keyCoord(0,11,false), keyCoord(0,12,false), keyCoord(0,13,false),

	keyCoord(1,0,false), keyCoord(1,1,false), keyCoord(1,2,false), keyCoord(1,3,false), keyCoord(1,4,true), keyCoord(1,5,true), keyCoord(1,6,false),
	keyCoord(1,7,false), keyCoord(1,8,true), keyCoord(1,9,true), keyCoord(1,10,false), keyCoord(1,11,false), keyCoord(1,12,false), keyCoord(1,13,false),

	keyCoord(2,0,false), keyCoord(2,1,true), keyCoord(2,2,true), keyCoord(2,3,true), keyCoord(2,4,false), keyCoord(2,5,true), keyCoord(2,6,true,1,1.5),
	keyCoord(2,7,true,1,1.5), keyCoord(2,8,true), keyCoord(2,9,false), keyCoord(2,10,true), keyCoord(2,11,true), keyCoord(2,12,true), keyCoord(2,13,false),

	keyCoord(3,0,false), keyCoord(3,1,false), keyCoord(3,2,false), keyCoord(3,3,false), keyCoord(3,4,true), keyCoord(3,5,true), keyCoord(3.5,6,true),
	keyCoord(3.5,7,true), keyCoord(3,8,true), keyCoord(3,9,true), keyCoord(3,10,false), keyCoord(3,11,false), keyCoord(3,12,false), keyCoord(3,13,false),

	keyCoord(7.5,5,true,2,1,30,0), keyCoord(4,5,true,1,1.5,-30,3.5), keyCoord(4,5,true,1,1.5,0,3.5), keyCoord(4,5,true,1,1.5,30,3.5), keyCoord(4,5,true,1,1.5,60,3.5), keyCoord(4,5,true,1,1.5,90,3.5),
	keyCoord(4,8,true,1,1.5,-90,3.5), keyCoord(4,8,true,1,1.5,-60,3.5), keyCoord(4,8,true,1,1.5,-30,3.5), keyCoord(4,8,true,1,1.5,0,3.5), keyCoord(4,8,true,1,1.5,30,3.5), keyCoord(7.5,8,true,2,1,-30,0)
];

for (var i = 0, xOffset = 0, yOffset = 0; i < keyCoordinates.length; ++i) {
	if (keyCoordinates[i].ry) {
		xOffset = keyCoordinates[i].rx;
		yOffset = keyCoordinates[i].ry;
		keyCoordinates[i].x -= xOffset;
		keyCoordinates[i].y -= yOffset;
	} else {
		keyCoordinates[i].x -= xOffset;
		xOffset = 0;

		var newYOffset = keyCoordinates[i].y + 1;
		keyCoordinates[i].y -= yOffset;
		yOffset = newYOffset;
	}
}

var keys = [keyCoordinates, keyLabels].zip();
console.log(JSON.stringify(keys).slice(1,-1));