OPENSCAD=/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD

SRCDIR=src
BUILDDIR=build

MAIN=$(SRCDIR)/main.scad

# Generate list of modules annotated with: // @make
MODULES_CMD=sed -n '\|//[[:space:]]*@make|{ n; s/module \(.*\)(.*/\1/; p; }' $(MAIN)
MODULES=$(shell $(MODULES_CMD))

SVG=$(MODULES:%=$(BUILDDIR)/%.svg)

all: $(SVG)

%.generated.scad:
	echo 'use <$(MAIN)>\n$*();' > $@

$(BUILDDIR)/%.svg: %.generated.scad $(MAIN)
	@mkdir -p $(@D)
	$(OPENSCAD) -o $@ $<
	sed -i '' -e 's/stroke="[^"]*"/stroke="rgb(0,0,255)"/; s/fill="[^"]*"/fill="rgb(255,255,255)"/; s/stroke-width="[^"]*"/stroke-width="0.01"/' $@

clean:
	rm -rf *.generated.scad $(BUILDDIR)

.PHONY: clean
