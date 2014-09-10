showKeyArea = false;
showKeycaps = false;
plateThickness = 1.5;
plateReinforcementThickness = 2;
plateGap = 1;
switchBaseSize = 14.6;
switchOuterBaseSize = 16.1;
keySpacing = 19;
keycapSize = 18;
useSwitchFlaps = false;

//rightFingerKeyplate();
//rightThumbKeyplate();
//testKeyswitchMount();
testFourSwitchMount();


//--------------------------------------------------------------------------------

//@make
module testKeyswitchMount() {
	difference() {
		cube([25,25,plateThickness + plateReinforcementThickness]);
		translate([25/2, 25/2, 0]) keyswitch();
	}
}

//@make
module testFourSwitchMount() {
	assign(ps=keySpacing*2+6) {
		difference() {
			cube([ps,ps,plateThickness + plateReinforcementThickness]);
			union() {
				translate([ps/2-keySpacing/2, ps/2-keySpacing/2, 0]) keyswitch();
				translate([ps/2+keySpacing/2, ps/2-keySpacing/2, 0]) keyswitch();
				translate([ps/2+keySpacing/2, ps/2+keySpacing/2, 0]) keyswitch();
				translate([ps/2-keySpacing/2, ps/2+keySpacing/2, 0]) keyswitch();
			}
		}
	}
}

//@make
module leftFingerKeyplate() {
	mirror([1,0,0]) rightFingerKeyplate();
}

//@make
module rightFingerKeyplate() {
	difference() {
		translate([30,73,0]) cube([147,113,plateThickness]);
		translate([0,0,-plateThickness/2]) {
			translate([102,110,0]) rightFingers();
			scale([1,1,2]) minkowski() {
				union() {
					cube([109,90,plateThickness]);
					translate([109,86,0]) cylinder(r=8, h=plateThickness);
				}
				cylinder(h=plateThickness, r=plateGap);
			}
		}
	}
}

//@make
module leftThumbKeyplate() {
	mirror([1,0,0]) rightThumbKeyplate();
}

//@make
module rightThumbKeyplate() {
	difference() {
		union() {
			cube([109,90,plateThickness]);
			translate([109,86,0]) cylinder(r=8, h=plateThickness);
		}
		translate([102,110,0]) rightThumb(thumbKeyY=1.5);
	}
}

//--------------------------------------------------------------------------------

module rightFingers() {
	// column 1
	translate([keySpacing*-3, -19, 0]) {
		translate([0,0.5*keySpacing,0]) keyswitch();
		translate([0,1.75*keySpacing,0]) keyswitch(keyY=1.5);
		translate([0,3.0*keySpacing,0]) keyswitch();
	}

	// column 2
	translate([keySpacing*-2, -19, 0]) keyColumn(4);

	// column 3
	translate([keySpacing*-1, -18, 0]) keyColumn(4);

	// column 4
	translate([keySpacing* 0,  -5, 0]) keyColumn(4);

	// column 5
	translate([keySpacing* 1, -13, 0]) keyColumn(4);

	rotate(-4, [0,0,1]) {
		// column 6
		translate([keySpacing* 2.05, -23, 0]) keyColumn(4);

		// column 7
		translate([keySpacing* 3.05, -23, 0]) keyColumn(4);
	}
}

module rightThumb(thumbKeyY) {
	translate([keySpacing*-2,-19-keySpacing*(thumbKeyY/2),0]) {
		keyArc(keys=5, angle=30, keyY=thumbKeyY);
		rotate(30, [0,0,1]) translate([-23,-50,0]) keyswitch(keySpacing, keyX=2);
	}
}

//--------------------------------------------------------------------------------

module keyArc(keys, angle, keyY=1.5) {
	for (i=[-1:keys-2]) {
		assign(t=(keyY/2)*keySpacing + (keySpacing/2)/tan(angle/2)) {
			translate([0,-t,0]) rotate(i*angle, [0,0,1]) translate([0,t,0])
				keyswitch(keyY=keyY);
		}
	}
}

module keyColumn(keys) {
	for (i=[0:keys-1]) {
		translate([0, (0.5+i)*keySpacing, 0]) keyswitch();
	}
}

//--------------------------------------------------------------------------------

// positioned at center X, center Y, positive Z
module keyswitch(keyX=1, keyY=1) {
	baseSize = switchBaseSize;
	flapWidth = 0.6;
	flapHeight = 3.5;
	gapBetweenFlaps = 5;
	
	assign(thickness=plateReinforcementThickness) {
		translate([0,0,plateThickness + thickness/2])
			cube([switchOuterBaseSize, switchOuterBaseSize, thickness], center=true);
	}

	assign(thickness=plateThickness) {
		translate([0,0,thickness/2]) {
			cube([switchBaseSize, switchBaseSize, thickness], center=true);
			if (useSwitchFlaps) {
				translate([0, (gapBetweenFlaps + flapHeight)/2, 0])
					cube([switchBaseSize + 2*flapWidth, flapHeight, thickness], center=true);
				translate([0, -(gapBetweenFlaps + flapHeight)/2, 0])
					cube([switchBaseSize + 2*flapWidth, flapHeight, thickness], center=true);
			}
		}
	}

	translate([0,0,0.75]) {
		%if (showKeyArea) translate([0,0,thickness/2]) cube([keySpacing*keyX, keySpacing*keyY, thickness*2], center=true);
		%if (showKeycaps) translate([0,0,2.5]) color("gray") keycap(keyX, keyY);
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

//--------------------------------------------------------------------------------

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
		], faces=[
			[0,1,2,3],
			[7,6,5,4],
			[1,0,4,5],
			[2,1,5,6],
			[3,2,6,7],
			[0,3,7,4]
		]
	);
}