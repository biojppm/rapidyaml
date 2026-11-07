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
- [PR#672](https://github.com/biojppm/rapidyaml/pull/672) Block scalar: fix empty scalar swallowing the next document marker, by @Wleeaf
- [PR#673](https://github.com/biojppm/rapidyaml/pull/673) Block scalar: accept empty scalar with explicit indentation indicator, by @Wleeaf
- [PR#674](https://github.com/biojppm/rapidyaml/pull/674) Block scalar: Reject text after the header indicators, by @Wleeaf
- [PR#676](https://github.com/biojppm/rapidyaml/pull/676) Tree: touch only used nodes, by @calpeyser. **~10% parse time improv, kudos!**.
- [PR#682](https://github.com/biojppm/rapidyaml/pull/682): Parser: ensure error on excess tags/anchors. Fixes [#680](https://github.com/biojppm/rapidyaml/issues/680).
- [PR#683](https://github.com/biojppm/rapidyaml/pull/683): Parser: ensure error on unterminated seq+map `[:`
- [PR#686](https://github.com/biojppm/rapidyaml/pull/686): Parser: fix parse of container keys in block mode, eg `- ? []:`
- [PR#695](https://github.com/biojppm/rapidyaml/pull/695): Parser: fix assertion causing accidental quadratic complexity in single line flow (fixes [PR#694](https://github.com/biojppm/rapidyaml/issues/694)).
- [PR#666](https://github.com/biojppm/rapidyaml/pull/666) extra ints events:
  - parse: add option to parse into resizing (owning) buffers
  - emit: implement fully round-trip enabled emitter, with behavior exactly equal to the tree emitter


### Thanks

- @AlexandrKhromov2005
- @chrisburr
- @musicinmybrain
- @fhgffy
- @sjh9714
- @Wleeaf
- @calpeyser
