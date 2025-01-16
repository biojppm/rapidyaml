SHELL := bash

YS_VERSION := 0.1.88
YS_PREFIX := /tmp/rapidyaml
YS := $(YS_PREFIX)/bin/ys-$(YS_VERSION)

INPUT_FILES := $(wildcard */*)
SOURCE_FILES := $(wildcard *.ys)
TARGET_FILES := $(SOURCE_FILES:%.ys=%.yml)

export PATH := $(YS_PREFIX)/bin:$(PATH)
export YSPATH := $(shell pwd -P)/ys


build: $(TARGET_FILES)


test: force build
	@git diff --exit-code $(TARGET_FILES) && \
	  echo -e '\nPASS - No normative changes to .github/workflows/*.yml'

DIFF_ORIG_COMMIT ?= HEAD
diff:
	@for s in $(SOURCE_FILES); do \
	  s=$${s##*/}; \
	  t=$${s%.yml}; \
	  [[ $$(git show $(DIFF_ORIG_COMMIT):.github/workflows/.$$t \
	          2>/dev/null) ]] && t=.$$t; \
	  diff -u --color=auto \
	    <(yq -P 'sort_keys(..)' \
	         -o=props <(git show $(DIFF_ORIG_COMMIT):.github/workflows/$$t) | \
		 grep -Ev '(^$$|^#)' \
	     ) \
	    <(yq -P 'sort_keys(..)' \
	         -o=props ../workflows/$$s | \
		 grep -Ev '(^$$|^#)'\
	     ); \
	  done

force:
	touch *.ys

%.yml: %.ys $(YS) $(INPUT_FILES)
	@if [ -f "$@" ] ; then chmod a+w $@ ; fi
	@echo "# DO NOT EDIT - GENERATED FROM .github/workflows/$<" > $@
	@echo >> $@
	$(YS) -Y $< >> $@
	@chmod a-w $@
	@wc -lm --total=never $< $@


# Auto install a specific version of ys
install-ys: $(YS)
$(YS):
	curl -s https://yamlscript.org/install | \
	  BIN=1 VERSION=$(YS_VERSION) PREFIX=$(YS_PREFIX) bash

stats:
	@echo "ys : $$(wc -l *.ys)"
	@echo "yml: $$(wc -l *.yml)"
