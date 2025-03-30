# this makefile contains development-related targets

include ./common.mk
include ./ys.mk


check: c4core-check workflows-check presets-check


c4core-check:
	$(MAKE) -C ../ext c4core-check
c4core-import:
	$(MAKE) -C ../ext c4core-import
c4core-export:
	$(MAKE) -C ../ext c4core-export


workflows:
	$(MAKE) -C ../.github/workflows
workflows-check:
	$(MAKE) -C ../.github/workflows test


presets: ../CMakePresets.json
presets-check: presets-force presets
	$(GIT) diff --exit-code --color=auto ../CMakePresets.json
presets-force:
	touch ../CMakePresets.ys
../CMakePresets.json: ../CMakePresets.ys $(YS)
	@( chmod a+w $@ || echo >/dev/null )
	$(YS) --json $< > $@
	@chmod a-w $@
