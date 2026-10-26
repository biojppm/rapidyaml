- [PR#653](https://github.com/biojppm/rapidyaml/pull/653) cmake: fix tests with RYML_SYSTEM_C4CORE (fixes [#651](https://github.com/biojppm/rapidyaml/issues/651))
- Fix [#656](https://github.com/biojppm/rapidyaml/issues/656):
  1. [PR#657](https://github.com/biojppm/rapidyaml/pull/657) anchors: reject cyclic references, by @AlexandrKhromov2005
  2. [PR#658](https://github.com/biojppm/rapidyaml/pull/658) anchors: add node expansion limit to defend against exponential attacks such as [Billion Laughs](https://en.wikipedia.org/wiki/Billion_laughs_attack):
    - Add struct `ResolveOptions` to pass to `resolve()` methods.
    - Deprecate `resolve()` methods accepting `bool clear_anchors`
- [PR#663](https://github.com/biojppm/rapidyaml/pull/663) api: don't require `Development.Embed` for the Python module, by @chrisburr
- [PR#662](https://github.com/biojppm/rapidyaml/pull/662) install: fix `c4_install_exports` call with system `c4core`, by @musicinmybrain
- [PR#665](https://github.com/biojppm/rapidyaml/pull/665) `scalar_charconv.hpp`: add missing include of  math.h, by @sjh9714
- [PR#668](https://github.com/biojppm/rapidyaml/pull/668) emit: double-quote scalars with whitespace next to a newline, by @fhgffy
- [PR#666](https://github.com/biojppm/rapidyaml/pull/666) extra ints events: 
  - parse: add option to parse into resizing (owning) buffers
  - emit: implement fully round-trip enabled emitter


### Thanks

- @AlexandrKhromov2005
- @chrisburr
- @musicinmybrain
- @fhgffy
- @sjh9714
