#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

#if defined(_MSC_VER)
#   pragma warning(push)
//#   pragma warning(disable: 4127/*conditional expression is constant*/)
//#   pragma warning(disable: 4389/*'==': signed/unsigned mismatch*/)
#elif defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#endif

namespace c4 {
namespace yml {

#define _(name) N(VP, #name) // makes it simpler
#define __(name) N(KP|VP, #name, #name) // makes it simpler

CASE_GROUP(SCALAR_NAMES)
{

ADD_CASE_TO_GROUP("funny names, seq block",
R"(
- a
- b:b
- c{c
- cc{
- c}c
- cc}
- c!c
- cc!
- .foo
- .
- -a
- +b
- /b
- :c
- $g
- "*"
- '*'
- >-
  *
- "*a"
- '*a'
- >-
  *a
)",
N(SB, L{_(a), _(b:b), _(c{c), _(cc{), _(c}c), _(cc}), _(c!c), _(cc!), _(.foo),  _(.),  _(-a), _(+b), _(/b), _(:c), _($g),
      N(VD, "*"), N(VS, "*"), N(VF, "*"),
      N(VD, "*a"), N(VS, "*a"), N(VF, "*a")})
);

ADD_CASE_TO_GROUP("funny names, seq flow",
R"([a, b, c, .foo, ., -a, +b, /b, :c, $g])",
N(SFS, L{_(a), _(b), _(c), _(.foo), _(.), _(-a), _(+b), _(/b), _(:c), _($g)})
);

ADD_CASE_TO_GROUP("funny names, map",
R"(
a: a
b: b
c: c
.foo: .foo
.: .
-a: -a
+b: +b
/b: /b
:c: :c
$g: $g
'*': '*'
'*a': '*a'
)",
N(MB, L{__(a), __(b), __(c), __(.foo), __(.), __(-a), __(+b), __(/b), __(:c), __($g),
  N(KS|VS, "*", "*"), N(KS|VS, "*a", "*a")})
);

ADD_CASE_TO_GROUP("funny names, map expl",
R"({a: a, b: b, c: c, .foo: .foo, .: ., -a: -a, +b: +b, /b: /b, :c: :c, $g: $g,
    '*': '*', '*a':'*a'})",
N(MFS, L{__(a), __(b), __(c), __(.foo), __(.), __(-a), __(+b), __(/b), __(:c), __($g),
  N(KS|VS, "*", "*"), N(KS|VS, "*a", "*a")})
);
}

} // namespace yml
} // namespace c4

#if defined(_MSC_VER)
#   pragma warning(pop)
#elif defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif
