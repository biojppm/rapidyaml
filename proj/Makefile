# this makefile contains development-related targets

include ./common.mk
include ./ys.mk


c4core-check:
	$(MAKE) -C ../ext c4core-check
c4core-import:
	$(MAKE) -C ../ext c4core-import
c4core-export:
	$(MAKE) -C ../ext c4core-export


workflows:
	$(MAKE) -C ../.github/workflows-in
workflows-test:
	$(MAKE) -C ../.github/workflows-in test


presets: ../CMakePresets.json
presets-test: presets-force presets
presets-force:
	touch ../CMakePresets.ys
../CMakePresets.json: ../CMakePresets.ys $(YS)
	@( chmod a+w $@ || echo >/dev/null )
	$(YS) --json $< > $@
	@chmod a-w $@
