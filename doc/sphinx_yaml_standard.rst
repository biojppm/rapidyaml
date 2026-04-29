YAML standard conformance
=========================

ryml is fully conformant and feature-complete with regards to the YAML
specification, and it passes all of the cases in the YAML test
suite. All the YAML features are well covered in the unit tests, and
expected to work, unless in the exceptions noted below.

Of course, there are many dark corners in YAML, and there certainly
can appear cases which ryml fails to parse. If you find any case where
ryml fails, your `bug reports or pull
requests <https://github.com/biojppm/rapidyaml/issues>`__ are very
welcome.

.. note::

   If you do run into trouble and would like to investigate
   conformance of your YAML code, **do not use existing online YAML
   linters**, many of which are not fully conformant; instead, try
   using `https://play.yaml.com
   <https://play.yaml.com/main/parser?input=IyBFZGl0IE1lIQoKJVlBTUwgMS4yCi0tLQpmb286IEhlbGxvLCBZQU1MIQpiYXI6IFsxMjMsIHRydWVdCmJhejoKLSBvbmUKLSB0d28KLSBudWxsCg==>`__,
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

-  ryml's parser engine correctly handles all YAML features. But the 
   destination tree does NOT accept containers as map keys: map keys
   must always be scalars. But note that this is a limitation
   only of the final tree.
-  Tab characters after ``:`` and ``-`` are not accepted tokens, unless
   ryml is compiled with the macro ``RYML_WITH_TAB_TOKENS``. This
   requirement exists because checking for tabs introduces branching
   into the parser’s hot code and in some cases costs as much as 10% in
   parsing time.
-  Non-unique map keys are allowed. Enforcing key uniqueness in the
   parser or in the tree would cause log-linear parsing complexity (for
   root children on a mostly flat tree), and would increase code size
   through added structural, logical and cyclomatic complexity. Enforcing
   uniqueness in the parser would hurt users who may not care
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



YAML test suite
---------------

As part of its CI testing, ryml uses and passes all the cases from the
`YAML test suite <https://github.com/yaml/yaml-test-suite>`__. This is
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
unit tests in ryml, also employing the same extensive combinatorial
approach.

Also, note that in `their own words <http://matrix.yaml.info/>`__, the
tests from the YAML test suite *contain a lot of edge cases that don't
play such an important role in real world examples*. And yet, despite
the extreme focus of the test suite, ryml passes all of its test cases.
