#include <ryml/ryml.hpp>

namespace yml = c4::yml;

const char ex[] = R"(# this is a comment
foo: fsdfkjhsdfkh
bar: sdfjkhfuu
bat: 1
seq: [1, 2, 3, 4, 5]
map:
  foo: 1
  bar: 2
  baz: 3
  submap:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
fdx: crl
)";

int main()
{
    auto s = yml::Tree(2);
    auto p = yml::Parser();
    p.parse(&s, ex);
    return 0;
}
