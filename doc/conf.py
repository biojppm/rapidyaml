# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
import os

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'rapidyaml'
copyright = '2018-2024 Joao Paulo Magalhaes <dev@jpmag.me>'
author = 'Joao Paulo Magalhaes <dev@jpmag.me>'
release = '0.10.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "myst_parser",
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'doxygen', 'Thumbs.db', '.DS_Store']

# Tell sphinx what the primary language being documented is.
primary_domain = 'cpp'

# Tell sphinx what the pygments highlight language should be.
highlight_language = 'cpp'


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

#html_theme = 'alabaster'
html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']

exclude_patterns += ["doxy_*.md"]

# ----------------------------------------------------------------------------
# Overwrite with the Doxygen documentation
# https://stackoverflow.com/a/41199722

"""
exclude_patterns += ["main.md", "include/*rst"]
import subprocess
this_dir = os.path.dirname(__file__)
subprocess.check_call(['doxygen', 'Doxyfile'], cwd=this_dir)
html_extra_path = ['./doxygen/html']
"""


# ----------------------------------------------------------------------------
# Setup the breathe extension
# https://breathe.readthedocs.io/en/latest/

# disabled as it was not working well with the groups
"""
extensions.append("breathe")
this_dir = os.path.dirname(__file__)
xml_dir = os.path.join(this_dir, "doxygen/xml")
assert os.path.exists(xml_dir)
breathe_projects = {
    "rapidyaml": xml_dir
}
breathe_default_project = "rapidyaml"
"""


# ----------------------------------------------------------------------------
# Setup the doxyrest extension
# https://vovkos.github.io/doxyrest/manual/basic.html
# https://github.com/vovkos/doxyrest

'''
# To get the binary:
# git clone --recursive https://github.com/vovkos/doxyrest_b
# cd doxyrest_b
# cmany i

# put the doxyrest dir in the path
import shutil
import sys
doxyrest_exe = shutil.which('doxyrest')
doxyrest_dir = os.path.abspath(os.path.dirname(os.path.dirname(doxyrest_exe)))
doxyrest_sphinx_dir = os.path.join(doxyrest_dir, "share/doxyrest/sphinx")
sys.path.insert(1, doxyrest_sphinx_dir)

extensions += ['doxyrest', 'cpplexer']

# commands to run
"""
set -xe ; \
cd ~/proj/rapidyaml ; \
cmany c -t Debug -c clang++ -V RYML_DEV=OFF -V RYML_BUILD_TESTS=ON ; \
cp -fav build/linux-x86_64-clangxx16.0-Debug/compile_commands.json doc ; \
cd doc ; \
make doxy ; \
export PATH=/home/jpmag/proj/doxyrest_b/install/linux-x86_64-gxx13.2-Release/bin:$PATH ; \
doxyrest -h ; \
doxyrest -c doxyrest-config.lua ; \
make html
"""
'''


# ----------------------------------------------------------------------------
# Setup the exhale extension
# https://exhale.readthedocs.io/en/latest/quickstart.html

# disabled as it was just not working will
"""
#extensions.append("exhale")
exhale_args = {
    # These arguments are required
    "containmentFolder":     "./api",
    "rootFileName":          "library_root.rst",
    "doxygenStripFromPath":  "..",
    # Heavily encouraged optional argument (see docs)
    "rootFileTitle":         "Library API",
    # Suggested optional arguments
    "createTreeView":        True,
    # TIP: if using the sphinx-bootstrap-theme, you need
    # "treeViewIsBootstrap": True,
    "exhaleExecutesDoxygen": True,
    "exhaleDoxygenStdin":    "INPUT = ../src"
}
"""
