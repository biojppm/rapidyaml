include ../proj/common.mk
include ./c4core.mk

# directory with temporary c4core clone
C4CORE_DIR_GIT = c4core

MANIFEST = ext/c4core.yml
SYNC_OPTS ?=
SYNC_TOOL = $(PYTHON) sync.py --root $(RYML_DIR) --manifest $(MANIFEST) $(SYNC_OPTS)

c4core-check: c4core-check-sync c4core-check-list c4core-clone-check

c4core-check-sync: c4core-clone
	$(SYNC_TOOL) check-sync

c4core-check-list: c4core-clone
	$(SYNC_TOOL) check-list

c4core-import: c4core-clone
	$(SYNC_TOOL) import

c4core-export: c4core-clone
	$(SYNC_TOOL) export

$(C4CORE_DIR_GIT):
	$(call mk_git_clone,$(C4CORE_DIR_GIT),$(C4CORE_TAG),"$(C4CORE_REPO)")

c4core-clone: $(C4CORE_DIR_GIT) c4core-clone-check
c4core-check-clone: c4core-clone-check
c4core-clone-check: $(C4CORE_DIR_GIT)
	@ \
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
