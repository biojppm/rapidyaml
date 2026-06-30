CMake build settings
====================

The following cmake variables can be used to control the build behavior
of ryml:

-  ``RYML_WITH_TAB_TOKENS=ON/OFF``. Enable/disable support for tabs as
   valid container tokens after ``:`` and ``-``. Defaults to ``OFF``,
   because this may cost up to 10% in processing time.
-  ``RYML_DEFAULT_CALLBACKS=ON/OFF``. Enable/disable ryml’s default
   implementation of error and allocation callbacks. Defaults to ``ON``.
-  ``RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS=ON/OFF`` - Enable/disable the
   same-named macro, which will make the default error handlers provided
   by ryml throw exceptions. Defaults to ``OFF``.
- ``RYML_USE_ASSERT=ON/OFF``. Enable/disable assertions regardless of
   of the ``NDEBUG`` macro. When set to ``OFF`` (the default), ryml
   asserts unless ``NDEBUG`` is defined, making it so that assertions
   are enabled in Debug builds and disabled in Release builds (which
   usually have ``NDEBUG`` defined). When ``RYML_USE_ASSERT`` is set to
   ``ON``, assertions are enabled even if ``NDEBUG`` is defined; with
   this, assertions will be enabled in all build types regardless of
   ``NDEBUG`` definition status. Failed assertions will trigger a call
   to the error callback.
-  ``RYML_SHORT_CHECK_MSG`` - Use shorter error message from
   checks/asserts: do not show the check condition in the error
   message.
- ``RYML_STANDALONE=ON/OFF``. Use ryml in standalone mode, where the
   code from `c4core <https://github.com/biojppm/c4core>`__ is used as
   part of ryml (ie, as part of the ryml library). Defaults to
   ``ON``. See the next section for further details.
- ``RYML_INSTALL=ON/OFF``. enable/disable install target. Defaults to ``ON``.

If you’re developing ryml or just debugging problems with ryml itself,
the following cmake variables can be helpful:

- ``RYML_DEV=ON/OFF``: a bool variable which enables development targets
  such as unit tests, benchmarks, etc. Defaults to ``OFF``.
- ``RYML_DBG=ON/OFF``: a bool variable which enables verbose prints from
  parsing code; can be useful to figure out parsing problems. Defaults
  to ``OFF``.


Forcing ryml to use a different c4core version
----------------------------------------------

ryml uses code from `c4core <https://github.com/biojppm/c4core>`__, a
C++ library with low-level multi-platform utilities. (c4core in turn
uses code from `fast_float
<https://github.com/fastfloat/fast_float>`__ and `debugbreak
<https://github.com/biojppm/debugbreak>`__). By default, the files
from c4core that ryml uses are provided as part of ryml's source
distribution and installation.

More importantly, **c4core is NOT a dependency** of ryml. To use ryml,
just link with ryml, and you're done.

c4core was initially a submodule of this project, and maybe this
prompted some users to see c4core as a hard dependency, despite this
not being the intended usage in the general case. So it was decided to
remove the submodule and provide the c4core files as part of the
source code of ryml. This makes it necessary for users wishing to opt
in to use a full c4core.

However, you may prefer to use a separate or installed version of
c4core, maybe even with a different version. This is opt-in behavior:
you need to set ``RYML_STANDALONE=OFF`` while invoking cmake. You can
find out how to achieve this by looking at the `custom_c4core sample
<https://github.com/biojppm/rapidyaml/blob/v0.15.2/samples/custom_c4core/CMakeLists.txt>`__.

