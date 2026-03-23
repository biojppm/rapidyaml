# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = _build
CLANG         ?= clang++
DOXYGEN       ?= doxygen #/home/jpmag/proj/doxygen/install/linux-x86_64-gxx14.2-Release/bin/doxygen
DOXYGEN_OPTS  ?= #-t
WARN_AS_ERR   ?= 1

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile doxy
	@echo "building from the makefile"
	$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	@echo "overwrite doxygen files"
	mkdir -p "$(BUILDDIR)"
	if [ -d "$(BUILDDIR)/html/doxygen" ] ; then rm -rf "$(BUILDDIR)/html/doxygen" ; fi
	cp -favr doxygen/html "$(BUILDDIR)/html/doxygen"

doxy:
	## use cmake to generate a compile database
	#cmake -S .. -B "$(BUILDDIR)/proj" -DCMAKE_CXX_COMPILER=$(CLANG) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	#ln -fs "$(BUILDDIR)/proj/compile_commands.json" .
	## finally, run doxygen
	if [ "$(WARN_AS_ERR)" -ne 0 ] ; then sed "s/WARN_AS_ERROR.*= .*/WARN_AS_ERROR = FAIL_ON_WARNINGS_PRINT/g" -i Doxyfile ; fi
	$(DOXYGEN) $(DOXYGEN_OPTS) Doxyfile
	if [ "$(WARN_AS_ERR)" -ne 0 ] ; then sed "s/WARN_AS_ERROR.*= .*/WARN_AS_ERROR = NO/g" -i Doxyfile ; fi

clean:
	rm -rf doxygen/
	if [ ! -z "$(BUILDDIR)" ] ; then rm -rf "$(BUILDDIR)" ; fi
