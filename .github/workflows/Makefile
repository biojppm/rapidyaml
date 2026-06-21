include ../../proj/common.mk
include ../../proj/ys.mk

SOURCE_FILES := $(wildcard *.ys)
INPUT_FILES := $(wildcard */*) $(SOURCE_FILES)
TARGET_FILES := $(SOURCE_FILES:%.ys=%.yml)

export YSPATH := $(shell pwd -P)/ys


build: $(TARGET_FILES)


test: force build
	@git diff --exit-code $(TARGET_FILES) && \
	  echo -e '\nPASS - No normative changes to .github/workflows/*.yml'


DIFF_ORIG_COMMIT ?= HEAD
diff:
	@for s in $(SOURCE_FILES); do \
	  s=$${s##*/}; \
	  t=../workflows/$${s%.yml}; \
	  [[ $$(git show $(DIFF_ORIG_COMMIT):.github/workflows/.$$t \
	          2>/dev/null) ]] && t=.$$t; \
	  diff -u --color=auto \
	    <(yq -P 'sort_keys(..)' \
	         -o=props <(git show $(DIFF_ORIG_COMMIT):.github/workflows/$$t) | \
		 grep -Ev '(^$$|^#)' \
	     ) \
	    <(yq -P 'sort_keys(..)' \
	         -o=props ../workflows/$$s | \
		 grep -Ev '(^$$|^#)'\
	     ); \
	  done


force:
	touch *.ys


%.yml: %.ys $(YS) $(INPUT_FILES)
	@if [ -f "$@" ] ; then chmod a+w $@ ; fi
	@echo "# DO NOT EDIT - GENERATED FROM $<" > $@
	@echo >> $@
	$(YS) -Y $< >> $@
	@chmod a-w $@
	@wc -lm --total=never $< $@ || wc -lm $< $@



stats:
	@echo "ys : $$(wc -lm *.ys)"
	@echo "yml: $$(wc -lm *.yml)"
