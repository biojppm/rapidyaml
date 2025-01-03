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

force:
	touch *.yml

../workflows/%: % $(YS)
	@echo "# DO NOT EDIT - GENERATED FROM .github/workflows-ys/$<" > $@
	@echo >> $@
	ys -Y $< >> $@

# Auto install a specific version of ys (under /tmp):
$(YS):
	curl -s https://yamlscript.org/install | \
	  BIN=1 VERSION=$(YS_VERSION) PREFIX=$(YS_PREFIX) bash
