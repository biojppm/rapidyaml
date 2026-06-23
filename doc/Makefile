# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = _build
CLANG         ?= clang++

DOXYGEN_OPTS   ?= #-t
DOXY_VERSION   ?= 1.15.0
DOXY_BASE      ?= /tmp
DOXY_DIR       ?= $(DOXY_BASE)/doxy-$(DOXY_VERSION)
DOXY_FORCE_VER ?= 0
ifeq ($(DOXY_FORCE_VER),0)
DOXYGEN        ?= doxygen
else
DOXYGEN        ?= $(DOXY_DIR)/bin/doxygen
endif

.PHONY: help Makefile doxy doxy_test _doxy_warnfail _doxy_warnpass doxy_install


# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)


# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile doxy
	@echo "building from the makefile"
	$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	@echo "overwrite doxygen files"
	mkdir -p "$(BUILDDIR)"
	if [ -d "$(BUILDDIR)/html/doxygen" ] ; then rm -rf "$(BUILDDIR)/html/doxygen" ; fi
	cp -favr doxygen/html "$(BUILDDIR)/html/doxygen"


doxy: doxy_install
	@## use cmake to generate a compile database
	@#cmake -S .. -B "$(BUILDDIR)/proj" -DCMAKE_CXX_COMPILER=$(CLANG) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	@#ln -fs "$(BUILDDIR)/proj/compile_commands.json" .
	@## finally, run doxygen
	$(DOXYGEN) --version
	$(DOXYGEN) $(DOXYGEN_OPTS) Doxyfile
	@echo "doxy: `pwd`/doxygen/html/index.html"

doxy_test:
	$(MAKE) --keep-going _doxy_warnfail doxy _doxy_warnpass
_doxy_warnfail:
	sed "s/WARN_AS_ERROR\(.*\)= .*/WARN_AS_ERROR\1= FAIL_ON_WARNINGS_PRINT/g" -i Doxyfile
_doxy_warnpass:
	sed "s/WARN_AS_ERROR\(.*\)= .*/WARN_AS_ERROR\1= NO/g" -i Doxyfile

clean:
	rm -rf doxygen/
	if [ ! -z "$(BUILDDIR)" ] ; then rm -rf "$(BUILDDIR)" ; fi

ifeq ($(DOXY_FORCE_VER),0)
doxy_install:
else
doxy_install: $(DOXY_DIR)/bin/doxygen
endif
doxy_force_install: $(DOXY_DIR)/bin/doxygen
$(DOXY_DIR)/bin/doxygen:
	set -xeu ; \
	name=$(DOXY_DIR) ; \
	ver=$(DOXY_VERSION) ; \
	uver=`echo $$ver | sed "s:\.:_:g"` ; \
	dst=`dirname $$name` ; \
	url="https://github.com/doxygen/doxygen/releases/download/Release_$$uver/doxygen-$$ver.linux.bin.tar.gz" ; \
	( \
	  pwd ; \
	  cd $$dst ; \
	  pwd ; \
	  wget -nv $$url ; \
	  tar xfz `basename $$url` ; \
	  ls -lFhp | grep dox ; \
	  mv -v doxygen-$$ver $$name ; \
	)
