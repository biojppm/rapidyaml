YAML standard conformance
=========================

ryml is feature complete with regards to the YAML specification. All the
YAML features are well covered in the unit tests, and expected to work,
unless in the exceptions noted below.

Of course, there are many dark corners in YAML, and there certainly can
appear cases which ryml fails to parse. Your `bug reports or pull
requests <https://github.com/biojppm/rapidyaml/issues>`__ are very
welcome.

.. note::

   If you do run into trouble and would like to investigate
   conformance of your YAML code, **do not use existing online YAML
   linters**, many of which are not fully conformant; instead, try
   using `https://play.yaml.io
   <https://play.yaml.io/main/parser?input=IyBFZGl0IE1lIQoKJVlBTUwgMS4yCi0tLQpmb286IEhlbGxvLCBZQU1MIQpiYXI6IFsxMjMsIHRydWVdCmJhejoKLSBvbmUKLSB0d28KLSBudWxsCg==>`__,
   an amazingly useful tool which lets you dynamically input your YAML and
   continuously see the results from all the existing parsers (kudos
   to @ingydotnet and the people from the YAML test suite). And of
   course, if you detect anything wrong with ryml, please `open an
   issue <https://github.com/biojppm/rapidyaml/issues>`__ so that we
   can improve.


Deliberate deviations
---------------------

ryml deliberately makes no effort to follow the standard in the
following situations:

-  ryml's tree does NOT accept containers as map keys: keys stored in
   the tree must always be scalars. HOWEVER, this is a limitation only
   of the final tree. The event-based parse engine DOES parse container
   keys, as it is is meant to be used by other programming languages to
   create their native data-structures, and it is fully tested and
   fully conformant (other than the general error permissiveness noted
   below).
-  Tab characters after ``:`` and ``-`` are not accepted tokens, unless
   ryml is compiled with the macro ``RYML_WITH_TAB_TOKENS``. This
   requirement exists because checking for tabs introduces branching
   into the parser’s hot code and in some cases costs as much as 10% in
   parsing time.
-  Non-unique map keys are allowed. Enforcing key uniqueness in the
   parser or in the tree would cause log-linear parsing complexity (for
   root children on a mostly flat tree), and would increase code size
   through added structural, logical and cyclomatic complexity. So
   enforcing uniqueness in the parser would hurt users who may not care
   about it (they may not care either because non-uniqueness is OK for
   their use case, or because it is impossible to occur). On the other
   hand, any user who requires uniqueness can easily enforce it by doing
   a post-parse walk through the tree. So choosing to not enforce key
   uniqueness adheres to the spirit of “don’t pay for what you don’t
   use”.
-  ``%YAML`` directives have no effect and are ignored.
-  ``%TAG`` directives are limited to a default maximum of 4 instances
   per ``Tree``. To increase this maximum, define the preprocessor
   symbol ``RYML_MAX_TAG_DIRECTIVES`` to a suitable value. This
   arbitrary limit reflects the usual practice of having at most 1 or 2
   tag directives; also, be aware that this feature is under
   consideration for removal in YAML 1.3.


Known (unintended) deviations
-----------------------------

ryml tends to be on the permissive side in several cases where the
YAML standard dictates that there should be an error; in many of these
cases, ryml will tolerate the input without producing an error. This
is being improved on, meaning **ryml will grow progressively less
tolerant of YAML errors** in the coming releases. So we strongly
suggest to stay away from those dark corners of YAML which are
generally a source of problems; this is good practice anyway.



YAML test suite
---------------

As part of its CI testing, ryml uses the `YAML test suite <https://github.com/yaml/yaml-test-suite>`__.  This is
an extensive and merciless set of reference cases covering the full
YAML spec. Each of these cases has several subparts:
 - ``in-yaml``: mildly, plainly or extremely difficult-to-parse YAML
 - ``in-json``: equivalent JSON (where possible/meaningful)
 - ``out-yaml``: equivalent standard YAML
 - ``emit-yaml``: equivalent standard YAML
 - ``events``: reference events according to the YAML standard

.. note::

   See the test suite results at
   `https://matrix.yaml.info/ <https://matrix.yaml.info/>`__,
   but be aware that the results there may be using an older
   version of ryml.

When testing, ryml parses each of the yaml/json parts, then emits the
parsed tree, then parses the emitted result and verifies that emission
is idempotent, ie that the round trip emitted result is semantically
the same as its input without any loss of information.

To ensure consistency, this happens over four successive levels of
parse->emit round trips. And to ensure correctness, each of the stages
is compared against the ``events`` spec from the test, which constitutes
the reference. The tests also check for equality between the reference
events in the test case and the events emitted by ryml from the data
tree parsed from the test case input. All of this is then carried out
with several variations: both unix ``\n`` vs windows ``\r\n`` line
endings, emitting to string, file or streams, which results in ~250
tests per case part.

With multiple parts per case and ~400 reference
cases in the test suite, this makes over several hundred thousand
individual tests to which ryml is subjected, which are added to the
unit tests in ryml, which also employ the same extensive combinatorial
approach.

Also, note that in `their own words <http://matrix.yaml.info/>`__, the
tests from the YAML test suite *contain a lot of edge cases that don't
play such an important role in real world examples*. And yet, despite
the extreme focus of the test suite, currently ryml only fails a minor
fraction of the test cases, mostly related with the deliberate
limitations noted above.

Other than those limitations, by far the main issue with ryml is that
several standard-mandated parse errors fail to materialize (this will
be addressed in the coming releases). For the up-to-date list of ryml
failures in the test-suite, refer to the `list of known exceptions <test/test_suite/test_suite_parts.cpp>`__ from ryml's test
suite runner, which is used as part of ryml's CI setup.
