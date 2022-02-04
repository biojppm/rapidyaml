#include "./test_group.hpp"

namespace c4 {
namespace yml {


CASE_GROUP(NUMBER)
{

ADD_CASE_TO_GROUP("integer numbers, expl",
R"(translation: [-2, -2, 5])",
L{N("translation", L{N("-2"), N("-2"), N("5")})}
);

ADD_CASE_TO_GROUP("integer numbers, impl",
R"(translation:
  - -2
  - -2
  - -5
)",
L{N("translation", L{N("-2"), N("-2"), N("-5")})}
);

ADD_CASE_TO_GROUP("floating point numbers, expl",
R"([-2.0, -2.1, 0.1, .1, -.2, -2.e+6, -3e-6, 1.12345e+011])",
L{N("-2.0"), N("-2.1"), N("0.1"), N(".1"), N("-.2"), N("-2.e+6"), N("-3e-6"), N("1.12345e+011")}
);

ADD_CASE_TO_GROUP("floating point numbers, impl",
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
L{N("-2.0"), N("-2.1"), N("0.1"), N(".1"), N("-.2"), N("-2.e+6"), N("-3e-6"), N("1.12345e+011")}
);
}

} // namespace yml
} // namespace c4
