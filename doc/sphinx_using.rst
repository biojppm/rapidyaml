Using ryml in your project
==========================


Note that ryml uses submodules. Take care to use the `--recursive` flag
when cloning the repo, to ensure ryml's submodules are checked out as well:

.. code:: bash

   git clone --recursive https://github.com/biojppm/rapidyaml

If you omit `--recursive`, after cloning you
will have to do `git submodule update --init --recursive`
to ensure ryml's submodules are checked out.


Quickstart build samples
------------------------

These samples show different ways of getting ryml into your application.
All the samples use `the same quickstart executable
source <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/quickstart.cpp>`__, but are built in different ways,
showing several alternatives to integrate ryml into your project. We
also encourage you to refer to the `quickstart
docs <doxygen/group__doc__quickstart.html>`__, which extensively covers
the functionality that you may want out of ryml.

If you're in a rush to start, try this:

.. include:: sphinx_try_quickstart.rst

For alternative ways, continue reading this section.

Each sample brings a ``run.sh`` script with the sequence of commands
required to successfully build and run the application (this is a bash
script and runs in Linux and MacOS, but it is also possible to run in
Windows via Git Bash or the WSL). Click on the links below to find out
more about each sample:


+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+------------------------------------------------------------------------------------------------------------------+
| Sample name                                                                                              | ryml is part of build?           | cmake file                                                                                                        | commands                                                                                                         |
+==========================================================================================================+==================================+===================================================================================================================+==================================================================================================================+
| `singleheader <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheader>`_                  | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheader/CMakeLists.txt>`_          | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheader/run.sh>`_                         |
|                                                                                                          | | ryml brought as a single       |                                                                                                                   |                                                                                                                  |
|                                                                                                          | | header, not as a library       |                                                                                                                   |                                                                                                                  |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+------------------------------------------------------------------------------------------------------------------+
| `singleheaderlib <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheaderlib>`_            | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheaderlib/CMakeLists.txt>`_       | | `run_shared.sh <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheaderlib/run_shared.sh>`_      |
|                                                                                                          | | ryml brought as library        |                                                                                                                   | | `run_static.sh <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheaderlib/run_static.sh>`_      |
|                                                                                                          | | but from the single header     |                                                                                                                   |                                                                                                                  |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+------------------------------------------------------------------------------------------------------------------+
| `singleheader-ints <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheader-ints>`_        | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheader-ints/CMakeLists.txt>`_     | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheader-ints/run.sh>`_                    |
|                                                                                                          | | ryml brought as a single       |                                                                                                                   |                                                                                                                  |
|                                                                                                          | | header, not as a library       |                                                                                                                   |                                                                                                                  |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+------------------------------------------------------------------------------------------------------------------+
| `singleheaderlib-ints <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheaderlib-ints>`_  | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheaderlib-ints/CMakeLists.txt>`_  | | `run_shared.sh <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheaderlib-ints/run_shared.sh>`_ |
|                                                                                                          | | ryml brought as library        |                                                                                                                   | | `run_static.sh <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/singleheaderlib-ints/run_static.sh>`_ |
|                                                                                                          | | but from the single header     |                                                                                                                   |                                                                                                                  |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+------------------------------------------------------------------------------------------------------------------+
| `add_subdirectory <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/add_subdirectory>`_          | **yes**                          | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/add_subdirectory/CMakeLists.txt>`_      | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/add_subdirectory/run.sh>`_                     |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+------------------------------------------------------------------------------------------------------------------+
| `fetch_content <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/fetch_content>`_                | **yes**                          | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/fetch_content/CMakeLists.txt>`_         | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/fetch_content/run.sh>`_                        |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+------------------------------------------------------------------------------------------------------------------+
| `find_package <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/find_package>`_                  | | **no**                         | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/find_package/CMakeLists.txt>`_          | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/find_package/run.sh>`_                         |
|                                                                                                          | | needs prior install or package |                                                                                                                   |                                                                                                                  |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+------------------------------------------------------------------------------------------------------------------+


As a single-header
------------------

ryml is provided chiefly as a cmake library project, but it can also
be used as a single header file, and there is a `tool to amalgamate
<https://github.com/biojppm/rapidyaml/blob/v0.10.0/tools/amalgamate.py>`__
the code into a single header file. The amalgamated header file is
provided with each release, but you can also generate a customized
file suiting your particular needs (or commit):


.. code-block:: console

   [user@host rapidyaml]$ python3 tools/amalgamate.py -h
   usage: amalgamate.py [-h] [--c4core | --no-c4core] [--fastfloat | --no-fastfloat]
                        [--stl | --no-stl]
                        [-e {tree,testsuite,int,all,none} [{tree,testsuite,int,all,none} ...]]
                        [output]

   positional arguments:
     output                output file. defaults to stdout

   options:
     -h, --help            show this help message and exit
     --c4core              amalgamate c4core together with ryml. this is the default.
     --no-c4core           amalgamate c4core together with ryml. the default is
                           --c4core.
     --fastfloat           enable fastfloat library. this is the default.
     --no-fastfloat        enable fastfloat library. the default is --fastfloat.
     --stl                 enable stl interop. this is the default.
     --no-stl              enable stl interop. the default is --stl.
     -e, --events {tree,testsuite,int,all,none} [{tree,testsuite,int,all,none} ...]
                           Specify which event handlers to include. Possible values
                           are: 'tree': (the default) enable the normal ryml event
                           handler to create the tree, and additionally the Tree, Node,
                           parser and emitter utilities; if this is not enabled, none
                           of these components will be included in the amalgamated
                           file. 'testsuite': enable the (extra) YAML test suite event
                           handler. 'int': enable the (extra) integer-based event
                           handler. 'all': enable all event handlers. 'none': disable
                           all event handlers. The default is tree.

Note that you can select which event handlers are to be included in the
amalgamated header. This is useful for example when using only the int
event parsing (withou the ryml tree/node/parse/emit) for a programming
language or special application.

The amalgamated header file contains all the function declarations and
definitions. To use it in the project, ``#include`` the header at will
in any header or source file in the project, but in one source file,
and only in that one source file, ``#define`` the macro
``RYML_SINGLE_HDR_DEFINE_NOW`` **before including the header**. This
will enable the function definitions. For example:

.. code-block:: console

   // foo.h
   #include <ryml_all.hpp>
   
   // foo.cpp
   // ensure that foo.h is not included before this define!
   #define RYML_SINGLE_HDR_DEFINE_NOW
   #include <foo.h>

If you wish to package the single header into a shared library, then
you will need to define the preprocessor symbol ``RYML_SHARED`` during
compilation.


From a package manager
----------------------

ryml is available in most package managers (thanks to all the
contributors!) and linux distributions.

.. note::
   
   These packages are maintained downstream of this repository, so if
   you have issues with the package, file a report with the respective
   package maintainer.

Here's a quick roundup (possibly outdated):

* Package managers

  * `conan <https://conan.io/center/recipes/rapidyaml>`_
    
  * `vcpkg <https://vcpkg.io/en/packages.html>`_: ``vcpkg install ryml``
    
  * `PyPI <https://pypi.org/project/rapidyaml/>`_
    
* Linux distributions:
  
  * Arch Linux/Manjaro:
    
    * `rapidyaml (aarch64) <https://archlinuxarm.org/packages/aarch64/rapidyaml>`_
      
    * `rapidyaml-git (AUR) <https://aur.archlinux.org/packages/rapidyaml-git/>`_
      
    * `python-rapidyaml-git (AUR) <https://aur.archlinux.org/packages/python-rapidyaml-git/>`_
      
  * `Fedora Linux <https://getfedora.org/>`_/`EPEL <https://docs.fedoraproject.org/en-US/epel/>`_:
    
    * ``dnf install rapidyaml-devel``
      
    * ``dnf install python3-rapidyaml``
      
  * `Gentoo <https://packages.gentoo.org/packages/dev-cpp/rapidyaml>`_
    
  * `OpenSuse <https://build.openbuildservice.org/package/show/Emulators/rapidyaml>`_
    
  * `Slackbuilds <https://slackbuilds.org/repository/15.0/libraries/rapidyaml/>`_
    
  * `AltLinux <https://packages.altlinux.org/en/sisyphus/srpms/rapidyaml/3006055151670528141>`_
  
Although package managers are very useful for quickly getting up to
speed, the advised way is still to bring ryml as a submodule of your
project, building both together. This makes it easy to track any
upstream changes in ryml. Also, ryml is small and quick to build, so
there's not much of a cost for building it with your project.


As a library
------------

The single header file is a good approach to quickly try the library,
but if you wish to make good use of CMake and its tooling ecosystem,
(and get better compile times), then ryml has you covered.

As with any other cmake library, you have the option to integrate ryml into
your project's build setup, thereby building ryml together with your
project, or -- prior to configuring your project -- you can have ryml
installed either manually or through package managers.

Currently `cmake <https://cmake.org/>`_ is required to build ryml; we
recommend a recent cmake version, at least 3.13.


Forcing ryml to use a different c4core version
----------------------------------------------

ryml is strongly coupled to c4core, and this is reinforced by the fact
that c4core is a submodule of the current repo. However, it is still
possible to use a c4core version different from the one in the repo (of
course, only if there are no incompatibilities between the versions).
You can find out how to achieve this by looking at the
`custom_c4core sample <https://github.com/biojppm/rapidyaml/blob/v0.10.0/samples/custom_c4core/CMakeLists.txt>`__.


CMake build settings for ryml
=============================

The following cmake variables can be used to control the build behavior
of ryml:

-  ``RYML_WITH_TAB_TOKENS=ON/OFF``. Enable/disable support for tabs as
   valid container tokens after ``:`` and ``-``. Defaults to ``OFF``,
   because this may cost up to 10% in processing time.
-  ``RYML_DEFAULT_CALLBACKS=ON/OFF``. Enable/disable ryml’s default
   implementation of error and allocation callbacks. Defaults to ``ON``.
-  ``RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS=ON/OFF`` - Enable/disable the
   same-named macro, which will make the default error handler provided
   by ryml throw a ``std::runtime_error`` exception.
-  ``RYML_USE_ASSERT`` - enable assertions in the code regardless of
   build type. This is disabled by default. Failed assertions will
   trigger a call to the error callback.
-  ``RYML_STANDALONE=ON/OFF``. ryml uses
   `c4core <https://github.com/biojppm/c4core>`__, a C++ library with
   low-level multi-platform utilities for C++. When
   ``RYML_STANDALONE=ON``, c4core is incorporated into ryml as if it is
   the same library. Defaults to ``ON``.
- ``RYML_INSTALL=ON/OFF``. enable/disable install target. Defaults to ``ON``.

If you’re developing ryml or just debugging problems with ryml itself,
the following cmake variables can be helpful:

- ``RYML_DEV=ON/OFF``: a bool variable which enables development targets
  such as unit tests, benchmarks, etc. Defaults to ``OFF``.
- ``RYML_DBG=ON/OFF``: a bool variable which enables verbose prints from
  parsing code; can be useful to figure out parsing problems. Defaults
  to ``OFF``.
