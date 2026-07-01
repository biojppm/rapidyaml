Using ryml in your project
==========================



Quickstart build samples
------------------------

These samples show different ways of getting ryml into your application.
All the samples use `the same quickstart executable
source <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/quickstart.cpp>`__, but are built in different ways,
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


+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+
| Sample name                                                                                              | ryml is part of build?           | cmake file                                                                                                        | commands                                                                                                          |
+==========================================================================================================+==================================+===================================================================================================================+===================================================================================================================+
| `add_subdirectory <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/add_subdirectory>`_         | **yes**                          | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/add_subdirectory/CMakeLists.txt>`_     | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/add_subdirectory/run.sh>`_                     |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+
| `fetch_content <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/fetch_content>`_               | **yes**                          | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/fetch_content/CMakeLists.txt>`_        | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/fetch_content/run.sh>`_                        |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+
| `find_package <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/find_package>`_                 | | **no**                         | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/find_package/CMakeLists.txt>`_         | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/find_package/run.sh>`_                         |
|                                                                                                          | | needs prior install or package |                                                                                                                   |                                                                                                                   |
|                                                                                                          |                                  |                                                                                                                   |                                                                                                                   |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+
| `singlesource <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singlesource>`_                 | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singlesource/CMakeLists.txt>`_         | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singlesource/run.sh>`_                         |
|                                                                                                          | | ryml brought as a single       |                                                                                                                   |                                                                                                                   |
|                                                                                                          | | source file + single header    |                                                                                                                   |                                                                                                                   |
|                                                                                                          | | file                           |                                                                                                                   |                                                                                                                   |
|                                                                                                          |                                  |                                                                                                                   |                                                                                                                   |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+
| `singleheader <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheader>`_                 | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheader/CMakeLists.txt>`_         | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheader/run.sh>`_                         |
|                                                                                                          | | ryml brought as a single       |                                                                                                                   |                                                                                                                   |
|                                                                                                          | | header, not as a library       |                                                                                                                   |                                                                                                                   |
|                                                                                                          |                                  |                                                                                                                   |                                                                                                                   |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+
| `singleheaderlib <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheaderlib>`_           | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheaderlib/CMakeLists.txt>`_      | | `run_shared.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheaderlib/run_shared.sh>`_      |
|                                                                                                          | | ryml brought as library        |                                                                                                                   | | `run_static.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheaderlib/run_static.sh>`_      |
|                                                                                                          | | but from the single header     |                                                                                                                   |                                                                                                                   |
|                                                                                                          |                                  |                                                                                                                   |                                                                                                                   |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+
| `singleheader-ints <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheader-ints>`_       | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheader-ints/CMakeLists.txt>`_    | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheader-ints/run.sh>`_                    |
|                                                                                                          | | ryml brought as a single       |                                                                                                                   |                                                                                                                   |
|                                                                                                          | | header, not as a library       |                                                                                                                   |                                                                                                                   |
|                                                                                                          |                                  |                                                                                                                   |                                                                                                                   |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+
| `singleheaderlib-ints <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheaderlib-ints>`_ | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheaderlib-ints/CMakeLists.txt>`_ | | `run_shared.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheaderlib-ints/run_shared.sh>`_ |
|                                                                                                          | | ryml brought as library        |                                                                                                                   | | `run_static.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/singleheaderlib-ints/run_static.sh>`_ |
|                                                                                                          | | but from the single header     |                                                                                                                   |                                                                                                                   |
|                                                                                                          |                                  |                                                                                                                   |                                                                                                                   |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+
| `ints_only <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/ints_only>`_                       | | **yes**                        | `CMakeLists.txt <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/ints_only/CMakeLists.txt>`_            | `run.sh <https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/ints_only/run.sh>`_                            |
|                                                                                                          | | but custom selection of ryml   |                                                                                                                   |                                                                                                                   |
|                                                                                                          | | files: only ints parsing       |                                                                                                                   |                                                                                                                   |
|                                                                                                          |                                  |                                                                                                                   |                                                                                                                   |
+----------------------------------------------------------------------------------------------------------+----------------------------------+-------------------------------------------------------------------------------------------------------------------+-------------------------------------------------------------------------------------------------------------------+


ryml is provided chiefly as a cmake library project, but it can also
be used as a single header file, and also as a single source+header file.

There is a tool to amalgamate the code into a single header
file. The amalgamated header file is provided with each release, but
you can also generate a customized file suiting your particular needs
(or commit). See below.


As a library
------------

The single-header or single-source approach is a good way to quickly
try the library, but the recommended way is still to use ryml as a
library. This is really easy to achieve with cmake and its tooling
ecosystem, and you also get better compile times.

As with any other cmake library, you have the option to integrate ryml
into your project's build setup, thereby building ryml together with
your project, or -- prior to configuring your project -- you can have
ryml installed either manually or through package managers. See the
samples above for different ways to accomplish this.

Currently `cmake <https://cmake.org/>`_ is required to build ryml; we
recommend a recent cmake version, at least 3.13.


As a single-header
------------------

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


As a single-source
------------------

The amalgamate tool can also be used to create a single source and
header pair. This has several advantages over the single-header
approach:

  - It is just as easy to copy into a project (two files instead of one)
  - Quicker to compile because the header file has less code
  - Does not require use of the ``RYML_SINGLE_HDR_DEFINE_NOW``

If you wish to package the single source into a shared library, then
you will need to define the preprocessor symbol ``RYML_SHARED`` during
compilation. Further, while building the ryml library you also need to
define ``RYML_EXPORTS``.


Amalgamate tool
---------------

There is a tool to amalgamate ryml in `tools/amalgamate.py
<https://github.com/biojppm/rapidyaml/blob/v0.15.2/tools/amalgamate.py>`__.
Note that the tool allows you to select different aspects of the library:

  - whether to amalgamate into a single-source, or a single-header
  - whether to amalgamate c4core code
  - whether to amalgamate fast-float code
  - whether to amalgamate STL serialization interop
  - which parse event handlers are to be included in the amalgamated
    code. This is useful for example when using only the int event
    parsing (without the ryml tree/node/parse/emit) for a programming
    language or special application.

Here's how to use the tool:

.. code-block:: console

   [user@host rapidyaml]$ python tools/amalgamate.py -h
   usage: amalgamate.py [-h] [--c4core | --no-c4core] [--c4core_dev | --no-c4core_dev]
                        [--fastfloat | --no-fastfloat] [--stl | --no-stl]
                        [--engine_hdr | --no-engine_hdr] [-t {single_hdr,single_src}]
                        [-e {tree,ints,ints_utils,ints_to_testsuite,all,none} [{tree,ints,ints_utils,ints_to_testsuite,all,none} ...]]
                        [output]
   
   positional arguments:
     output                output file. defaults to stdout
   
   options:
     -h, --help            show this help message and exit
     --c4core, --no-c4core
                           amalgamate c4core together with ryml. the default is --c4core
     --c4core_dev, --no-c4core_dev
                           amalgamate c4core (dev utilities) together with ryml. the
                           default is --no-c4core_dev
     --fastfloat, --no-fastfloat
                           enable fastfloat library. the default is --fastfloat
     --stl, --no-stl       enable stl interop. the default is --stl
     --engine_hdr, --no-engine_hdr
                           place the parse engine definition header on the header file
                           when amalgamating as single_src. the default is --no-
                           engine_hdr
     -t, --type {single_hdr,single_src}
                           Choose amalgamation type. Possible values are - 'single_hdr':
                           amalgamate everything (all .cpp+.hpp) files into a single .hpp
                           file. 'single_src': amalgamate all .cpp files into a single
                           .cpp file, and all .hpp files into a single .hpp file. The
                           default is single_hdr.
     -e, --events {tree,ints,ints_utils,ints_to_testsuite,all,none} [{tree,ints,ints_utils,ints_to_testsuite,all,none} ...]
                           Specify which event handlers to include. Possible values are -
                           'tree': (the default) enable the normal ryml event handler to
                           create the tree, and additionally the Tree, Node, parser and
                           emitter utilities; if this is not enabled, none of these
                           components will be included in the amalgamated file. 'ints':
                           enable the (extra) integer-based event handler. 'ints_utils':
                           enable the (extra) integer-based event handler utils.
                           'ints_to_testsuite': enable the (extra) integer events
                           conversion to testsuite events. 'all': enable all event
                           handlers. 'none': disable all event handlers. The default is
                           tree.


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
  
Although package managers are very useful for getting quickly up to
speed, the advised way is still to bring ryml as a submodule of your
project, building both together. This makes it easy to track any
upstream changes in ryml. Also, ryml is small and quick to build, so
there's not much of a cost for building it with your project.
