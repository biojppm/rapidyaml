#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


//static constexpr const bool multiline = true;
static constexpr const bool singleline = false;


ENGINE_TEST(TagPlacementTerminatingColonFlow3SeqK,
            "[!tag30: :,!tag31: :]\n"
            ,
            "[{!tag30: : },{!tag31: : }]"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL <!tag30:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL <!tag31:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag30:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag31:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow3SeqK1,
            "[&anch30: !tag30: :,&anch31: !tag31: :]\n"
            ,
            "[{&anch30: !tag30: : },{&anch31: !tag31: : }]"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &anch30: <!tag30:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL &anch31: <!tag31:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch30:"));
    ___(ps.set_key_tag("!tag30:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch31:"));
    ___(ps.set_key_tag("!tag31:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow3SeqK2,
            "[!tag30: &anch30: :,!tag31: &anch31: :]\n"
            ,
            "[{&anch30: !tag30: : },{&anch31: !tag31: : }]"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &anch30: <!tag30:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL &anch31: <!tag31:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch30:"));
    ___(ps.set_key_tag("!tag30:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch31:"));
    ___(ps.set_key_tag("!tag31:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow3SeqK3,
            "[&anch30: :,&anch31: :]\n"
            ,
            "[{&anch30: : },{&anch31: : }]"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &anch30: :\n"
            "=VAL :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL &anch31: :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch30:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch31:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow5SeqV1,
            "[: !tag50:,: !tag51:]"
            ,
            "[{: !tag50: },{: !tag51: }]"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL <!tag50:> :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL <!tag51:> :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!tag50:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!tag51:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow5SeqV2,
            "[: &anch50: !tag50:,: &anch51: !tag51:]"
            ,
            "[{: &anch50: !tag50: },{: &anch51: !tag51: }]"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL &anch50: <!tag50:> :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL &anch51: <!tag51:> :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch50:"));
    ___(ps.set_val_tag("!tag50:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch51:"));
    ___(ps.set_val_tag("!tag51:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow5SeqV3,
            "[: !tag50: &anch50:,: !tag51: &anch51:]"
            ,
            "[{: &anch50: !tag50: },{: &anch51: !tag51: }]"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL &anch50: <!tag50:> :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL &anch51: <!tag51:> :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch50:"));
    ___(ps.set_val_tag("!tag50:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch51:"));
    ___(ps.set_val_tag("!tag51:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow5SeqV4,
            "[: &anch50:,: &anch51:]"
            ,
            "[{: &anch50: },{: &anch51: }]"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL &anch50: :\n"
            "-MAP\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL &anch51: :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch50:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.add_sibling());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch51:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(DoubleAnchorFuzz_0_anch, Location(1, 10), ""
                   "[? &? &a ]")
ENGINE_TEST_ERRLOC(DoubleAnchorFuzz_0_tag, Location(1, 10), ""
                   "[? !? !t ]")
ENGINE_TEST_ERRLOC(DoubleAnchorFuzz_1_anch, Location(1, 16), ""
                   "[? &? !? &a !t ]")
ENGINE_TEST_ERRLOC(DoubleAnchorFuzz_1_tag, Location(1, 16), ""
                   "[? !? &? !t &a ]")

ENGINE_TEST(Qmrk0AnchEmpty,
            "[? &a]"
            ,
            "[{&a : }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_scalar_plain_empty());
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0TagEmpty,
            "[? !t]"
            ,
            "[{!t : }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL <!t> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_plain_empty());
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0AnchAndTagEmpty,
            "[? &a !t]"
            ,
            "[{&a !t : }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a <!t> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_plain_empty());
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk0AnchEmptyComma,
            "[? &a,]"
            ,
            "[{&a : }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_scalar_plain_empty());
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0TagEmptyComma,
            "[? !t,]"
            ,
            "[{!t : }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL <!t> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_plain_empty());
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0AnchAndTagEmptyComma,
            "[? &a !t,]"
            ,
            "[{&a !t : }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a <!t> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_plain_empty());
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk0AnchPlain,
            "[? &a v]"
            ,
            "[{&a v: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a :v\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_scalar_plain("v"));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0TagPlain,
            "[? !t v]"
            ,
            "[{!t v: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL <!t> :v\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_plain("v"));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0AnchAndTagPlain,
            "[? &a !t v]"
            ,
            "[{&a !t v: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a <!t> :v\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_plain("v"));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk0AnchSquo,
            "[? &a 'v']"
            ,
            "[{&a 'v': }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a 'v\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_scalar_squoted("v"));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0TagSquo,
            "[? !t 'v']"
            ,
            "[{!t 'v': }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL <!t> 'v\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_squoted("v"));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0AnchAndTagSquo,
            "[? &a !t 'v']"
            ,
            "[{&a !t 'v': }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a <!t> 'v\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_squoted("v"));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk0AnchDquo,
            "[? &a \"v\"]"
            ,
            "[{&a \"v\": }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a \"v\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_scalar_dquoted("v"));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0TagDquo,
            "[? !t \"v\"]"
            ,
            "[{!t \"v\": }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL <!t> \"v\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_dquoted("v"));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0AnchAndTagDquo,
            "[? &a !t \"v\"]"
            ,
            "[{&a !t \"v\": }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "=VAL &a <!t> \"v\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_tag("!t"));
      ___(ps.set_key_scalar_dquoted("v"));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk0AnchSeq, HAS_CONTAINER_KEYS,
            "[? &a []]"
            ,
            "[{? &a []: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+SEQ &a []\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.begin_seq_key_flow());
      ___(ps.end_seq_flow(singleline));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0TagSeq, HAS_CONTAINER_KEYS,
            "[? !t []]"
            ,
            "[{? !t []: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+SEQ <!t> []\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_tag("!t"));
      ___(ps.begin_seq_key_flow());
      ___(ps.end_seq_flow(singleline));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0AnchAndTagSeq, HAS_CONTAINER_KEYS,
            "[? &a !t []]"
            ,
            "[{? &a !t []: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+SEQ &a <!t> []\n"
            "-SEQ\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_tag("!t"));
      ___(ps.begin_seq_key_flow());
      ___(ps.end_seq_flow(singleline));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(Qmrk0AnchMap, HAS_CONTAINER_KEYS,
            "[? &a {}]"
            ,
            "[{? &a {}: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+MAP &a {}\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.begin_map_key_flow());
      ___(ps.end_map_flow(singleline));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0TagMap, HAS_CONTAINER_KEYS,
            "[? !t {}]"
            ,
            "[{? !t {}: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+MAP <!t> {}\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_tag("!t"));
      ___(ps.begin_map_key_flow());
      ___(ps.end_map_flow(singleline));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST(Qmrk0AnchAndTagMap, HAS_CONTAINER_KEYS,
            "[? &a !t {}]"
            ,
            "[{? &a !t {}: }]"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ []\n"
            "+MAP {}\n"
            "+MAP &a <!t> {}\n"
            "-MAP\n"
            "=VAL :\n"
            "-MAP\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_flow());
    ___(ps.begin_map_val_flow());
      ___(ps.set_key_anchor("a"));
      ___(ps.set_key_tag("!t"));
      ___(ps.begin_map_key_flow());
      ___(ps.end_map_flow(singleline));
      ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_seq_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
