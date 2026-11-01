include ../proj/common.mk


SUFFIX ?= .$(RYML_ID)
DST_DIR ?= amalgamate


.PHONY: amalg amalgamate
amalg amalgamate: singlehdr singlesrc


# function to create all amalgamations
# $1=sfx
# $2=common options
amalg_all = \
	( \
	  name=$(DST_DIR)/rapidyaml$(SUFFIX).$1 ; \
	  set -xe ; \
	  mkdir -p $(DST_DIR) ; \
	  $(PYTHON) amalgamate.py $2 -- $$name ; \
	  $(PYTHON) amalgamate.py $2 --no-c4core -- $$name.noc4core ; \
	  $(PYTHON) amalgamate.py $2 --no-fastfloat -- $$name.nofastfloat ; \
	  $(PYTHON) amalgamate.py $2 --fastfloat_sys -- $$name.fastfloatsys ; \
	  $(PYTHON) amalgamate.py $2 --events ints --events ints_utils -- $$name.ints ; \
	)


singlehdr:
	@$(call amalg_all,singlehdr,--type single_hdr)


singlesrc:
	@$(call amalg_all,singlesrc,--type single_src)
