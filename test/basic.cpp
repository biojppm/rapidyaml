#include <ryml/ryml.hpp>

#include <map>
#include <vector>

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
seq:
  - 0
  - 1
  - 2
  - 3
  - 4
foo: fsdfkjhsdfkh
bar: sdfjkhfuu
bat: 1
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




class RefNode
{
public:

    using seq_type = std::vector< RefNode >;
    using seq_value_type = seq_type::value_type;
    using seq_init_type = std::initializer_list< seq_value_type >;

    using map_type = std::map< yml::cspan, RefNode >;
    using map_value_type = map_type::value_type;
    using map_init_type = std::initializer_list< map_value_type >;

public:

    yml::NodeType_e type;
    yml::cspan key;
    yml::cspan value;
    map_type map;
    seq_type seq;

    bool is_val() const { return type == yml::TYPE_VAL && key.empty(); }
    bool is_key_val() const { return type == yml::TYPE_VAL && ! key.empty(); }
    bool is_map() const { return type == yml::TYPE_MAP || (type == yml::TYPE_DOC && ! map.empty()); }
    bool is_seq() const { return type == yml::TYPE_SEQ || (type == yml::TYPE_SEQ && ! map.empty()) || type == yml::TYPE_ROOT; }
/*
    void print(int level = 0) const
    {
        if(is_val())
        {
            printf("%.*s", (int)key.len, key.str);
            return;
        }
        else if(is_key_val())
        {
            printf("%.*s", (int)key.len, key.str);
            return;
        }
        ++level;
        if(is_seq())
        {
            printf("\n");
            for(auto const& c : seq)
            {
                printf("%*s", level, "  ");
                printf("%.*s", (int)key.len, key.str);
                c.print(level);
            }
        }
        else if(is_map())
        {
            ++level;
            for(auto const& c : map)
            {
                c.print(level);
            }
        }
    }
*/
public:

    RefNode(RefNode     &&) = default;
    RefNode(RefNode const&) = default;

    RefNode& operator= (RefNode     &&) = default;
    RefNode& operator= (RefNode const&) = default;

    RefNode() : type(yml::TYPE_NONE), key(), value(), map(), seq() {}

    template< size_t N >
    RefNode(const char (&v)[N]) : type(yml::TYPE_VAL), key(), value(v), map(), seq() {}
    RefNode(yml::cspan const& v) : type(yml::TYPE_VAL), key(), value(v), map(), seq() {}

    template< size_t N, size_t M >
    RefNode(const char (&k)[N], const char (&v)[M]) : type(yml::TYPE_VAL), key(k), value(v), map(), seq() {}
    RefNode(yml::cspan const& k, yml::cspan const& v) : type(yml::TYPE_VAL), key(k), value(v), map(), seq() {}

    template< size_t N >
    RefNode(const char (&k)[N], map_init_type m) : RefNode(yml::TYPE_MAP, k, m) {}
    RefNode(yml::cspan const& k, map_init_type m) : RefNode(yml::TYPE_MAP, k, m) {}

    template< size_t N >
    RefNode(yml::NodeType_e t,  const char (&k)[N], map_init_type m) : type(t), key(k), value(), map(m), seq() {}
    RefNode(yml::NodeType_e t, yml::cspan const& k, map_init_type m) : type(t), key(k), value(), map(m), seq() {}

    RefNode(map_init_type m) : RefNode(yml::TYPE_MAP, {}, m) {}
    RefNode(yml::NodeType_e t, map_init_type m) : RefNode(t, {}, m) {}

    template< size_t N >
    RefNode( const char (&k)[N], seq_init_type m) : RefNode(yml::TYPE_SEQ, k, m) {}
    RefNode(yml::cspan const& k, seq_init_type m) : RefNode(yml::TYPE_SEQ, k, m) {}

    template< size_t N >
    RefNode(yml::NodeType_e t,  const char (&k)[N], seq_init_type s) : type(t), key(k), value(), map(), seq(s) {}
    RefNode(yml::NodeType_e t, yml::cspan const& k, seq_init_type s) : type(t), key(k), value(), map(), seq(s) {}

    RefNode(seq_init_type m) : RefNode(yml::TYPE_SEQ, {}, m) {}
    RefNode(yml::NodeType_e t, seq_init_type m) : RefNode(t, {}, m) {}

};


struct ExampleSpec
{
    yml::cspan file;
    RefNode root;

    template< size_t N >
    ExampleSpec(const char (&f)[N], RefNode const& ref) : file(f)
    {

    }
};

using namespace yml;


template< size_t N, class... Args >
ExampleSpec mkex(const char (&txt)[N], Args&&... a)
{
    return ExampleSpec(txt, RefNode(TYPE_ROOT, std::forward< Args >(a)...));
}


void do_test()
{
    using namespace yml;

    using S = RefNode::seq_init_type;
    using M = RefNode::map_init_type;

    ExampleSpec examples[] = {

//-----------------------------------------------------------------------------
// https://en.wikipedia.org/wiki/YAML
mkex(R"(--- # Favorite movies
- Casablanca
- North by Northwest
- The Man Who Wasn't There
)",
     S{{TYPE_DOC, {"Casablanca", "North by Northwest", "The Man Who Wasn't There"}}}),

//-----------------------------------------------------------------------------
mkex(R"(--- # Shopping list
[milk, pumpkin pie, eggs, juice]
)",
     S{{TYPE_DOC, {"milk", "pumpkin pie", "eggs", "juice"}}}),

//-----------------------------------------------------------------------------
mkex(R"(--- # Indented Block
  name: John Smith
  age: 33
--- # Inline Block
{name: John Smith, age: 33}
)",
     S{
         {TYPE_DOC, M{{"name","John Smith"}, {"age","33"}}},
         {TYPE_DOC, M{{"name","John Smith"}, {"age","33"}}},
     }),

//-----------------------------------------------------------------------------
mkex(R"(
- {name: John Smith, age: 33}
- name: Mary Smith
  age: 27
)",
     S{
         M{{"name", "John Smith"}, {"age", "33"}},
         M{{"name", "Mary Smith"}, {"age", "27"}},
     }),

//-----------------------------------------------------------------------------
mkex(R"(
men: [John Smith, Bill Jones]
women:
  - Mary Smith
  - Susan Williams
)",
     M{
         {"men", S{"John Smith", "Bill Jones"}},
         {"women", S{"Mary Smith", "Susan Williams"}},
     }),

//-----------------------------------------------------------------------------
mkex(R"(
---
receipt:     Oz-Ware Purchase Invoice
date:        2012-08-06
customer:
    first_name:   Dorothy
    family_name:  Gale

items:
    - part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4

    - part_no:   E1628
      descrip:   High Heeled \"Ruby\" Slippers
      size:      8
      price:     133.7
      quantity:  1

bill-to:  &id001
    street: |
            123 Tornado Alley
            Suite 16
    city:   East Centerville
    state:  KS

ship-to:  *id001

specialDelivery:  >
    Follow the Yellow Brick
    Road to the Emerald City.
    Pay no attention to the
    man behind the curtain.
...
)",
     S{{TYPE_DOC, M{
{"receipt", ""},
{"receipt", "Oz-Ware Purchase Invoice"},
{"date", "2012-08-06"},
{"customer", M{{"first_name", "Dorothy"}, {"family_name", "Gale"}}},
{"items", S{
        M{{"part_no", "A4786"},
            {"descrip",   "Water Bucket (Filled)"},
            {"price",     "1.47"},
            {"quantity",  "4"},},
        M{{"part_no", "E1628"},
            {"descrip",   "High Heeled \"Ruby\" Slippers"},
            {"size",     "8"},
            {"price",     "133.7"},
            {"quantity",  "1"},},
{"bill-to", M{
        {"street", R"(123 Tornado Alley
Suite 16
)"},
        {"city", "East Centerville"},
        {"state", "KS"},}},
{"ship-to", M{
        {"street", R"(123 Tornado Alley
Suite 16
)"},
        {"city", "East Centerville"},
        {"state", "KS"},}},
{"specialDelivery", R"(Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.
)"}
    }
},
     }}}),

//-----------------------------------------------------------------------------
mkex(R"(
# sequencer protocols for Laser eye surgery
---
- step:  &id001                  # defines anchor label &id001
    instrument:      Lasik 2000
    pulseEnergy:     5.4
    pulseDuration:   12
    repetition:      1000
    spotSize:        1mm

- step: &id002
    instrument:      Lasik 2000
    pulseEnergy:     5.0
    pulseDuration:   10
    repetition:      500
    spotSize:        2mm
- step: *id001                   # refers to the first step (with anchor &id001)
- step: *id002                   # refers to the second step
- step:
    <<: *id001
    spotSize: 2mm                # redefines just this key, refers rest from &id001
- step: *id002
)",
     S{{TYPE_DOC, {
M{{"step", M{
    {"instrument",      "Lasik 2000"},
    {"pulseEnergy",     "5.4"},
    {"pulseDuration",   "12"},
    {"repetition",      "1000"},
    {"spotSize",        "1mm"},
        }}},
M{{"step", M{
    {"instrument",      "Lasik 2000"},
    {"pulseEnergy",     "5.0"},
    {"pulseDuration",   "10"},
    {"repetition",      "500"},
    {"spotSize",        "2mm"},
        }}},
M{{"step", M{
    {"instrument",      "Lasik 2000"},
    {"pulseEnergy",     "5.4"},
    {"pulseDuration",   "12"},
    {"repetition",      "1000"},
    {"spotSize",        "1mm"},
        }}},
M{{"step", M{
    {"instrument",      "Lasik 2000"},
    {"pulseEnergy",     "5.0"},
    {"pulseDuration",   "10"},
    {"repetition",      "500"},
    {"spotSize",        "2mm"},
        }}},
M{{"step", M{
    {"instrument",      "Lasik 2000"},
    {"pulseEnergy",     "5.4"},
    {"pulseDuration",   "12"},
    {"repetition",      "1000"},
    {"spotSize",        "2mm"},
        }}},
M{{"step", M{
    {"instrument",      "Lasik 2000"},
    {"pulseEnergy",     "5.0"},
    {"pulseDuration",   "10"},
    {"repetition",      "500"},
    {"spotSize",        "2mm"},
        }}},
         }
     }}),


//-----------------------------------------------------------------------------
mkex(R"(
data: |
   There once was a short man from Ealing
   Who got on a bus to Darjeeling
       It said on the door
       \"Please don't spit on the floor\"
   So he carefully spat on the ceiling
)",
     M{{"data", R"(There once was a short man from Ealing
Who got on a bus to Darjeeling
    It said on the door
    \\\"Please don't spit on the floor\\\"
So he carefully spat on the ceiling
)"}}
     ),

//-----------------------------------------------------------------------------
mkex(R"(
data: >
   Wrapped text
   will be folded
   into a single
   paragraph

   Blank lines denote
   paragraph breaks
)",
     M{{"data",R"(Wrapped text will be folded into a single paragraph
Blank lines denote paragraph breaks
)"}}
     ),

//-----------------------------------------------------------------------------
mkex(R"(
---
example: >
        HTML goes into YAML without modification
message: |
        <blockquote style=\"font: italic 12pt Times\">
        <p>\"Three is always greater than two,
           even for large values of two\"</p>
        <p>--Author Unknown</p>
        </blockquote>
date: 2007-06-01
)",
     S{{TYPE_DOC, M{
                 {"example","HTML goes into YAML without modification"},
                 {"message",R"(<blockquote style=\"font: italic 12pt Times\">
<p>\"Three is always greater than two,
   even for large values of two\"</p>
<p>--Author Unknown</p>
</blockquote>
)"},
                 {"date","2007-06-01"},
                     }}}
     ),

    }; // end examples
        ;
}



char const* const structure_examples[] = {
// https://en.wikipedia.org/wiki/YAML
R"(
---
a: 123                     # an integer
b: \"123\"                   # a string, disambiguated by quotes
c: 123.0                   # a float
d: !!float 123             # also a float via explicit data type prefixed by (!!)
e: !!str 123               # a string, disambiguated by explicit type
f: !!str Yes               # a string via explicit type
g: Yes                     # a boolean True (yaml1.1), string \"Yes\" (yaml1.2)
h: Yes we have No bananas  # a string, \"Yes\" and \"No\" disambiguated by context.
)",
R"(
---
picture: !!binary |
 R0lGODdhDQAIAIAAAAAAANn
 Z2SwAAAAADQAIAAACF4SDGQ
 ar3xxbJ9p0qa7R0YxwzaFME
 1IAADs=
)",
R"(
---
myObject:  !myClass { name: Joe, age: 15 }
)",
R"(

)",
R"(
)",
R"(
)",
R"(
)",
R"(
)",
};
