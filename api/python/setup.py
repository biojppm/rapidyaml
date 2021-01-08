#!/usr/bin/env python
from pathlib import Path
from setuptools import setup
from cmake_build_extension import BuildExtension, CMakeExtension

# define a CMake package
ext = CMakeExtension('ryml.ryml',
                     install_prefix='',
                     source_dir=str(Path("../../").absolute()),
                     cmake_component='python',
                     cmake_configure_options=[
                        "-DRYML_BUILD_API:BOOL=ON",
                        ])

setup(name='rapidyaml',
    version='1.0',
    packages=['ryml'],
    ext_modules=[ext],
    cmdclass=dict(build_ext=BuildExtension),
    author='Joao Paulo Magalhaes',
    url='https://github.com/biojppm/rapidyaml',
    description='This is a test',
    license='MIT',
    )
