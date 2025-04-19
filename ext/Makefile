include ../proj/common.mk

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
c4core-check-sync-src:
	@echo "check c4core list src..."
	for s in $(C4CORE_SRC); do \
	  ( $(COLORDIFF) -u $(C4CORE_DIR_PROJ)/$$s $(C4CORE_DIR_SRC)/$$s ) ; \
	done
	@echo "check c4core sync src: PASS"
c4core-check-sync-dev:
	@echo "check c4core list dev..."
	for s in $(C4CORE_DEV); do \
	  ( $(COLORDIFF) -u $(C4CORE_DIR_PROJ)/$$s $(C4CORE_DIR_DEV)/$$s ) ; \
	done
	@echo "check c4core sync dev: PASS"
c4core-check-list-src:
	@echo "check c4core list src..."
	$(COLORDIFF) -u \
	    <(cat c4core.src.txt | sort) \
	    <(cd c4core.src ; find . ! -type d | sed s:\./c4:c4:g | sort)
	@echo "check c4core list src: PASS"
c4core-check-list-dev:
	@echo "check c4core list dev..."
	$(COLORDIFF) -u \
	    <(cat c4core.dev.txt | sort) \
	    <(cd c4core.dev ; find . ! -type d | sed s:\./c4:c4:g | sort)
	@echo "check c4core list dev: PASS"


c4core-import: $(C4CORE_INPUT_SRC) $(C4CORE_INPUT_SRC)
	$(call tarcopy,$(C4CORE_DIR_PROJ),$(C4CORE_SRC),$(C4CORE_DIR_SRC))
	$(call tarcopy,$(C4CORE_DIR_PROJ),$(C4CORE_DEV),$(C4CORE_DIR_DEV))
c4core-export: $(C4CORE_OUTPUT_SRC) $(C4CORE_OUTPUT_EXPORT)
	$(call tarcopy,$(C4CORE_DIR_SRC),$(C4CORE_SRC),$(C4CORE_DIR_PROJ))
	$(call tarcopy,$(C4CORE_DIR_DEV),$(C4CORE_DEV),$(C4CORE_DIR_PROJ))
