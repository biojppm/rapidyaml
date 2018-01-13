#include <ryml/ryml.hpp>

namespace yml = c4::yml;

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
    C4_ASSERT( ! yml::cspan(":").begins_with(": "));

    yml::Tree s(2);
    yml::Parser p;
    p.parse(&s, ex);
    C4_ASSERT(s.root() != nullptr);
    auto &root = *s.root();
    C4_ASSERT(root.type() == yml::TYPE_ROOT);
    show_children(root);
    auto &doc = *s.first_doc();
    C4_ASSERT(doc.type() == yml::TYPE_DOC);
    C4_ASSERT(doc.num_children() == 6);
    show_children(doc);
    show_children(doc["seq"]);
    show_children(doc["map"]);
    show_children(doc["map"]["submap"]);
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

    //-----------------------------------------------

    //    doc["new_child"] = "new_value";
    doc.append_child("new_child", "new_value");
    C4_ASSERT(&doc[6] == &doc["new_child"]);
    C4_ASSERT(doc["new_child"].val() == "new_value");

    doc.append_child("new_map", yml::TYPE_MAP);
    C4_ASSERT(&doc[7] == &doc["new_map"]);
    C4_ASSERT(doc["new_map"].type() == yml::TYPE_MAP);

    auto &nm = doc["new_map"];
    nm.prepend_child("fdx", "crl");
    nm.prepend_child("pqp", "ccm");
    C4_ASSERT(nm.num_children() == 2);
    C4_ASSERT(&nm[0] == &nm["pqp"] && nm[0] == "ccm");
    C4_ASSERT(&nm[1] == &nm["fdx"] && nm[1] == "crl");
    nm.insert_child("xxx", "yyy", &nm["pqp"]);
    show_children(nm);
    C4_ASSERT(nm.num_children() == 3);
    C4_ASSERT(&nm[0] == &nm["pqp"] && nm[0] == "ccm");
    C4_ASSERT(&nm[1] == &nm["xxx"] && nm[1] == "yyy");
    C4_ASSERT(&nm[2] == &nm["fdx"] && nm[2] == "crl");

    yml::Parser p2;
    nm.append_child("serialized", yml::TYPE_MAP);
    C4_ASSERT(nm.num_children() == 4);

    C4_ASSERT(nm["serialized"].type() == yml::TYPE_MAP);
    s.load(&nm["serialized"], R"(
prop1: val1
prop2: val2
prop3: val3
prop4: [seq1, seq2, seq3]
)", &p2);
    show_children((*s.first_doc())["new_map"]["serialized"]);


    //------------------------------------------------------------------------
    printf(".....\n");

    yml::Node *n = s.first_doc()->find_child("seq");
    size_t num = yml::emit(n);
    printf("stdout result: %zd chars\n", num);

    char buf[1024] = {0};
    yml::span sp(buf, sizeof(buf));

    auto result = yml::emit(n, sp);
    printf("str result: %zd chars:\n%.*s\n", result.len, (int)result.len, result.str);

    C4_ASSERT(yml::emit(n) == yml::emit(n, sp).len);

    printf("-----------------\nchecks ok!!!!!\n-----------------\n");


    //-----------------------------------------------------------------------------
const char ex2[] = R"(# this is a comment
foo: fsdfkjhsdfkh
bar: sdfjkhfuu
bat: 1
seq:
  - 0
  - 1
  - 2
  - 3
  - 4
map:
  foo: 1
  bar: 2
  baz: 3
explseq: [0, 1, 2, 3, 4]
explmap: {foo: 1, bar: 2, baz: 3}
explseq2: [[0, 1, 2], [3, 4, 5], [6, 7, 8], [9, 10, 11]]
explmap2: {keyfoo: {foo: 1, bar: 2, baz: 3}, keybar: {foo: 4, bar: 5, baz: 6}, keybaz: {foo: 7, bar: 8, baz: 9}}
explseq3: [
  [0, 1, 2],
  [3, 4, 5],
  [6, 7, 8],
  [9, 10, 11]
]
explmap3: {
  keyfoo: {foo: 1, bar: 2, baz: 3},
  keybar: {foo: 4, bar: 5, baz: 6},
  keybaz: {foo: 7, bar: 8, baz: 9}
}
explseq4: [
  [
    0,
    1,
    2
  ]
  ,
  [ 3,
    4,
    5],
  [6,
    7,
    8],[
   9, 10, 11]]
explmap3: {
  keyfoo: {
    foo: 1,
    bar: 2,
    baz: 3
  }
  ,
  keybar: {
    foo: 4,
    bar: 5,
    baz: 6
  },
  keybaz:
  {
    foo: 7,
    bar: 8,
    baz: 9
}
}
seq2:
  - - 0
    - 1
    - 2
  - [3, 4, 5]
  - - 6
    - 7
    - 8
  - [9, 10, 11]
map2:
  indent: 1
  submap:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
  deindent: 1
  submap2:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
seq3:
  - - - 0
      - 1
      - 2
    - - 3
      - 4
      - 5
  - -
      - 6
  -
    -
      - 7
  -
    -
      -
        8
map3:
  indent: 1
  submap:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
  deindent: 1
  submap2:
    subfoo: 11
    subbar: 12
    subbaz: 13
    subbat: 14
seqmap:
  -
    foo: 1
    bar: 2
    baz: 3
  -
    foo: 4
    bar: 5
    baz: 6
fdx: crl
)";

    yml::NextParser np;
    yml::Tree t = np.parse("inline source", ex2);
    return 0;
}
