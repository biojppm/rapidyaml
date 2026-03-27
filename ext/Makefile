include ../proj/common.mk
include ./c4core.mk

# c4core files used by ryml
C4CORE_SRC := $(shell cat c4core.src.txt)
# c4core files used by ryml in dev builds (debug/test)
C4CORE_DEV := $(shell cat c4core.dev.txt)
# c4proj files
C4CORE_PROJ := $(shell cat c4core.proj.txt)

C4PROJ_DIR = ../proj/c4proj
C4CORE_DIR_SRC = c4core.src
C4CORE_DIR_DEV = c4core.dev
C4CORE_DIR_GIT = c4core

C4CORE_DIR_PROJ = $(C4CORE_DIR_GIT)/src

C4CORE_INPUT_SRC = $(C4CORE_SRC:%=$(C4CORE_DIR_PROJ)/%)
C4CORE_INPUT_DEV = $(C4CORE_DEV:%=$(C4CORE_DIR_PROJ)/%)
C4CORE_INPUT_PROJ = $(C4CORE_DEV:%=$(CORE_DIR_GIT)/cmake/%)

C4CORE_OUTPUT_SRC = $(C4CORE_SRC:%=$(C4CORE_DIR_SRC)/%)
C4CORE_OUTPUT_DEV = $(C4CORE_DEV:%=$(C4CORE_DIR_DEV)/%)
C4CORE_OUTPUT_PROJ = $(C4CORE_PROJ:%=$(C4PROJ_DIR)/%)


c4core-check: c4core-check-sync c4core-check-list c4core-clone-check
c4core-check-sync: c4core-check-sync-src c4core-check-sync-dev c4core-check-sync-proj
c4core-check-list: c4core-check-list-src c4core-check-list-dev c4core-check-list-proj
c4core-check-sync-src: c4core-clone
	@echo "check c4core list src..."
	set -e ; \
	for s in $(C4CORE_SRC); do \
	  ( $(COLORDIFF) -u $(C4CORE_DIR_PROJ)/$$s $(C4CORE_DIR_SRC)/$$s ) ; \
	done
	@echo "check c4core sync src: PASS"
c4core-check-sync-dev: c4core-clone
	@echo "check c4core list dev..."
	set -e ; \
	for s in $(C4CORE_DEV); do \
	  ( $(COLORDIFF) -u $(C4CORE_DIR_PROJ)/$$s $(C4CORE_DIR_DEV)/$$s ) ; \
	done
	@echo "check c4core sync dev: PASS"
c4core-check-sync-proj: c4core-clone
	@echo "check c4core list proj..."
	set -e ; \
	for s in $(C4CORE_PROJ); do \
	  ( $(COLORDIFF) -u $(C4CORE_DIR_GIT)/cmake/$$s $(C4PROJ_DIR)/$$s ) ; \
	done
	@echo "check c4core sync proj: PASS"
c4core-check-list-src: c4core-clone
	@echo "check c4core list src..."
	$(COLORDIFF) -u \
	    <(cat c4core.src.txt | sort) \
	    <(cd $(C4CORE_DIR_SRC) ; find . ! -type d | sed s:\./c4:c4:g | sort)
	@echo "check c4core list src: PASS"
c4core-check-list-dev: c4core-clone
	@echo "check c4core list dev..."
	$(COLORDIFF) -u \
	    <(cat c4core.dev.txt | sort) \
	    <(cd $(C4CORE_DIR_DEV) ; find . ! -type d | sed s:\./c4:c4:g | sort)
	@echo "check c4core list dev: PASS"
c4core-check-list-proj: c4core-clone
	@echo "check c4core list proj..."
	set -eo pipefail ; \
	cd $(C4PROJ_DIR) ; \
	existing_files="$$(find . ! -type d | sed s:^\./::g | sort)" ; \
        for s in $(C4CORE_PROJ); do \
	    echo "$$existing_files" | grep "^$$s$$" >/dev/null || echo "missing: $$s"; \
	done ; \
	cd -
	@echo "check c4core list proj: PASS"


c4core-import: c4core-import-src c4core-import-dev c4core-import-proj
c4core-export: c4core-export-src c4core-export-dev c4core-export-proj

c4core-import-src: $(C4CORE_INPUT_SRC)
	$(call mk_safe_sync,$(C4CORE_SRC),$(C4CORE_DIR_PROJ),$(C4CORE_DIR_SRC))
c4core-import-dev: $(C4CORE_INPUT_DEV)
	$(call mk_safe_sync,$(C4CORE_DEV),$(C4CORE_DIR_PROJ),$(C4CORE_DIR_DEV))
c4core-import-proj: $(C4CORE_INPUT_PROJ)
	$(call mk_safe_sync,$(C4CORE_PROJ),$(C4CORE_DIR_GIT)/cmake,$(C4PROJ_DIR))

c4core-export-src: $(C4CORE_OUTPUT_SRC)
	$(call mk_safe_sync,$(C4CORE_SRC),$(C4CORE_DIR_SRC),$(C4CORE_DIR_PROJ))
c4core-export-dev: $(C4CORE_OUTPUT_DEV)
	$(call mk_safe_sync,$(C4CORE_DEV),$(C4CORE_DIR_DEV),$(C4CORE_DIR_PROJ))
c4core-export-proj: $(C4CORE_OUTPUT_PROJ)
	$(call mk_safe_sync,$(C4CORE_PROJ),$(C4PROJ_DIR),$(CORE_DIR_GIT)/cmake)


$(C4CORE_INPUT_SRC): c4core-clone
$(C4CORE_INPUT_DEV): c4core-clone
$(C4CORE_INPUT_PROJ): c4core-clone

$(C4CORE_DIR_GIT):
	$(call mk_git_clone,$(C4CORE_DIR_GIT),$(C4CORE_TAG),"$(C4CORE_REPO)")

c4core-clone: $(C4CORE_DIR_GIT) c4core-clone-check
c4core-check-clone: c4core-clone-check
c4core-clone-check: $(C4CORE_DIR_GIT)
	@set -e ; \
	currsha=$$(git -C $(C4CORE_DIR_GIT) rev-parse HEAD) ; \
	currtag=$$(git -C $(C4CORE_DIR_GIT) tag --points-at HEAD || echo -n) ; \
	echo "c4core sha: $$currsha" ; \
	echo "c4core tag: $$currtag" ; \
	if [ "$$currtag" != "$(C4CORE_TAG)" ] && [ "$$currsha" != "$(C4CORE_TAG)" ] ; then \
	    echo "$(C4CORE_DIR_GIT) has a different version:" ; \
	    echo "    expected=$(C4CORE_TAG)" ; \
	    echo "    actual tag=$$currtag" ; \
	    echo "    actual sha=$$currsha" ; \
	    exit 1 ; \
	fi ; \
	echo "c4core tag ok! $(C4CORE_TAG)"
