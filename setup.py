#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# SPDX-License-Identifier: MIT

import os
import shutil
import sys
import shlex

from pathlib import Path
from distutils import log as log_
from setuptools import setup, find_packages
from cmake_build_extension import BuildExtension, CMakeExtension

TOP_DIR = (Path(__file__).parent).resolve()

# where the Python library is actually found
PYTHON_DIR = "api/python"


def log(msg, *args, **kwargs):
    log_.info("rapidyaml: " + msg, *args, **kwargs)


def get_readme_for_python():
    with open(TOP_DIR / "README.md", "r", encoding="utf8") as fh:
        marker = "<!-- endpythonreadme -->"  # get everything up to this tag
        return fh.read().split(marker)[0]


def get_cmake_flags_environment():
    return shlex.split(os.environ.get("CMAKE_FLAGS", ""))


def get_cmake_flags():
    return [
        "-DBUILD_SHARED_LIBS:BOOL=ON",
        "-DRYML_DEV:BOOL=OFF",
        "-DRYML_BUILD_API:BOOL=ON",
        "-DRYML_DEFAULT_CALLBACKS:BOOL=ON",
        "-DRYML_DEFAULT_CALLBACK_USES_EXCEPTIONS:BOOL=ON",
        # Force cmake to use the Python interpreter we are currently
        # using to run setup.py
        "-DPython3_EXECUTABLE:FILEPATH=" + sys.executable,
    ] + get_cmake_flags_environment()


setup_kw = {
    # read the module description from the README.md file
    'long_description': get_readme_for_python(),
    'long_description_content_type': "text/markdown",
}


# read the package version when not in a git repository
VERSION_FILE = os.path.join(PYTHON_DIR, 'ryml', 'version.py')
if not (TOP_DIR / '.git').exists() and os.path.exists(VERSION_FILE):
    exec(open(VERSION_FILE).read())
    setup_kw['version'] = version
else:
    setup_kw['use_scm_version'] = {
        "version_scheme": "post-release",
        "local_scheme": "no-local-version",
        "write_to": VERSION_FILE,
    }


cmake_flags = get_cmake_flags()

print('Compiling with CMake flags:\n  ' + '\n  '.join(cmake_flags))

# define a CMake package
cmake_args = dict(
    name='ryml.ryml',
    install_prefix='',
    source_dir='',
    #cmake_component='python',
    cmake_configure_options=get_cmake_flags(),
)

try:
    ext = CMakeExtension(**cmake_args)
    log("Using standard CMakeExtension")
except TypeError:
    log("Using custom CMakeExtension")
    # If the CMakeExtension doesn't support `cmake_component` then we
    # have to do some manual cleanup.
    del cmake_args['cmake_component']
    ext = CMakeExtension(**cmake_args)


# use a custom BuildExtension to ensure extra files are removed
class _BuildExtension(BuildExtension):
    def build_extension(self, ext):
        BuildExtension.build_extension(self, ext)
        ext_dir = Path(self.get_ext_fullpath(ext.name)).parent.absolute()
        cmake_install_prefix = ext_dir / ext.install_prefix
        assert cmake_install_prefix.exists(), cmake_install_prefix
        try:
            def _cleanup(suffix):
                path = cmake_install_prefix / suffix
                if path.exists():
                    log("Removing everything under: %s", path)
                    shutil.rmtree(path)
                else:
                    log("%s not found", path)
            _cleanup("lib")
            _cleanup("include")
            _cleanup("cmake")
        except:
            log('Found following installed files:')
            for f in cmake_install_prefix.rglob("*"):
                log(' - %s', f)
            raise


log('Compiling with CMake cfg:\n  ' + '\n  '.join(ext.cmake_configure_options))

setup(
    name='rapidyaml',
    description='Rapid YAML - a library to parse and emit YAML, and do it fast',
    url='https://github.com/biojppm/rapidyaml',
    license='MIT',
    license_files=['LICENSE.txt'],
    author="Joao Paulo Magalhaes",
    author_email="dev@jpmag.me",
    # Package contents control
    cmdclass={"build_ext": _BuildExtension,},
    package_dir={"": PYTHON_DIR},
    packages=['ryml'],
    ext_modules=[ext],
    include_package_data=True,
    # Requirements
    python_requires=">=3.6",
    setup_requires=[
        'setuptools_scm',
        'setuptools-git',
        'setuptools',
    ],
    install_requires=['deprecation'],
    # Extra arguments
    **setup_kw,
)
