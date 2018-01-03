TARGETS=arms brain
FLASH=$(patsubst %,flash-%,$(TARGETS))
FUSE=$(patsubst %,fuse-%,$(TARGETS))

all: $(TARGETS)

clean:
	@for dir in $(TARGETS); do \
		$(MAKE) -C $$dir clean; \
	done

$(TARGETS):
	@$(MAKE) -C $@

$(FLASH):
	@$(MAKE) -C $(patsubst flash-%,%,$@) flash

$(FUSE):
	@$(MAKE) -C $(patsubst fuse-%,%,$@) fuse

.PHONY: all clean $(TARGETS) $(FLASH) $(FUSE)
