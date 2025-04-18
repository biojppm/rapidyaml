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


c4core-check:
	for s in $(C4CORE_SRC); do \
	  ( $(COLORDIFF) -u $(C4CORE_DIR_PROJ)/$$s $(C4CORE_DIR_SRC)/$$s ) ; \
	done
	for s in $(C4CORE_DEV); do \
	  ( $(COLORDIFF) -u $(C4CORE_DIR_PROJ)/$$s $(C4CORE_DIR_DEV)/$$s ) ; \
	done

c4core-import: $(C4CORE_INPUT_SRC) $(C4CORE_INPUT_SRC)
	$(call tarcopy,$(C4CORE_DIR_PROJ),$(C4CORE_SRC),$(C4CORE_DIR_SRC))
	$(call tarcopy,$(C4CORE_DIR_PROJ),$(C4CORE_DEV),$(C4CORE_DIR_DEV))
c4core-export: $(C4CORE_OUTPUT_SRC) $(C4CORE_OUTPUT_EXPORT)
	$(call tarcopy,$(C4CORE_DIR_SRC),$(C4CORE_SRC),$(C4CORE_DIR_PROJ))
	$(call tarcopy,$(C4CORE_DIR_DEV),$(C4CORE_DEV),$(C4CORE_DIR_PROJ))
