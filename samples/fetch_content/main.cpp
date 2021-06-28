#include <ryml.hpp>

namespace ryml {
using substr = c4::substr;
} // namespace ryml

int main()
{
    // ryml can parse in situ (and read-only buffers too):
    char src[] = "{foo: 1, bar: [2, 3]}";
    ryml::substr srcview = src; // a mutable view to the source buffer
    ryml::Tree tree = ryml::parse(srcview); // you can also reuse the tree and/or parser
    ryml::NodeRef root = tree.rootref(); // get a reference to the root

    // after parsing, the tree holds views to the source buffer:
    C4_CHECK(root["foo"].val() == "1");
    C4_CHECK(root["foo"].val().data() == src + strlen("{foo: "));

    C4_CHECK(root["bar"][0].val() == "2");
    C4_CHECK(root["bar"][0].val().data() == src + strlen("{foo: 1, bar: ["));

    C4_CHECK(root["bar"][1].val() == "3");
    C4_CHECK(root["bar"][1].val().data() == src + strlen("{foo: 1, bar: [2, "));

    // deserializing:
    int foo = 0, bar0 = 0, bar1 = 0;
    root["foo"] >> foo;
    root["bar"][0] >> bar0;
    root["bar"][1] >> bar1;
    C4_CHECK(foo == 1);
    C4_CHECK(bar0 == 2);
    C4_CHECK(bar1 == 3);
}
