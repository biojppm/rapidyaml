#include "./test_lib/test_engine.hpp"

// WARNING: don't use raw string literals -- g++4.8 cannot accept them
// as macro arguments

RYML_DEFINE_TEST_MAIN()

namespace c4 {
namespace yml {


//-----------------------------------------------------------------------------


ENGINE_TEST(TabsY79Y_004_01,
            "- \ta\n"
            ,
            "- a\n"
            ,
            "+STR\n"
            "+DOC\n"
            "+SEQ\n"
            "=VAL :a\n"
            "-SEQ\n"
            "-DOC\n"
            "-STR\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}
ENGINE_TEST_ERRLOC(Y79Y_004_02, Location(1, 7),
                   "- \ta: b\n"
                   )
#ifdef RYML_WITH_TAB_TOKENS
ENGINE_TEST_ERRLOC(Y79Y_004, Location(1, 3),
                   "-\t- a"
                   )
#endif
ENGINE_TEST_ERRLOC(Y79Y_005, Location(1, 4),
                   "- \t- a"
                   )


//-----------------------------------------------------------------------------

ENGINE_TEST_ERRLOC(NonFirstDashErr0, Location(1, 6),
                   "map: - a\n"
                   "     - b\n"
                   )

ENGINE_TEST_ERRLOC_(NonFirstDashErr1_1, HAS_CONTAINER_KEYS,
                    Location(2, 1),
                    "  - b\n"
                    "? - a\n"
                    )
ENGINE_TEST_ERRLOC_(NonFirstDashErr1_2, HAS_CONTAINER_KEYS,
                    Location(2, 3),
                    "  - b\n"
                    "  ? - a\n"
                    )
ENGINE_TEST_ERRLOC_(NonFirstDashErr1_3, HAS_CONTAINER_KEYS,
                    Location(2, 3),
                    "  - b\n"
                    "  ?- a\n"
                    )

ENGINE_TEST_ERRLOC(NonFirstDashErr2_0, Location(1, 4),
                   "&a - a\n"
                   "   - b\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr2_1, Location(2, 4), // FIXME column
                   "   - b\n"
                   "&a - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr2_2, Location(2, 4),
                   "   - b\n"
                   "   &a - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr2_3, Location(2, 4),
                   "   - b\n"
                   "   &a- a\n"
                   )

ENGINE_TEST_ERRLOC(NonFirstDashErr3_0, Location(1, 6),
                   "!tag - a\n"
                   "     - b\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr3_1, Location(2, 6), // FIXME column
                   "     - b\n"
                   "!tag - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr3_2, Location(2, 6),
                   "     - b\n"
                   "     !tag - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr3_3, Location(2, 6),
                   "     - b\n"
                   "     !tag- a\n"
                   )

ENGINE_TEST_ERRLOC(NonFirstDashErr4_0, Location(1, 7),
                   "&a !t - a\n"
                   "      - b\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr4_1, Location(2, 4), // FIXME column
                   "      - b\n"
                   "&a !t - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr4_2, Location(2, 7),
                   "      - b\n"
                   "      &a !t - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr4_3, Location(2, 7),
                   "      - b\n"
                   "      &a !t- a\n"
                   )

ENGINE_TEST_ERRLOC(NonFirstDashErr5_0, Location(1, 7),
                   "&a !t - a\n"
                   "      - b\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr5_1, Location(2, 4), // FIXME column
                   "      - b\n"
                   "&a !t - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr5_2, Location(2, 7),
                   "      - b\n"
                   "      &a !t - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr5_3, Location(2, 7),
                   "      - b\n"
                   "      &a !t- a\n"
                   )

ENGINE_TEST_ERRLOC(NonFirstDashErr6_0, Location(1, 4),
                   " \t - a\n"
                   "    - b\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr6_1, Location(2, 1),
                   "    - b\n"
                   " \t - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr6_2, Location(2, 5),
                   "    - b\n"
                   "    \t - a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr6_3, Location(2, 5),
                   "    - b\n"
                   "    \t- a\n"
                   )

ENGINE_TEST_ERRLOC(NonFirstDashErr7_0, Location(2, 3),
                   "  - a\n"
                   "  -a\n"
                   )
ENGINE_TEST_ERRLOC(NonFirstDashErr7_1, Location(2, 1),
                   "  - a\n"
                   " - a\n"
                   )

ENGINE_TEST(NonFirstDashOk0,
            "  - a\n"
            "  - b\n"
            ,
            "- a\n"
            "- b\n"
            ,
            "+STR"     "\n"
            "+DOC"     "\n"
            "+SEQ"     "\n"
            "=VAL :a"  "\n"
            "=VAL :b"  "\n"
            "-SEQ"     "\n"
            "-DOC"     "\n"
            "-STR"     "\n")
{
    ___(ps.begin_stream());
    ___(ps.begin_doc());
    ___(ps.begin_seq_val_block());
    ___(ps.set_val_scalar_plain("a"));
    ___(ps.add_sibling());
    ___(ps.set_val_scalar_plain("b"));
    ___(ps.end_seq_block());
    ___(ps.end_doc());
    ___(ps.end_stream());
}


struct TestNonFirstDashBOM : public testing::TestWithParam<bomspec> {};

template<class T>
static std::string namefor(testing::TestParamInfo<T> const& pinfo) // NOLINT
{
    return namefor(pinfo.param);
}
INSTANTIATE_TEST_SUITE_P(_, TestNonFirstDashBOM,
                         testing::ValuesIn(bomspecs.begin(), bomspecs.end()),
                         namefor<bomspec>);

static void test_non_first_dash_bom_success(csubstr fmt, csubstr bom)
{
    std::string src = formatrs<std::string>(fmt, bom);
    Tree t;
    ExpectError::check_success(&t, [&]{
        parse_in_place(to_substr(src), &t);
        ConstNodeRef r = t.rootref();
        EXPECT_TRUE(r.is_seq());
        EXPECT_EQ(r[0].val(), "a");
        EXPECT_EQ(r[1].val(), "b");
    });
}

static void test_non_first_dash_bom_error(csubstr fmt, csubstr bom, Location loc)
{
    std::string src = formatrs<std::string>(fmt, bom);
    Tree t;
    ExpectError::check_error_parse(&t, [&]{
        parse_in_place(to_substr(src), &t);
    }, loc);
}

TEST_P(TestNonFirstDashBOM, success_prevline)
{
    const bomspec p = GetParam();
    SCOPED_TRACE(p.name);
    csubstr fmt("{}\n"
                "- a\n"
                "- b\n");
    test_non_first_dash_bom_success(fmt, p.bom);
}

TEST_P(TestNonFirstDashBOM, success_before_no_spaces)
{
    const bomspec p = GetParam();
    SCOPED_TRACE(p.name);
    csubstr fmt("{}- a\n"
                "- b\n");
    test_non_first_dash_bom_success(fmt, p.bom);
}

TEST_P(TestNonFirstDashBOM, success_before_interleaved_spaces)
{
    const bomspec p = GetParam();
    SCOPED_TRACE(p.name);
    csubstr fmt("{}   - a\n"
                "   - b\n");
    test_non_first_dash_bom_success(fmt, p.bom);
}

TEST_P(TestNonFirstDashBOM, err_after)
{
    const bomspec p = GetParam();
    if(p.name == "NOBOM")
        return;
    SCOPED_TRACE(p.name);
    csubstr fmt("   - b\n"
                "{}   - a\n");
    size_t expected_col = p.name == "UTF8" ? 7 : 1; // FIXME
    test_non_first_dash_bom_error(fmt, p.bom, Location(2, expected_col));
}

TEST_P(TestNonFirstDashBOM, err_before)
{
    const bomspec p = GetParam();
    SCOPED_TRACE(p.name);
    csubstr fmt("{}&a - a\n"
                "   - b\n");
    size_t expected_col = p.bom.len + 4; // FIXME
    test_non_first_dash_bom_error(fmt, p.bom, Location(1, expected_col));
}

} // namespace yml
} // namespace c4


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// this is needed to use the test case library

#ifndef RYML_SINGLE_HEADER
#include "c4/substr.hpp"
#endif

namespace c4 {
namespace yml {
struct Case;
Case const* get_case(csubstr /*name*/)
{
    return nullptr;
}
} // namespace yml
} // namespace c4
