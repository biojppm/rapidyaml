SHELL := bash

RYML_DIR = $(shell cd .. ; pwd)
DIFF = colordiff

# c4core files used by ryml
C4CORE_SRC = \
    c4/base64.cpp \
    c4/base64.hpp \
    c4/blob.hpp \
    c4/charconv.hpp \
    c4/compiler.hpp \
    c4/cpu.hpp \
    c4/error.cpp \
    c4/error.hpp \
    c4/export.hpp \
    c4/ext/fast_float.hpp \
    c4/ext/fast_float_all.h \
    c4/format.cpp \
    c4/format.hpp \
    c4/gcc-4.8.hpp \
    c4/language.hpp \
    c4/memory_util.hpp \
    c4/memory_util.cpp \
    c4/platform.hpp \
    c4/preprocessor.hpp \
    c4/std/std_fwd.hpp \
    c4/std/string.hpp \
    c4/std/string_fwd.hpp \
    c4/std/vector.hpp \
    c4/std/vector_fwd.hpp \
    c4/substr_fwd.hpp \
    c4/substr.hpp \
    c4/utf.cpp \
    c4/utf.hpp \
    c4/version.cpp \
    c4/version.hpp \

# c4core files used by ryml in debug/test builds
C4CORE_SRC_DEV = \
    c4/dump.hpp \
    c4/memory_resource.hpp \
    c4/memory_resource.cpp \
    c4/span.hpp \
    c4/szconv.hpp \
    c4/types.hpp \


C4CORE_DIR_SRC = $(RYML_DIR)/ext/c4core/src
C4CORE_DIR_DST = $(RYML_DIR)/src

C4CORE_INPUT_FILES = $(C4CORE_SRC:%=$(C4CORE_DIR_SRC)/%)
C4CORE_OUTPUT_FILES = $(C4CORE_SRC:%=$(C4CORE_DIR_DST)/%)

# copy files using tar
# $1=src_dir
# $2=src_files
# $3=dst_dir
tarcopy = \
	cd $1 ; tar cf - $2 | ( cd $3 ; tar xvfp - )

update-src-c4core-to-ryml: $(C4CORE_INPUT_FILES)
	$(call tarcopy,$(C4CORE_DIR_SRC),$(C4CORE_SRC) $(C4CORE_SRC_DEV),$(C4CORE_DIR_DST))
update-src-ryml-to-c4core: $(C4CORE_OUTPUT_FILES)
	$(call tarcopy,$(C4CORE_DIR_DST),$(C4CORE_SRC) $(C4CORE_SRC_DEV),$(C4CORE_DIR_SRC))

c4core-check:
	for s in $(C4CORE_SRC) $(C4CORE_SRC_DEV); do \
	  src=$(C4CORE_DIR_SRC)/$$s ; \
	  dst=$(C4CORE_DIR_DST)/$$s ; \
	  set -x ; \
	  $(DIFF) -u $$src $$dst ; \
	done

c4core-amalgamate:
	@echo
