SHELL := bash

SRC_FILES := $(wildcard *.yml)
TARGET_FILES := $(SRC_FILES:%=../workflows/%)

YS_VERSION := 0.1.87
YS_PREFIX := /tmp/rapidyaml
YS := $(YS_PREFIX)/bin/ys-$(YS_VERSION)


default:

build: $(TARGET_FILES)

../workflows/%: % $(YS)
	@echo "# DO NOT EDIT - GENERATED FROM .github/workflows-ys/$<" > $@
	@echo >> $@
	$(YS) -Y $< >> $@

$(YS):
	curl -s https://yamlscript.org/install | \
	  BIN=1 VERSION=$(YS_VERSION) PREFIX=$(YS_PREFIX) bash
