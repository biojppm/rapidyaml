## General fixes and improvements

This release focuses in compliance with the YAML standard, mostly by ensuring parse errors in invalid YAML cases.

- Narrow the scope of `%TAG` to only the following document, as per the standard ([PR#588](https://github.com/biojppm/rapidyaml/pull/588)). This required or prompted some API changes:
   - Added new type `TagDirectives`
   - Added `ParseOptions::resolve_tags()` and `ParseOptions::resolve_tags_all()` options to enforce resolution of tags while parsing. When disabled (which is the default), the tree still has `Tree::resolve_tags()` to perform post-parsing or programmatic resolution.
   - Changed `TagDirective` to point at its in-scope document, changed API to reflect this.
   - `ParseEngine` now has an instance of `TagDirectives`, and is in charge of updating it and checking for directive errors.
   - Removed the old `TagDirective m_tag_directives[]` members from the ints and testsuite handlers. `Tree` also has its own `TagDirectives` member, redundantly updated during parsing: this enables programmatic manipulation the tree's tag directives .
   - The event handlers now track the current document id, in order to enable the document scope check for `%TAG` directives. This required adding node id tracking to the ints and testsuite handlers.
   - Added `Tree::ancestor_doc(node_id)` and `ConstNodeRef::ancestor_doc()` to query for the parent document of a node. This is needed to implement resolution of tags.
   - `NodeType`: rename TAGD->TAGH and TAGV->TAGP
   - Internal changes to improve the design of event handlers, moving relocation and error checking logic to `ParseEngine`, where it is most suited.
   - Fix warnings with clang-tidy 22


## YAML fixes: valid cases

Fix parsing of **valid** YAML corner cases:
  - Ambiguity of tags/anchors in ? mode ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    ? &mapanchor
      key: val
    ?
      &keyanchor key: val
    ```
  - flow tags/anchors with omitted plain scalar ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    # ... likewise for !tag
    - [&anchor,&anchor]
    - {&anchor,&anchor}
    - [&anchor :,&anchor :]
    - {&anchor :,&anchor :}
    - [: &anchor,: &anchor]
    - {: &anchor,: &anchor}
    ---
    ? anchor
    ```
  - flow tags/anchors terminating with `:` (the colon is part of the tag/anchor) ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    # ... likewise for !tag:
    - [&anchor:,&anchor:]
    - {&anchor:,&anchor:}
    - [&anchor: :,&anchor: :]
    - {&anchor: :,&anchor: :}
    - [: &anchor:,: &anchor:]
    - {: &anchor:,: &anchor:}
    ---
    ? anchor
    ```
  - Fix corner cases of explicit keys now allow same-line containers ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    ? - a     # same-line container key now parses successfully. both seqs and maps
    : - b     # same-line container val now parses successfully. both seqs and maps
    ? ? - c   # nested explicit keys were also fixed
      ? - d
    ```
  - Missing tags/anchors in some flow maps ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)).


## YAML fixes: invalid cases

Ensure parse errors for **invalid** YAML cases, and improve reported error location:
  - `%TAG` directives are valid only in the following document ([PR#588](https://github.com/biojppm/rapidyaml/pull/588)).
    ```yaml
    %TAG !m my-
    --- !m!first a
    --- !m!second a  # error: %TAG directive out of scope here
    ...
    %TAG !m your-
    --- !m!second a  # ok: %TAG provided now.
    ```
  - colon on newline at top level ([PR#585](https://github.com/biojppm/rapidyaml/pull/585)):
    ```yaml
    scalar
    : bad
    ---
    [seq]
    : bad
    ---
    {map: }
    : bad
    ```
  - colon on newline generally in block containers ([PR#585](https://github.com/biojppm/rapidyaml/pull/585)):
    ```yaml
    bad cases:
      scalar
        : bad colon
      [seq]
        : bad colon
      {map: }
        : bad colon
    ```
  - colon on newline in flow sequences ([PR#586](https://github.com/biojppm/rapidyaml/pull/586)):
    ```yaml
    [a
      : 
      b]
    ```
  - tokens after explicit document end ([PR#585](https://github.com/biojppm/rapidyaml/pull/585)):
    ```yaml
    foo: bar
    ... bad tokens
    ```
  - `-` is invalid scalar in flow sequences ([PR#586](https://github.com/biojppm/rapidyaml/pull/586)):
    ```yaml
    [-]
    ---
    [-,-]
    ---
    [--,-]
    ---
    [-
     ]
    ```
  - doc start/begin tokens at zero indentation in seq flow and quoted scalars ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    [
    ---,
    --- ,
    ---\t,
    ...,
    ... ,
    ...\t,
    # etc
    ]
    ---
    "
    --- # error as well
    ... # error as well
    "
    ---
    '
    --- # error as well
    ... # error as well
    '
    ```
  - nested flow containers now enforce the contextual parent indentation ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    - - - [
        a  # now this is a parse error
         ]
    - - - [
         a  # this is ok
         ]
    ```
  - single/double-quoted scalars now enforce the contextual parent indentation ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    - - - "a
        b"  # now this is a parse error
    - - - "a
         b"  # this is ok
    ```
  - plain scalars in block mode starting with `,` ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    all invalid:
      - , foo
      - ,foo
      - ,
    ```
  - references with anchors or tags ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    all invalid:
      - &anchor *ref
      - !tag *ref
    ```
  - directives with extra tokens ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    %YAML 1.2 blabla     # invalid
    %YAML 1. # this is ok
    %TAG ! my- ! blabla  # this is also wrong
    ---
    ```
  - multiline implicit keys are invalid ([PR#587](https://github.com/biojppm/rapidyaml/pull/587)):
    ```yaml
    multiline
        plain: invalid
    'multiline
        squoted': invalid
    "multiline
        dquoted": invalid
    [multiline,
        seq]: invalid
    {multiline:
        map}: invalid
    ```
  - invalid block containers after document open ([PR#592](https://github.com/biojppm/rapidyaml/pull/592)):
    ```yaml
    --- a: b  # invalid
    --- - a   # invalid
    --- ? a   # invalid
    ```
  - invalid block containers after in-block open ([PR#592](https://github.com/biojppm/rapidyaml/pull/592)):
    ```yaml
    a: - b  # invalid
    a: ? b  # invalid
    ```
  - same-line repeated annotations ([PR#592](https://github.com/biojppm/rapidyaml/pull/592)):
    ```yaml
    !a
    !b foo: bar  # ok
    ---
    &a
    &b foo: bar  # ok
    ---
    !a !b foo: bar  # invalid
    ---
    &a &b foo: bar  # invalid
    ```
  - Fix parsing of invalid YAML: block scalars with deindented first line ([PR#593](https://github.com/biojppm/rapidyaml/pull/593)):
    ```yaml
    # the _ characters are not part of the YAML;
    # they are used here only to show the leading whitespace
    empty block scalar: >
     _
      _
       _
     # comment
    ```
