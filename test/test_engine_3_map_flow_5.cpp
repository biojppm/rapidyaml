#include "./test_lib/test_engine.hpp"
C4_SUPPRESS_WARNING_MSVC_WITH_PUSH(4702)

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {

static constexpr const bool singleline = false; // NOLINT


ENGINE_TEST(TagPlacementTerminatingColonFlow2MapK,
            "{!tag20:,!tag21:}\n"
            ,
            "{!tag20: : ,!tag21: : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag20:> :\n"
            "=VAL :\n"
            "=VAL <!tag21:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag20:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag21:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow2MapK1,
            "{&anch20: !tag20:,&anch21: !tag21:}\n"
            ,
            "{&anch20: !tag20: : ,&anch21: !tag21: : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch20: <!tag20:> :\n"
            "=VAL :\n"
            "=VAL &anch21: <!tag21:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch20:"));
    ___(ps.set_key_tag("!tag20:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("anch21:"));
    ___(ps.set_key_tag("!tag21:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow2MapK2,
            "{!tag20: &anch20:,!tag21: &anch21:}\n"
            ,
            "{&anch20: !tag20: : ,&anch21: !tag21: : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch20: <!tag20:> :\n"
            "=VAL :\n"
            "=VAL &anch21: <!tag21:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch20:"));
    ___(ps.set_key_tag("!tag20:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("anch21:"));
    ___(ps.set_key_tag("!tag21:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow2MapK3,
            "{&anch20:,&anch21:}\n"
            ,
            "{&anch20: : ,&anch21: : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch20: :\n"
            "=VAL :\n"
            "=VAL &anch21: :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch20:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("anch21:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

//-----------------------------------------------------------------------------

ENGINE_TEST(TagPlacementTerminatingColonFlow4MapV1,
            "{!tag40: :,!tag41: :}\n"
            ,
            "{!tag40: : ,!tag41: : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL <!tag40:> :\n"
            "=VAL :\n"
            "=VAL <!tag41:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_tag("!tag40:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_tag("!tag41:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow4MapV2,
            "{&anch40: !tag40: :,&anch41: !tag41: :}\n"
            ,
            "{&anch40: !tag40: : ,&anch41: !tag41: : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch40: <!tag40:> :\n"
            "=VAL :\n"
            "=VAL &anch41: <!tag41:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch40:"));
    ___(ps.set_key_tag("!tag40:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("anch41:"));
    ___(ps.set_key_tag("!tag41:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow4MapV3,
            "{!tag40: &anch40: :,!tag41: &anch41: :}\n"
            ,
            "{&anch40: !tag40: : ,&anch41: !tag41: : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch40: <!tag40:> :\n"
            "=VAL :\n"
            "=VAL &anch41: <!tag41:> :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch40:"));
    ___(ps.set_key_tag("!tag40:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("anch41:"));
    ___(ps.set_key_tag("!tag41:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow4MapV4,
            "{&anch40: :,&anch41: :}\n"
            ,
            "{&anch40: : ,&anch41: : }"
            ,
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL &anch40: :\n"
            "=VAL :\n"
            "=VAL &anch41: :\n"
            "=VAL :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_anchor("anch40:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_anchor("anch41:"));
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow6MapV1,
            "{: !tag60:,: !tag61:}\n"
            ,
            "{: !tag60: ,: !tag61: }"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL <!tag60:> :\n"
            "=VAL :\n"
            "=VAL <!tag61:> :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!tag60:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_tag("!tag61:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow6MapV2,
            "{: &anch60: !tag60:,: &anch61: !tag61:}\n"
            ,
            "{: &anch60: !tag60: ,: &anch61: !tag61: }"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL &anch60: <!tag60:> :\n"
            "=VAL :\n"
            "=VAL &anch61: <!tag61:> :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch60:"));
    ___(ps.set_val_tag("!tag60:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch61:"));
    ___(ps.set_val_tag("!tag61:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow6MapV3,
            "{: !tag60: &anch60:,: !tag61: &anch61:}\n"
            ,
            "{: &anch60: !tag60: ,: &anch61: !tag61: }"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL &anch60: <!tag60:> :\n"
            "=VAL :\n"
            "=VAL &anch61: <!tag61:> :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch60:"));
    ___(ps.set_val_tag("!tag60:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch61:"));
    ___(ps.set_val_tag("!tag61:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

ENGINE_TEST(TagPlacementTerminatingColonFlow6MapV4,
            "{: &anch60:,: &anch61:}\n"
            ,
            "{: &anch60: ,: &anch61: }"
            ,
            ""
            "+STR\n"
            "+DOC\n"
            "+MAP {}\n"
            "=VAL :\n"
            "=VAL &anch60: :\n"
            "=VAL :\n"
            "=VAL &anch61: :\n"
            "-MAP\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_map_val_flow());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch60:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.add_sibling());
    ___(ps.set_key_scalar_plain_empty());
    ___(ps.set_val_anchor("anch61:"));
    ___(ps.set_val_scalar_plain_empty());
    ___(ps.end_map_flow(singleline));
    ___(ps.end_doc());
    ___(ps.end_stream());
}

} // namespace yml
} // namespace c4

C4_SUPPRESS_WARNING_MSVC_POP
