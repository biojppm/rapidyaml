
### Fixes

- Fix major error handling problem reported in [#389](https://github.com/biojppm/rapidyaml/issues/389) ([PR#411](https://github.com/biojppm/rapidyaml/pull/411)):
  - The `NodeRef` and `ConstNodeRef` classes had many methods marked `noexcept` that were doing assertions which could throw exceptions, causing an abort instead of a throw whenever the assertion called an exception-throwing error callback.
  - Also, this problem was compounded by exceptions being enabled in every build type -- despite the intention to have them only in debug builds. There was a problem in the preprocessor code to enable assertions which led to assertions being enabled in release builds even when `RYML_USE_ASSERT` was defined to 0. Thanks to @jdrouhard for reporting this.
  - Although the code is and was extensively tested, the testing was addressing mostly the happy path. In the fix, I added tests to ensure that the error behavior is as intended.
  - Together with this changeset, a major revision was carried out of the asserting/checking status of each function in the node classes. In most cases, assertions were added to cases that were missing them. So **beware** - user code that was invalid will now assert or error out. Also, assertions and checks are now directed as much as possible to the callbacks of the closest scope, ie if a tree has custom callbacks, errors should go through those callbacks.
  - Also, the intended assertion behavior is now in place: *no assertions in release builds*. **Beware** as well - user code which was relying on this will now silently succeed and return garbage in release builds. See the next points, which may help:
  - Added new methods to the node class:
    ```c++
    /** Distinguish between a valid seed vs a valid non-seed ref. */
    bool readable() const { return valid() && !is_seed(); }

    /** Get a child by name, with error checking; complexity is
     * O(num_children).
     *
     * Behaves as operator[](csubstr) const, but always raises an
     * error (even when RYML_USE_ASSERT is set to false) when the
     * returned node does not exist, or when this node is not
     * readable, or when it is not a map. This behaviour is similar to
     * std::vector::at(), but the error consists in calling the error
     * callback instead of directly raising an exception. */
    ConstNodeRef ConstNodeRef::at(csubstr key) const;
    ConstNodeRef NodeRef::at(csubstr key) const;

    /** Get a child by position, with error checking; complexity is
     * O(pos).
     *
     * Behaves as operator[](size_t) const, but always raises an error
     * (even when RYML_USE_ASSERT is set to false) when the returned
     * node does not exist, or when this node is not readable, or when
     * it is not a map. This behaviour is similar to
     * std::vector::at(), but the error consists in calling the error
     * callback instead of directly raising an exception. */
    ConstNodeRef ConstNodeRef::at(size_t pos) const;
   ConstNodeRef NodeRef::at(size_t pos) const;
   ```
  - Added macros and respective cmake options to control error handling:
    - `RYML_USE_ASSERT` - enable assertions regardless of build type. This is disabled by default.
    - `RYML_DEFAULT_CALLBACK_USES_EXCEPTIONS` - defines the same macro, which will make the default error handler provided by ryml throw exceptions instead of calling `std::abort()`. This is disabled by default.
  - Also, `RYML_DEBUG_BREAK()` is now enabled only if `RYML_DBG` is defined, as reported in [#362](https://github.com/biojppm/rapidyaml/issues/362).
- Fix [#390](https://github.com/biojppm/rapidyaml/pull/390) - `csubstr::first_real_span()` failed on scientific numbers with one digit in the exponent.
- Fix [#361](https://github.com/biojppm/rapidyaml/pull/361) - parse error on map scalars containing `:` and starting on the next line:
  ```yaml
  ---
  # failed to parse:
  description:
    foo:bar
  ---
  # but this was ok:
  description: foo:bar
  ```
- [PR#368](https://github.com/biojppm/rapidyaml/pull/368) - fix pedantic compiler warnings.
- Fix [#373](https://github.com/biojppm/rapidyaml/issues/373) - false parse error with empty quoted keys in block-style map ([PR#374](https://github.com/biojppm/rapidyaml/pull/374)).
- Fix [#356](https://github.com/biojppm/rapidyaml/issues/356) - fix overzealous check in `emit_as()`. An id may be larger than the tree's size, eg when nodes were removed. ([PR#357](https://github.com/biojppm/rapidyaml/pull/357)).
- Fix [#417](https://github.com/biojppm/rapidyaml/issues/417)) - add quickstart example explaining how to avoid precision loss while serializing floats ([PR#420](https://github.com/biojppm/rapidyaml/pull/420)).

### Thanks

- @Neko-Box-Coder
- @jdrouhard
