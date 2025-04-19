include ../proj/common.mk

C4CORE_REPO := https://github.com/biojppm/c4core
C4CORE_TAG := v0.2.6
C4CORE_DIR_PROJ := c4core/src

# c4core files used by ryml
C4CORE_SRC := $(shell cat c4core.src.txt)
# c4core files used by ryml in debug/test builds
C4CORE_DEV := $(shell cat c4core.dev.txt)

C4CORE_DIR_SRC = c4core.src
C4CORE_DIR_DEV = c4core.dev

C4CORE_INPUT_SRC = $(C4CORE_SRC:%=$(C4CORE_DIR_PROJ)/%)
C4CORE_INPUT_DEV = $(C4CORE_DEV:%=$(C4CORE_DIR_PROJ)/%)

C4CORE_OUTPUT_SRC = $(C4CORE_SRC:%=$(C4CORE_DIR_SRC)/%)
C4CORE_OUTPUT_DEV = $(C4CORE_DEV:%=$(C4CORE_DIR_DEV)/%)


c4core-check: c4core-check-sync c4core-check-list
c4core-check-sync: c4core-check-sync-src c4core-check-sync-dev
c4core-check-list: c4core-check-list-src c4core-check-list-dev
c4core-check-sync-src: c4core-clone
	@echo "check c4core list src..."
	for s in $(C4CORE_SRC); do \
	  ( $(COLORDIFF) -u $(C4CORE_DIR_PROJ)/$$s $(C4CORE_DIR_SRC)/$$s ) ; \
	done
	@echo "check c4core sync src: PASS"
c4core-check-sync-dev: c4core-clone
	@echo "check c4core list dev..."
	for s in $(C4CORE_DEV); do \
	  ( $(COLORDIFF) -u $(C4CORE_DIR_PROJ)/$$s $(C4CORE_DIR_DEV)/$$s ) ; \
	done
	@echo "check c4core sync dev: PASS"
c4core-check-list-src: c4core-clone
	@echo "check c4core list src..."
	$(COLORDIFF) -u \
	    <(cat c4core.src.txt | sort) \
	    <(cd c4core.src ; find . ! -type d | sed s:\./c4:c4:g | sort)
	@echo "check c4core list src: PASS"
c4core-check-list-dev: c4core-clone
	@echo "check c4core list dev..."
	$(COLORDIFF) -u \
	    <(cat c4core.dev.txt | sort) \
	    <(cd c4core.dev ; find . ! -type d | sed s:\./c4:c4:g | sort)
	@echo "check c4core list dev: PASS"


c4core-import: c4core-import-src c4core-import-dev
c4core-export: c4core-export-src c4core-export-dev

c4core-import-src: $(C4CORE_INPUT_SRC)
	$(call safe_sync,$(C4CORE_SRC),$(C4CORE_DIR_PROJ),$(C4CORE_DIR_SRC))
c4core-import-dev: $(C4CORE_INPUT_DEV)
	$(call safe_sync,$(C4CORE_DEV),$(C4CORE_DIR_PROJ),$(C4CORE_DIR_DEV))

c4core-export-src: $(C4CORE_OUTPUT_SRC)
	$(call safe_sync,$(C4CORE_SRC),$(C4CORE_DIR_SRC),$(C4CORE_DIR_PROJ))
c4core-export-dev: $(C4CORE_OUTPUT_DEV)
	$(call safe_sync,$(C4CORE_DEV),$(C4CORE_DIR_DEV),$(C4CORE_DIR_PROJ))

$(C4CORE_INPUT_SRC): c4core-clone
$(C4CORE_INPUT_DEV): c4core-clone

c4core-clone: c4core-clone-check
c4core-clone-check: $(RYML_DIR)/build/c4core.done
	@curr=$$(cd c4core ; git describe --tags --abbrev=0) ; \
	echo "c4core tag: $$curr" ; \
	if [ "$$curr" != "$(C4CORE_TAG)" ] ; then \
	    echo "c4core: different version: expected=$(C4CORE_TAG) actual=$$curr" ; \
	    exit 1 ; \
	fi ; \
	echo "c4core tag ok! $$curr"
$(RYML_DIR)/build/c4core.done: c4core
	mkdir -p $(RYML_DIR)/build
	echo $(C4CORE_TAG) > $(RYML_DIR)/build/c4core.done
c4core:
	mkdir -p $@
	$(GIT) -C $@ init -q .
	$(GIT) -C $@ remote add origin $(C4CORE_REPO)
	$(GIT) -C $@ fetch --tags origin $(C4CORE_TAG)
	$(GIT) -C $@ reset --hard FETCH_HEAD
	$(GIT) -C $@ submodule update --init --recursive
