#include "./test_group.hpp"

namespace c4 {
namespace yml {

#define SIMPLE_ANCHOR_CASES                            \
    "simple anchor 1, implicit, unresolved",\
    "simple anchor 1, implicit, resolved",\
    "simple anchor 1, explicit, unresolved",\
    "simple anchor 1, explicit, resolved",\
    "anchor example 2, unresolved",   \
    "anchor example 2, resolved",\
    "anchor example 3, unresolved",   \
    "anchor example 3, resolved"


CASE_GROUP(SIMPLE_ANCHOR)
{
    APPEND_CASES(

C("simple anchor 1, implicit, unresolved",
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
  L{
      N("anchored_content", "This string will appear as the value of two keys.", AR(VALANCH, "anchor_name")),
      N("other_anchor", "*anchor_name", AR(VALREF, "anchor_name")),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence", AR(VALANCH, "anchor_in_seq")),
              N(VALREF, "*anchor_in_seq", AR(VALREF, "anchor_in_seq")),
          }, AR()),
      N("base", L{N("name", "Everyone has same name")}, AR(VALANCH, "base")),
      N("foo", L{N(VALREF, "<<", "*base"), N("age", "10")}, AR(VALANCH, "foo")),
      N("bar", L{N(VALREF, "<<", "*base"), N("age", "20")}, AR(VALANCH, "bar")),
  }
),

C("simple anchor 1, explicit, unresolved",
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
  L{
      N("anchored_content", "This string will appear as the value of two keys.", AR(VALANCH, "anchor_name")),
      N("other_anchor", "*anchor_name", AR(VALREF, "anchor_name")),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence", AR(VALANCH, "anchor_in_seq")),
              N(VALREF, "*anchor_in_seq", AR(VALREF, "anchor_in_seq")),
          }, AR()),
      N("base", L{N("name", "Everyone has same name")}, AR(VALANCH, "base")),
      N("foo", L{N(VALREF, "<<", "*base"), N("age", "10")}, AR(VALANCH, "foo")),
      N("bar", L{N(VALREF, "<<", "*base"), N("age", "20")}, AR(VALANCH, "bar")),
  }
),

C("simple anchor 1, implicit, resolved", RESOLVE_REFS,
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
  L{
      N("anchored_content", "This string will appear as the value of two keys."),
      N("other_anchor", "This string will appear as the value of two keys."),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence"),
              N("this value appears in both elements of the sequence"),
          }),
      N("base", L{N("name", "Everyone has same name")}),
      N("foo", L{N("name", "Everyone has same name"), N("age", "10")}),
      N("bar", L{N("name", "Everyone has same name"), N("age", "20")}),
  }
),

C("simple anchor 1, explicit, resolved", RESOLVE_REFS,
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
  L{
      N("anchored_content", "This string will appear as the value of two keys."),
      N("other_anchor", "This string will appear as the value of two keys."),
      N("anchors_in_seqs", L{
              N("this value appears in both elements of the sequence"),
              N("this value appears in both elements of the sequence"),
          }),
      N("base", L{N("name", "Everyone has same name")}),
      N("foo", L{N("name", "Everyone has same name"), N("age", "10")}),
      N("bar", L{N("name", "Everyone has same name"), N("age", "20")}),
  }
),


C("anchor example 2, unresolved",
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
L{
  N{"receipt", "Oz-Ware Purchase Invoice"},
  N{"date",    "2012-08-06"},
  N{"customer", L{N{"first_name", "Dorothy"}, N{"family_name", "Gale"}}},
  N{"items", L{
    N{L{N{"part_no",   "A4786"},
        N{"descrip",   "Water Bucket (Filled)"},
        N{"price",     "1.47"},
        N{"quantity",  "4"},}},
    N{L{N{"part_no", "E1628"},
        N{"descrip",   "High Heeled \"Ruby\" Slippers"},
        N{"size",      "8"},
        N{"price",     "133.7"},
        N{"quantity",  "1"},}}}},
   N{"bill-to", L{
        N{"street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}, AR(VALANCH, "id001")},
   N{VALREF, "ship-to", "*id001", AR(VALREF, "id001")},
   N{"specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
  }
),


C("anchor example 2, resolved", RESOLVE_REFS,
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
L{
  N{"receipt", "Oz-Ware Purchase Invoice"},
  N{"date",    "2012-08-06"},
  N{"customer", L{N{"first_name", "Dorothy"}, N{"family_name", "Gale"}}},
  N{"items", L{
    N{L{N{"part_no",   "A4786"},
        N{"descrip",   "Water Bucket (Filled)"},
        N{"price",     "1.47"},
        N{"quantity",  "4"},}},
    N{L{N{"part_no", "E1628"},
        N{"descrip",   "High Heeled \"Ruby\" Slippers"},
        N{"size",      "8"},
        N{"price",     "133.7"},
        N{"quantity",  "1"},}}}},
   N{"bill-to", L{
        N{"street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}},
   N{"ship-to", L{
        N{"street", "123 Tornado Alley\nSuite 16\n"},
        N{"city", "East Centerville"},
        N{"state", "KS"},}},
   N{"specialDelivery", "Follow the Yellow Brick Road to the Emerald City. Pay no attention to the man behind the curtain.\n"}
  }
),

C("anchor example 3, unresolved",
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
L{N(L{
N("step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }, AR(VALANCH, "id001")),
    }), N(L{
N("step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }, AR(VALANCH, "id002")),
    }), N(L{
N{VALREF, "step", "*id001"},
    }), N(L{
N{VALREF, "step", "*id002"},
    }), N(L{
N{"step", L{
    N{VALREF, "<<", "*id001"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{VALREF, "step", "*id002"},
    }),
    }
),

C("anchor example 3, resolved", RESOLVE_REFS,
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
  L{N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "1mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.4"},
    N{"pulseDuration",   "12"},
    N{"repetition",      "1000"},
    N{"spotSize",        "2mm"},
        }},
    }), N(L{
N{"step", L{
    N{"instrument",      "Lasik 2000"},
    N{"pulseEnergy",     "5.0"},
    N{"pulseDuration",   "10"},
    N{"repetition",      "500"},
    N{"spotSize",        "2mm"},
        }},
    }),
    }
),
    )
}

INSTANTIATE_GROUP(SIMPLE_ANCHOR);

} // namespace yml
} // namespace c4
