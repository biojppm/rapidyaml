- [PR#653](https://github.com/biojppm/rapidyaml/pull/653) cmake: fix tests with RYML_SYSTEM_C4CORE (fixes [#651](https://github.com/biojppm/rapidyaml/issues/651))
- Fix [#656](https://github.com/biojppm/rapidyaml/issues/656):
  1. [PR#657](https://github.com/biojppm/rapidyaml/pull/657) anchors: reject cyclic references. Thanks @ AlexandrKhromov2005!
  2. [PR#658](https://github.com/biojppm/rapidyaml/pull/658) anchors: add node expansion limit to defend against exponential attacks such as [Billion Laughs](https://en.wikipedia.org/wiki/Billion_laughs_attack):
    - Add struct `ResolveOptions` to pass to `resolve()` methods.
    - Deprecate `resolve()` methods accepting `bool clear_anchors`


### Thanks

- @ AlexandrKhromov2005
