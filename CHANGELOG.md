# Changelog

Currently there are no ryml releases. However, the master branch is always
stable; it is validated by requiring all tests to succeed before merging to it.
So for now, only major changes to master are listed.

* 2020/September
  * [***Breaking change***] [MR#85](https://github.com/biojppm/rapidyaml/pull/85)
    null values in YAML are now parsed to null
    strings instead of YAML null token "~":
    ```c++
    auto tree = parse("{foo: , bar: ''}");
    // previous:
    assert(tree["foo"].val() == "~");
    assert(tree["bar"].val() == "");
    // now:
    assert(tree["foo"].val() == nullptr); // notice that this is now null
    assert(tree["bar"].val() == "");
    ```
  * [MR#85](https://github.com/biojppm/rapidyaml/pull/85) Commas after tags are now allowed:
    ```yaml
    {foo: !!str, bar: ''}  # now the comma does not cause an error
    ```
  * [MR#81](https://github.com/biojppm/rapidyaml/pull/81): Always compile
    with extra pedantic warnings.

* 2020/May
  *  [***Breaking change***] the error callback now receives a source location object:
    ```c++
    // previous
    using pfn_error = void (*)(const char* msg, size_t msg_len, void *user_data);
    // now:
    using pfn_error = void (*)(const char* msg, size_t msg_len, Location location, void *user_data);
    ```
  * Parser fixes to improve test suite success:
    [MR#73](https://github.com/biojppm/rapidyaml/pull/73),
    [MR#71](https://github.com/biojppm/rapidyaml/pull/71),
    [MR#68](https://github.com/biojppm/rapidyaml/pull/68),
    [MR#67](https://github.com/biojppm/rapidyaml/pull/67),
    [MR#66](https://github.com/biojppm/rapidyaml/pull/66)
  * Fix compilation as DLL on windows [MR#69](https://github.com/biojppm/rapidyaml/pull/69)
