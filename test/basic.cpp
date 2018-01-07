#include <ryml/ryml.hpp>

namespace yml = c4::yml;

const char* type_str(yml::Node const& p)
{
    switch(p.type())
    {
#define _c4_case(which) case yml::TYPE_##which: return #which;
    _c4_case(NONE)
    _c4_case(ROOT)
    _c4_case(DOC)
    _c4_case(VAL)
    _c4_case(SEQ)
    _c4_case(MAP)
#undef _c4_case
    default: return "(unknown?)";
    }
}

void show_children(yml::Node const& p)
{
    printf("--------\n%zd children for %p(%s):\n", p.num_children(), (void*)&p, type_str(p));
    for(yml::Node *n = p.first_child(); n; n = n->next_sibling())
    {
        printf("  %p(%s) %.*s", (void*)n, type_str(*n), (int)n->name().len, n->name().str);
        if(n->type() == yml::TYPE_VAL)
        {
            printf(": %.*s", (int)n->val().len, n->val().str);
        }
        printf("\n");
    }
}

const char ex[] = R"(# this is a comment
foo: fsdfkjhsdfkh
bar: sdfjkhfuu
bat: 1
seq: [0, 1, 2, 3, 4]
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
    C4_ASSERT(s.root() != nullptr);
    auto &root = *s.root();
    C4_ASSERT(root.type() == yml::TYPE_ROOT);
    show_children(root);
    auto &doc = *s.first_doc();
    C4_ASSERT(doc.type() == yml::TYPE_DOC);
    C4_ASSERT(doc.num_children() == 6);
    //show_children(doc);
    //show_children(doc["seq"]);
    //show_children(doc["map"]);
    //show_children(doc["map"]["submap"]);
    C4_ASSERT(&doc[0] == &doc["foo"]);
    C4_ASSERT(&doc[1] == &doc["bar"]);
    C4_ASSERT(&doc[2] == &doc["bat"]);
    C4_ASSERT(&doc[3] == &doc["seq"]);
    C4_ASSERT(&doc[4] == &doc["map"]);
    C4_ASSERT(&doc[5] == &doc["fdx"]);
    C4_ASSERT(doc[0].name() == "foo" && doc[0].type() == yml::TYPE_VAL && doc[0].val() == "fsdfkjhsdfkh");
    C4_ASSERT(doc[1].name() == "bar" && doc[1].type() == yml::TYPE_VAL && doc[1].val() == "sdfjkhfuu");
    C4_ASSERT(doc[2].name() == "bat" && doc[2].type() == yml::TYPE_VAL && doc[2].val() == "1");
    C4_ASSERT(doc[3].name() == "seq" && doc[3].type() == yml::TYPE_SEQ);
    C4_ASSERT(doc[4].name() == "map" && doc[4].type() == yml::TYPE_MAP);
    C4_ASSERT(doc[5].name() == "fdx" && doc[5].type() == yml::TYPE_VAL && doc[5].val() == "crl");

    auto &seq = doc["seq"];
    C4_ASSERT(seq.num_children() == 5);
    C4_ASSERT(seq[0].val() == "0");
    C4_ASSERT(seq[1].val() == "1");
    C4_ASSERT(seq[2].val() == "2");
    C4_ASSERT(seq[3].val() == "3");
    C4_ASSERT(seq[4].val() == "4");

    #define _chm(m, i, n, v) C4_ASSERT(&m[n] == &m[i] && m[n].name() == n && m[n].val() == v);
    auto &map = doc["map"];
    C4_ASSERT(map.num_children() == 4);
    _chm(map, 0, "foo", "1");
    _chm(map, 1, "bar", "2");
    _chm(map, 2, "baz", "3");

    C4_ASSERT(&map[3] == &map["submap"] && map["submap"].type() == yml::TYPE_MAP);
    auto &smap = doc["map"]["submap"];
    _chm(smap, 0, "subfoo", "11");
    _chm(smap, 1, "subbar", "12");
    _chm(smap, 2, "subbaz", "13");
    _chm(smap, 3, "subbat", "14");

    return 0;
}
