# yamlscript utils

YS_VERSION := 0.1.88
YS_PREFIX := $(RYML_DIR)/build/ys/$(YS_VERSION)
YS := $(YS_PREFIX)/bin/ys-$(YS_VERSION)
export PATH := $(YS_PREFIX)/bin:$(PATH)

# Auto install a specific version of ys
install-ys: $(YS)
$(YS):
	mkdir -p $(YS_PREFIX)
	curl -s https://yamlscript.org/install | \
	  BIN=1 VERSION=$(YS_VERSION) PREFIX=$(YS_PREFIX) bash

# function to generate an output file from a ys file
# usage: $(call ys_gen_file,ys_args,input.ys,output_file)
ys_gen_file = \
	if [ -f "$3" ] ; then chmod a+w "$3" ; fi ; \
	echo -e '\# DO NOT EDIT - GENERATED FROM $2\n' > $3 ; \
	( set -x ; $(YS) $1 "$2" >> "$3" ) ; \
	chmod a-w "$3"

# usage: $(call ys_check_file,output_file(s))
ys_check_file = \
	git diff --exit-code $1 && echo -e '\n$1 - PASS'

# usage: $(call ys_wc_file,input.ys,output_file)
ys_wc_file = \
	@wc -lm --total=never $1 $2 || wc -lm $1 $2
