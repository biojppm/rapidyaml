SHELL := bash

YS_VERSION := 0.1.87
YS_PREFIX := /tmp/rapidyaml
YS := $(YS_PREFIX)/bin/ys-$(YS_VERSION)

SOURCE_FILES := $(wildcard *.yml)
TARGET_FILES := $(SOURCE_FILES:%=../workflows/%)

export PATH := $(YS_PREFIX)/bin:$(PATH)
export YSPATH := $(shell pwd -P)/ys


default:

build: $(TARGET_FILES)

test: force build
	@git diff --exit-code ../workflows && \
	  echo -e '\nPASS - No normative changes to .github/workflows'

DIFF_ORIG_COMMIT ?= HEAD
diff:
	@for f in $(SOURCE_FILES); do \
	  f=$${f##*/}; \
	  diff -u --color=auto \
	    <(yq -P 'sort_keys(..)' \
	         -o=props <(git show $(DIFF_ORIG_COMMIT):.github/workflows/$$f) | \
		 grep -Ev '(^$$|^#)' \
	     ) \
	    <(yq -P 'sort_keys(..)' \
	         -o=props ../workflows/$$f | \
		 grep -Ev '(^$$|^#)'\
	     ); \
	  done

force:
	touch *.yml

../workflows/%: % $(YS)
	@[ -f $@ ] && chmod a+w $@
	@echo "# DO NOT EDIT - GENERATED FROM .github/workflows-ys/$<" > $@
	@echo >> $@
	ys -Y $< >> $@
	@chmod a-w $@

# Auto install a specific version of ys (under /tmp):
$(YS):
	curl -s https://yamlscript.org/install | \
	  BIN=1 VERSION=$(YS_VERSION) PREFIX=$(YS_PREFIX) bash
