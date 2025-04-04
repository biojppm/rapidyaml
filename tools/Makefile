SHELL := bash

RYML_DIR = $(shell cd .. ; pwd)
DIFF = colordiff

# c4core files used by ryml
C4CORE_SRC = \
        c4/base64.cpp \
        c4/base64.hpp \
        c4/charconv.hpp \
        c4/compiler.hpp \
        c4/cpu.hpp \
        c4/export.hpp \
        c4/format.cpp \
        c4/format.hpp \
        c4/gcc-4.8.hpp \
        c4/language.hpp \
        c4/memory_util.hpp \
        c4/memory_util.cpp \
        c4/platform.hpp \
        c4/substr_fwd.hpp \
        c4/substr.hpp \
        c4/utf.cpp \
        c4/utf.hpp \
        c4/version.cpp \
        c4/version.hpp \

# c4core files used by ryml in debug/test builds
C4CORE_SRC_DEV = \
        c4/dump.hpp \
        c4/error.cpp \
        c4/error.hpp \


C4CORE_DIR_SRC = $(RYML_DIR)/ext/c4core/src
C4CORE_DIR_DST = $(RYML_DIR)/src

C4CORE_INPUT_FILES = $(C4CORE_SRC:%=$(C4CORE_DIR_SRC)/%)
C4CORE_OUTPUT_FILES = $(C4CORE_SRC:%=$(C4CORE_DIR_DST)/%)


c4core-update: $(C4CORE_OUTPUT_FILES)

c4core-check:
	for s in $(C4CORE_SRC) $(C4CORE_SRC_DEV); do \
	  src=$(C4CORE_DIR_SRC)/$$s ; \
	  dst=$(C4CORE_DIR_DST)/$$s ; \
	  set -x ; \
	  $(DIFF) -u $$src $$dst ; \
	done

c4core-amalgamate:
	@echo

$(C4CORE_OUTPUT_FILES): $(C4CORE_INPUT_FILES)
	cp -fav $(C4CORE_INPUT_FILES) $(C4CORE_DIR_DST)/.
