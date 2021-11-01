# -*- coding: utf-8 -*-
# SPDX-License-Identifier: MIT

# Use bash even on Windows
SHELL := /bin/bash

# On Windows the activate script is stored in a different location.
ACTIVATE_SCRIPT := venv/bin/activate
ifeq ($(OS),Windows_NT)
ACTIVATE_SCRIPT := venv/Scripts/activate
endif

ACTIVATE=[[ -e $(ACTIVATE_SCRIPT) ]] && source $(ACTIVATE_SCRIPT);

clean:
	rm -rf dist *.egg-info
	rm -rf ../../build ../../.egg*
	rm -rf ryml/*.so ryml/ryml.py ryml/include ryml/lib

.PHONY: clean

venv-clean:
	rm -rf venv

.PHONY: venv-clean

$(ACTIVATE_SCRIPT): requirements.txt Makefile
	make venv
	@touch $(ACTIVATE_SCRIPT)

venv:
	virtualenv --python=python3 --always-copy venv
	# Packaging tooling.
	${ACTIVATE} pip install -U pip
	# Setup requirements.
	${ACTIVATE} pip install -v -r requirements.txt
	${ACTIVATE} pip install -v -e ../..
	@${ACTIVATE} python -c "from ryml.version import version as v; print('Installed version:', v)"

.PHONY: venv

build-sdist: | $(ACTIVATE_SCRIPT)
	${ACTIVATE} (cd ../..; python -m build --sdist --outdir $(PWD)/dist)

.PHONY: build-sdist

build-wheel: | $(ACTIVATE_SCRIPT)
	rm -rf dist
	$(MAKE) build-sdist
	@ls -l dist/*.tar.gz
	${ACTIVATE} pip wheel -v dist/*.tar.gz --wheel-dir $(PWD)/dist

.PHONY: build-wheel

build:
	rm -rf build dist
	$(MAKE) build-sdist
	$(MAKE) build-wheel

.PHONY: build

# PYPI_TEST = --repository-url https://test.pypi.org/legacy/
PYPI_TEST = --repository testpypi

upload-test: | $(ACTIVATE_SCRIPT)
	make clean
	make build-sdist
	${ACTIVATE} twine upload ${PYPI_TEST} dist/*

.PHONY: upload-test

upload: | $(ACTIVATE_SCRIPT)
	make clean
	make build-sdist
	${ACTIVATE} twine upload --verbose dist/*

.PHONY: upload

check: | $(ACTIVATE_SCRIPT)
	make clean
	make build-wheel
	${ACTIVATE} twine check dist/*.whl

.PHONY: check

install: | $(ACTIVATE_SCRIPT)
	${ACTIVATE} python setup.py install

.PHONY: install

test: | $(ACTIVATE_SCRIPT)
	${ACTIVATE} python -m nose ryml/tests

.PHONY: test

version: | $(ACTIVATE_SCRIPT)
	${ACTIVATE} python setup.py --version

.PHONY: version
