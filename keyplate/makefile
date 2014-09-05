# match "module foobar() { // `make` me" 
TARGETS=$(shell cat base.scad | sed '/\/\/\s*@make\s*/,/module [a-zA-Z0-9_-]*/!d' | sed -n 's/module \([a-zA-Z0-9_-]*\).*/\1.stl/p' ) 
OPENSCAD=/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD

all: ${TARGETS}

# auto-generated .scad files with .deps make make re-build always. keeping the
# scad files solves this problem.
.SECONDARY: $(shell echo "${TARGETS}" | sed 's/\.stl/.generated.scad/g')

# explicit wildcard expansion suppresses errors when no files are found
include $(wildcard *.deps)

%.generated.scad:
	echo 'use <base.scad>\n$*();' > $@

%.stl: %.generated.scad
	$(OPENSCAD) -m make -o $@ $<

clean:
	rm -f *.stl *.generated.scad *.deps

.PHONY: clean