use <scad-utils/linalg.scad>
include <layout.scad>

$fn = 60;

switchBaseSize = 14;
keySpacing = 19;

useSwitchFlaps = true;
flapWidth = 0.8;
flapHeight = 3.5;
gapBetweenFlaps = 5;

screwHoleDiameter = 3.4;

display();

module display() {
	color("black") translate([0,0,-10]) linear_extrude(height=20) wristpad();
	color("green") translate([0,0,-5]) linear_extrude(height=1.76) pcb();
	linear_extrude(height=1.5) keyplate();
	color("yellow") translate([0,0,1.5]) linear_extrude(height=4.5) faceplate();
	color("orange") translate([0,0,6]) linear_extrude(height=1.5) faceplate();
	keyswitches();
	// keyArea();
}

// @make
module keyplate() {
	difference() {
		baseShape();
		for (key = keys) {
			translate(key_xy(key))
			rotate(key_rotation(key))
			keyCutout();
		}
	}
}

module wristpad() {
	hull() {
		translate([120, -17]) circle(25, center=true);
		translate([140, -12]) circle(30, center=true);
	}
}

module pcb() {
	difference() {
		baseShape();
		for (key = keys) {
			translate(key_xy(key))
			rotate(key_rotation(key)) {
				circle(2.5);
				translate([4.95, 0]) circle(1.25);
				translate([-4.95, 0]) circle(1.25);
			}
		}
	}
}

// module basePlate() {
// 	translate([0, 3]) square([70, 72]);
// 	rotate(30) translate([34, -74]) square([120, 87]);
// 	translate([20, 26]) square([152, 148]);
// 	translate([66.5, -47.07]) square([105.55, 100]);
// }

// @make
module faceplate() {
	difference() {
		baseShape();
		minkowski() {
			union() {
				for (key = keys) {
					translate(key_xy(key))
						rotate(key_rotation(key))
						square([18 * key_w(key), 18 * key_h(key)], center=true);
				};

				// Smooth edges between aligned keys
				for (i = [3, 7, 10, 11, 16, 17, 18, 20, 21, 22]) {
					key = keys[i];
					translate(key_xy(key))
						rotate(key_rotation(key))
						translate([-keySpacing * 0.5, 0])
						square([18 * key_w(key), 18 * key_h(key)], center=true);
				};
			}
			circle(1);
		}
	}
}

module baseShape() {
	difference() {
		union() {
			translate([-4, -1]) square([78, 78]);
			rotate(30) translate([34, -44]) square([120, 87]);
			translate([20, 26]) square([152, 148]);
		}
		for (pos = [
			[  1,   4], [ 35,   4],
			[167,  31], [131,  31],
			[  1,  72], [ 25,  72], [108,  72], [167, 72],
			[ 25, 169], [108, 169], [167, 169]
		]) translate(pos) circle(screwHoleDiameter / 2);
		rotate(30) translate([39, -39]) circle(screwHoleDiameter / 2);
	}
}

module keyCutout() {
	square([switchBaseSize, switchBaseSize], center=true);
	if (useSwitchFlaps) {
		translate([0, (gapBetweenFlaps + flapHeight) / 2])
			square([switchBaseSize + 2 * flapWidth, flapHeight], center=true);
		translate([0, -(gapBetweenFlaps + flapHeight) / 2])
			square([switchBaseSize + 2 * flapWidth, flapHeight], center=true);
	}
}

module keyArea() {
	for (key = keys) {
		translate(key_xy(key))
		rotate(key_rotation(key))
		translate([0, 0, 1.5]) {
			%alignedCube([
				keySpacing * key_w(key),
				keySpacing * key_h(key),
				1
			], align=[0, 0, 1]);
		}
	}
}

module keyswitches() {
	for (key = keys) {
		translate(key_xy(key))
		rotate(key_rotation(key))
		translate([0, 0, 1.5])
		{
			translate([0, 0, 5])
				color("gray")
				keycap(key_w(key), key_h(key));
			keyswitch();
		}
	}
}

module keyswitch() {
	translate([0, 0, 13.125])
	rotate(180) {
		include <keyswitch.scad>
	}
}

// positioned at center X, center Y, positive Z
module keycap(w=1, h=1) {

	// Keycap dimensions
	// https://pimpmykeyboard.com/template/images/DSAFamily.pdf
	// http://i.imgur.com/Fy0xVsC.png
	keyBase = 18.415;
	keyTop = 12.7;
	keycapHeight = 7.3914;

	keyInnerBase = 15;
	keyInnerTop = 12.2;
	keyInnerHeight = 5;

	cornerRounding = 0.4;

	translate(0, 0, -keycapHeight / 2)
	difference() {
		minkowski() {
			skewb(
				keyBase * w - (cornerRounding * 2),
				keyBase * h - (cornerRounding * 2),
				keyTop * w - (cornerRounding * 2),
				keyTop * h - (cornerRounding * 2),
				keycapHeight
			);
			cylinder(r=cornerRounding, h=keyBase/10);
		}
		translate([0, 0, -0.001])
		skewb(
			keyInnerBase * w,
			keyInnerBase * h,
			keyInnerTop * w,
			keyInnerTop * h,
			5
		);
		translate([0, 0, keycapHeight + 20.75])
			scale([w, h, 1])
			sphere(20.9);
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
