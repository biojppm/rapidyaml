M := .cache/makes
$(shell [ -d $M ] || git clone -q https://github.com/makeplus/makes $M)
include $M/init.mk
YS-VERSION := 0.2.8
include $M/ys.mk
include $M/clean.mk
include $M/shell.mk

INPUT-FILES := $(wildcard */*)
SOURCE-FILES := $(wildcard *.ys)
TARGET-FILES := $(SOURCE-FILES:%.ys=../workflows/%.yml)

export YSPATH := $(ROOT)/ys


build: $(TARGET-FILES)


test: force build
	@git diff --exit-code $(TARGET-FILES) && \
	  echo -e '\nPASS - No normative changes to .github/workflows/*.yml'


DIFF-ORIG-COMMIT ?= HEAD
diff:
	@for s in $(SOURCE-FILES); do \
	  s=$${s##*/}; \
	  t=../workflows/$${s%.yml}; \
	  [[ $$(git show $(DIFF-ORIG-COMMIT):.github/workflows/.$$t \
	          2>/dev/null) ]] && t=.$$t; \
	  diff -u --color=auto \
	    <(yq -P 'sort_keys(..)' \
	         -o=props <(git show $(DIFF-ORIG-COMMIT):.github/workflows/$$t) | \
		 grep -Ev '(^$$|^#)' \
	     ) \
	    <(yq -P 'sort_keys(..)' \
	         -o=props ../workflows/$$s | \
		 grep -Ev '(^$$|^#)'\
	     ); \
	  done


force:
	touch *.ys


../workflows/%.yml: %.ys $(YS) $(INPUT-FILES)
	@if [[ -f $@ ]] ; then chmod a+w $@ ; fi
	@echo "# DO NOT EDIT - GENERATED FROM .github/workflows-in/$<" > $@
	@echo >> $@
	$(YS) -Y $< >> $@
	@chmod a-w $@
	@wc -lm --total=never $< $@ || wc -lm $< $@


# Auto install a specific version of ys
install-ys: $(YS)
$(YS):


stats:
	@echo "ys : $$(wc -lm *.ys)"
	@echo "yml: $$(wc -lm ../workflows/*.yml)"
