include ./common.mk
include ./ys.mk


c4core-check:
	$(MAKE) -C ../tools/ext c4core-check
c4core-import:
	$(MAKE) -C ../tools/ext c4core-import
c4core-export:
	$(MAKE) -C ../tools/ext c4core-export


workflows:
	$(MAKE) -C ../.github/workflows-in
workflows-test:
	$(MAKE) -C ../.github/workflows-in test
