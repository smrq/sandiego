use <scad-utils/linalg.scad>

switchBaseSize = 14;
switchOuterBaseSize = 16;
keySpacing = 19.05;
keycapSize = 18;
thumbKeyY = 1.5;
useSwitchFlaps = true;

// Preview
showKeyArea = true;
showKeycaps = false;
plateThickness = 1.5;
linear_extrude(height=plateThickness) keyplate();

// @make
module keyplate() {
	difference() {
		square([176, 176]);
		keys();
	}
}

module keys() {
	translate([103, 105]) {
		// column 1
		translate([keySpacing*-3, -19]) {
			translate([0,0.5*keySpacing]) keyswitch();
			translate([0,1.75*keySpacing]) keyswitch(keyY=1.5);
			translate([0,3.0*keySpacing]) keyswitch();
		}

		// column 2
		translate([keySpacing*-2, -19]) keyColumn(4);

		// column 3
		translate([keySpacing*-1, -19]) keyColumn(4);

		// column 4
		translate([keySpacing* 0, -7.5]) keyColumn(4);

		// column 5
		rotate(-2, [0,0,1]) {
			translate([keySpacing* 1.025, -14.5]) keyColumn(4);
		}

		rotate(-4, [0,0,1]) {
			// column 6
			translate([keySpacing* 2.05, -27]) keyColumn(4);

			// column 7
			translate([keySpacing* 3.05, -30]) keyColumn(4);
		}

		// thumb
		translate([
			keySpacing*-2,
			-19-keySpacing*(thumbKeyY/2)
		]) {
			keyArc(keys=5, angle=30, keyY=thumbKeyY);
			rotate(30, [0,0,1]) translate([-23,-50]) keyswitch(keySpacing, keyX=2);
		}
	}
}

module keyArc(keys, angle, keyY=1.5) {
	for (i=[-1:keys-2]) {
		t=(keyY/2) * keySpacing + (keySpacing/2) / tan(angle/2);
		translate([0,-t]) rotate(i*angle, [0,0,1]) translate([0,t]) {
			keyswitch(keyY=keyY);
		}
	}
}

module keyColumn(keys) {
	for (i=[0:keys-1]) {
		translate([0, (0.5+i) * keySpacing]) {
			keyswitch();
		}
	}
}

// positioned at center X, center Y, positive Z
module keyswitch(keyX=1, keyY=1) {
	baseSize = switchBaseSize;
	flapWidth = 0.8;
	flapHeight = 3.5;
	gapBetweenFlaps = 5;

	keyswitchCutout(keyX, keyY);
	%keyswitchPreview(keyX, keyY);
}

module keyswitchPreview(keyX=1, keyY=1) {
	translate([0, 0, plateThickness]) {
		if (showKeyArea) {
			alignedCube([keySpacing*keyX, keySpacing*keyY, 5], align=[0, 0, 1]);
		}
		if (showKeycaps) {
			translate([0,0,2.5]) color("gray") keycap(keyX, keyY);
		}
	}
}

module keyswitchCutout(keyX=1, keyY=1) {
	baseSize = switchBaseSize;
	flapWidth = 0.8;
	flapHeight = 3.5;
	gapBetweenFlaps = 5;

	translate([0, 0, plateThickness]) {
		translate([0, 0, 1]) {
			square([switchBaseSize, switchBaseSize], center=true);
			if (useSwitchFlaps) {
				translate([0, (gapBetweenFlaps + flapHeight) / 2])
					square(
						[switchBaseSize + 2 * flapWidth, flapHeight],
						center=true);
				translate([0, -(gapBetweenFlaps + flapHeight) / 2])
					square(
						[switchBaseSize + 2 * flapWidth, flapHeight],
						center=true);
			}
		}
	}
}

// positioned at center X, center Y, positive Z
module keycap(keyX=1, keyY=1) {
	keyBase = keycapSize;
	keyTop = keyBase*0.7;
	keyHeight = keyBase*0.5;
	cornerRounding = 0.1;
	difference() {
		minkowski() {
			skewb(
				keyBase*(keyX-cornerRounding),
				keyBase*(keyY-cornerRounding),
				keyTop*(keyX-cornerRounding),
				keyTop*(keyY-cornerRounding),
				keyHeight
			);
			cylinder(r=keyBase*cornerRounding/2, h=keyBase/10);
		}
		translate([0,0,keyBase*3 + keyHeight])
			scale([keyX,keyY,1]) sphere(keyBase*3, $fn=100);
	}
}

// positioned at center X, center Y, positive Z
module skewb(baseX, baseY, topX, topY, z) {
	polyhedron(
		points=[
			[-baseX/2, -baseY/2, 0],
			[ baseX/2, -baseY/2, 0],
			[ baseX/2,  baseY/2, 0],
			[-baseX/2,  baseY/2, 0],
			[-topX/2, -topY/2, z],
			[ topX/2, -topY/2, z],
			[ topX/2,  topY/2, z],
			[-topX/2,  topY/2, z]
		],
		faces=[
			[0,1,2,3],
			[7,6,5,4],
			[1,0,4,5],
			[2,1,5,6],
			[3,2,6,7],
			[0,3,7,4]
		]
	);
}

module alignedCube(size, align = [0, 0, 0]) {
	translate(hadamard(size / 2, align))
		cube(size, center = true);
}
