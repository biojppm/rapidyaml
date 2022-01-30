#include "./test_group.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(NESTED_MAPX2)
{

ADD_CASE_TO_GROUP("nested map x2, explicit, same line",
R"({foo: {foo0: 00, bar0: 01, baz0: 02}, bar: {foo1: 10, bar1: 11, baz1: 12}, baz: {foo2: 20, bar2: 21, baz2: 22}})",
    L{
      N{"foo", L{N{"foo0", "00"}, N{"bar0", "01"}, N{"baz0", "02"}}},
      N{"bar", L{N{"foo1", "10"}, N{"bar1", "11"}, N{"baz1", "12"}}},
      N{"baz", L{N{"foo2", "20"}, N{"bar2", "21"}, N{"baz2", "22"}}},
          }
);

ADD_CASE_TO_GROUP("nested map x2, explicit",
R"({
foo: {foo0: 00, bar0: 01, baz0: 02},
bar: {foo1: 10, bar1: 11, baz1: 12},
baz: {foo2: 20, bar2: 21, baz2: 22}
})",
    L{
      N{"foo", L{N{"foo0", "00"}, N{"bar0", "01"}, N{"baz0", "02"}}},
      N{"bar", L{N{"foo1", "10"}, N{"bar1", "11"}, N{"baz1", "12"}}},
      N{"baz", L{N{"foo2", "20"}, N{"bar2", "21"}, N{"baz2", "22"}}},
          }
);

ADD_CASE_TO_GROUP("nested map x2",
R"(
foo:
  foo0: 00
  bar0: 01
  baz0: 02
bar:
  foo1: 10
  bar1: 11
  baz1: 12
baz:
  foo2: 20
  bar2: 21
  baz2: 22
)",
    L{
      N{"foo", L{N{"foo0", "00"}, N{"bar0", "01"}, N{"baz0", "02"}}},
      N{"bar", L{N{"foo1", "10"}, N{"bar1", "11"}, N{"baz1", "12"}}},
      N{"baz", L{N{"foo2", "20"}, N{"bar2", "21"}, N{"baz2", "22"}}},
          }
);


ADD_CASE_TO_GROUP("nested map x2, commented",
	R"(
send_to:
  #host: 192.168.1.100
  #port: 7000
  host: 192.168.1.101
  port: 7001
  #host: 192.168.1.102
  #port: 7002
)",
    L{
	  N("send_to", L{
	    N("host", "192.168.1.101"),
        N("port", "7001") })
	}
);
}

} // namespace yml
} // namespace c4
