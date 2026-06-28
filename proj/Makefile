# this makefile contains development-related targets

include ./common.mk
include ./ys.mk


check: c4core-check workflows-check doxy


c4core-check:
	$(MAKE) -C ../ext c4core-check
c4core-import:
	$(MAKE) -C ../ext c4core-import
c4core-export:
	$(MAKE) -C ../ext c4core-export


doxy:
	$(MAKE) -C ../doc doxy_test
sphinx:
	$(MAKE) -C ../doc doc


workflows:
	$(MAKE) -C ../.github/workflows
workflows-check:
	$(MAKE) -C ../.github/workflows test
