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


# function to sync files safely
# $1=src_dir
# $2=src_files
# $3=dst_dir
safe_sync = \
	set -xe -o pipefail ; \
	( cd $3 ; git diff --exit-code --color=auto $2 || ( echo "$$(pwd): destination files are dirty" ; exit 1 ) ) ; \
	for f in $2 ; do \
	   $(COLORDIFF) -u $1/$$f $3/$$f || ( cp -fav $1/$$f $3/$$f ) ; \
	done


c4core-import: c4core-import-src c4core-import-dev
c4core-export: c4core-export-src c4core-export-dev
c4core-import-src: $(C4CORE_INPUT_SRC)
	$(call safe_sync,$(C4CORE_DIR_PROJ),$(C4CORE_SRC),$(C4CORE_DIR_SRC))
c4core-import-dev: $(C4CORE_INPUT_DEV)
	$(call safe_sync,$(C4CORE_DIR_PROJ),$(C4CORE_DEV),$(C4CORE_DIR_DEV))
c4core-export-src: $(C4CORE_OUTPUT_SRC)
	$(call safe_sync,$(C4CORE_DIR_SRC),$(C4CORE_SRC),$(C4CORE_DIR_PROJ))
c4core-export-dev: $(C4CORE_OUTPUT_DEV)
	$(call safe_sync,$(C4CORE_DIR_DEV),$(C4CORE_DEV),$(C4CORE_DIR_PROJ))
