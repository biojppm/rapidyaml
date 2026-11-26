#include "./test_group/test_group.hpp"
#include "./test_group/test_group.def.hpp"
#include "./test_lib/tree_and_ints.hpp"
#ifndef C4_YML_REFERENCE_RESOLVER_HPP_
#include "c4/yml/reference_resolver.hpp"
#endif

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//     SIMPLE_ANCHOR/YmlTestCase.parse_using_ryml/0

C4_SUPPRESS_WARNING_GCC_WITH_PUSH("-Wuseless-cast")

/** verify that the reference class is working correctly (yay, testing the tests) */
TEST(CaseNode, anchors)
{
    const NodeType mask = KEYREF|KEYANCH|VALREF|VALANCH;

    {
        auto n = N("*vref", AR(KEYREF, "vref"), "c");
        EXPECT_EQ(n.key, "*vref");
        EXPECT_EQ(n.val, "c");
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)KEYREF);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)KEYREF);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ(n.key_anchor.str, "vref");
        EXPECT_EQ(n.val_anchor.str, "");
    }

    {
        TestCaseNode n("<<", "*base", AR(VALANCH, "base"));
        EXPECT_EQ(n.key, "<<");
        EXPECT_EQ(n.val, "*base");
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)VALANCH);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)VALANCH);
        EXPECT_EQ(n.key_anchor.str, "");
        EXPECT_EQ(n.val_anchor.str, "base");
    }

    {
        TestCaseNode n("base", L{N("name", "Everyone has same name")}, AR(VALANCH, "base"));
        EXPECT_EQ(n.key, "base");
        EXPECT_EQ(n.val, "");
        EXPECT_NE(n.type.is_seq(), true);
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)VALANCH);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)VALANCH);
        EXPECT_EQ(n.key_anchor.str, "");
        EXPECT_EQ(n.val_anchor.str, "base");
    }

    {
        L l{N("<<", "*base", AR(VALREF, "base"))};
        TestCaseNode const& base = *l.begin();
        EXPECT_EQ(base.key, "<<");
        EXPECT_EQ(base.val, "*base");
        EXPECT_EQ(base.type.is_keyval(), true);
        EXPECT_EQ((type_bits)(base.type & mask), (type_bits)VALREF);
        EXPECT_EQ((type_bits)base.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)base.val_anchor.type, (type_bits)VALREF);
        EXPECT_EQ(base.key_anchor.str, "");
        EXPECT_EQ(base.val_anchor.str, "base");
    }

    {
        L l{N("<<", "*base", AR(VALREF, "base")), N("age", "10")};
        TestCaseNode const& base = *l.begin();
        TestCaseNode const& age = *(&base + 1);
        EXPECT_EQ(base.key, "<<");
        EXPECT_EQ(base.val, "*base");
        EXPECT_EQ(base.type.is_keyval(), true);
        EXPECT_EQ((type_bits)(base.type & mask), (type_bits)VALREF);
        EXPECT_EQ((type_bits)base.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)base.val_anchor.type, (type_bits)VALREF);
        EXPECT_EQ(base.key_anchor.str, "");
        EXPECT_EQ(base.val_anchor.str, "base");

        EXPECT_EQ(age.key, "age");
        EXPECT_EQ(age.val, "10");
        EXPECT_EQ(age.type.is_keyval(), true);
        EXPECT_EQ((type_bits)(age.type & mask), (type_bits)0);
        EXPECT_EQ((type_bits)age.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)age.val_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ(age.key_anchor.str, "");
        EXPECT_EQ(age.val_anchor.str, "");
    }

    {
        TestCaseNode n("foo", L{N("<<", "*base", AR(VALREF, "base")), N("age", "10")}, AR(VALANCH, "foo"));
        EXPECT_EQ(n.key, "foo");
        EXPECT_EQ(n.val, "");
        EXPECT_EQ(n.type.has_key(), true);
        EXPECT_EQ(n.type.is_map(), true);
        EXPECT_EQ((type_bits)(n.type & mask), (type_bits)VALANCH);
        EXPECT_EQ((type_bits)n.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)n.val_anchor.type, (type_bits)VALANCH);
        EXPECT_EQ(n.key_anchor.str, "");
        EXPECT_EQ(n.val_anchor.str, "foo");

        TestCaseNode const& base = n.children[0];
        EXPECT_EQ(base.key, "<<");
        EXPECT_EQ(base.val, "*base");
        EXPECT_EQ(base.type.has_key() && base.type.has_val(), true);
        EXPECT_EQ((type_bits)(base.type & mask), (type_bits)VALREF);
        EXPECT_EQ((type_bits)base.key_anchor.type, (type_bits)NOTYPE);
        EXPECT_EQ((type_bits)base.val_anchor.type, (type_bits)VALREF);
        EXPECT_EQ(base.key_anchor.str, "");
        EXPECT_EQ(base.val_anchor.str, "base");
    }

}

C4_SUPPRESS_WARNING_GCC_POP


//-----------------------------------------------------------------------------

CASE_GROUP(SIMPLE_ANCHOR)
{

ADD_CASE_TO_GROUP("anchor colon ambiguity 1",
R"({*foo : bar})",
N(MFS, L{N(KEY|VP, "*foo", AR(KEYREF, "foo"), "bar")})
);

ADD_CASE_TO_GROUP("anchor colon ambiguity 2", EXPECT_PARSE_ERROR,
R"({*foo: bar})",
   Location(1, 8)
);

ADD_CASE_TO_GROUP("merge example, unresolved",
R"(# https://yaml.org/type/merge.html
- &CENTER { x: 1, y: 2 }
- &LEFT { x: 0, y: 2 }
- &BIG { r: 10 }
- &SMALL { r: 1 }

# All the following maps are equal:

- # Explicit keys
  x: 1
  y: 2
  r: 10
  label: center/big

- # Merge one map
  << : *CENTER
  r: 10
  label: center/big

- # Merge multiple maps
  << : [ *CENTER, *BIG ]
  label: center/big

- # Override
  << : [ *BIG, *LEFT, *SMALL ]
  x: 1
  label: center/big
)",
N(SB, L{
    N(MFS, L{N(KP|VP, "x", "1" ), N(KP|VP, "y", "2")}, AR(VALANCH, "CENTER")),
    N(MFS, L{N(KP|VP, "x", "0" ), N(KP|VP, "y", "2")}, AR(VALANCH, "LEFT"  )),
    N(MFS, L{N(KP|VP, "r", "10")}, AR(VALANCH, "BIG"   )),
    N(MFS, L{N(KP|VP, "r", "1" )}, AR(VALANCH, "SMALL" )),
    N(MB, L{N(KP|VP, "x", "1" ), N(KP|VP, "y", "2"), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big")}),
    N(MB, L{N(KP|VAL, "<<", "*CENTER", AR(VALREF, "*CENTER")), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big")}),
    N(MB, L{N(KP|SFS, "<<", L{N(VAL, "*CENTER", AR(VALREF, "*CENTER")), N(VAL, "*BIG", AR(VALREF, "*BIG"))}), N(KP|VP, "label", "center/big")}),
    N(MB, L{N(KP|SFS, "<<", L{N(VAL, "*BIG", AR(VALREF, "*BIG")), N(VAL, "*LEFT", AR(VALREF, "*LEFT")), N(VAL, "*SMALL", AR(VALREF, "*SMALL"))}), N(KP|VP, "x", "1"), N(KP|VP, "label", "center/big")}),
})
);

ADD_CASE_TO_GROUP("merge example, resolved", RESOLVE_REFS,
R"(# https://yaml.org/type/merge.html
- &CENTER { x: 1, y: 2 }
- &LEFT { x: 0, y: 2 }
- &BIG { r: 10 }
- &SMALL { r: 1 }

# All the following maps are equal:

- # Explicit keys
  x: 1
  y: 2
  r: 10
  label: center/big0

- # Merge one map
  << : *CENTER
  r: 10
  label: center/big1

- # Merge multiple maps
  << : [ *CENTER, *BIG ]
  label: center/big2

- # Override
  << : [ *SMALL, *LEFT, *BIG ]
  x: 1
  label: center/big3
)",
N(SB, L{
    N(MFS, L{N(KP|VP, "x", "1" ), N(KP|VP, "y", "2")}),
    N(MFS, L{N(KP|VP, "x", "0" ), N(KP|VP, "y", "2")}),
    N(MFS, L{N(KP|VP, "r", "10")}),
    N(MFS, L{N(KP|VP, "r", "1" )}),
    N(MB, L{N(KP|VP, "x", "1"), N(KP|VP, "y", "2"), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big0")}),
    N(MB, L{N(KP|VP, "x", "1"), N(KP|VP, "y", "2"), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big1")}),
    N(MB, L{N(KP|VP, "x", "1"), N(KP|VP, "y", "2"), N(KP|VP, "r", "10"), N(KP|VP, "label", "center/big2")}),
    N(MB, L{N(KP|VP, "r", "1"), N(KP|VP, "x", "1"), N(KP|VP, "y", "2" ), N(KP|VP, "label", "center/big3")}),
})
);

ADD_CASE_TO_GROUP("simple anchor 1, implicit, unresolved",
R"(
anchored_content: &anchor_name This string will appear as the value of two keys.
other_anchor: *anchor_name
anchors_in_seqs:
  - &anchor_in_seq this value appears in both elements of the sequence
  - *anchor_in_seq
base: &base
    name: Everyone has same name
foo: &foo
    <<: *base
    age: 10
bar: &bar
    <<: *base
    age: 20
)",
N(MB, L{
      N(KP|VP, "anchored_content", "This string will appear as the value of two keys.", AR(VALANCH, "anchor_name")),
      N(KP|VAL, "other_anchor", "*anchor_name", AR(VALREF, "anchor_name")),
      N(KP|SB, "anchors_in_seqs", L{
              N(VP, "this value appears in both elements of the sequence", AR(VALANCH, "anchor_in_seq")),
              N(VAL, "*anchor_in_seq", AR(VALREF, "anchor_in_seq")),
          }),
      N(KP|MB, "base", L{N(KP|VP, "name", "Everyone has same name")}, AR(VALANCH, "base")),
      N(KP|MB, "foo", L{N(KP|VAL, "<<", "*base", AR(VALREF, "base")), N(KP|VP, "age", "10")}, AR(VALANCH, "foo")),
      N(KP|MB, "bar", L{N(KP|VAL, "<<", "*base", AR(VALREF, "base")), N(KP|VP, "age", "20")}, AR(VALANCH, "bar")),
  })
);

ADD_CASE_TO_GROUP("simple anchor 1, explicit, unresolved",
R"({
anchored_content: &anchor_name This string will appear as the value of two keys.,
other_anchor: *anchor_name,
anchors_in_seqs: [
  &anchor_in_seq this value appears in both elements of the sequence,
  *anchor_in_seq
  ],
base: &base {
    name: Everyone has same name
  },
foo: &foo {
    <<: *base,
    age: 10
  },
bar: &bar {
    <<: *base,
    age: 20
  }
})",
N(MFM,  L{
      N(KP|VP, "anchored_content", "This string will appear as the value of two keys.", AR(VALANCH, "anchor_name")),
      N(KP|VAL, "other_anchor", "*anchor_name", AR(VALREF, "anchor_name")),
      N(KP|SFM, "anchors_in_seqs", L{
              N(VP, "this value appears in both elements of the sequence", AR(VALANCH, "anchor_in_seq")),
              N(VAL, "*anchor_in_seq", AR(VALREF, "anchor_in_seq")),
          }),
      N(KP|MFM, "base", L{N(KP|VP, "name", "Everyone has same name")}, AR(VALANCH, "base")),
      N(KP|MFM, "foo", L{N(KP|VAL, "<<", "*base", AR(VALREF, "base")), N(KP|VP, "age", "10")}, AR(VALANCH, "foo")),
      N(KP|MFM, "bar", L{N(KP|VAL, "<<", "*base", AR(VALREF, "base")), N(KP|VP, "age", "20")}, AR(VALANCH, "bar")),
  })
);

ADD_CASE_TO_GROUP("simple anchor 1, implicit, resolved", RESOLVE_REFS,
R"(
anchored_content: &anchor_name This string will appear as the value of two keys.
other_anchor: *anchor_name
anchors_in_seqs:
  - &anchor_in_seq this value appears in both elements of the sequence
  - *anchor_in_seq
base: &base
    name: Everyone has same name
foo: &foo
    <<: *base
    age: 10
bar: &bar
    <<: *base
    age: 20
)",
N(MB, L{
      N(KP|VP, "anchored_content", "This string will appear as the value of two keys."),
      N(KP|VP, "other_anchor", "This string will appear as the value of two keys."),
      N(KP|SB, "anchors_in_seqs", L{
              N(VP, "this value appears in both elements of the sequence"),
              N(VP, "this value appears in both elements of the sequence"),
          }),
      N(KP|MB, "base", L{N(KP|VP, "name", "Everyone has same name")}),
      N(KP|MB, "foo", L{N(KP|VP, "name", "Everyone has same name"), N(KP|VP, "age", "10")}),
      N(KP|MB, "bar", L{N(KP|VP, "name", "Everyone has same name"), N(KP|VP, "age", "20")}),
  })
);

ADD_CASE_TO_GROUP("simple anchor 1, explicit, resolved", RESOLVE_REFS,
R"({
anchored_content: &anchor_name This string will appear as the value of two keys.,
other_anchor: *anchor_name,
anchors_in_seqs: [
  &anchor_in_seq this value appears in both elements of the sequence,
  *anchor_in_seq
  ],
base: &base {
    name: Everyone has same name
  },
foo: &foo {
    <<: *base,
    age: 10
  },
bar: &bar {
    <<: *base,
    age: 20
  }
})",
N(MFM, L{
      N(KP|VP, "anchored_content", "This string will appear as the value of two keys."),
      N(KP|VP, "other_anchor", "This string will appear as the value of two keys."),
      N(KP|SFM, "anchors_in_seqs", L{
              N(VP, "this value appears in both elements of the sequence"),
              N(VP, "this value appears in both elements of the sequence"),
          }),
      N(KP|MFM, "base", L{N(KP|VP, "name", "Everyone has same name")}),
      N(KP|MFM, "foo", L{N(KP|VP, "name", "Everyone has same name"), N(KP|VP, "age", "10")}),
      N(KP|MFM, "bar", L{N(KP|VP, "name", "Everyone has same name"), N(KP|VP, "age", "20")}),
  })
);


ADD_CASE_TO_GROUP("anchor example 2, unresolved",
R"(
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
      descrip:   High Heeled "Ruby" Slippers
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
)",
N(MB, L{
  N{KP|VP, "receipt", "Oz-Ware Purchase Invoice"},
  N{KP|VP, "date",    "2012-08-06"},
  N{KP|MB, "customer", L{N{KP|VP, "first_name", "Dorothy"}, N{KP|VP, "family_name", "Gale"}}},
  N{KP|SB, "items", L{
    N{MB, L{N{KP|VP, "part_no",   "A4786"},
        N{KP|VP, "descrip",   "Water Bucket (Filled)"},
        N{KP|VP, "price",     "1.47"},
        N{KP|VP, "quantity",  "4"},}},
    N{MB, L{N{KP|VP, "part_no", "E1628"},
        N{KP|VP, "descrip",   "High Heeled \"Ruby\" Slippers"},
        N{KP|VP, "size",      "8"},
        N{KP|VP, "price",     "133.7"},
        N{KP|VP, "quantity",  "1"},}}}},
   N{KP|MB, "bill-to", L{
        N{KP|VL, "street", "123 Tornado Alley\nSuite 16\n"},
        N{KP|VP, "city", "East Centerville"},
        N{KP|VP, "state", "KS"},}, AR(VALANCH, "id001")},
   N{KP|VAL, "ship-to", "*id001", AR(VALREF, "id001")},
   N{KP|VF, "specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
  })
);


ADD_CASE_TO_GROUP("anchor example 2, resolved", RESOLVE_REFS,
R"(
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
      descrip:   High Heeled "Ruby" Slippers
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
)",
N(MB, L{
  N{KP|VP, "receipt", "Oz-Ware Purchase Invoice"},
  N{KP|VP, "date",    "2012-08-06"},
  N{KP|MB, "customer", L{
    N{KP|VP, "first_name", "Dorothy"},
    N{KP|VP, "family_name", "Gale"}
  }},
  N{KP|SB, "items", L{
    N{MB, L{
      N{KP|VP, "part_no",   "A4786"},
      N{KP|VP, "descrip",   "Water Bucket (Filled)"},
      N{KP|VP, "price",     "1.47"},
      N{KP|VP, "quantity",  "4"},
    }},
    N{MB, L{
      N{KP|VP, "part_no", "E1628"},
      N{KP|VP, "descrip",   "High Heeled \"Ruby\" Slippers"},
      N{KP|VP, "size",      "8"},
      N{KP|VP, "price",     "133.7"},
      N{KP|VP, "quantity",  "1"},
    }}
  }},
  N{KP|MB, "bill-to", L{
    N{KP|VL, "street", "123 Tornado Alley\nSuite 16\n"},
    N{KP|VP, "city", "East Centerville"},
    N{KP|VP, "state", "KS"},
  }},
  N{KP|MB, "ship-to", L{
    N{KP|VL, "street", "123 Tornado Alley\nSuite 16\n"},
    N{KP|VP, "city", "East Centerville"},
    N{KP|VP, "state", "KS"},
  }},
  N{KP|VF, "specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
})
);

ADD_CASE_TO_GROUP("anchor example 3, unresolved",
R"(
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
N(SB, L{
  N(MB, L{
    N(KP|MB, "step", L{
      N{KP|VP, "instrument",      "Lasik 2000"},
      N{KP|VP, "pulseEnergy",     "5.4"},
      N{KP|VP, "pulseDuration",   "12"},
      N{KP|VP, "repetition",      "1000"},
      N{KP|VP, "spotSize",        "1mm"},
    }, AR(VALANCH, "id001")),
  }),
  N(MB, L{
    N(KP|MB, "step", L{
      N{KP|VP, "instrument",      "Lasik 2000"},
      N{KP|VP, "pulseEnergy",     "5.0"},
      N{KP|VP, "pulseDuration",   "10"},
      N{KP|VP, "repetition",      "500"},
      N{KP|VP, "spotSize",        "2mm"},
    }, AR(VALANCH, "id002")),
  }),
  N(MB, L{
    N{KP|VAL, "step", "*id001", AR(VALREF, "id001")},
  }),
  N(MB, L{
    N{KP|VAL, "step", "*id002", AR(VALREF, "id002")},
  }),
  N(MB, L{
    N{KP|MB, "step", L{
      N{KP|VAL, "<<", "*id001", AR(VALREF, "id002")},
      N{KP|VP, "spotSize", "2mm"},
    }},
  }),
  N(MB, L{
    N{KP|VAL, "step", "*id002", AR(VALREF, "id002")},
  }),
})
);

ADD_CASE_TO_GROUP("anchor example 3, resolved", RESOLVE_REFS,
R"(
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
N(SB, L{
        N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.4"},
    N{KP|VP, "pulseDuration",   "12"},
    N{KP|VP, "repetition",      "1000"},
    N{KP|VP, "spotSize",        "1mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.0"},
    N{KP|VP, "pulseDuration",   "10"},
    N{KP|VP, "repetition",      "500"},
    N{KP|VP, "spotSize",        "2mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.4"},
    N{KP|VP, "pulseDuration",   "12"},
    N{KP|VP, "repetition",      "1000"},
    N{KP|VP, "spotSize",        "1mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.0"},
    N{KP|VP, "pulseDuration",   "10"},
    N{KP|VP, "repetition",      "500"},
    N{KP|VP, "spotSize",        "2mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.4"},
    N{KP|VP, "pulseDuration",   "12"},
    N{KP|VP, "repetition",      "1000"},
    N{KP|VP, "spotSize",        "2mm"},
        }},
    }), N(MB, L{
N{KP|MB, "step", L{
    N{KP|VP, "instrument",      "Lasik 2000"},
    N{KP|VP, "pulseEnergy",     "5.0"},
    N{KP|VP, "pulseDuration",   "10"},
    N{KP|VP, "repetition",      "500"},
    N{KP|VP, "spotSize",        "2mm"},
        }},
    }),
})
);

ADD_CASE_TO_GROUP("tagged doc with anchors 9KAX",
R"(
---
&a1
!!str
scalar1
--- &a1 !!str scalar1
---
!!str
&a1
scalar1
--- !!str &a1 scalar1
---
!!str
&a2
scalar2
--- &a2 !!str scalar2
---
&a3
!!str scalar3
--- &a3 !!str scalar3
---
&a4 !!map
&a5 !!str key5: value4
--- &a4 !!map
&a5 !!str key5: value4
---
a6: 1
&anchor6 b6: 2
---
!!map
&a8 !!str key8: value7
--- !!map
&a8 !!str key8: value7
---
!!map
!!str &a10 key10: value9
--- !!map
&a10 !!str key10: value9
---
!!str &a11
value11
--- &a11 !!str value11
)",
N(STREAM, L{
    N(DOC|VP, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOC|VP, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOC|VP, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOC|VP, TS("!!str", "scalar1"), AR(VALANCH, "a1")),
    N(DOC|VP, TS("!!str", "scalar2"), AR(VALANCH, "a2")),
    N(DOC|VP, TS("!!str", "scalar2"), AR(VALANCH, "a2")),
    N(DOC|VP, TS("!!str", "scalar3"), AR(VALANCH, "a3")),
    N(DOC|VP, TS("!!str", "scalar3"), AR(VALANCH, "a3")),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key5"), AR(KEYANCH, "a5"), "value4")}), AR(VALANCH, "a4")),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key5"), AR(KEYANCH, "a5"), "value4")}), AR(VALANCH, "a4")),
    N(DOC|MB, L{N(KP|VP, "a6", "1"), N(KP|VP, "b6", AR(KEYANCH, "anchor6"), "2")}),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key8"), AR(KEYANCH, "a8"), "value7")})),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key8"), AR(KEYANCH, "a8"), "value7")})),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key10"), AR(KEYANCH, "a10"), "value9")})),
    N(DOC|MB, TL("!!map", L{N(KP|VP, TS("!!str", "key10"), AR(KEYANCH, "a10"), "value9")})),
    N(DOC|VP, TS("!!str", "value11"), AR(VALANCH, "a11")),
    N(DOC|VP, TS("!!str", "value11"), AR(VALANCH, "a11")),
})
);

ADD_CASE_TO_GROUP("github131 1, unresolved",
R"(
a: &vref b
*vref : c
&kref aa: bb
aaa: &kvref bbb
foo:
  *kref : cc
  *kvref : dd
)",
N(MB, L{
    N(KP|VP, "a", "b", AR(VALANCH, "vref")),
    N(KEY|VP, "*vref", AR(KEYREF, "vref"), "c"),
    N(KP|VP, "aa", AR(KEYANCH, "kref"), "bb"),
    N(KP|VP, "aaa", "bbb", AR(VALANCH, "kvref")),
    N(KP|MB, "foo", L{
         N(KEY|VP, "*kref", AR(KEYREF, "kref"), "cc"),
         N(KEY|VP, "*kvref", AR(KEYREF, "kvref"), "dd"),
    })
})
);

ADD_CASE_TO_GROUP("github131 1, resolved", RESOLVE_REFS,
R"(
a: &vref b
*vref : c
&kref aa: bb
aaa: &kvref bbb
foo:
  *kref : cc
  *kvref : dd
)",
N(MB, L{
    N(KP|VP, "a", "b"),
    N(KP|VP, "b", "c"),
    N(KP|VP, "aa", "bb"),
    N(KP|VP, "aaa", "bbb"),
    N(KP|MB, "foo", L{N(KP|VP, "aa", "cc"), N(KP|VP, "bbb", "dd")})
})
);


ADD_CASE_TO_GROUP("anchors+refs on key+val, unresolved",
R"({&a0 a0: &b0 b0, *b0 : *a0})",
N(MFS, L{
    N(KP|VP, "a0", AR(KEYANCH, "a0"), "b0", AR(VALANCH, "b0")),
    N(AR(KEYREF, "*b0"), AR(VALREF, "*a0")),
})
);

ADD_CASE_TO_GROUP("anchors+refs on key+val, resolved", RESOLVE_REFS,
R"({&a0 a0: &b0 b0, *b0 : *a0})",
N(MFS, L{
    N(KP|VP, "a0", "b0"),
    N(KP|VP, "b0", "a0"),
})
);


ADD_CASE_TO_GROUP("ambiguous anchor, unresolved", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(&rootanchor
&a0 a0: &b0 b0
*b0 : *a0
map1:
  &a1 a1: &b1 b1  # &a1 must be a KEY anchor on a1, not a VAL anchor on map1
  *b1 : *a1
map2:
  *b0 : *a0  # ensure the anchor is enough to establish the indentation
  &a2 a2: &b2 b2
  *b2 : *a2
map3: &a3   # &a3 must be a VAL anchor on map3, not a KEY anchor on a3
  a3: &b3 b3
  *b3 : *b0
map4: *a0
map5:
  &map5
  &a5 a5: &b5 b5
  *b5 : *a5
map6:
  &map6
  a6: &b6 b6
  *b6 : *b6
)",
N(MB, L{
    N(KP|VP, "a0", AR(KEYANCH, "a0"), "b0", AR(VALANCH, "b0")),
    N(AR(KEYREF, "*b0"), AR(VALREF, "*a0")),
    N(KP|MB, "map1", L{N(KP|VP, "a1", AR(KEYANCH, "a1"), "b1", AR(VALANCH, "b1")), N(AR(KEYREF, "*b1"), AR(VALREF, "*a1")),}),
    N(KP|MB, "map2", L{N(AR(KEYREF, "*b0"), AR(VALREF, "*a0")), N(KP|VP, "a2", AR(KEYANCH, "a2"), "b2", AR(VALANCH, "b2")), N(AR(KEYREF, "*b2"), AR(VALREF, "*a2")),}),
    N(KP|MB, "map3", L{N(KP|VP, "a3", "b3", AR(VALANCH, "b3")), N(AR(KEYREF, "*b3"), AR(VALREF, "*b0")),}, AR(VALANCH, "a3")),
    N(KP|VAL, "map4", "*a0", AR(VALREF, "a0")),
    N(KP|MB, "map5", L{N(KP|VP, "a5", AR(KEYANCH, "a5"), "b5", AR(VALANCH, "b5")), N(AR(KEYREF, "*b5"), AR(VALREF, "*a5")),}, AR(VALANCH, "map5")),
    N(KP|MB, "map6", L{N(KP|VP, "a6", "b6", AR(VALANCH, "b6")), N(AR(KEYREF, "*b6"), AR(VALREF, "*b6")),}, AR(VALANCH, "map6")),
}, AR(VALANCH, "rootanchor"))
);

ADD_CASE_TO_GROUP("ambiguous anchor, resolved", RESOLVE_REFS,
R"(
&a0 a0: &b0 b0
*b0 : *a0
map1:
  &a1 a1: &b1 b1  # &a1 must be a KEY anchor on a1, not a VAL anchor on map1
  *b1 : *a1
map2:
  *b0 : *a0  # ensure the anchor is enough to establish the indentation
  &a2 a2: &b2 b2
  *b2 : *a2
map3: &a3   # &a3 must be a VAL anchor on map3, not a KEY anchor on a3
  a3: &b3 b3
  *b3 : *b0
map4: *a0
map5:
  &map5
  &a5 a5: &b5 b5
  *b5 : *a5
map6:
  &map6
  a6: &b6 b6
  *b6 : *b6
)",
N(MB, L{
    N(KP|VP, "a0", "b0"), N(KP|VP, "b0", "a0"),
    N(KP|MB, "map1", L{N(KP|VP, "a1", "b1"), N(KP|VP, "b1", "a1"),}),
    N(KP|MB, "map2", L{N(KP|VP, "b0", "a0"), N(KP|VP, "a2", "b2"), N(KP|VP, "b2", "a2"),}),
    N(KP|MB, "map3", L{N(KP|VP, "a3", "b3"), N(KP|VP, "b3", "b0"),}),
    N(KP|VP, "map4", "a0"),
    N(KP|MB, "map5", L{N(KP|VP, "a5", "b5"), N(KP|VP, "b5", "a5"),}),
    N(KP|MB, "map6", L{N(KP|VP, "a6", "b6"), N(KP|VP, "b6", "b6"),}),
})
);


ADD_CASE_TO_GROUP("ambiguous anchor in seq, unresolved", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
&seq
- &a0
  &a1 k1: v1
  &a2 k2: v2
  &a3 k3: v3
- &a4 k4: v4
  &a5 k5: v5
  &a6 k6: v6
- &a7
  &a8 k8: v8
- &a9
  k10: v10
- *a1 : w1
  *a2 : w2
  *a3 : w3
  *a4 : w4
  *a5 : w5
  *a6 : w6
  *a8 : w8
- *a0
- *a7
- *a8
- *a9
)",
N(SB, L{
  N(MB, L{N(KP|VP, "k1", AR(KEYANCH, "a1"), "v1"), N(KP|VP, "k2", AR(KEYANCH, "a2"), "v2"), N(KP|VP, "k3", AR(KEYANCH, "a3"), "v3")}, AR(VALANCH, "a0")),
  N(MB, L{N(KP|VP, "k4", AR(KEYANCH, "a4"), "v4"), N(KP|VP, "k5", AR(KEYANCH, "a5"), "v5"), N(KP|VP, "k6", AR(KEYANCH, "a6"), "v6")}),
  N(MB, L{N(KP|VP, "k8", AR(KEYANCH, "a8"), "v8")}, AR(VALANCH, "a7")),
  N(MB, L{N(KP|VP, "k10", "v10")}, AR(VALANCH, "a9")),
  N(MB, L{
      N(KEY|VP, "*a1", AR(KEYREF, "*a1"), "w1"),
      N(KEY|VP, "*a2", AR(KEYREF, "*a2"), "w2"),
      N(KEY|VP, "*a3", AR(KEYREF, "*a3"), "w3"),
      N(KEY|VP, "*a4", AR(KEYREF, "*a4"), "w4"),
      N(KEY|VP, "*a5", AR(KEYREF, "*a5"), "w5"),
      N(KEY|VP, "*a6", AR(KEYREF, "*a6"), "w6"),
      N(KEY|VP, "*a8", AR(KEYREF, "*a8"), "w8"),
  }),
  N(VAL, "*a0", AR(VALREF, "*a0")),
  N(VAL, "*a7", AR(VALREF, "*a7")),
  N(VAL, "*a8", AR(VALREF, "*a8")),
  N(VAL, "*a9", AR(VALREF, "*a9")),
}, AR(VALANCH, "seq"))
);

ADD_CASE_TO_GROUP("ambiguous anchor in seq, resolved", RESOLVE_REFS,
R"(
&seq
- &a0
  &a1 k1: v1
  &a2 k2: v2
  &a3 k3: v3
- &a4 k4: v4
  &a5 k5: v5
  &a6 k6: v6
- &a7
  &a8 k8: v8
- &a9
  k10 : v10
- *a1 : w1
  *a2 : w2
  *a3 : w3
  *a4 : w4
  *a5 : w5
  *a6 : w6
  *a8 : w8
- *a0
- *a7
- *a8
- *a9
)",
N(SB, L{
  N(MB, L{N(KP|VP, "k1", "v1"), N(KP|VP, "k2", "v2"), N(KP|VP, "k3", "v3")}),
  N(MB, L{N(KP|VP, "k4", "v4"), N(KP|VP, "k5", "v5"), N(KP|VP, "k6", "v6")}),
  N(MB, L{N(KP|VP, "k8", "v8")}),
  N(MB, L{N(KP|VP, "k10", "v10")}),
  N(MB, L{
      N(KP|VP, "k1", "w1"),
      N(KP|VP, "k2", "w2"),
      N(KP|VP, "k3", "w3"),
      N(KP|VP, "k4", "w4"),
      N(KP|VP, "k5", "w5"),
      N(KP|VP, "k6", "w6"),
      N(KP|VP, "k8", "w8"),
  }),
  N(MB, L{N(KP|VP, "k1", "v1"), N(KP|VP, "k2", "v2"), N(KP|VP, "k3", "v3")}),
  N(MB, L{N(KP|VP, "k8", "v8")}),
  N(VP, "k8"),
  N(MB, L{N(KP|VP, "k10", "v10")}),
})
);

ADD_CASE_TO_GROUP("anchor after complex key without value ZWK4", NO_COMPARE_EMITTED_INTS_JSON,//FIXME
R"(
a: 1
? b
&anchor c: 3
)",
N(MB, L{
    N(KP|VP, "a", "1"), N(KP|VN, "b", {}), N(KP|VP, "c", AR(KEYANCH, "anchor"), "3")
  })
);

ADD_CASE_TO_GROUP("anchor mixed with tag HMQ5, unresolved",
R"(
!!str &a1 "foo":
  !!str bar
&a2 baz : *a1
)",
N(MB, L{
      N(KD|VP, TS("!!str", "foo"), AR(KEYANCH, "a1"), TS("!!str", "bar")),
      N(KP|VAL, "baz", AR(KEYANCH, "a2"), "*a1", AR(VALREF, "*a1")),
  })
);

ADD_CASE_TO_GROUP("anchor mixed with tag HMQ5, resolved", RESOLVE_REFS,
R"(
!!str &a1 "foo":
  !!str bar
&a2 baz : *a1
)",
N(MB, L{
      N(KD|VP, TS("!!str", "foo"), TS("!!str", "bar")),
      N(KP|VD, "baz", "foo"),
  })
);

ADD_CASE_TO_GROUP("github 484, resolved", RESOLVE_REFS,
R"(
base_1: &base_1
  a: 10
  b: 10
base_2: &base_2
  b: 20
k1:
  !!merge <<: *base_1
  !!merge <<: *base_2
k2:
  !!merge <<: *base_2
  !!merge <<: *base_1
)",
N(MB, L{
      N(KP|MB, "base_1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "base_2", L{/*                 */N(KP|VP, "b", "20")}),
      N(KP|MB, "k1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "k2", L{N(KP|VP, "b", "20"), N(KP|VP, "a", "10")}),
  })
);

ADD_CASE_TO_GROUP("github 484, resolved, anch", RESOLVE_REFS,
R"(
base_1: &base_1
  a: 10
  b: 10
base_2: &base_2
  b: 20
k1:
  &k1a <<: *base_1
  &k2b <<: *base_2
k2:
  &k2a <<: *base_2
  &k2b <<: *base_1
)",
N(MB, L{
      N(KP|MB, "base_1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "base_2", L{/*                 */N(KP|VP, "b", "20")}),
      N(KP|MB, "k1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "k2", L{N(KP|VP, "b", "20"), N(KP|VP, "a", "10")}),
  })
);

ADD_CASE_TO_GROUP("github 484, resolved, anch + tag", RESOLVE_REFS,
R"(
base_1: &base_1
  a: 10
  b: 10
base_2: &base_2
  b: 20
k1:
  &k1a !!merge <<: *base_1
  &k2b !!merge <<: *base_2
k2:
  &k2a !!merge <<: *base_2
  &k2b !!merge <<: *base_1
)",
N(MB, L{
      N(KP|MB, "base_1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "base_2", L{/*                 */N(KP|VP, "b", "20")}),
      N(KP|MB, "k1", L{N(KP|VP, "a", "10"), N(KP|VP, "b", "10")}),
      N(KP|MB, "k2", L{N(KP|VP, "b", "20"), N(KP|VP, "a", "10")}),
  })
);

}


C4_SUPPRESS_WARNING_GCC_POP

} // namespace yml
} // namespace c4
