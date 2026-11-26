#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

CASE_GROUP(NUMBER)
{

ADD_CASE_TO_GROUP("integer numbers, flow", JSON_WRITE,
R"(translation: [-2, -2, 5, 0xa, -0xb, 0XA, -0XA, 0b10, -0b10, 0B10, -0B10, 0o17, -0o17, 0O17, -0O17])",
N(MB, L{
  N(KP|SFS, "translation", L{
    N(VP, "-2"), N(VP, "-2"), N(VP, "5"),
    N(VP, "0xa"), N(VP, "-0xb"),
    N(VP, "0XA"), N(VP, "-0XA"),
    N(VP, "0b10"), N(VP, "-0b10"),
    N(VP, "0B10"), N(VP, "-0B10"),
    N(VP, "0o17"), N(VP, "-0o17"),
    N(VP, "0O17"), N(VP, "-0O17"),
  })
})
);

ADD_CASE_TO_GROUP("integer numbers, block", JSON_WRITE,
R"(translation:
  - -2
  - -2
  - -5
)",
N(MB, L{N(KP|SB, "translation", L{N(VP, "-2"), N(VP, "-2"), N(VP, "-5")})})
);

ADD_CASE_TO_GROUP("floating point numbers, flow",
R"([-2.0, -2.1, 0.1, .1, -.2, -2.e+6, -3e-6, 1.12345e+011])",
N(SFS, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, ".1"), N(VP, "-.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("floating point numbers, flow jsonified", JSON_WRITE,
R"([-2.0, -2.1, 0.1, 0.1, -0.2, -2.e+6, -3e-6, 1.12345e+011])",
N(SFS, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, "0.1"), N(VP, "-0.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("floating point numbers, block",
R"(
- -2.0
- -2.1
- 0.1
- .1
- -.2
- -2.e+6
- -3e-6
- 1.12345e+011
)",
N(SB, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, ".1"), N(VP, "-.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("floating point numbers, block jsonified", JSON_WRITE,
R"(
- -2.0
- -2.1
- 0.1
- 0.1
- -0.2
- -2.e+6
- -3e-6
- 1.12345e+011
)",
N(SB, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, "0.1"), N(VP, "-0.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("hex floating point numbers, block",
R"(
- -2.0
- -2.1
- 0.1
- .1
- -.2
- -2.e+6
- -3e-6
- 1.12345e+011
)",
N(SB, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, ".1"), N(VP, "-.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("hex floating point numbers, block jsonified", JSON_WRITE,
R"(
- -2.0
- -2.1
- 0.1
- 0.1
- -0.2
- -2.e+6
- -3e-6
- 1.12345e+011
)",
N(SB, L{N(VP, "-2.0"), N(VP, "-2.1"), N(VP, "0.1"), N(VP, "0.1"), N(VP, "-0.2"), N(VP, "-2.e+6"), N(VP, "-3e-6"), N(VP, "1.12345e+011")})
);

ADD_CASE_TO_GROUP("version numbers", JSON_WRITE,
R"(
- 1.2.3
- 1.2.3.4
- [1.2.3, 4.5.6]
- [1.2.3.4, 4.5.6.7]
- - 1.2.3
  - 4.5.6
- - 1.2.3.4
  - 4.5.6.7
- a: 1.2.3
- a: 1.2.3.4
- {a: 1.2.3}
- {a: 1.2.3.4}
- a: 1.2.3
  b: 4.5.6
- a: 1.2.3.4
  b: 4.5.6.7
- {a: 1.2.3, b: 4.5.6}
- {a: 1.2.3.4, b: 4.5.6.7}
)",
N(SB, L{
    N(VP, "1.2.3"),
    N(VP, "1.2.3.4"),
    N(SFS, L{N(VP, "1.2.3"), N(VP, "4.5.6")}),
    N(SFS, L{N(VP, "1.2.3.4"), N(VP, "4.5.6.7")}),
    N(SB, L{N(VP, "1.2.3"), N(VP, "4.5.6")}),
    N(SB, L{N(VP, "1.2.3.4"), N(VP, "4.5.6.7")}),
    N(MB, L{N(KP|VP, "a", "1.2.3")}),
    N(MB, L{N(KP|VP, "a", "1.2.3.4")}),
    N(MFS, L{N(KP|VP, "a", "1.2.3")}),
    N(MFS, L{N(KP|VP, "a", "1.2.3.4")}),
    N(MB, L{N(KP|VP, "a", "1.2.3"), N(KP|VP, "b", "4.5.6")}),
    N(MB, L{N(KP|VP, "a", "1.2.3.4"), N(KP|VP, "b", "4.5.6.7")}),
    N(MFS, L{N(KP|VP, "a", "1.2.3"), N(KP|VP, "b", "4.5.6")}),
    N(MFS, L{N(KP|VP, "a", "1.2.3.4"), N(KP|VP, "b", "4.5.6.7")}),
}));
}

} // namespace yml
} // namespace c4
