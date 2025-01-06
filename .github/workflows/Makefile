SHELL := bash

YS_VERSION := 0.1.87
YS_PREFIX := /tmp/rapidyaml
YS := $(YS_PREFIX)/bin/ys-$(YS_VERSION)

SOURCE_FILES := $(wildcard *.ys)
TARGET_FILES := $(SOURCE_FILES:%.ys=%.yml)
TARGET_FILES := $(TARGET_FILES:%=.%)

export PATH := $(YS_PREFIX)/bin:$(PATH)
export YSPATH := $(shell pwd -P)/ys


default:

build: $(TARGET_FILES)

test: force build
	@git diff --exit-code .*.yml && \
	  echo -e '\nPASS - No normative changes to .github/workflows/.*.yml'

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

.%.yml: %.ys $(YS)
	@[ -f $@ ] && chmod a+w $@
	@echo "# DO NOT EDIT - GENERATED FROM .github/workflows/$<" > $@
	@echo >> $@
	ys -Y $< >> $@
	@chmod a-w $@

# Auto install a specific version of ys (under /tmp):
$(YS):
	curl -s https://yamlscript.org/install | \
	  BIN=1 VERSION=$(YS_VERSION) PREFIX=$(YS_PREFIX) bash
