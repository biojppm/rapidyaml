#include "./test_lib/test_group.hpp"
#include "./test_lib/test_group.def.hpp"

namespace c4 {
namespace yml {

CASE_GROUP(NESTED_MAPX2)
{

ADD_CASE_TO_GROUP("nested map x2, explicit, same line",
R"({foo: {foo0: 00, bar0: 01, baz0: 02}, bar: {foo1: 10, bar1: 11, baz1: 12}, baz: {foo2: 20, bar2: 21, baz2: 22}})",
N(MFS,L{
      N{KP|MFS, "foo", L{N{KP|VP, "foo0", "00"}, N{KP|VP, "bar0", "01"}, N{KP|VP, "baz0", "02"}}},
      N{KP|MFS, "bar", L{N{KP|VP, "foo1", "10"}, N{KP|VP, "bar1", "11"}, N{KP|VP, "baz1", "12"}}},
      N{KP|MFS, "baz", L{N{KP|VP, "foo2", "20"}, N{KP|VP, "bar2", "21"}, N{KP|VP, "baz2", "22"}}},
          })
);

ADD_CASE_TO_GROUP("nested map x2, explicit",
R"({
foo: {foo0: 00, bar0: 01, baz0: 02},
bar: {foo1: 10, bar1: 11, baz1: 12},
baz: {foo2: 20, bar2: 21, baz2: 22}
})",
N(MFS, L{
      N{KP|MFS, "foo", L{N{KP|VP, "foo0", "00"}, N{KP|VP, "bar0", "01"}, N{KP|VP, "baz0", "02"}}},
      N{KP|MFS, "bar", L{N{KP|VP, "foo1", "10"}, N{KP|VP, "bar1", "11"}, N{KP|VP, "baz1", "12"}}},
      N{KP|MFS, "baz", L{N{KP|VP, "foo2", "20"}, N{KP|VP, "bar2", "21"}, N{KP|VP, "baz2", "22"}}},
          })
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
N(MB, L{
      N{KP|MB, "foo", L{N{KP|VP, "foo0", "00"}, N{KP|VP, "bar0", "01"}, N{KP|VP, "baz0", "02"}}},
      N{KP|MB, "bar", L{N{KP|VP, "foo1", "10"}, N{KP|VP, "bar1", "11"}, N{KP|VP, "baz1", "12"}}},
      N{KP|MB, "baz", L{N{KP|VP, "foo2", "20"}, N{KP|VP, "bar2", "21"}, N{KP|VP, "baz2", "22"}}},
          })
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
N(MB, L{
	  N(KP|MB, "send_to", L{
	    N(KP|VP, "host", "192.168.1.101"),
        N(KP|VP, "port", "7001") })
	})
);

} // end CASE_GROUP()

} // namespace yml
} // namespace c4
