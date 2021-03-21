#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# SPDX-License-Identifier: MIT

import os
import shutil
import sys

from pathlib import Path
from distutils import log
from setuptools import setup
from setuptools.command.sdist import sdist as SdistCommand
from cmake_build_extension import BuildExtension, CMakeExtension

TOP_DIR = (Path(__file__).parent).resolve()

# Where the Python library is actually found.
PYTHON_DIR = "api/python"

setup_kw = {}

# Read in the package version when not in a git repository.
VERSION_FILE = os.path.join(PYTHON_DIR, 'ryml', 'version.py')
if not (TOP_DIR / '.git').exists() and os.path.exists(VERSION_FILE):
    exec(open(VERSION_FILE).read())
    setup_kw['version'] = version
else:
    setup_kw['use_scm_version']= {
        "version_scheme": "post-release",
        "local_scheme": "no-local-version",
        "write_to": VERSION_FILE,
    }

# Read in the module description from the README.md file.
README_FILE = TOP_DIR / "README.md"
if README_FILE.exists():
    with open(TOP_DIR / "README.md", "r") as fh:
        setup_kw['long_description'] = fh.read()
        setup_kw['long_description_content_type'] = "text/markdown"

# define a CMake package
cmake_args = dict(
    name='ryml.ryml',
    install_prefix='',
    source_dir='',
    cmake_component='python',
    cmake_configure_options=[
        "-DRYML_BUILD_API:BOOL=ON",
        # Force cmake to use the Python interpreter we are currently using to
        # run setup.py
        "-DPython3_EXECUTABLE:FILEPATH="+sys.executable,
    ],
)

try:
    ext = CMakeExtension(**cmake_args)
except TypeError:
    del cmake_args['cmake_component']
    ext = CMakeExtension(**cmake_args)

    # If the CMakeExtension doesn't support `cmake_component` then we have to
    # do some manual cleanup.
    _BuildExtension=BuildExtension
    class BuildExtension(_BuildExtension):
        def build_extension(self, ext):
            _BuildExtension.build_extension(self, ext)
            ext_dir = Path(self.get_ext_fullpath(ext.name)).parent.absolute()
            cmake_install_prefix = ext_dir / ext.install_prefix
            assert cmake_install_prefix.exists(), cmake_install_prefix
            try:
                lib_path = cmake_install_prefix / "lib"
                assert lib_path.exists(), lib_path
                log.info("Removing everything under: %s", lib_path)
                shutil.rmtree(lib_path)

                inc_path = cmake_install_prefix / "include"
                assert inc_path.exists(), inc_path
                log.info("Removing everything under: %s", inc_path)
                shutil.rmtree(inc_path)

                # Windows only
                cm_path = cmake_install_prefix / "cmake"
                if cm_path.exists():
                    log.info("Removing everything under: %s", cm_path)
                    shutil.rmtree(cm_path)
            except:
                log.info('Found following installed files:')
                for f in cmake_install_prefix.rglob("*"):
                    log.info(' - %s', f)
                raise

setup(
    # Package human readable information
    name='rapidyaml',
    #author='Joao Paulo Magalhaes',
    description='Rapid YAML - a library to parse and emit YAML, and do it fast.',
    url='https://github.com/biojppm/rapidyaml',
    license='MIT',
    license_files=['LICENSE.txt'],
    # Package contents control
    cmdclass={
        "build_ext": BuildExtension,
    },
    package_dir={"": PYTHON_DIR},
    packages=['ryml'],
    ext_modules=[ext],
    include_package_data=True,
    # Requirements
    python_requires=">=3.7",
    setup_requires=['setuptools_scm'],
    # Extra arguments
    **setup_kw,
)
