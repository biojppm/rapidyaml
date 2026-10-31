#include "test_lib/test_case.hpp"
#include "test_lib/test_helpers_ints.hpp"
#include <c4/yml/extra/event_handler_ints.hpp>

RYML_DEFINE_TEST_MAIN()

// NOLINTBEGIN(hicpp-signed-bitwise)

namespace c4 {
namespace yml {
namespace extra {


namespace {
#define test_cyclic_reference_(...) SCOPED_CALL(test_cyclic_reference, (__VA_ARGS__))
C4_NO_INLINE
void test_cyclic_reference(ievt::TestBuffers const& ints, bool cyclic,
                           ievt::evt_bits anchor_bits, ievt::evt_size anchor_pos, csubstr anchor,
                           ievt::evt_bits ref_bits, ievt::evt_size ref_pos, csubstr ref)
{
    ASSERT_TRUE(ievt::detail::hasall(ints.evts.ptr[anchor_pos], anchor_bits|ievt::ANCH));
    ASSERT_EQ(ints.getstr(anchor_pos), anchor);
    ASSERT_TRUE(ievt::detail::hasall(ints.evts.ptr[ref_pos], ref_bits|ievt::ALIA));
    ASSERT_EQ(ints.getstr(ref_pos), ref);
    if(anchor == ref)
    {
        EXPECT_EQ(cyclic, ievt::detail::is_cyclic_ref_(ints.evts.ptr, ints.evts.len, ref_pos, anchor_pos));
    }
}
C4_NO_INLINE
void test_cyclic_reference(ievt::TestBuffers & ints, bool cyclic)
{
    RYML_TRACE_FMT("src=~~~{}~~~", escaped_scalar(ints.src));
    if(cyclic)
    {
        RYML_EXPECT_ERROR(check_error_basic([&]{
            ints.resolve();
        }));
    }
    else
    {
        RYML_EXPECT_ERROR(check_success([&]{
            ints.resolve();
        }));
    }
    if(testing::Test::HasFailure())
        ints.print();
}
bool cyclic = true, noncyc = false;


#define test_ints_resolve(...) SCOPED_CALL(test_ints_resolve_, (__VA_ARGS__))
C4_NO_INLINE
void test_ints_resolve_(std::string & src,
                        std::string const& emitted_yaml,
                        std::string const& emitted_json,
                        std::string const& resolved_yaml,
                        std::string const& resolved_json,
                        cspan<ievt::evt_bits> evts_expected={},
                        cspan<ievt::evt_bits> evts_resolved={},
                        cspan<ievt::evt_size> pos_seq={},
                        cspan<ievt::evt_size> pos_seq_resolved={},
                        bool expect_resolve_error=false)
{
    SCOPED_TRACE("test_ints_resolve_");
    using namespace ievt;
    TestBuffers actual;
    parse_ints(to_substr(src), &actual);
    {
        SCOPED_TRACE("unresolved");
        bool exp = !evts_expected.empty();
        if(pos_seq.empty())
        {
            if(exp)
                SCOPED_CALL(verify_visit_seq, (evts_expected));
            SCOPED_CALL(verify_visit_seq, (actual.evts));
        }
        else
        {
            if(exp)
                SCOPED_CALL(verify_visit_seq, (evts_expected, pos_seq));
            SCOPED_CALL(verify_visit_seq, (actual.evts, pos_seq));
        }
        TestBuffers expected;
        if(exp)
        {
            expected = mkevts(actual, evts_expected);
            SCOPED_CALL(expected.test_invariants, ());
        }
        SCOPED_CALL(actual.test_invariants, ());
        if(exp)
            SCOPED_CALL(actual.test_compare, (expected));
        if(testing::Test::HasFailure())
        {
            printf("--------------\nexpected:");
            expected.print();
            printf("--------------\nactual:");
            actual.print();
            return;
        }
        {
            SCOPED_TRACE("yaml");
            std::string expect_emitted = exp ? expected.emitrs_yaml<std::string>() : "";
            std::string actual_emitted = actual.emitrs_yaml<std::string>();
            EXPECT_EQ(emitted_yaml, actual_emitted);
            if(exp)
            {
                EXPECT_EQ(expect_emitted, actual_emitted);
            }
            if(testing::Test::HasFailure())
            {
                printf("--------------\nactual:");
                actual.print();
                printf("--------------\nactual emitted:");
                printf("%.*s\n", (int)actual_emitted.size(), actual_emitted.data());
                printf("--------------\nemitted:");
                printf("%.*s\n", (int)emitted_yaml.size(), emitted_yaml.data());
                return;
            }
        }
        if(!emitted_json.empty())
        {
            SCOPED_TRACE("json");
            std::string expect_emitted = exp ? expected.emitrs_json<std::string>() : "";
            std::string actual_emitted = actual.emitrs_json<std::string>();
            EXPECT_EQ(emitted_json, actual_emitted);
            if(exp)
            {
                EXPECT_EQ(expect_emitted, actual_emitted);
            }
            if(testing::Test::HasFailure())
            {
                printf("--------------\nactual:");
                actual.print();
                printf("--------------\nactual emitted:");
                printf("%.*s\n", (int)actual_emitted.size(), actual_emitted.data());
                printf("--------------\nemitted:");
                printf("%.*s\n", (int)emitted_yaml.size(), emitted_yaml.data());
                return;
            }
        }
    }
    if(expect_resolve_error)
    {
        RYML_EXPECT_ERROR(check_error_basic([&]{
            actual.resolve();
        }));
    }
    else
    {
        SCOPED_TRACE("resolved");
        bool exp = !evts_resolved.empty();
        if(exp)
        {
            if(pos_seq_resolved.empty())
                SCOPED_CALL(verify_visit_seq, (evts_resolved));
            else
                SCOPED_CALL(verify_visit_seq, (evts_resolved, pos_seq_resolved));
        }
        TestBuffers expected;
        if(exp)
            expected = mkevts(actual, evts_resolved);
        RYML_EXPECT_ERROR(check_success([&]{
            actual.resolve();
        }));
printf("CRLCRLCRLCRLCRL!!!!!------------------\n");
actual.print();
        if(exp)
            SCOPED_CALL(expected.test_invariants, ());
        SCOPED_CALL(actual.test_invariants, ());
        if(exp)
            actual.test_compare(expected);
        if(testing::Test::HasFailure())
        {
            if(exp)
            {
                printf("--------------\nexpected:");
                expected.print();
            }
            printf("--------------\nactual:");
            actual.print();
            return;
        }
        {
            SCOPED_TRACE("yaml");
            std::string expect_resolved_emitted = exp ? expected.emitrs_yaml<std::string>() : "";
            std::string actual_resolved_emitted = actual.emitrs_yaml<std::string>();
            EXPECT_EQ(resolved_yaml, actual_resolved_emitted);
            if(exp)
            {
                EXPECT_EQ(expect_resolved_emitted, actual_resolved_emitted);
            }
        }
        if(!resolved_json.empty())
        {
            SCOPED_TRACE("json");
            std::string expect_resolved_emitted = exp ? expected.emitrs_json<std::string>() : "";
            std::string actual_resolved_emitted = actual.emitrs_json<std::string>();
            EXPECT_EQ(resolved_json, actual_resolved_emitted);
            if(exp)
            {
                EXPECT_EQ(expect_resolved_emitted, actual_resolved_emitted);
            }
        }
        if(testing::Test::HasFailure())
        {
            actual.print();
        }
    }
}

C4_NO_INLINE
void test_ints_resolve_(std::string & src,
                        std::string const& emitted_yaml,
                        std::string const& emitted_json,
                        cspan<ievt::evt_bits> evts_expected,
                        cspan<ievt::evt_size> pos_seq={})
{
    test_ints_resolve(src,
                      emitted_yaml,
                      emitted_json,
                      {},
                      {},
                      evts_expected,
                      {},
                      pos_seq,
                      {},
                      /*expect_resolve_error*/true);
}

#if 0
C4_NO_INLINE
void test_ints_resolve_(std::string & src,
                        std::string const& emitted_yaml,
                        std::string const& resolved_yaml)
{
    test_ints_resolve(src,
                      emitted_yaml,
                      "",
                      resolved_yaml,
                      "",
                      {},
                      {},
                      {},
                      {},
                      /*expect_resolve_error*/false);
}
#endif

} // namespace


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(ints, cyclic_references_00)
{
    txtbuf src =
        "&a b\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, noncyc);
}
TEST(ints, cyclic_references_00_1)
{
    txtbuf src =
        "&a *a\n"
        "";
    ievt::TestBuffers ints;
    RYML_EXPECT_ERROR(check_error_parse([&]{
        parse_ints(src, &ints);
    }));
}
TEST(ints, cyclic_references_00_2)
{
    txtbuf src =
        "*a\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, /*expect_resolve_error*/true);
}

TEST(ints, cyclic_references_0)
{
    txtbuf src =
        "- &x [a,b]\n"
        "- &y [*x,*y]\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  3, "x", ievt::VAL_, 18, "x");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  3, "x", ievt::VAL_, 21, "y");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 14, "y", ievt::VAL_, 18, "x");
    test_cyclic_reference_(ints, cyclic, ievt::VAL_, 14, "y", ievt::VAL_, 21, "y");
    test_cyclic_reference_(ints, cyclic);
}
TEST(ints, cyclic_references_0_1)
{
    txtbuf src =
        "- &x [a,b]\n"
        "- &y [*x,*x]\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  3, "x", ievt::VAL_, 18, "x");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  3, "x", ievt::VAL_, 21, "x");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 14, "y", ievt::VAL_, 18, "x");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 14, "y", ievt::VAL_, 21, "x");
    test_cyclic_reference_(ints, noncyc);
}

TEST(ints, cyclic_references_1_0)
{
    txtbuf src =
        "{&z x: *z}\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, noncyc, ievt::KEY_,  3, "z", ievt::VAL_, 9, "z");
    test_cyclic_reference_(ints, noncyc);
}
TEST(ints, cyclic_references_1_1)
{
    txtbuf src =
        "&z {&x x: a, *x : *z}\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  2, "z", ievt::KEY_, 15, "x");
    test_cyclic_reference_(ints, cyclic, ievt::VAL_,  2, "z", ievt::VAL_, 18, "z");
    test_cyclic_reference_(ints, noncyc, ievt::KEY_,  6, "x", ievt::KEY_, 15, "x");
    test_cyclic_reference_(ints, noncyc, ievt::KEY_,  6, "x", ievt::VAL_, 18, "z");
    test_cyclic_reference_(ints, cyclic);
}
TEST(ints, cyclic_references_1_2)
{
    txtbuf src =
        "&z {&x x: a, *z : *x}\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, cyclic, ievt::VAL_,  2, "z", ievt::KEY_, 15, "z");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  2, "z", ievt::VAL_, 18, "x");
    test_cyclic_reference_(ints, noncyc, ievt::KEY_,  6, "x", ievt::KEY_, 15, "z");
    test_cyclic_reference_(ints, noncyc, ievt::KEY_,  6, "x", ievt::VAL_, 18, "x");
    test_cyclic_reference_(ints, cyclic);
}

TEST(ints, cyclic_references_2_0)
{
    txtbuf src =
        "- &a0\n"
        "- &a1\n"
        "  - *a0\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  3, "a0", ievt::VAL_, 13, "a0");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  9, "a1", ievt::VAL_, 13, "a0");
    test_cyclic_reference_(ints, noncyc);
}
TEST(ints, cyclic_references_2_1)
{
    txtbuf src =
        "- &a0\n"
        "- &a1\n"
        "  - *a1\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  3, "a0", ievt::VAL_, 13, "a1");
    test_cyclic_reference_(ints, cyclic, ievt::VAL_,  9, "a1", ievt::VAL_, 13, "a1");
    test_cyclic_reference_(ints, cyclic);
}


TEST(ints, cyclic_references_3_0)
{
    txtbuf src =
        "- &a0\n"
        "  - - - - &a1\n"
        "        - *a1\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, noncyc, ievt::VAL_,  3, "a0", ievt::VAL_, 16, "a1");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 10, "a1", ievt::VAL_, 16, "a1");
    test_cyclic_reference_(ints, noncyc);
}
TEST(ints, cyclic_references_3_1)
{
    txtbuf src =
        "- &a0\n"
        "  - - - - &a1\n"
        "        - *a0\n"
        "";
    ievt::TestBuffers ints;
    parse_ints(src, &ints);
    test_cyclic_reference_(ints, cyclic, ievt::VAL_,  3, "a0", ievt::VAL_, 16, "a0");
    test_cyclic_reference_(ints, noncyc, ievt::VAL_, 10, "a1", ievt::VAL_, 16, "a0");
    test_cyclic_reference_(ints, cyclic);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

TEST(ints_resolve, empty)
{
    ievt::TestBuffers ints;
    ASSERT_EQ(ints.evts.ptr, nullptr);
    ASSERT_EQ(ints.evts.len, 0);
    ints.resolve();
    ASSERT_EQ(ints.evts.ptr, nullptr);
    ASSERT_EQ(ints.evts.len, 0);
}


//-----------------------------------------------------------------------------

TEST(ints_resolve, 0_err_missing_anchor)
{
    using namespace ievt;
    std::string src = ""
        "*anchor\n"
        "";
    std::string json = ""
        "\"anchor\""
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|ALIA, 1,6, /*anchor*/
        /*[3][5]*/   EDOC|PEX2,
        /*[4][6]*/ ESTR,
    };
    test_ints_resolve(src, src, json, evts_expected);
}


TEST(ints_resolve, 0_billion_laughs)
{
    using namespace ievt;
    std::string src = ""
        "l0: &l0 [x,x]\n"
        "l1: &l1 [*l0,*l0]\n"
        "l2: &l2 [*l1,*l1]\n"
        "";
    std::string resolved = ""
        "l0: [x,x]\n"
        "l1: [[x,x],[x,x]]\n"
        "l2: [[[x,x],[x,x]],[[x,x],[x,x]]]\n"
        "";
    std::string json = ""
        "{\n"
        "  \"l0\": [\"x\",\"x\"],\n"
        "  \"l1\": [\"*l0\",\"*l0\"],\n"
        "  \"l2\": [\"*l1\",\"*l1\"]\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"l0\": [\"x\",\"x\"],\n"
        "  \"l1\": [[\"x\",\"x\"],[\"x\",\"x\"]],\n"
        "  \"l2\": [[[\"x\",\"x\"],[\"x\",\"x\"]],[[\"x\",\"x\"],[\"x\",\"x\"]]]\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,2, /*l0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 5,2, /*l0*/
        /*[5][9]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[6][10]*/         VAL_|SCLR|PLAI, 9,1, /*x*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 11,1, /*x*/
        /*[8][16]*/       ESEQ|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 14,2, /*l1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 19,2, /*l1*/
        /*[11][23]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[12][24]*/         VAL_|ALIA, 24,2, /*l0*/
        /*[13][27]*/         VAL_|ALIA|PEX2, 28,2, /*l0*/
        /*[14][30]*/       ESEQ|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 32,2, /*l2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 37,2, /*l2*/
        /*[17][37]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[18][38]*/         VAL_|ALIA, 42,2, /*l1*/
        /*[19][41]*/         VAL_|ALIA|PEX2, 46,2, /*l1*/
        /*[20][44]*/       ESEQ|PEX2,
        /*[21][45]*/     EMAP,
        /*[22][46]*/   EDOC,
        /*[23][47]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,2, /*l0*/
        /*[4][6]*/       PEX2, 0,0, /*l0*/
        /*[5][9]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[6][10]*/         VAL_|SCLR|PLAI, 9,1, /*x*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 11,1, /*x*/
        /*[8][16]*/       ESEQ|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 14,2, /*l1*/
        /*[10][20]*/       PEX2, 0,0, /*l1*/
        /*[11][23]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[12][24]*/         RREF, 9,17, /*l0*/
        /*[13][27]*/         RREF|PEX2, 9,17, /*l0*/
        /*[14][30]*/       ESEQ|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 32,2, /*l2*/
        /*[16][34]*/       PEX2, 0,0, /*l2*/
        /*[17][37]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[18][38]*/         RREF, 23,31, /*l1*/
        /*[19][41]*/         RREF|PEX2, 23,31, /*l1*/
        /*[20][44]*/       ESEQ|PEX2,
        /*[21][45]*/     EMAP,
        /*[22][46]*/   EDOC,
        /*[23][47]*/ ESTR,
    };
    const evt_size pos[] = {
        0,1,2,3,6,9,10,13,16,17,20,23,24,27,30,31,34,37,38,41,44,45,46,47,
    };
    const evt_size pos_resolved[] = {
        0,1,2,3,6,7,8,9,10,13,16,17,20,21,22,23,/*rref@24*/
                      9,10,13,16,/*rref@27*/
                      9,10,13,16,
                      30,31,34,35,36,37,/*rref@38*/
                      23,/*rref@24*/
                      9,10,13,16,/*rref@27*/
                      9,10,13,16,
                      30,
                      23,/*rref@24*/
                      9,10,13,16,/*rref@27*/
                      9,10,13,16,
                      30,
                      44,45,46,47,};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 1_to_scalar_seq_block)
{
    using namespace ievt;
    std::string src = "- &anc foo\n- *anc\n";
    std::string resolved = "- foo\n- foo\n";
    std::string json = "[\n  \"foo\",\n  \"*anc\"\n]\n";
    std::string resolved_json = "[\n  \"foo\",\n  \"foo\"\n]\n";
    const evt_bits evts_expected[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ,
        VAL_|ANCH,            3, 3,
        VAL_|SCLR|PLAI|PEX2,  7, 3,
        VAL_|ALIA|PEX2,      14, 3,
        ESEQ|PEX2,
        EDOC,
        ESTR,
    };
    const evt_bits evts_resolved[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ,
        0, 0, 0,
        VAL_|SCLR|PLAI,  7, 3,
        RREF|PEX2,  6, 9, // this was resolved
        ESEQ|PEX2,
        EDOC,
        ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,9,12,13,14};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,6,12,13,14};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 1_to_scalar_seq_flow_ml)
{
    using namespace ievt;
    std::string src = "[\n  &anc foo,\n  *anc\n]\n";
    std::string resolved = "[\n  foo,\n  foo\n]\n";
    std::string json = "[\n  \"foo\",\n  \"*anc\"\n]\n";
    std::string resolved_json = "[\n  \"foo\",\n  \"foo\"\n]\n";
    const evt_bits evts_expected[] = {
        /*0*/BSTR,
        /*1*/BDOC,
        /*2*/VAL_|BSEQ|FLOW|FML1,
        /*3*/VAL_|ANCH,            5, 3,
        /*6*/VAL_|SCLR|PLAI|PEX2,  9, 3,
        /*9*/VAL_|ALIA|PEX2,      17, 3,
        /*12*/ESEQ|PEX2,
        /*13*/EDOC,
        /*14*/ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*0*/BSTR,
        /*1*/BDOC,
        /*2*/VAL_|BSEQ|FLOW|FML1,
        /*3*/0, 0, 0,
        /*6*/VAL_|SCLR|PLAI,  9, 3,
        /*9*/RREF|PEX2,  6, 9, // this was resolved
        /*12*/ESEQ|PEX2,
        /*13*/EDOC,
        /*14*/ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,9,12,13,14};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,6,12,13,14};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 1_to_scalar_seq_flow_sl)
{
    using namespace ievt;
    std::string src = "[&anc foo,*anc]";
    std::string resolved = "[foo,foo]";
    std::string json = "[\"foo\",\"*anc\"]";
    std::string resolved_json = "[\"foo\",\"foo\"]";
    const evt_bits evts_expected[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ|FLOW|FSL_,
        VAL_|ANCH,            2, 3,
        VAL_|SCLR|PLAI|PEX2,  6, 3,
        VAL_|ALIA|PEX2,      11, 3,
        ESEQ|PEX2,
        EDOC,
        ESTR,
    };
    const evt_bits evts_resolved[] = {
        BSTR,
        BDOC,
        VAL_|BSEQ|FLOW|FSL_,
        0, 0, 0,
        VAL_|SCLR|PLAI,  6, 3,
        RREF|PEX2,  6, 9, // this was resolved
        ESEQ|PEX2,
        EDOC,
        ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,9,12,13,14};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,6,12,13,14};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 2_to_container_seq_blck)
{
    using namespace ievt;
    std::string src = "- &foo [0,1,2,3]\n- *foo\n";
    std::string resolved = "- [0,1,2,3]\n- [0,1,2,3]\n";
    std::string json = "[\n  [0,1,2,3],\n  \"*foo\"\n]\n";
    std::string resolved_json = "[\n  [0,1,2,3],\n  [0,1,2,3]\n]\n";
    const evt_bits evts_expected[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ,
        /* 3*/VAL_|ANCH,            3, 3,
        /* 6*/VAL_|BSEQ|FLOW|FSL_|PEX2,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/VAL_|ALIA,           20, 3,
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,7,10,13,16,19,20,23,24,25};
    const evt_bits evts_resolved[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ,
        /* 3*/0, 0, 0,
        /* 6*/VAL_|BSEQ|FLOW|FSL_,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/RREF,            6,20, // this was resolved
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,7,10,13,16,19,6,7,10,13,16,19,23,24,25};
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 2_to_container_seq_flow_ml)
{
    using namespace ievt;
    std::string src = "[\n&foo [0,1,2,3],\n *foo\n]\n";
    std::string emitted = "[\n  &foo [0,1,2,3],\n  *foo\n]\n";
    std::string resolved = "[\n  [0,1,2,3],\n  [0,1,2,3]\n]\n";
    std::string json = "[\n  [0,1,2,3],\n  \"*foo\"\n]\n";
    const evt_bits evts_expected[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ|FLOW|FML1,
        /* 3*/VAL_|ANCH,            3, 3,
        /* 6*/VAL_|BSEQ|FLOW|FSL_|PEX2,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/VAL_|ALIA,           20, 3,
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_bits evts_resolved[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ|FLOW|FML1,
        /* 3*/0, 0, 0,
        /* 6*/VAL_|BSEQ|FLOW|FSL_,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/RREF,            6,20, // this was resolved
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,7,10,13,16,19,20,23,24,25};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,7,10,13,16,19,6,7,10,13,16,19,23,24,25};
    test_ints_resolve(src, emitted, json, resolved, resolved, evts_expected, evts_resolved, pos, pos_resolved);
}

TEST(ints_resolve, 2_to_container_seq_flow_sl)
{
    using namespace ievt;
    std::string src = "[ &foo [0,1,2,3],  *foo ]";
    std::string emitted = "[&foo [0,1,2,3],*foo]";
    std::string resolved = "[[0,1,2,3],[0,1,2,3]]";
    std::string json = "[[0,1,2,3],\"*foo\"]";
    const evt_bits evts_expected[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ|FLOW|FSL_,
        /* 3*/VAL_|ANCH,            3, 3,
        /* 6*/VAL_|BSEQ|FLOW|FSL_|PEX2,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/VAL_|ALIA,           20, 3,
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_bits evts_resolved[] = {
        /* 0*/BSTR,
        /* 1*/BDOC,
        /* 2*/VAL_|BSEQ|FLOW|FSL_,
        /* 3*/0, 0, 0,
        /* 6*/VAL_|BSEQ|FLOW|FSL_,
        /* 7*/VAL_|SCLR|PLAI,       8, 1,
        /*10*/VAL_|SCLR|PLAI|PEX2, 10, 1,
        /*13*/VAL_|SCLR|PLAI|PEX2, 12, 1,
        /*16*/VAL_|SCLR|PLAI|PEX2, 14, 1,
        /*19*/ESEQ|PEX2,
        /*20*/RREF,            6,20, // this was resolved
        /*23*/ESEQ|PEX2,
        /*24*/EDOC,
        /*25*/ESTR,
    };
    const evt_size pos[] = {0,1,2,3,6,7,10,13,16,19,20,23,24,25};
    const evt_size pos_resolved[] = {0,1,2,3,4,5,6,7,10,13,16,19,6,7,10,13,16,19,23,24,25};
    test_ints_resolve(src, emitted, json, resolved, resolved, evts_expected, evts_resolved, pos, pos_resolved);
}


TEST(ints_resolve, 3_key_vs_seq)
{
    using namespace ievt;
    std::string src = ""
        "&keyscalar key: &valscalar val\n"
        "&keyseq [a, b]: &valseq [c, d]\n"
        "&keymap {e: f}: &valmap {g: h}\n"
        "*valscalar : *keyscalar\n"
        "*valseq : *keyseq\n"
        "*valmap : *keymap\n"
        "";
    std::string emitted = ""
        "&keyscalar key: &valscalar val\n"
        "? &keyseq [a,b]\n"
        ": &valseq [c,d]\n"
        "? &keymap {e: f}\n"
        ": &valmap {g: h}\n"
        "*valscalar : *keyscalar\n"
        "*valseq : *keyseq\n"
        "*valmap : *keymap\n"
        "";
    std::string resolved = ""
        "key: val\n"
        "? [a,b]\n"
        ": [c,d]\n"
        "? {e: f}\n"
        ": {g: h}\n"
        "val: key\n"
        "? [c,d]\n"
        ": [a,b]\n"
        "? {g: h}\n"
        ": {e: f}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|ANCH, 1,9,  /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI|PEX2, 11,3,  /*key*/
        /*[5][9]*/       VAL_|ANCH|PEX2, 17,9,  /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI|PEX2, 27,3,  /*val*/
        /*[7][15]*/       KEY_|ANCH|PEX2, 32,6,  /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_|PEX2,
        /*[9][19]*/         VAL_|SCLR|PLAI, 40,1,  /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 43,1,  /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       VAL_|ANCH, 48,6,  /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[14][30]*/         VAL_|SCLR|PLAI, 56,1,  /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 59,1,  /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       KEY_|ANCH, 63,6,  /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_|PEX2,
        /*[19][41]*/         KEY_|SCLR|PLAI, 71,1,  /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 74,1,  /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       VAL_|ANCH, 79,6,  /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[24][52]*/         KEY_|SCLR|PLAI, 87,1,  /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 90,1,  /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       KEY_|ALIA, 94,9,  /*valscalar*/
        /*[28][62]*/       VAL_|ALIA|PEX2, 107,9,  /*keyscalar*/
        /*[29][65]*/       KEY_|ALIA|PEX2, 118,6,  /*valseq*/
        /*[30][68]*/       VAL_|ALIA|PEX2, 128,6,  /*keyseq*/
        /*[31][71]*/       KEY_|ALIA|PEX2, 136,6,  /*valmap*/
        /*[32][74]*/       VAL_|ALIA|PEX2, 146,6,  /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       0,0,0,  /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI, 11,3,  /*key*/
        /*[5][9]*/       PEX2, 0,0,  /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI, 27,3,  /*val*/
        /*[7][15]*/       PEX2, 0,0,  /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_,
        /*[9][19]*/         VAL_|SCLR|PLAI, 40,1,  /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 43,1,  /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       0, 0,0,  /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[14][30]*/         VAL_|SCLR|PLAI, 56,1,  /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 59,1,  /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       0, 0,0,  /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_,
        /*[19][41]*/         KEY_|SCLR|PLAI, 71,1,  /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 74,1,  /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       0, 0,0,  /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_,
        /*[24][52]*/         KEY_|SCLR|PLAI, 87,1,  /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 90,1,  /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       RREF, 12,15,  /*valscalar*/
        /*[28][62]*/       RREF|PEX2, 6,9,  /*keyscalar*/
        /*[29][65]*/       RREF|PEX2, 29,37,  /*valseq*/
        /*[30][68]*/       RREF|PEX2, 18,26,  /*keyseq*/
        /*[31][71]*/       RREF|PEX2, 51,59,  /*valmap*/
        /*[32][74]*/       RREF|PEX2, 40,48,  /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    test_ints_resolve(src, emitted, {}, resolved, {}, evts_expected, evts_resolved);
}

TEST(ints_resolve, 3_key_vs_seq_flow_ml)
{
    using namespace ievt;
    std::string src = ""
        "{\n"
        "  &keyscalar key: &valscalar val,\n"
        "  &keyseq [a, b]: &valseq [c, d],\n"
        "  &keymap {e: f}: &valmap {g: h},\n"
        "  *valscalar : *keyscalar,\n"
        "  *valseq : *keyseq,\n"
        "  *valmap : *keymap,\n"
        "}\n"
        "";
    std::string emitted = ""
        "{\n"
        "  &keyscalar key: &valscalar val,\n"
        "  ? &keyseq [a,b]: &valseq [c,d],\n"
        "  ? &keymap {e: f}: &valmap {g: h},\n"
        "  *valscalar : *keyscalar,\n"
        "  *valseq : *keyseq,\n"
        "  *valmap : *keymap\n"
        "}\n"
        "";
    std::string resolved = ""
        "{\n"
        "  key: val,\n"
        "  ? [a,b]: [c,d],\n"
        "  ? {e: f}: {g: h},\n"
        "  val: key,\n"
        "  ? [c,d]: [a,b],\n"
        "  ? {g: h}: {e: f}\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|ANCH, 5,9, /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI|PEX2, 15,3, /*key*/
        /*[5][9]*/       VAL_|ANCH|PEX2, 21,9, /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI|PEX2, 31,3, /*val*/
        /*[7][15]*/       KEY_|ANCH|PEX2, 39,6, /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_|PEX2,
        /*[9][19]*/         VAL_|SCLR|PLAI, 47,1, /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 50,1, /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       VAL_|ANCH, 55,6, /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[14][30]*/         VAL_|SCLR|PLAI, 63,1, /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 66,1, /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       KEY_|ANCH, 73,6, /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_|PEX2,
        /*[19][41]*/         KEY_|SCLR|PLAI, 81,1, /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 84,1, /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       VAL_|ANCH, 89,6, /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[24][52]*/         KEY_|SCLR|PLAI, 97,1, /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 100,1, /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       KEY_|ALIA, 107,9, /*valscalar*/
        /*[28][62]*/       VAL_|ALIA|PEX2, 120,9, /*keyscalar*/
        /*[29][65]*/       KEY_|ALIA|PEX2, 134,6, /*valseq*/
        /*[30][68]*/       VAL_|ALIA|PEX2, 144,6, /*keyseq*/
        /*[31][71]*/       KEY_|ALIA|PEX2, 155,6, /*valmap*/
        /*[32][74]*/       VAL_|ALIA|PEX2, 165,6, /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       0, 0,0, /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI, 15,3, /*key*/
        /*[5][9]*/       PEX2, 0,0, /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI, 31,3, /*val*/
        /*[7][15]*/       PEX2, 0,0, /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_,
        /*[9][19]*/         VAL_|SCLR|PLAI, 47,1, /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 50,1, /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       0, 0,0, /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[14][30]*/         VAL_|SCLR|PLAI, 63,1, /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 66,1, /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       0, 0,0, /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_,
        /*[19][41]*/         KEY_|SCLR|PLAI, 81,1, /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 84,1, /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       0, 0,0, /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_,
        /*[24][52]*/         KEY_|SCLR|PLAI, 97,1, /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 100,1, /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       RREF, 12,15,  /*valscalar*/
        /*[28][62]*/       RREF|PEX2, 6,9,  /*keyscalar*/
        /*[29][65]*/       RREF|PEX2, 29,37,  /*valseq*/
        /*[30][68]*/       RREF|PEX2, 18,26,  /*keyseq*/
        /*[31][71]*/       RREF|PEX2, 51,59,  /*valmap*/
        /*[32][74]*/       RREF|PEX2, 40,48,  /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    test_ints_resolve(src, emitted, {}, resolved, {}, evts_expected, evts_resolved);
}

TEST(ints_resolve, 3_key_vs_seq_flow_sl)
{
    using namespace ievt;
    std::string src = ""
        "{ "
        "  &keyscalar key: &valscalar val, "
        "  &keyseq [a, b]: &valseq [c, d], "
        "  &keymap {e: f}: &valmap {g: h}, "
        "  *valscalar : *keyscalar, "
        "  *valseq : *keyseq, "
        "  *valmap : *keymap, "
        "}\n"
        "";
    std::string emitted = ""
        "{"
        "&keyscalar key: &valscalar val,"
        "? &keyseq [a,b]: &valseq [c,d],"
        "? &keymap {e: f}: &valmap {g: h},"
        "*valscalar : *keyscalar,"
        "*valseq : *keyseq,"
        "*valmap : *keymap"
        "}"
        "";
    std::string resolved = ""
        "{"
        "key: val,"
        "? [a,b]: [c,d],"
        "? {e: f}: {g: h},"
        "val: key,"
        "? [c,d]: [a,b],"
        "? {g: h}: {e: f}"
        "}"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|ANCH, 5,9, /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI|PEX2, 15,3, /*key*/
        /*[5][9]*/       VAL_|ANCH|PEX2, 21,9, /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI|PEX2, 31,3, /*val*/
        /*[7][15]*/       KEY_|ANCH|PEX2, 39,6, /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_|PEX2,
        /*[9][19]*/         VAL_|SCLR|PLAI, 47,1, /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 50,1, /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       VAL_|ANCH, 55,6, /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[14][30]*/         VAL_|SCLR|PLAI, 63,1, /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 66,1, /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       KEY_|ANCH, 73,6, /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_|PEX2,
        /*[19][41]*/         KEY_|SCLR|PLAI, 81,1, /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 84,1, /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       VAL_|ANCH, 89,6, /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[24][52]*/         KEY_|SCLR|PLAI, 97,1, /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 100,1, /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       KEY_|ALIA, 107,9, /*valscalar*/
        /*[28][62]*/       VAL_|ALIA|PEX2, 120,9, /*keyscalar*/
        /*[29][65]*/       KEY_|ALIA|PEX2, 134,6, /*valseq*/
        /*[30][68]*/       VAL_|ALIA|PEX2, 144,6, /*keyseq*/
        /*[31][71]*/       KEY_|ALIA|PEX2, 155,6, /*valmap*/
        /*[32][74]*/       VAL_|ALIA|PEX2, 165,6, /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       0, 0,0, /*keyscalar*/
        /*[4][6]*/       KEY_|SCLR|PLAI, 15,3, /*key*/
        /*[5][9]*/       PEX2, 0,0, /*valscalar*/
        /*[6][12]*/       VAL_|SCLR|PLAI, 31,3, /*val*/
        /*[7][15]*/       PEX2, 0,0, /*keyseq*/
        /*[8][18]*/       KEY_|BSEQ|FLOW|FSL_,
        /*[9][19]*/         VAL_|SCLR|PLAI, 47,1, /*a*/
        /*[10][22]*/         VAL_|SCLR|PLAI|PEX2, 50,1, /*b*/
        /*[11][25]*/       ESEQ|PEX2,
        /*[12][26]*/       0, 0,0, /*valseq*/
        /*[13][29]*/       VAL_|BSEQ|FLOW|FSL_,
        /*[14][30]*/         VAL_|SCLR|PLAI, 63,1, /*c*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 66,1, /*d*/
        /*[16][36]*/       ESEQ|PEX2,
        /*[17][37]*/       0, 0,0, /*keymap*/
        /*[18][40]*/       KEY_|BMAP|FLOW|FSL_,
        /*[19][41]*/         KEY_|SCLR|PLAI, 81,1, /*e*/
        /*[20][44]*/         VAL_|SCLR|PLAI|PEX2, 84,1, /*f*/
        /*[21][47]*/       EMAP|PEX2,
        /*[22][48]*/       0, 0,0, /*valmap*/
        /*[23][51]*/       VAL_|BMAP|FLOW|FSL_,
        /*[24][52]*/         KEY_|SCLR|PLAI, 97,1, /*g*/
        /*[25][55]*/         VAL_|SCLR|PLAI|PEX2, 100,1, /*h*/
        /*[26][58]*/       EMAP|PEX2,
        /*[27][59]*/       RREF, 12,15,  /*valscalar*/
        /*[28][62]*/       RREF|PEX2, 6,9,  /*keyscalar*/
        /*[29][65]*/       RREF|PEX2, 29,37,  /*valseq*/
        /*[30][68]*/       RREF|PEX2, 18,26,  /*keyseq*/
        /*[31][71]*/       RREF|PEX2, 51,59,  /*valmap*/
        /*[32][74]*/       RREF|PEX2, 40,48,  /*keymap*/
        /*[33][77]*/     EMAP|PEX2,
        /*[34][78]*/   EDOC,
        /*[35][79]*/ ESTR,
    };
    test_ints_resolve(src, emitted, {}, resolved, {}, evts_expected, evts_resolved);
}

TEST(ints_resolve, 4_inherit_block)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "inherit:\n"
        "  <<: *map0\n"
        "";
    std::string resolved = ""
        "map0:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "inherit:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": \"*map0\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 27,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 33,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 38,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|PEX2,
        /*[13][27]*/         KEY_|SCLR|PLAI, 49,2, /*<<*/
        /*[14][30]*/         VAL_|ALIA|PEX2, 54,4, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       PEX2, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 27,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 33,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 38,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|PEX2,
        /*[13][27]*/         0,0,0, /*<<*/
        /*[14][30]*/         RREF, 10,22, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 4_inherit_flow_ml)
{
    using namespace ievt;
    std::string src = ""
        "{\n"
        "  map0: &map0 {\n"
        "    key0: val0,\n"
        "    key1: val1\n"
        "  },\n"
        "  inherit: {\n"
        "    <<: *map0\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved = ""
        "{\n"
        "  map0: {\n"
        "    key0: val0,\n"
        "    key1: val1\n"
        "  },\n"
        "  inherit: {\n"
        "    key0: val0,\n"
        "    key1: val1\n"
        "  }\n"
        "}\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": \"*map0\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 11,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 38,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 44,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 56,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[13][27]*/         KEY_|SCLR|PLAI, 71,2, /*<<*/
        /*[14][30]*/         VAL_|ALIA|PEX2, 76,4, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       PEX2, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FML1,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 38,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 44,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 56,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[13][27]*/         0,0,0, /*<<*/
        /*[14][30]*/         RREF, 10,22, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 4_inherit_flow_sl)
{
    using namespace ievt;
    std::string src = ""
        "{ "
        "  map0: &map0 { "
        "    key0: val0, "
        "    key1: val1 "
        "  }, "
        "  inherit: { "
        "    <<: *map0 "
        "  } "
        "} "
        "";
    std::string emitted = ""
        "{map0: &map0 {key0: val0,key1: val1},inherit: {<<: *map0}}";
    std::string resolved = ""
        "{map0: {key0: val0,key1: val1},inherit: {key0: val0,key1: val1}}";
    std::string json = ""
        "{"
        "\"map0\": {"
        "\"key0\": \"val0\","
        "\"key1\": \"val1\""
        "},"
        "\"inherit\": {"
        "\"<<\": \"*map0\""
        "}"
        "}"
        "";
    std::string resolved_json = ""
        "{"
        "\"map0\": {"
        "\"key0\": \"val0\","
        "\"key1\": \"val1\""
        "},"
        "\"inherit\": {"
        "\"key0\": \"val0\","
        "\"key1\": \"val1\""
        "}"
        "}"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 11,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 38,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 44,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 56,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[13][27]*/         KEY_|SCLR|PLAI, 71,2, /*<<*/
        /*[14][30]*/         VAL_|ALIA|PEX2, 76,4, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       PEX2, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FSL_,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 38,4, /*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 44,4, /*val1*/
        /*[10][22]*/       EMAP|PEX2,
        /*[11][23]*/       KEY_|SCLR|PLAI, 56,7, /*inherit*/
        /*[12][26]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[13][27]*/         0,0,0, /*<<*/
        /*[14][30]*/         RREF, 10,22, /*map0*/
        /*[15][33]*/       EMAP|PEX2,
        /*[16][34]*/     EMAP,
        /*[17][35]*/   EDOC,
        /*[18][36]*/ ESTR,
    };
    test_ints_resolve(src, emitted, json, resolved, resolved_json, evts_expected, evts_resolved);
}



TEST(ints_resolve, 5_inherit_seq_block)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n"
        "  key0: val0\n"
        "map1: &map1\n"
        "  key1: val1\n"
        "map2: &map2\n"
        "  key2: val2\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n"
        "";
    std::string resolved = ""
        "map0:\n"
        "  key0: val0\n"
        "map1:\n"
        "  key1: val1\n"
        "map2:\n"
        "  key2: val2\n"
        "inherit:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "  key2: val2\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\",\n"
        "    \"key2\": \"val2\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 32,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 57,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|PEX2,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PEX2,
        /*[23][49]*/         KEY_|SCLR|PLAI, 86,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[25][53]*/           VAL_|ALIA, 92,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PEX2, 98,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PEX2, 104,4, /*map2*/
        /*[28][62]*/         ESEQ|PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       PEX2, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       PEX2, 0,0, /*map1*/
        /*[11][23]*/       VAL_|BMAP,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       PEX2, 0,0, /*map2*/
        /*[17][37]*/       VAL_|BMAP,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PEX2,
        /*[23][49]*/         0, 0,0, /*<<*/
        /*[24][52]*/         0,
        /*[25][53]*/         RREF, 10,16, /*map0*/
        /*[26][56]*/         RREF|PEX2, 24,30, /*map1*/
        /*[27][59]*/         RREF|PEX2, 38,44, /*map2*/
        /*[28][62]*/         PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 5_inherit_seq_flowml)
{
    using namespace ievt;
    std::string src = ""
        "{\n"
        "  map0: &map0 {\n"
        "    key0: val0\n"
        "  },\n"
        "  map1: &map1 {\n"
        "    key1: val1\n"
        "  },\n"
        "  map2: &map2 {\n"
        "    key2: val2\n"
        "  },\n"
        "  inherit: {\n"
        "    <<: [*map0,*map1,*map2]\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved = ""
        "{\n"
        "  map0: {\n"
        "    key0: val0\n"
        "  },\n"
        "  map1: {\n"
        "    key1: val1\n"
        "  },\n"
        "  map2: {\n"
        "    key2: val2\n"
        "  },\n"
        "  inherit: {\n"
        "    key0: val0,\n"
        "    key1: val1,\n"
        "    key2: val2\n"
        "  }\n"
        "}\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\",\n"
        "    \"key2\": \"val2\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 11,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 40,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 47,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 58,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 64,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 76,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 83,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[18][38]*/         KEY_|SCLR|PLAI, 94,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 100,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 112,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[23][49]*/         KEY_|SCLR|PLAI, 127,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[25][53]*/           VAL_|ALIA, 133,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PEX2, 139,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PEX2, 145,4, /*map2*/
        /*[28][62]*/         ESEQ|PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FML1,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       PEX2, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FML1,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 40,4, /*map1*/
        /*[10][20]*/       PEX2, 0,0, /*map1*/
        /*[11][23]*/       VAL_|BMAP|FLOW|FML1,
        /*[12][24]*/         KEY_|SCLR|PLAI, 58,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 64,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 76,4, /*map2*/
        /*[16][34]*/       PEX2, 0,0, /*map2*/
        /*[17][37]*/       VAL_|BMAP|FLOW|FML1,
        /*[18][38]*/         KEY_|SCLR|PLAI, 94,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 100,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 112,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|FLOW|FML1|PEX2,
        /*[23][49]*/         0, 0,0, /*<<*/
        /*[24][52]*/         0,
        /*[25][53]*/         RREF, 10,16, /*map0*/
        /*[26][56]*/         RREF|PEX2, 24,30, /*map1*/
        /*[27][59]*/         RREF|PEX2, 38,44, /*map2*/
        /*[28][62]*/         PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 5_inherit_seq_flowsl)
{
    using namespace ievt;
    std::string src = ""
        "{ "
        "  map0: &map0 { "
        "    key0: val0 "
        "  }, "
        "  map1: &map1 { "
        "    key1: val1 "
        "  }, "
        "  map2: &map2 { "
        "    key2: val2 "
        "  }, "
        "  inherit: { "
        "    <<: [*map0,*map1,*map2] "
        "  } "
        "} "
        "";
    std::string emitted = ""
        "{"
        "map0: &map0 {key0: val0},"
        "map1: &map1 {key1: val1},"
        "map2: &map2 {key2: val2},"
        "inherit: {<<: [*map0,*map1,*map2]}"
        "}"
        "";
    std::string resolved = ""
        "{"
        "map0: {key0: val0},"
        "map1: {key1: val1},"
        "map2: {key2: val2},"
        "inherit: {key0: val0,key1: val1,key2: val2}"
        "}"
        "";
    std::string json = ""
        "{"
        "\"map0\": {"
        "\"key0\": \"val0\""
        "},"
        "\"map1\": {"
        "\"key1\": \"val1\""
        "},"
        "\"map2\": {"
        "\"key2\": \"val2\""
        "},"
        "\"inherit\": {"
        "\"<<\": [\"*map0\",\"*map1\",\"*map2\"]"
        "}"
        "}"
        "";
    std::string resolved_json = ""
        "{"
        "\"map0\": {"
        "\"key0\": \"val0\""
        "},"
        "\"map1\": {"
        "\"key1\": \"val1\""
        "},"
        "\"map2\": {"
        "\"key2\": \"val2\""
        "},"
        "\"inherit\": {"
        "\"key0\": \"val0\","
        "\"key1\": \"val1\","
        "\"key2\": \"val2\""
        "}"
        "}"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 11,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 40,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 47,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 58,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 64,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 76,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 83,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[18][38]*/         KEY_|SCLR|PLAI, 94,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 100,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 112,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[23][49]*/         KEY_|SCLR|PLAI, 127,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[25][53]*/           VAL_|ALIA, 133,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PEX2, 139,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PEX2, 145,4, /*map2*/
        /*[28][62]*/         ESEQ|PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP|FLOW|FSL_,
        /*[3][3]*/       KEY_|SCLR|PLAI, 4,4, /*map0*/
        /*[4][6]*/       PEX2, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FSL_,
        /*[6][10]*/         KEY_|SCLR|PLAI, 22,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 28,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 40,4, /*map1*/
        /*[10][20]*/       PEX2, 0,0, /*map1*/
        /*[11][23]*/       VAL_|BMAP|FLOW|FSL_,
        /*[12][24]*/         KEY_|SCLR|PLAI, 58,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 64,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 76,4, /*map2*/
        /*[16][34]*/       PEX2, 0,0, /*map2*/
        /*[17][37]*/       VAL_|BMAP|FLOW|FSL_,
        /*[18][38]*/         KEY_|SCLR|PLAI, 94,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 100,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 112,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[23][49]*/         0, 0,0, /*<<*/
        /*[24][52]*/         0,
        /*[25][53]*/         RREF, 10,16, /*map0*/
        /*[26][56]*/         RREF|PEX2, 24,30, /*map1*/
        /*[27][59]*/         RREF|PEX2, 38,44, /*map2*/
        /*[28][62]*/         PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/     EMAP,
        /*[31][65]*/   EDOC,
        /*[32][66]*/ ESTR,
    };
    test_ints_resolve(src, emitted, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 6_inherit_seq_with_more_members)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n"
        "  key0: val0\n"
        "map1: &map1\n"
        "  key1: val1\n"
        "map2: &map2\n"
        "  key2: val2\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n"
        "another: member\n"
        "";
    std::string resolved = ""
        "map0:\n"
        "  key0: val0\n"
        "map1:\n"
        "  key1: val1\n"
        "map2:\n"
        "  key2: val2\n"
        "inherit:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "  key2: val2\n"
        "another: member\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  },\n"
        "  \"another\": \"member\"\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\",\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"another\": \"member\"\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4, /*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 32,4, /*map1*/
        /*[11][23]*/       VAL_|BMAP|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       VAL_|ANCH|PEX2, 57,4, /*map2*/
        /*[17][37]*/       VAL_|BMAP|PEX2,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PEX2,
        /*[23][49]*/         KEY_|SCLR|PLAI, 86,2, /*<<*/
        /*[24][52]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[25][53]*/           VAL_|ALIA, 92,4, /*map0*/
        /*[26][56]*/           VAL_|ALIA|PEX2, 98,4, /*map1*/
        /*[27][59]*/           VAL_|ALIA|PEX2, 104,4, /*map2*/
        /*[28][62]*/         ESEQ|PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/       KEY_|SCLR|PLAI, 110,7, /*another*/
        /*[31][67]*/       VAL_|SCLR|PLAI|PEX2, 119,6, /*member*/
        /*[32][70]*/     EMAP|PEX2,
        /*[33][71]*/   EDOC,
        /*[34][72]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4, /*map0*/
        /*[4][6]*/       PEX2, 0,0, /*map0*/
        /*[5][9]*/       VAL_|BMAP,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4, /*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4, /*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4, /*map1*/
        /*[10][20]*/       PEX2, 0,0, /*map1*/
        /*[11][23]*/       VAL_|BMAP,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4, /*key1*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,4, /*val1*/
        /*[14][30]*/       EMAP|PEX2,
        /*[15][31]*/       KEY_|SCLR|PLAI, 50,4, /*map2*/
        /*[16][34]*/       PEX2, 0,0, /*map2*/
        /*[17][37]*/       VAL_|BMAP,
        /*[18][38]*/         KEY_|SCLR|PLAI, 64,4, /*key2*/
        /*[19][41]*/         VAL_|SCLR|PLAI|PEX2, 70,4, /*val2*/
        /*[20][44]*/       EMAP|PEX2,
        /*[21][45]*/       KEY_|SCLR|PLAI, 75,7, /*inherit*/
        /*[22][48]*/       VAL_|BMAP|PEX2,
        /*[23][49]*/         0, 0,0, /*<<*/
        /*[24][52]*/         0,
        /*[25][53]*/         RREF, 10,16, /*map0*/
        /*[26][56]*/         RREF|PEX2, 24,30, /*map1*/
        /*[27][59]*/         RREF|PEX2, 38,44, /*map2*/
        /*[28][62]*/         PEX2,
        /*[29][63]*/       EMAP,
        /*[30][64]*/       KEY_|SCLR|PLAI, 110,7, /*another*/
        /*[31][67]*/       VAL_|SCLR|PLAI|PEX2, 119,6, /*member*/
        /*[32][70]*/     EMAP|PEX2,
        /*[33][71]*/   EDOC,
        /*[34][72]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}


//-----------------------------------------------------------------------------

TEST(ints_resolve, 7_inherit_seq_merge_0_empty)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0 {}\n"
        "map1: &map1 {}\n"
        "map2: &map2 {}\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n"
        "";
    std::string resolved = ""
        "map0: {}\n"
        "map1: {}\n"
        "map2: {}\n"
        "inherit:\n"
        "  {}" // FIXME
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {},\n"
        "  \"map1\": {},\n"
        "  \"map2\": {},\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {},\n"
        "  \"map1\": {},\n"
        "  \"map2\": {},\n"
        "  \"inherit\": {\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[6][10]*/       EMAP,
        /*[7][11]*/       KEY_|SCLR|PLAI, 15,4,/*map1*/
        /*[8][14]*/       VAL_|ANCH|PEX2, 22,4,/*map1*/
        /*[9][17]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[10][18]*/       EMAP,
        /*[11][19]*/       KEY_|SCLR|PLAI, 30,4,/*map2*/
        /*[12][22]*/       VAL_|ANCH|PEX2, 37,4,/*map2*/
        /*[13][25]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[14][26]*/       EMAP,
        /*[15][27]*/       KEY_|SCLR|PLAI, 45,7,/*inherit*/
        /*[16][30]*/       VAL_|BMAP|PEX2,
        /*[17][31]*/         KEY_|SCLR|PLAI, 56,2,/*<<*/
        /*[18][34]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[19][35]*/           VAL_|ALIA, 62,4,/*map0*/
        /*[20][38]*/           VAL_|ALIA|PEX2, 68,4,/*map1*/
        /*[21][41]*/           VAL_|ALIA|PEX2, 74,4,/*map2*/
        /*[22][44]*/         ESEQ|PEX2,
        /*[23][45]*/       EMAP,
        /*[24][46]*/     EMAP,
        /*[25][47]*/   EDOC,
        /*[26][48]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,
        /*[5][7]*/       0,
        /*[6][8]*/       0,
        /*[7][9]*/       VAL_|BMAP|FLOW|FSL_,
        /*[8][10]*/       EMAP,
        /*[9][11]*/       KEY_|SCLR|PLAI, 15,4,/*map1*/
        /*[10][14]*/       PEX2,
        /*[11][15]*/       0,
        /*[12][16]*/       0,
        /*[13][17]*/       VAL_|BMAP|FLOW|FSL_,
        /*[14][18]*/       EMAP,
        /*[15][19]*/       KEY_|SCLR|PLAI, 30,4,/*map2*/
        /*[16][22]*/       PEX2,
        /*[17][23]*/       0,
        /*[18][24]*/       0,
        /*[19][25]*/       VAL_|BMAP|FLOW|FSL_,
        /*[20][26]*/       EMAP,
        /*[21][27]*/       KEY_|SCLR|PLAI, 45,7,/*inherit*/
        /*[22][30]*/       VAL_|BMAP|PEX2,
        /*[23][31]*/         0,
        /*[24][32]*/         0,
        /*[25][33]*/         0,
        /*[26][34]*/         0,
        /*[27][35]*/         0,
        /*[28][36]*/         0,
        /*[29][37]*/         0,
        /*[30][38]*/         0,
        /*[31][39]*/         0,
        /*[32][40]*/         0,
        /*[33][41]*/         0,
        /*[34][42]*/         0,
        /*[35][43]*/         0,
        /*[36][44]*/         0,
        /*[37][45]*/       EMAP,
        /*[38][46]*/     EMAP,
        /*[39][47]*/   EDOC,
        /*[40][48]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 7_inherit_seq_merge)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n"
        "  key0: val0\n"
        "map1: &map1\n"
        "  key0: val01\n"
        "  key1: val1\n"
        "map2: &map2\n"
        "  key0: val02\n"
        "  key1: val12\n"
        "  key2: val2\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n"
        "another: member\n"
        "";
    std::string resolved = ""
        "map0:\n"
        "  key0: val0\n"
        "map1:\n"
        "  key0: val01\n"
        "  key1: val1\n"
        "map2:\n"
        "  key0: val02\n"
        "  key1: val12\n"
        "  key2: val2\n"
        "inherit:\n"
        "  key0: val0\n"
        "  key1: val1\n"
        "  key2: val2\n"
        "another: member\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key0\": \"val01\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key0\": \"val02\",\n"
        "    \"key1\": \"val12\",\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  },\n"
        "  \"another\": \"member\"\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val0\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key0\": \"val01\",\n"
        "    \"key1\": \"val1\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key0\": \"val02\",\n"
        "    \"key1\": \"val12\",\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val0\",\n"
        "    \"key1\": \"val1\",\n"
        "    \"key2\": \"val2\"\n"
        "  },\n"
        "  \"another\": \"member\"\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4,/*val0*/
        /*[8][16]*/       EMAP|PEX2,
        /*[9][17]*/       KEY_|SCLR|PLAI, 25,4,/*map1*/
        /*[10][20]*/       VAL_|ANCH|PEX2, 32,4,/*map1*/
        /*[11][23]*/       VAL_|BMAP|PEX2,
        /*[12][24]*/         KEY_|SCLR|PLAI, 39,4,/*key0*/
        /*[13][27]*/         VAL_|SCLR|PLAI|PEX2, 45,5,/*val01*/
        /*[14][30]*/         KEY_|SCLR|PLAI|PEX2, 53,4,/*key1*/
        /*[15][33]*/         VAL_|SCLR|PLAI|PEX2, 59,4,/*val1*/
        /*[16][36]*/       EMAP|PEX2,
        /*[17][37]*/       KEY_|SCLR|PLAI, 64,4,/*map2*/
        /*[18][40]*/       VAL_|ANCH|PEX2, 71,4,/*map2*/
        /*[19][43]*/       VAL_|BMAP|PEX2,
        /*[20][44]*/         KEY_|SCLR|PLAI, 78,4,/*key0*/
        /*[21][47]*/         VAL_|SCLR|PLAI|PEX2, 84,5,/*val02*/
        /*[22][50]*/         KEY_|SCLR|PLAI|PEX2, 92,4,/*key1*/
        /*[23][53]*/         VAL_|SCLR|PLAI|PEX2, 98,5,/*val12*/
        /*[24][56]*/         KEY_|SCLR|PLAI|PEX2, 106,4,/*key2*/
        /*[25][59]*/         VAL_|SCLR|PLAI|PEX2, 112,4,/*val2*/
        /*[26][62]*/       EMAP|PEX2,
        /*[27][63]*/       KEY_|SCLR|PLAI, 117,7,/*inherit*/
        /*[28][66]*/       VAL_|BMAP|PEX2,
        /*[29][67]*/         KEY_|SCLR|PLAI, 128,2,/*<<*/
        /*[30][70]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[31][71]*/           VAL_|ALIA, 134,4,/*map0*/
        /*[32][74]*/           VAL_|ALIA|PEX2, 140,4,/*map1*/
        /*[33][77]*/           VAL_|ALIA|PEX2, 146,4,/*map2*/
        /*[34][80]*/         ESEQ|PEX2,
        /*[35][81]*/       EMAP,
        /*[36][82]*/       KEY_|SCLR|PLAI, 152,7,/*another*/
        /*[37][85]*/       VAL_|SCLR|PLAI|PEX2, 161,6,/*member*/
        /*[38][88]*/     EMAP|PEX2,
        /*[39][89]*/   EDOC,
        /*[40][90]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,
        /*[5][7]*/       0,
        /*[6][8]*/       0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|SCLR|PLAI|PEX2, 20,4,/*val0*/
        /*[10][16]*/       EMAP|PEX2,
        /*[11][17]*/       KEY_|SCLR|PLAI, 25,4,/*map1*/
        /*[12][20]*/       PEX2,
        /*[13][21]*/       0,
        /*[14][22]*/       0,
        /*[15][23]*/       VAL_|BMAP,
        /*[16][24]*/         KEY_|SCLR|PLAI, 39,4,/*key0*/
        /*[17][27]*/         VAL_|SCLR|PLAI|PEX2, 45,5,/*val01*/
        /*[18][30]*/         KEY_|SCLR|PLAI|PEX2, 53,4,/*key1*/
        /*[19][33]*/         VAL_|SCLR|PLAI|PEX2, 59,4,/*val1*/
        /*[20][36]*/       EMAP|PEX2,
        /*[21][37]*/       KEY_|SCLR|PLAI, 64,4,/*map2*/
        /*[22][40]*/       PEX2,
        /*[23][41]*/       0,
        /*[24][42]*/       0,
        /*[25][43]*/       VAL_|BMAP,
        /*[26][44]*/         KEY_|SCLR|PLAI, 78,4,/*key0*/
        /*[27][47]*/         VAL_|SCLR|PLAI|PEX2, 84,5,/*val02*/
        /*[28][50]*/         KEY_|SCLR|PLAI|PEX2, 92,4,/*key1*/
        /*[29][53]*/         VAL_|SCLR|PLAI|PEX2, 98,5,/*val12*/
        /*[30][56]*/         KEY_|SCLR|PLAI|PEX2, 106,4,/*key2*/
        /*[31][59]*/         VAL_|SCLR|PLAI|PEX2, 112,4,/*val2*/
        /*[32][62]*/       EMAP|PEX2,
        /*[33][63]*/       KEY_|SCLR|PLAI, 117,7,/*inherit*/
        /*[34][66]*/       VAL_|BMAP|PEX2,
        /*[35][67]*/         0,
        /*[36][68]*/         0,
        /*[37][69]*/         0,
        /*[38][70]*/         0,
        /*[39][71]*/         RREF, 10,16,
        /*[40][74]*/         RREF|PEX2, 91,97,
        /*[41][77]*/         RREF|PEX2, 97,103,
        /*[42][80]*/         PEX2,
        /*[43][81]*/       EMAP,
        /*[44][82]*/       KEY_|SCLR|PLAI, 152,7,/*another*/
        /*[45][85]*/       VAL_|SCLR|PLAI|PEX2, 161,6,/*member*/
        /*[46][88]*/     EMAP|PEX2,
        /*[47][89]*/   EDOC,
        /*[48][90]*/ ESTR,
        // these entries are added while merging:
        /*[49][91]*/         KEY_|SCLR|PLAI, 53,4,/*key1*/
        /*[50][94]*/         VAL_|SCLR|PLAI|PEX2, 59,4,/*val1*/
        /*[51][97]*/         KEY_|SCLR|PLAI|PEX2, 106,4,/*key2*/
        /*[52][100]*/         VAL_|SCLR|PLAI|PEX2, 112,4,/*val2*/
        /*[53][103]*/         PEX2,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}


//-----------------------------------------------------------------------------

TEST(ints_resolve, 8_inherit_seq_merge)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n"
        "  key0: val00\n"
        "  key1: val10\n"
        "  key2: val20\n"
        "map1: &map1\n"
        "  key1: val11\n"
        "map2: &map2\n"
        "  key2: val22\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n"
        "";
    std::string resolved = ""
        "map0:\n"
        "  key0: val00\n"
        "  key1: val10\n"
        "  key2: val20\n"
        "map1:\n"
        "  key1: val11\n"
        "map2:\n"
        "  key2: val22\n"
        "inherit:\n"
        "  key0: val00\n"
        "  key1: val10\n"
        "  key2: val20\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val00\",\n"
        "    \"key1\": \"val10\",\n"
        "    \"key2\": \"val20\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val11\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val22\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"]\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": \"val00\",\n"
        "    \"key1\": \"val10\",\n"
        "    \"key2\": \"val20\"\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val11\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val22\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val00\",\n"
        "    \"key1\": \"val10\",\n"
        "    \"key2\": \"val20\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[10][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[11][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[12][28]*/       EMAP|PEX2,
        /*[13][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[14][32]*/       VAL_|ANCH|PEX2, 61,4,/*map1*/
        /*[15][35]*/       VAL_|BMAP|PEX2,
        /*[16][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[17][39]*/         VAL_|SCLR|PLAI|PEX2, 74,5,/*val11*/
        /*[18][42]*/       EMAP|PEX2,
        /*[19][43]*/       KEY_|SCLR|PLAI, 80,4,/*map2*/
        /*[20][46]*/       VAL_|ANCH|PEX2, 87,4,/*map2*/
        /*[21][49]*/       VAL_|BMAP|PEX2,
        /*[22][50]*/         KEY_|SCLR|PLAI, 94,4,/*key2*/
        /*[23][53]*/         VAL_|SCLR|PLAI|PEX2, 100,5,/*val22*/
        /*[24][56]*/       EMAP|PEX2,
        /*[25][57]*/       KEY_|SCLR|PLAI, 106,7,/*inherit*/
        /*[26][60]*/       VAL_|BMAP|PEX2,
        /*[27][61]*/         KEY_|SCLR|PLAI, 117,2,/*<<*/
        /*[28][64]*/         VAL_|ALIA|PEX2, 122,4,/*map1*/
        /*[29][67]*/         KEY_|SCLR|PLAI|PEX2, 129,4,/*seq0*/
        /*[30][70]*/         VAL_|BSEQ|PEX2,
        /*[31][71]*/           VAL_|SCLR|PLAI, 141,6,/*sval00*/
        /*[32][74]*/           VAL_|ANCH|PEX2, 155,6,/*sval01*/
        /*[33][77]*/           VAL_|SCLR|PLAI|PEX2, 162,6,/*sval01*/
        /*[34][80]*/         ESEQ|PEX2,
        /*[35][81]*/         KEY_|SCLR|PLAI, 171,4,/*seq1*/
        /*[36][84]*/         VAL_|BSEQ|PEX2,
        /*[37][85]*/           VAL_|SCLR|PLAI, 183,6,/*sval10*/
        /*[38][88]*/           VAL_|ANCH|PEX2, 197,6,/*sval11*/
        /*[39][91]*/           VAL_|SCLR|PLAI|PEX2, 204,6,/*sval11*/
        /*[40][94]*/         ESEQ|PEX2,
        /*[41][95]*/         KEY_|ALIA, 214,6,/*sval01*/
        /*[42][98]*/         VAL_|ALIA|PEX2, 224,6,/*sval11*/
        /*[43][101]*/       EMAP|PEX2,
        /*[44][102]*/     EMAP,
        /*[45][103]*/   EDOC,
        /*[46][104]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2, 0, 0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[10][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[11][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[12][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[13][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[14][28]*/       EMAP|PEX2,
        /*[15][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[16][32]*/       PEX2, 0, 0,
        /*[19][35]*/       VAL_|BMAP,
        /*[20][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[21][39]*/         VAL_|SCLR|PLAI|PEX2, 74,5,/*val11*/
        /*[22][42]*/       EMAP|PEX2,
        /*[23][43]*/       KEY_|SCLR|PLAI, 80,4,/*map2*/
        /*[24][46]*/       PEX2, 0, 0,
        /*[27][49]*/       VAL_|BMAP,
        /*[28][50]*/         KEY_|SCLR|PLAI, 94,4,/*key2*/
        /*[29][53]*/         VAL_|SCLR|PLAI|PEX2, 100,5,/*val22*/
        /*[30][56]*/       EMAP|PEX2,
        /*[31][57]*/       KEY_|SCLR|PLAI, 106,7,/*inherit*/
        /*[32][60]*/       VAL_|BMAP|PEX2,
        /*[33][61]*/         0, 0, 0,
        /*[36][64]*/         0,
        /*[37][65]*/         RREF, 10,28,
        /*[38][68]*/         PEX2, 0, 0,
        /*[41][71]*/         0, 0, 0,
        /*[44][74]*/         0,
        /*[45][75]*/       EMAP,
        /*[46][76]*/     EMAP,
        /*[47][77]*/   EDOC,
        /*[48][78]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}


//-----------------------------------------------------------------------------

TEST(ints_resolve, 9_inherit_merge_0_no_need_to_move_elements)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1: &map1\n  key1: val11\n"
        "map2: &map2\n  key1: val12\n"
        "inherit:\n"
        "  <<: *map1\n" // this uses same space
        "  seq0:\n    - sval00\n    - &sval01 sval01\n"
        "  seq1:\n    - sval10\n    - &sval11 sval11\n"
        "  *sval01 : *sval11\n"
        "";
    std::string resolved = ""
        "map0:\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1:\n  key1: val11\n"
        "map2:\n  key1: val12\n"
        "inherit:\n"
        "  key1: val11\n"
        "  seq0:\n    - sval00\n    - sval01\n"
        "  seq1:\n    - sval10\n    - sval11\n"
        "  sval01: sval11\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key1\": \"val11\"\n  },\n"
        "  \"map2\": {\n    \"key1\": \"val12\"\n  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": \"*map1\",\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"*sval01\": \"*sval11\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key1\": \"val11\"\n  },\n"
        "  \"map2\": {\n    \"key1\": \"val12\"\n  },\n"
        "  \"inherit\": {\n"
        "    \"key1\": \"val11\",\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"sval01\": \"sval11\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[10][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[11][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[12][28]*/       EMAP|PEX2,
        /*[13][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[14][32]*/       VAL_|ANCH|PEX2, 61,4,/*map1*/
        /*[15][35]*/       VAL_|BMAP|PEX2,
        /*[16][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[17][39]*/         VAL_|SCLR|PLAI|PEX2, 74,5,/*val11*/
        /*[18][42]*/       EMAP|PEX2,
        /*[19][43]*/       KEY_|SCLR|PLAI, 80,4,/*map2*/
        /*[20][46]*/       VAL_|ANCH|PEX2, 87,4,/*map2*/
        /*[21][49]*/       VAL_|BMAP|PEX2,
        /*[22][50]*/         KEY_|SCLR|PLAI, 94,4,/*key1*/
        /*[23][53]*/         VAL_|SCLR|PLAI|PEX2, 100,5,/*val12*/
        /*[24][56]*/       EMAP|PEX2,
        /*[25][57]*/       KEY_|SCLR|PLAI, 106,7,/*inherit*/
        /*[26][60]*/       VAL_|BMAP|PEX2,
        /*[27][61]*/         KEY_|SCLR|PLAI, 117,2,/*<<*/
        /*[28][64]*/         VAL_|ALIA|PEX2, 122,4,/*map1*/
        /*[29][67]*/         KEY_|SCLR|PLAI|PEX2, 129,4,/*seq0*/
        /*[30][70]*/         VAL_|BSEQ|PEX2,
        /*[31][71]*/           VAL_|SCLR|PLAI, 141,6,/*sval00*/
        /*[32][74]*/           VAL_|ANCH|PEX2, 155,6,/*sval01*/
        /*[33][77]*/           VAL_|SCLR|PLAI|PEX2, 162,6,/*sval01*/
        /*[34][80]*/         ESEQ|PEX2,
        /*[35][81]*/         KEY_|SCLR|PLAI, 171,4,/*seq1*/
        /*[36][84]*/         VAL_|BSEQ|PEX2,
        /*[37][85]*/           VAL_|SCLR|PLAI, 183,6,/*sval10*/
        /*[38][88]*/           VAL_|ANCH|PEX2, 197,6,/*sval11*/
        /*[39][91]*/           VAL_|SCLR|PLAI|PEX2, 204,6,/*sval11*/
        /*[40][94]*/         ESEQ|PEX2,
        /*[41][95]*/         KEY_|ALIA, 214,6,/*sval01*/
        /*[42][98]*/         VAL_|ALIA|PEX2, 224,6,/*sval11*/
        /*[43][101]*/       EMAP|PEX2,
        /*[44][102]*/     EMAP,
        /*[45][103]*/   EDOC,
        /*[46][104]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,0,0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[10][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[11][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[12][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[13][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[14][28]*/       EMAP|PEX2,
        /*[15][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[16][32]*/       PEX2,0,0,
        /*[19][35]*/       VAL_|BMAP,
        /*[20][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[21][39]*/         VAL_|SCLR|PLAI|PEX2, 74,5,/*val11*/
        /*[22][42]*/       EMAP|PEX2,
        /*[23][43]*/       KEY_|SCLR|PLAI, 80,4,/*map2*/
        /*[24][46]*/       PEX2,0,0,
        /*[27][49]*/       VAL_|BMAP,
        /*[28][50]*/         KEY_|SCLR|PLAI, 94,4,/*key1*/
        /*[29][53]*/         VAL_|SCLR|PLAI|PEX2, 100,5,/*val12*/
        /*[30][56]*/       EMAP|PEX2,
        /*[31][57]*/       KEY_|SCLR|PLAI, 106,7,/*inherit*/
        /*[32][60]*/       VAL_|BMAP|PEX2,
        /*[33][61]*/         0,0,0,
        /*[36][64]*/         RREF, 36,42,
        /*[37][67]*/         KEY_|SCLR|PLAI|PEX2, 129,4,/*seq0*/
        /*[38][70]*/         VAL_|BSEQ|PEX2,
        /*[39][71]*/           VAL_|SCLR|PLAI, 141,6,/*sval00*/
        /*[40][74]*/           PEX2,0,0,
        /*[43][77]*/           VAL_|SCLR|PLAI, 162,6,/*sval01*/
        /*[44][80]*/         ESEQ|PEX2,
        /*[45][81]*/         KEY_|SCLR|PLAI, 171,4,/*seq1*/
        /*[46][84]*/         VAL_|BSEQ|PEX2,
        /*[47][85]*/           VAL_|SCLR|PLAI, 183,6,/*sval10*/
        /*[48][88]*/           PEX2,0,0,
        /*[51][91]*/           VAL_|SCLR|PLAI, 204,6,/*sval11*/
        /*[52][94]*/         ESEQ|PEX2,
        /*[53][95]*/         KEY_|SCLR|PLAI, 162,6,/*sval01*/
        /*[54][98]*/         VAL_|SCLR|PLAI|PEX2, 204,6,/*sval11*/
        /*[55][101]*/       EMAP|PEX2,
        /*[56][102]*/     EMAP,
        /*[57][103]*/   EDOC,
        /*[58][104]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 9_inherit_merge_0_no_need_to_move_elements_seqs)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n  key0: [val00,val00]\n  key1: [val10,val10]\n  key2: [val20,val20]\n"
        "map1: &map1\n  key1: val11\n"
        "map2: &map2\n  key2: val12\n"
        "inherit:\n"
        "  <<: *map1\n" // this uses same space
        "  seq0:\n    - sval00\n    - &sval01 sval01\n"
        "  seq1:\n    - sval10\n    - &sval11 sval11\n"
        "  *sval01 : *sval11\n"
        "  <<: *map0\n"
        "";
    std::string resolved = ""
        "map0:\n  key0: [val00,val00]\n  key1: [val10,val10]\n  key2: [val20,val20]\n"
        "map1:\n  key1: val11\n"
        "map2:\n  key2: val12\n"
        "inherit:\n"
        "  key1: val11\n"
        "  seq0:\n    - sval00\n    - sval01\n"
        "  seq1:\n    - sval10\n    - sval11\n"
        "  sval01: sval11\n"
        "  key0: [val00,val00]\n"
        "  key2: [val20,val20]\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": [\"val00\",\"val00\"],\n"
        "    \"key1\": [\"val10\",\"val10\"],\n"
        "    \"key2\": [\"val20\",\"val20\"]\n"
        "  },\n"
        "  \"map1\": {\n    \"key1\": \"val11\"\n  },\n"
        "  \"map2\": {\n    \"key2\": \"val12\"\n  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": \"*map1\",\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"*sval01\": \"*sval11\",\n"
        "    \"<<\": \"*map0\"\n"
        "  }\n}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": [\"val00\",\"val00\"],\n"
        "    \"key1\": [\"val10\",\"val10\"],\n"
        "    \"key2\": [\"val20\",\"val20\"]\n"
        "  },\n"
        "  \"map1\": {\n    \"key1\": \"val11\"\n  },\n"
        "  \"map2\": {\n    \"key2\": \"val12\"\n  },\n"
        "  \"inherit\": {\n"
        "    \"key1\": \"val11\",\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"sval01\": \"sval11\",\n"
        "    \"key0\": [\"val00\",\"val00\"],\n"
        "    \"key2\": [\"val20\",\"val20\"]\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[8][14]*/           VAL_|SCLR|PLAI, 21,5,/*val00*/
        /*[9][17]*/           VAL_|SCLR|PLAI|PEX2, 27,5,/*val00*/
        /*[10][20]*/         ESEQ|PEX2,
        /*[11][21]*/         KEY_|SCLR|PLAI, 36,4,/*key1*/
        /*[12][24]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[13][25]*/           VAL_|SCLR|PLAI, 43,5,/*val10*/
        /*[14][28]*/           VAL_|SCLR|PLAI|PEX2, 49,5,/*val10*/
        /*[15][31]*/         ESEQ|PEX2,
        /*[16][32]*/         KEY_|SCLR|PLAI, 58,4,/*key2*/
        /*[17][35]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[18][36]*/           VAL_|SCLR|PLAI, 65,5,/*val20*/
        /*[19][39]*/           VAL_|SCLR|PLAI|PEX2, 71,5,/*val20*/
        /*[20][42]*/         ESEQ|PEX2,
        /*[21][43]*/       EMAP,
        /*[22][44]*/       KEY_|SCLR|PLAI, 78,4,/*map1*/
        /*[23][47]*/       VAL_|ANCH|PEX2, 85,4,/*map1*/
        /*[24][50]*/       VAL_|BMAP|PEX2,
        /*[25][51]*/         KEY_|SCLR|PLAI, 92,4,/*key1*/
        /*[26][54]*/         VAL_|SCLR|PLAI|PEX2, 98,5,/*val11*/
        /*[27][57]*/       EMAP|PEX2,
        /*[28][58]*/       KEY_|SCLR|PLAI, 104,4,/*map2*/
        /*[29][61]*/       VAL_|ANCH|PEX2, 111,4,/*map2*/
        /*[30][64]*/       VAL_|BMAP|PEX2,
        /*[31][65]*/         KEY_|SCLR|PLAI, 118,4,/*key2*/
        /*[32][68]*/         VAL_|SCLR|PLAI|PEX2, 124,5,/*val12*/
        /*[33][71]*/       EMAP|PEX2,
        /*[34][72]*/       KEY_|SCLR|PLAI, 130,7,/*inherit*/
        /*[35][75]*/       VAL_|BMAP|PEX2,
        /*[36][76]*/         KEY_|SCLR|PLAI, 141,2,/*<<*/
        /*[37][79]*/         VAL_|ALIA|PEX2, 146,4,/*map1*/
        /*[38][82]*/         KEY_|SCLR|PLAI|PEX2, 153,4,/*seq0*/
        /*[39][85]*/         VAL_|BSEQ|PEX2,
        /*[40][86]*/           VAL_|SCLR|PLAI, 165,6,/*sval00*/
        /*[41][89]*/           VAL_|ANCH|PEX2, 179,6,/*sval01*/
        /*[42][92]*/           VAL_|SCLR|PLAI|PEX2, 186,6,/*sval01*/
        /*[43][95]*/         ESEQ|PEX2,
        /*[44][96]*/         KEY_|SCLR|PLAI, 195,4,/*seq1*/
        /*[45][99]*/         VAL_|BSEQ|PEX2,
        /*[46][100]*/           VAL_|SCLR|PLAI, 207,6,/*sval10*/
        /*[47][103]*/           VAL_|ANCH|PEX2, 221,6,/*sval11*/
        /*[48][106]*/           VAL_|SCLR|PLAI|PEX2, 228,6,/*sval11*/
        /*[49][109]*/         ESEQ|PEX2,
        /*[50][110]*/         KEY_|ALIA, 238,6,/*sval01*/
        /*[51][113]*/         VAL_|ALIA|PEX2, 248,6,/*sval11*/
        /*[52][116]*/         KEY_|SCLR|PLAI|PEX2, 257,2,/*<<*/
        /*[53][119]*/         VAL_|ALIA|PEX2, 262,4,/*map0*/
        /*[54][122]*/       EMAP|PEX2,
        /*[55][123]*/     EMAP,
        /*[56][124]*/   EDOC,
        /*[57][125]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,0,0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[10][14]*/           VAL_|SCLR|PLAI, 21,5,/*val00*/
        /*[11][17]*/           VAL_|SCLR|PLAI|PEX2, 27,5,/*val00*/
        /*[12][20]*/         ESEQ|PEX2,
        /*[13][21]*/         KEY_|SCLR|PLAI, 36,4,/*key1*/
        /*[14][24]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[15][25]*/           VAL_|SCLR|PLAI, 43,5,/*val10*/
        /*[16][28]*/           VAL_|SCLR|PLAI|PEX2, 49,5,/*val10*/
        /*[17][31]*/         ESEQ|PEX2,
        /*[18][32]*/         KEY_|SCLR|PLAI, 58,4,/*key2*/
        /*[19][35]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[20][36]*/           VAL_|SCLR|PLAI, 65,5,/*val20*/
        /*[21][39]*/           VAL_|SCLR|PLAI|PEX2, 71,5,/*val20*/
        /*[22][42]*/         ESEQ|PEX2,
        /*[23][43]*/       EMAP,
        /*[24][44]*/       KEY_|SCLR|PLAI, 78,4,/*map1*/
        /*[25][47]*/       PEX2,0,0,
        /*[28][50]*/       VAL_|BMAP,
        /*[29][51]*/         KEY_|SCLR|PLAI, 92,4,/*key1*/
        /*[30][54]*/         VAL_|SCLR|PLAI|PEX2, 98,5,/*val11*/
        /*[31][57]*/       EMAP|PEX2,
        /*[32][58]*/       KEY_|SCLR|PLAI, 104,4,/*map2*/
        /*[33][61]*/       PEX2,0,0,
        /*[36][64]*/       VAL_|BMAP,
        /*[37][65]*/         KEY_|SCLR|PLAI, 118,4,/*key2*/
        /*[38][68]*/         VAL_|SCLR|PLAI|PEX2, 124,5,/*val12*/
        /*[39][71]*/       EMAP|PEX2,
        /*[40][72]*/       KEY_|SCLR|PLAI, 130,7,/*inherit*/
        /*[41][75]*/       VAL_|BMAP|PEX2,
        /*[42][76]*/         KEY_|SCLR|PLAI, 92,4,/*key1*/
        /*[43][79]*/         VAL_|SCLR|PLAI|PEX2, 98,5,/*val11*/
        /*[44][82]*/         KEY_|SCLR|PLAI|PEX2, 153,4,/*seq0*/
        /*[45][85]*/         VAL_|BSEQ|PEX2,
        /*[46][86]*/           VAL_|SCLR|PLAI, 165,6,/*sval00*/
        /*[47][89]*/           PEX2,0,0,
        /*[50][92]*/           VAL_|SCLR|PLAI, 186,6,/*sval01*/
        /*[51][95]*/         ESEQ|PEX2,
        /*[52][96]*/         KEY_|SCLR|PLAI, 195,4,/*seq1*/
        /*[53][99]*/         VAL_|BSEQ|PEX2,
        /*[54][100]*/           VAL_|SCLR|PLAI, 207,6,/*sval10*/
        /*[55][103]*/           PEX2,0,0,
        /*[58][106]*/           VAL_|SCLR|PLAI, 228,6,/*sval11*/
        /*[59][109]*/         ESEQ|PEX2,
        /*[60][110]*/         KEY_|SCLR|PLAI, 186,6,/*sval01*/
        /*[61][113]*/         VAL_|SCLR|PLAI|PEX2, 228,6,/*sval11*/
        /*[62][116]*/         RREF|PEX2,126,148,
        /*[63][119]*/         PEX2,0,0,
        /*[66][122]*/       EMAP,
        /*[67][123]*/     EMAP,
        /*[68][124]*/   EDOC,
        /*[69][125]*/ ESTR,
        /*[70][126]*/ KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[71][129]*/ VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[72][130]*/   VAL_|SCLR|PLAI, 21,5,/*val00*/
        /*[73][133]*/   VAL_|SCLR|PLAI|PEX2, 27,5,/*val00*/
        /*[74][136]*/ ESEQ|PEX2,
        /*[75][137]*/ KEY_|SCLR|PLAI, 58,4,/*key2*/
        /*[76][140]*/ VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[77][141]*/   VAL_|SCLR|PLAI, 65,5,/*val20*/
        /*[78][144]*/   VAL_|SCLR|PLAI|PEX2, 71,5,/*val20*/
        /*[79][147]*/ ESEQ|PEX2,
        /*[80][148]*/ 0,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 9_inherit_merge_0_no_need_to_move_elements_maps)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n  key0: {key00: val00}\n  key1: {key10: val10}\n  key2: {val20: val20}\n"
        "map1: &map1\n  key1: val11\n"
        "map2: &map2\n  key2: val12\n"
        "inherit:\n"
        "  <<: *map1\n" // this uses same space
        "  seq0:\n    - sval00\n    - &sval01 sval01\n"
        "  seq1:\n    - sval10\n    - &sval11 sval11\n"
        "  *sval01 : *sval11\n"
        "  <<: *map0\n"
        "";
    std::string resolved = ""
        "map0:\n  key0: {key00: val00}\n  key1: {key10: val10}\n  key2: {val20: val20}\n"
        "map1:\n  key1: val11\n"
        "map2:\n  key2: val12\n"
        "inherit:\n"
        "  key1: val11\n"
        "  seq0:\n    - sval00\n    - sval01\n"
        "  seq1:\n    - sval10\n    - sval11\n"
        "  sval01: sval11\n"
        "  key0: {key00: val00}\n"
        "  key2: {val20: val20}\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": {\"key00\": \"val00\"},\n"
        "    \"key1\": {\"key10\": \"val10\"},\n"
        "    \"key2\": {\"val20\": \"val20\"}\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val11\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val12\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": \"*map1\",\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"*sval01\": \"*sval11\",\n"
        "    \"<<\": \"*map0\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": {\"key00\": \"val00\"},\n"
        "    \"key1\": {\"key10\": \"val10\"},\n"
        "    \"key2\": {\"val20\": \"val20\"}\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key1\": \"val11\"\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": \"val12\"\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key1\": \"val11\",\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"sval01\": \"sval11\",\n"
        "    \"key0\": {\"key00\": \"val00\"},\n"
        "    \"key2\": {\"val20\": \"val20\"}\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[8][14]*/           KEY_|SCLR|PLAI, 21,5,/*key00*/
        /*[9][17]*/           VAL_|SCLR|PLAI|PEX2, 28,5,/*val00*/
        /*[10][20]*/         EMAP|PEX2,
        /*[11][21]*/         KEY_|SCLR|PLAI, 37,4,/*key1*/
        /*[12][24]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[13][25]*/           KEY_|SCLR|PLAI, 44,5,/*key10*/
        /*[14][28]*/           VAL_|SCLR|PLAI|PEX2, 51,5,/*val10*/
        /*[15][31]*/         EMAP|PEX2,
        /*[16][32]*/         KEY_|SCLR|PLAI, 60,4,/*key2*/
        /*[17][35]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[18][36]*/           KEY_|SCLR|PLAI, 67,5,/*val20*/
        /*[19][39]*/           VAL_|SCLR|PLAI|PEX2, 74,5,/*val20*/
        /*[20][42]*/         EMAP|PEX2,
        /*[21][43]*/       EMAP,
        /*[22][44]*/       KEY_|SCLR|PLAI, 81,4,/*map1*/
        /*[23][47]*/       VAL_|ANCH|PEX2, 88,4,/*map1*/
        /*[24][50]*/       VAL_|BMAP|PEX2,
        /*[25][51]*/         KEY_|SCLR|PLAI, 95,4,/*key1*/
        /*[26][54]*/         VAL_|SCLR|PLAI|PEX2, 101,5,/*val11*/
        /*[27][57]*/       EMAP|PEX2,
        /*[28][58]*/       KEY_|SCLR|PLAI, 107,4,/*map2*/
        /*[29][61]*/       VAL_|ANCH|PEX2, 114,4,/*map2*/
        /*[30][64]*/       VAL_|BMAP|PEX2,
        /*[31][65]*/         KEY_|SCLR|PLAI, 121,4,/*key2*/
        /*[32][68]*/         VAL_|SCLR|PLAI|PEX2, 127,5,/*val12*/
        /*[33][71]*/       EMAP|PEX2,
        /*[34][72]*/       KEY_|SCLR|PLAI, 133,7,/*inherit*/
        /*[35][75]*/       VAL_|BMAP|PEX2,
        /*[36][76]*/         KEY_|SCLR|PLAI, 144,2,/*<<*/
        /*[37][79]*/         VAL_|ALIA|PEX2, 149,4,/*map1*/
        /*[38][82]*/         KEY_|SCLR|PLAI|PEX2, 156,4,/*seq0*/
        /*[39][85]*/         VAL_|BSEQ|PEX2,
        /*[40][86]*/           VAL_|SCLR|PLAI, 168,6,/*sval00*/
        /*[41][89]*/           VAL_|ANCH|PEX2, 182,6,/*sval01*/
        /*[42][92]*/           VAL_|SCLR|PLAI|PEX2, 189,6,/*sval01*/
        /*[43][95]*/         ESEQ|PEX2,
        /*[44][96]*/         KEY_|SCLR|PLAI, 198,4,/*seq1*/
        /*[45][99]*/         VAL_|BSEQ|PEX2,
        /*[46][100]*/           VAL_|SCLR|PLAI, 210,6,/*sval10*/
        /*[47][103]*/           VAL_|ANCH|PEX2, 224,6,/*sval11*/
        /*[48][106]*/           VAL_|SCLR|PLAI|PEX2, 231,6,/*sval11*/
        /*[49][109]*/         ESEQ|PEX2,
        /*[50][110]*/         KEY_|ALIA, 241,6,/*sval01*/
        /*[51][113]*/         VAL_|ALIA|PEX2, 251,6,/*sval11*/
        /*[52][116]*/         KEY_|SCLR|PLAI|PEX2, 260,2,/*<<*/
        /*[53][119]*/         VAL_|ALIA|PEX2, 265,4,/*map0*/
        /*[54][122]*/       EMAP|PEX2,
        /*[55][123]*/     EMAP,
        /*[56][124]*/   EDOC,
        /*[57][125]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,0,0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[10][14]*/           KEY_|SCLR|PLAI, 21,5,/*key00*/
        /*[11][17]*/           VAL_|SCLR|PLAI|PEX2, 28,5,/*val00*/
        /*[12][20]*/         EMAP|PEX2,
        /*[13][21]*/         KEY_|SCLR|PLAI, 37,4,/*key1*/
        /*[14][24]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[15][25]*/           KEY_|SCLR|PLAI, 44,5,/*key10*/
        /*[16][28]*/           VAL_|SCLR|PLAI|PEX2, 51,5,/*val10*/
        /*[17][31]*/         EMAP|PEX2,
        /*[18][32]*/         KEY_|SCLR|PLAI, 60,4,/*key2*/
        /*[19][35]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[20][36]*/           KEY_|SCLR|PLAI, 67,5,/*val20*/
        /*[21][39]*/           VAL_|SCLR|PLAI|PEX2, 74,5,/*val20*/
        /*[22][42]*/         EMAP|PEX2,
        /*[23][43]*/       EMAP,
        /*[24][44]*/       KEY_|SCLR|PLAI, 81,4,/*map1*/
        /*[25][47]*/       PEX2,0,0,
        /*[28][50]*/       VAL_|BMAP,
        /*[29][51]*/         KEY_|SCLR|PLAI, 95,4,/*key1*/
        /*[30][54]*/         VAL_|SCLR|PLAI|PEX2, 101,5,/*val11*/
        /*[31][57]*/       EMAP|PEX2,
        /*[32][58]*/       KEY_|SCLR|PLAI, 107,4,/*map2*/
        /*[33][61]*/       PEX2,0,0,
        /*[36][64]*/       VAL_|BMAP,
        /*[37][65]*/         KEY_|SCLR|PLAI, 121,4,/*key2*/
        /*[38][68]*/         VAL_|SCLR|PLAI|PEX2, 127,5,/*val12*/
        /*[39][71]*/       EMAP|PEX2,
        /*[40][72]*/       KEY_|SCLR|PLAI, 133,7,/*inherit*/
        /*[41][75]*/       VAL_|BMAP|PEX2,
        /*[42][76]*/         KEY_|SCLR|PLAI, 95,4,/*key1*/
        /*[43][79]*/         VAL_|SCLR|PLAI|PEX2, 101,5,/*val11*/
        /*[44][82]*/         KEY_|SCLR|PLAI|PEX2, 156,4,/*seq0*/
        /*[45][85]*/         VAL_|BSEQ|PEX2,
        /*[46][86]*/           VAL_|SCLR|PLAI, 168,6,/*sval00*/
        /*[47][89]*/           PEX2,0,0,
        /*[50][92]*/           VAL_|SCLR|PLAI, 189,6,/*sval01*/
        /*[51][95]*/         ESEQ|PEX2,
        /*[52][96]*/         KEY_|SCLR|PLAI, 198,4,/*seq1*/
        /*[53][99]*/         VAL_|BSEQ|PEX2,
        /*[54][100]*/           VAL_|SCLR|PLAI, 210,6,/*sval10*/
        /*[55][103]*/           PEX2,0,0,
        /*[58][106]*/           VAL_|SCLR|PLAI, 231,6,/*sval11*/
        /*[59][109]*/         ESEQ|PEX2,
        /*[60][110]*/         KEY_|SCLR|PLAI, 189,6,/*sval01*/
        /*[61][113]*/         VAL_|SCLR|PLAI|PEX2, 231,6,/*sval11*/
        /*[62][116]*/         RREF|PEX2,126,148,
        /*[63][119]*/         PEX2,0,0,
        /*[66][122]*/       EMAP,
        /*[67][123]*/     EMAP,
        /*[68][124]*/   EDOC,
        /*[69][125]*/ ESTR,
        /*[70][126]*/ KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[71][129]*/ VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[72][130]*/   KEY_|SCLR|PLAI, 21,5,/*key00*/
        /*[73][133]*/   VAL_|SCLR|PLAI|PEX2, 28,5,/*val00*/
        /*[74][136]*/ EMAP|PEX2,
        /*[75][137]*/ KEY_|SCLR|PLAI, 60,4,/*key2*/
        /*[76][140]*/ VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[77][141]*/   KEY_|SCLR|PLAI, 67,5,/*val20*/
        /*[78][144]*/   VAL_|SCLR|PLAI|PEX2, 74,5,/*val20*/
        /*[79][147]*/ EMAP|PEX2,
        /*[80][148]*/ 0,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}



//-----------------------------------------------------------------------------

TEST(ints_resolve, 10_inherit_merge_1_move_elements_back)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1: &map1\n  key1: val11\n"
        "map2: &map2\n  key1: val12\n"
        "inherit:\n"
        "  <<: [*map1,*map2]\n" // this uses less space
        "  seq0:\n    - sval00\n    - &sval01 sval01\n"
        "  seq1:\n    - sval10\n    - &sval11 sval11\n"
        "  *sval01 : *sval11\n"
        "";
    std::string resolved = ""
        "map0:\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1:\n  key1: val11\n"
        "map2:\n  key1: val12\n"
        "inherit:\n"
        "  key1: val11\n"
        "  seq0:\n    - sval00\n    - sval01\n"
        "  seq1:\n    - sval10\n    - sval11\n"
        "  sval01: sval11\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key1\": \"val11\"\n  },\n"
        "  \"map2\": {\n    \"key1\": \"val12\"\n  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map1\",\"*map2\"],\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"*sval01\": \"*sval11\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key1\": \"val11\"\n  },\n"
        "  \"map2\": {\n    \"key1\": \"val12\"\n  },\n"
        "  \"inherit\": {\n"
        "    \"key1\": \"val11\",\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"sval01\": \"sval11\",\n" // FIXME trailing comma
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[10][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[11][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[12][28]*/       EMAP|PEX2,
        /*[13][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[14][32]*/       VAL_|ANCH|PEX2, 61,4,/*map1*/
        /*[15][35]*/       VAL_|BMAP|PEX2,
        /*[16][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[17][39]*/         VAL_|SCLR|PLAI|PEX2, 74,5,/*val11*/
        /*[18][42]*/       EMAP|PEX2,
        /*[19][43]*/       KEY_|SCLR|PLAI, 80,4,/*map2*/
        /*[20][46]*/       VAL_|ANCH|PEX2, 87,4,/*map2*/
        /*[21][49]*/       VAL_|BMAP|PEX2,
        /*[22][50]*/         KEY_|SCLR|PLAI, 94,4,/*key1*/
        /*[23][53]*/         VAL_|SCLR|PLAI|PEX2, 100,5,/*val12*/
        /*[24][56]*/       EMAP|PEX2,
        /*[25][57]*/       KEY_|SCLR|PLAI, 106,7,/*inherit*/
        /*[26][60]*/       VAL_|BMAP|PEX2,
        /*[27][61]*/         KEY_|SCLR|PLAI, 117,2,/*<<*/
        /*[28][64]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[29][65]*/           VAL_|ALIA, 123,4,/*map1*/
        /*[30][68]*/           VAL_|ALIA|PEX2, 129,4,/*map2*/
        /*[31][71]*/         ESEQ|PEX2,
        /*[32][72]*/         KEY_|SCLR|PLAI, 137,4,/*seq0*/
        /*[33][75]*/         VAL_|BSEQ|PEX2,
        /*[34][76]*/           VAL_|SCLR|PLAI, 149,6,/*sval00*/
        /*[35][79]*/           VAL_|ANCH|PEX2, 163,6,/*sval01*/
        /*[36][82]*/           VAL_|SCLR|PLAI|PEX2, 170,6,/*sval01*/
        /*[37][85]*/         ESEQ|PEX2,
        /*[38][86]*/         KEY_|SCLR|PLAI, 179,4,/*seq1*/
        /*[39][89]*/         VAL_|BSEQ|PEX2,
        /*[40][90]*/           VAL_|SCLR|PLAI, 191,6,/*sval10*/
        /*[41][93]*/           VAL_|ANCH|PEX2, 205,6,/*sval11*/
        /*[42][96]*/           VAL_|SCLR|PLAI|PEX2, 212,6,/*sval11*/
        /*[43][99]*/         ESEQ|PEX2,
        /*[44][100]*/         KEY_|ALIA, 222,6,/*sval01*/
        /*[45][103]*/         VAL_|ALIA|PEX2, 232,6,/*sval11*/
        /*[46][106]*/       EMAP|PEX2,
        /*[47][107]*/     EMAP,
        /*[48][108]*/   EDOC,
        /*[49][109]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2, 0,0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[10][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[11][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[12][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[13][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[14][28]*/       EMAP|PEX2,
        /*[15][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[16][32]*/       PEX2, 0,0,
        /*[19][35]*/       VAL_|BMAP,
        /*[20][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[21][39]*/         VAL_|SCLR|PLAI|PEX2, 74,5,/*val11*/
        /*[22][42]*/       EMAP|PEX2,
        /*[23][43]*/       KEY_|SCLR|PLAI, 80,4,/*map2*/
        /*[24][46]*/       PEX2, 0,0,
        /*[27][49]*/       VAL_|BMAP,
        /*[28][50]*/         KEY_|SCLR|PLAI, 94,4,/*key1*/
        /*[29][53]*/         VAL_|SCLR|PLAI|PEX2, 100,5,/*val12*/
        /*[30][56]*/       EMAP|PEX2,
        /*[31][57]*/       KEY_|SCLR|PLAI, 106,7,/*inherit*/
        /*[32][60]*/       VAL_|BMAP|PEX2,
        /*[33][61]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[34][64]*/         VAL_|SCLR|PLAI|PEX2, 74,5,/*val11*/
        /*[35][67]*/         KEY_|SCLR|PLAI|PEX2, 137,4,/*seq0*/
        /*[36][70]*/         VAL_|BSEQ|PEX2,
        /*[37][71]*/           VAL_|SCLR|PLAI, 149,6,/*sval00*/
        /*[38][74]*/           PEX2, 0,0,
        /*[41][77]*/           VAL_|SCLR|PLAI, 170,6,/*sval01*/
        /*[42][80]*/         ESEQ|PEX2,
        /*[43][81]*/         KEY_|SCLR|PLAI, 179,4,/*seq1*/
        /*[44][84]*/         VAL_|BSEQ|PEX2,
        /*[45][85]*/           VAL_|SCLR|PLAI, 191,6,/*sval10*/
        /*[46][88]*/           PEX2, 0,0,
        /*[49][91]*/           VAL_|SCLR|PLAI, 212,6,/*sval11*/
        /*[50][94]*/         ESEQ|PEX2,
        /*[51][95]*/         KEY_|SCLR|PLAI, 170,6,/*sval01*/
        /*[52][98]*/         VAL_|SCLR|PLAI|PEX2, 212,6,/*sval11*/
        /*[53][101]*/        PEX2,0,0,0,0,
        /*[62][106]*/       EMAP,
        /*[63][107]*/     EMAP,
        /*[64][108]*/   EDOC,
        /*[65][109]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 10_inherit_merge_1_move_elements_back_seqs)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1: &map1\n  key1: [sub11,val11]\n"
        "map2: &map2\n  key1: [sub12,val12]\n"
        "map3: &map3 {}\n"
        "inherit:\n"
        "  <<: [*map1,*map2,*map3]\n" // less space after resolving
        "  seq0:\n    - sval00\n    - &sval01 sval01\n"
        "  seq1:\n    - sval10\n    - &sval11 sval11\n"
        "  *sval01 : *sval11\n"
        "";
    std::string resolved = ""
        "map0:\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1:\n  key1: [sub11,val11]\n"
        "map2:\n  key1: [sub12,val12]\n"
        "map3: {}\n"
        "inherit:\n"
        "  key1: [sub11,val11]\n"
        "  seq0:\n    - sval00\n    - sval01\n"
        "  seq1:\n    - sval10\n    - sval11\n"
        "  sval01: sval11\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key1\": [\"sub11\",\"val11\"]\n  },\n"
        "  \"map2\": {\n    \"key1\": [\"sub12\",\"val12\"]\n  },\n"
        "  \"map3\": {},\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map1\",\"*map2\",\"*map3\"],\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"*sval01\": \"*sval11\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key1\": [\"sub11\",\"val11\"]\n  },\n"
        "  \"map2\": {\n    \"key1\": [\"sub12\",\"val12\"]\n  },\n"
        "  \"map3\": {},\n"
        "  \"inherit\": {\n"
        "    \"key1\": [\"sub11\",\"val11\"],\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"sval01\": \"sval11\",\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {

        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[10][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[11][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[12][28]*/       EMAP|PEX2,
        /*[13][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[14][32]*/       VAL_|ANCH|PEX2, 61,4,/*map1*/
        /*[15][35]*/       VAL_|BMAP|PEX2,
        /*[16][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[17][39]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[18][40]*/           VAL_|SCLR|PLAI, 75,5,/*sub11*/
        /*[19][43]*/           VAL_|SCLR|PLAI|PEX2, 81,5,/*val11*/
        /*[20][46]*/         ESEQ|PEX2,
        /*[21][47]*/       EMAP,
        /*[22][48]*/       KEY_|SCLR|PLAI, 88,4,/*map2*/
        /*[23][51]*/       VAL_|ANCH|PEX2, 95,4,/*map2*/
        /*[24][54]*/       VAL_|BMAP|PEX2,
        /*[25][55]*/         KEY_|SCLR|PLAI, 102,4,/*key1*/
        /*[26][58]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[27][59]*/           VAL_|SCLR|PLAI, 109,5,/*sub12*/
        /*[28][62]*/           VAL_|SCLR|PLAI|PEX2, 115,5,/*val12*/
        /*[29][65]*/         ESEQ|PEX2,
        /*[30][66]*/       EMAP,
        /*[31][67]*/       KEY_|SCLR|PLAI, 122,4,/*map3*/
        /*[32][70]*/       VAL_|ANCH|PEX2, 129,4,/*map3*/
        /*[33][73]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[34][74]*/       EMAP,
        /*[35][75]*/       KEY_|SCLR|PLAI, 137,7,/*inherit*/
        /*[36][78]*/       VAL_|BMAP|PEX2,
        /*[37][79]*/         KEY_|SCLR|PLAI, 148,2,/*<<*/
        /*[38][82]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[39][83]*/           VAL_|ALIA, 154,4,/*map1*/
        /*[40][86]*/           VAL_|ALIA|PEX2, 160,4,/*map2*/
        /*[41][89]*/           VAL_|ALIA|PEX2, 166,4,/*map3*/
        /*[42][92]*/         ESEQ|PEX2,
        /*[43][93]*/         KEY_|SCLR|PLAI, 174,4,/*seq0*/
        /*[44][96]*/         VAL_|BSEQ|PEX2,
        /*[45][97]*/           VAL_|SCLR|PLAI, 186,6,/*sval00*/
        /*[46][100]*/           VAL_|ANCH|PEX2, 200,6,/*sval01*/
        /*[47][103]*/           VAL_|SCLR|PLAI|PEX2, 207,6,/*sval01*/
        /*[48][106]*/         ESEQ|PEX2,
        /*[49][107]*/         KEY_|SCLR|PLAI, 216,4,/*seq1*/
        /*[50][110]*/         VAL_|BSEQ|PEX2,
        /*[51][111]*/           VAL_|SCLR|PLAI, 228,6,/*sval10*/
        /*[52][114]*/           VAL_|ANCH|PEX2, 242,6,/*sval11*/
        /*[53][117]*/           VAL_|SCLR|PLAI|PEX2, 249,6,/*sval11*/
        /*[54][120]*/         ESEQ|PEX2,
        /*[55][121]*/         KEY_|ALIA, 259,6,/*sval01*/
        /*[56][124]*/         VAL_|ALIA|PEX2, 269,6,/*sval11*/
        /*[57][127]*/       EMAP|PEX2,
        /*[58][128]*/     EMAP,
        /*[59][129]*/   EDOC,
        /*[60][130]*/ ESTR,

    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,0,0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[10][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[11][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[12][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[13][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[14][28]*/       EMAP|PEX2,
        /*[15][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[16][32]*/       PEX2,0,0,
        /*[19][35]*/       VAL_|BMAP,
        /*[20][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[21][39]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[22][40]*/           VAL_|SCLR|PLAI, 75,5,/*sub11*/
        /*[23][43]*/           VAL_|SCLR|PLAI|PEX2, 81,5,/*val11*/
        /*[24][46]*/         ESEQ|PEX2,
        /*[25][47]*/       EMAP,
        /*[26][48]*/       KEY_|SCLR|PLAI, 88,4,/*map2*/
        /*[27][51]*/       PEX2,0,0,
        /*[30][54]*/       VAL_|BMAP,
        /*[31][55]*/         KEY_|SCLR|PLAI, 102,4,/*key1*/
        /*[32][58]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[33][59]*/           VAL_|SCLR|PLAI, 109,5,/*sub12*/
        /*[34][62]*/           VAL_|SCLR|PLAI|PEX2, 115,5,/*val12*/
        /*[35][65]*/         ESEQ|PEX2,
        /*[36][66]*/       EMAP,
        /*[37][67]*/       KEY_|SCLR|PLAI, 122,4,/*map3*/
        /*[38][70]*/       PEX2,0,0,
        /*[41][73]*/       VAL_|BMAP|FLOW|FSL_,
        /*[42][74]*/       EMAP,
        /*[43][75]*/       KEY_|SCLR|PLAI, 137,7,/*inherit*/
        /*[44][78]*/       VAL_|BMAP|PEX2,
        /*[45][79]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[46][82]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[47][83]*/           VAL_|SCLR|PLAI, 75,5,/*sub11*/
        /*[48][86]*/           VAL_|SCLR|PLAI|PEX2, 81,5,/*val11*/
        /*[49][89]*/         ESEQ|PEX2,
        /*[50][90]*/         KEY_|SCLR|PLAI, 174,4,/*seq0*/
        /*[51][93]*/         VAL_|BSEQ|PEX2,
        /*[52][94]*/           VAL_|SCLR|PLAI, 186,6,/*sval00*/
        /*[53][97]*/           PEX2,0,0,
        /*[56][100]*/           VAL_|SCLR|PLAI, 207,6,/*sval01*/
        /*[57][103]*/         ESEQ|PEX2,
        /*[58][104]*/         KEY_|SCLR|PLAI, 216,4,/*seq1*/
        /*[59][107]*/         VAL_|BSEQ|PEX2,
        /*[60][108]*/           VAL_|SCLR|PLAI, 228,6,/*sval10*/
        /*[61][111]*/           PEX2,0,0,
        /*[64][114]*/           VAL_|SCLR|PLAI, 249,6,/*sval11*/
        /*[65][117]*/         ESEQ|PEX2,
        /*[66][118]*/         KEY_|SCLR|PLAI, 207,6,/*sval01*/
        /*[67][121]*/         VAL_|SCLR|PLAI|PEX2, 249,6,/*sval11*/
        /*[68][124]*/         PEX2,0,0,
        /*[71][127]*/       EMAP,
        /*[72][128]*/     EMAP,
        /*[73][129]*/   EDOC,
        /*[74][130]*/ ESTR,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 10_inherit_merge_1_move_elements_back_maps)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1: &map1\n  key1: {sub11: val11}\n"
        "map2: &map2\n  key1: {sub11: val12}\n"
        "map3: &map3 {}\n"
        "inherit:\n"
        "  <<: [*map1,*map2,*map3]\n" // less space after resolving
        "  seq0:\n    - sval00\n    - &sval01 sval01\n"
        "  seq1:\n    - sval10\n    - &sval11 sval11\n"
        "  *sval01 : *sval11\n"
        "";
    std::string resolved = ""
        "map0:\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1:\n  key1: {sub11: val11}\n"
        "map2:\n  key1: {sub11: val12}\n"
        "map3: {}\n"
        "inherit:\n"
        "  key1: {sub11: val11}\n"
        "  seq0:\n    - sval00\n    - sval01\n"
        "  seq1:\n    - sval10\n    - sval11\n"
        "  sval01: sval11\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key1\": {\"sub11\": \"val11\"}\n  },\n"
        "  \"map2\": {\n    \"key1\": {\"sub11\": \"val12\"}\n  },\n"
        "  \"map3\": {},\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map1\",\"*map2\",\"*map3\"],\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"*sval01\": \"*sval11\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key1\": {\"sub11\": \"val11\"}\n  },\n"
        "  \"map2\": {\n    \"key1\": {\"sub11\": \"val12\"}\n  },\n"
        "  \"map3\": {},\n"
        "  \"inherit\": {\n"
        "    \"key1\": {\"sub11\": \"val11\"},\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"sval01\": \"sval11\",\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[10][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[11][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[12][28]*/       EMAP|PEX2,
        /*[13][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[14][32]*/       VAL_|ANCH|PEX2, 61,4,/*map1*/
        /*[15][35]*/       VAL_|BMAP|PEX2,
        /*[16][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[17][39]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[18][40]*/           KEY_|SCLR|PLAI, 75,5,/*sub11*/
        /*[19][43]*/           VAL_|SCLR|PLAI|PEX2, 82,5,/*val11*/
        /*[20][46]*/         EMAP|PEX2,
        /*[21][47]*/       EMAP,
        /*[22][48]*/       KEY_|SCLR|PLAI, 89,4,/*map2*/
        /*[23][51]*/       VAL_|ANCH|PEX2, 96,4,/*map2*/
        /*[24][54]*/       VAL_|BMAP|PEX2,
        /*[25][55]*/         KEY_|SCLR|PLAI, 103,4,/*key1*/
        /*[26][58]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[27][59]*/           KEY_|SCLR|PLAI, 110,5,/*sub11*/
        /*[28][62]*/           VAL_|SCLR|PLAI|PEX2, 117,5,/*val12*/
        /*[29][65]*/         EMAP|PEX2,
        /*[30][66]*/       EMAP,
        /*[31][67]*/       KEY_|SCLR|PLAI, 124,4,/*map3*/
        /*[32][70]*/       VAL_|ANCH|PEX2, 131,4,/*map3*/
        /*[33][73]*/       VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[34][74]*/       EMAP,
        /*[35][75]*/       KEY_|SCLR|PLAI, 139,7,/*inherit*/
        /*[36][78]*/       VAL_|BMAP|PEX2,
        /*[37][79]*/         KEY_|SCLR|PLAI, 150,2,/*<<*/
        /*[38][82]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[39][83]*/           VAL_|ALIA, 156,4,/*map1*/
        /*[40][86]*/           VAL_|ALIA|PEX2, 162,4,/*map2*/
        /*[41][89]*/           VAL_|ALIA|PEX2, 168,4,/*map3*/
        /*[42][92]*/         ESEQ|PEX2,
        /*[43][93]*/         KEY_|SCLR|PLAI, 176,4,/*seq0*/
        /*[44][96]*/         VAL_|BSEQ|PEX2,
        /*[45][97]*/           VAL_|SCLR|PLAI, 188,6,/*sval00*/
        /*[46][100]*/           VAL_|ANCH|PEX2, 202,6,/*sval01*/
        /*[47][103]*/           VAL_|SCLR|PLAI|PEX2, 209,6,/*sval01*/
        /*[48][106]*/         ESEQ|PEX2,
        /*[49][107]*/         KEY_|SCLR|PLAI, 218,4,/*seq1*/
        /*[50][110]*/         VAL_|BSEQ|PEX2,
        /*[51][111]*/           VAL_|SCLR|PLAI, 230,6,/*sval10*/
        /*[52][114]*/           VAL_|ANCH|PEX2, 244,6,/*sval11*/
        /*[53][117]*/           VAL_|SCLR|PLAI|PEX2, 251,6,/*sval11*/
        /*[54][120]*/         ESEQ|PEX2,
        /*[55][121]*/         KEY_|ALIA, 261,6,/*sval01*/
        /*[56][124]*/         VAL_|ALIA|PEX2, 271,6,/*sval11*/
        /*[57][127]*/       EMAP|PEX2,
        /*[58][128]*/     EMAP,
        /*[59][129]*/   EDOC,
        /*[60][130]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {

        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,0,0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[10][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[11][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[12][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[13][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[14][28]*/       EMAP|PEX2,
        /*[15][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[16][32]*/       PEX2,0,0,
        /*[19][35]*/       VAL_|BMAP,
        /*[20][36]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[21][39]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[22][40]*/           KEY_|SCLR|PLAI, 75,5,/*sub11*/
        /*[23][43]*/           VAL_|SCLR|PLAI|PEX2, 82,5,/*val11*/
        /*[24][46]*/         EMAP|PEX2,
        /*[25][47]*/       EMAP,
        /*[26][48]*/       KEY_|SCLR|PLAI, 89,4,/*map2*/
        /*[27][51]*/       PEX2,0,0,
        /*[30][54]*/       VAL_|BMAP,
        /*[31][55]*/         KEY_|SCLR|PLAI, 103,4,/*key1*/
        /*[32][58]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[33][59]*/           KEY_|SCLR|PLAI, 110,5,/*sub11*/
        /*[34][62]*/           VAL_|SCLR|PLAI|PEX2, 117,5,/*val12*/
        /*[35][65]*/         EMAP|PEX2,
        /*[36][66]*/       EMAP,
        /*[37][67]*/       KEY_|SCLR|PLAI, 124,4,/*map3*/
        /*[38][70]*/       PEX2,0,0,
        /*[41][73]*/       VAL_|BMAP|FLOW|FSL_,
        /*[42][74]*/       EMAP,
        /*[43][75]*/       KEY_|SCLR|PLAI, 139,7,/*inherit*/
        /*[44][78]*/       VAL_|BMAP|PEX2,
        /*[45][79]*/         KEY_|SCLR|PLAI, 68,4,/*key1*/
        /*[46][82]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[47][83]*/           KEY_|SCLR|PLAI, 75,5,/*sub11*/
        /*[48][86]*/           VAL_|SCLR|PLAI|PEX2, 82,5,/*val11*/
        /*[49][89]*/         EMAP|PEX2,
        /*[50][90]*/         KEY_|SCLR|PLAI, 176,4,/*seq0*/
        /*[51][93]*/         VAL_|BSEQ|PEX2,
        /*[52][94]*/           VAL_|SCLR|PLAI, 188,6,/*sval00*/
        /*[53][97]*/           PEX2,0,0,
        /*[56][100]*/           VAL_|SCLR|PLAI, 209,6,/*sval01*/
        /*[57][103]*/         ESEQ|PEX2,
        /*[58][104]*/         KEY_|SCLR|PLAI, 218,4,/*seq1*/
        /*[59][107]*/         VAL_|BSEQ|PEX2,
        /*[60][108]*/           VAL_|SCLR|PLAI, 230,6,/*sval10*/
        /*[61][111]*/           PEX2,0,0,
        /*[64][114]*/           VAL_|SCLR|PLAI, 251,6,/*sval11*/
        /*[65][117]*/         ESEQ|PEX2,
        /*[66][118]*/         KEY_|SCLR|PLAI, 209,6,/*sval01*/
        /*[67][121]*/         VAL_|SCLR|PLAI|PEX2, 251,6,/*sval11*/
        /*[68][124]*/         PEX2,0,0,
        /*[71][127]*/       EMAP,
        /*[72][128]*/     EMAP,
        /*[73][129]*/   EDOC,
        /*[74][130]*/ ESTR,

    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}


//-----------------------------------------------------------------------------

TEST(ints_resolve, 11_inherit_merge_2_have_to_move_forward)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1: &map1\n  key11: val11\n  key1: val11\n  key12: val12\n"
        "map2: &map2\n  key2: val22\n  key1: val21\n  key22: val22\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n" // causes move fwd
        "  seq0:\n    - sval00\n    - &sval01 sval01\n"
        "  seq1:\n    - sval10\n    - &sval11 sval11\n"
        "  *sval01 : *sval11\n"
        "";
    std::string resolved = ""
        "map0:\n  key0: val00\n  key1: val10\n  key2: val20\n"
        "map1:\n  key11: val11\n  key1: val11\n  key12: val12\n"
        "map2:\n  key2: val22\n  key1: val21\n  key22: val22\n"
        "inherit:\n"
        "  key0: val00\n"
        "  key1: val10\n"
        "  key2: val20\n"
        "  key11: val11\n"
        "  key12: val12\n"
        "  key22: val22\n"
        "  seq0:\n    - sval00\n    - sval01\n"
        "  seq1:\n    - sval10\n    - sval11\n"
        "  sval01: sval11\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key11\": \"val11\",\n    \"key1\": \"val11\",\n    \"key12\": \"val12\"\n  },\n"
        "  \"map2\": {\n    \"key2\": \"val22\",\n    \"key1\": \"val21\",\n    \"key22\": \"val22\"\n  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"],\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"*sval01\": \"*sval11\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n    \"key0\": \"val00\",\n    \"key1\": \"val10\",\n    \"key2\": \"val20\"\n  },\n"
        "  \"map1\": {\n    \"key11\": \"val11\",\n    \"key1\": \"val11\",\n    \"key12\": \"val12\"\n  },\n"
        "  \"map2\": {\n    \"key2\": \"val22\",\n    \"key1\": \"val21\",\n    \"key22\": \"val22\"\n  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": \"val00\",\n"
        "    \"key1\": \"val10\",\n"
        "    \"key2\": \"val20\",\n"
        "    \"key11\": \"val11\",\n"
        "    \"key12\": \"val12\",\n"
        "    \"key22\": \"val22\",\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"sval01\": \"sval11\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[8][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[9][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[10][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[11][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[12][28]*/       EMAP|PEX2,
        /*[13][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[14][32]*/       VAL_|ANCH|PEX2, 61,4,/*map1*/
        /*[15][35]*/       VAL_|BMAP|PEX2,
        /*[16][36]*/         KEY_|SCLR|PLAI, 68,5,/*key11*/
        /*[17][39]*/         VAL_|SCLR|PLAI|PEX2, 75,5,/*val11*/
        /*[18][42]*/         KEY_|SCLR|PLAI|PEX2, 83,4,/*key1*/
        /*[19][45]*/         VAL_|SCLR|PLAI|PEX2, 89,5,/*val11*/
        /*[20][48]*/         KEY_|SCLR|PLAI|PEX2, 97,5,/*key12*/
        /*[21][51]*/         VAL_|SCLR|PLAI|PEX2, 104,5,/*val12*/
        /*[22][54]*/       EMAP|PEX2,
        /*[23][55]*/       KEY_|SCLR|PLAI, 110,4,/*map2*/
        /*[24][58]*/       VAL_|ANCH|PEX2, 117,4,/*map2*/
        /*[25][61]*/       VAL_|BMAP|PEX2,
        /*[26][62]*/         KEY_|SCLR|PLAI, 124,4,/*key2*/
        /*[27][65]*/         VAL_|SCLR|PLAI|PEX2, 130,5,/*val22*/
        /*[28][68]*/         KEY_|SCLR|PLAI|PEX2, 138,4,/*key1*/
        /*[29][71]*/         VAL_|SCLR|PLAI|PEX2, 144,5,/*val21*/
        /*[30][74]*/         KEY_|SCLR|PLAI|PEX2, 152,5,/*key22*/
        /*[31][77]*/         VAL_|SCLR|PLAI|PEX2, 159,5,/*val22*/
        /*[32][80]*/       EMAP|PEX2,
        /*[33][81]*/       KEY_|SCLR|PLAI, 165,7,/*inherit*/
        /*[34][84]*/       VAL_|BMAP|PEX2,
        /*[35][85]*/         KEY_|SCLR|PLAI, 176,2,/*<<*/
        /*[36][88]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[37][89]*/           VAL_|ALIA, 182,4,/*map0*/
        /*[38][92]*/           VAL_|ALIA|PEX2, 188,4,/*map1*/
        /*[39][95]*/           VAL_|ALIA|PEX2, 194,4,/*map2*/
        /*[40][98]*/         ESEQ|PEX2,
        /*[41][99]*/         KEY_|SCLR|PLAI, 202,4,/*seq0*/
        /*[42][102]*/         VAL_|BSEQ|PEX2,
        /*[43][103]*/           VAL_|SCLR|PLAI, 214,6,/*sval00*/
        /*[44][106]*/           VAL_|ANCH|PEX2, 228,6,/*sval01*/
        /*[45][109]*/           VAL_|SCLR|PLAI|PEX2, 235,6,/*sval01*/
        /*[46][112]*/         ESEQ|PEX2,
        /*[47][113]*/         KEY_|SCLR|PLAI, 244,4,/*seq1*/
        /*[48][116]*/         VAL_|BSEQ|PEX2,
        /*[49][117]*/           VAL_|SCLR|PLAI, 256,6,/*sval10*/
        /*[50][120]*/           VAL_|ANCH|PEX2, 270,6,/*sval11*/
        /*[51][123]*/           VAL_|SCLR|PLAI|PEX2, 277,6,/*sval11*/
        /*[52][126]*/         ESEQ|PEX2,
        /*[53][127]*/         KEY_|ALIA, 287,6,/*sval01*/
        /*[54][130]*/         VAL_|ALIA|PEX2, 297,6,/*sval11*/
        /*[55][133]*/       EMAP|PEX2,
        /*[56][134]*/     EMAP,
        /*[57][135]*/   EDOC,
        /*[58][136]*/ ESTR,
    };
    const evt_bits evts_resolved[] = {
        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,0,0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[10][16]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[11][19]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[12][22]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[13][25]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[14][28]*/       EMAP|PEX2,
        /*[15][29]*/       KEY_|SCLR|PLAI, 54,4,/*map1*/
        /*[16][32]*/       PEX2,0,0,
        /*[19][35]*/       VAL_|BMAP,
        /*[20][36]*/         KEY_|SCLR|PLAI, 68,5,/*key11*/
        /*[21][39]*/         VAL_|SCLR|PLAI|PEX2, 75,5,/*val11*/
        /*[22][42]*/         KEY_|SCLR|PLAI|PEX2, 83,4,/*key1*/
        /*[23][45]*/         VAL_|SCLR|PLAI|PEX2, 89,5,/*val11*/
        /*[24][48]*/         KEY_|SCLR|PLAI|PEX2, 97,5,/*key12*/
        /*[25][51]*/         VAL_|SCLR|PLAI|PEX2, 104,5,/*val12*/
        /*[26][54]*/       EMAP|PEX2,
        /*[27][55]*/       KEY_|SCLR|PLAI, 110,4,/*map2*/
        /*[28][58]*/       PEX2,0,0,
        /*[31][61]*/       VAL_|BMAP,
        /*[32][62]*/         KEY_|SCLR|PLAI, 124,4,/*key2*/
        /*[33][65]*/         VAL_|SCLR|PLAI|PEX2, 130,5,/*val22*/
        /*[34][68]*/         KEY_|SCLR|PLAI|PEX2, 138,4,/*key1*/
        /*[35][71]*/         VAL_|SCLR|PLAI|PEX2, 144,5,/*val21*/
        /*[36][74]*/         KEY_|SCLR|PLAI|PEX2, 152,5,/*key22*/
        /*[37][77]*/         VAL_|SCLR|PLAI|PEX2, 159,5,/*val22*/
        /*[38][80]*/       EMAP|PEX2,
        /*[39][81]*/       KEY_|SCLR|PLAI, 165,7,/*inherit*/
        /*[40][84]*/       VAL_|BMAP|PEX2,
        /*[41][85]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[42][88]*/         VAL_|SCLR|PLAI|PEX2, 20,5,/*val00*/
        /*[43][91]*/         KEY_|SCLR|PLAI|PEX2, 28,4,/*key1*/
        /*[44][94]*/         VAL_|SCLR|PLAI|PEX2, 34,5,/*val10*/
        /*[45][97]*/         KEY_|SCLR|PLAI|PEX2, 42,4,/*key2*/
        /*[46][100]*/         VAL_|SCLR|PLAI|PEX2, 48,5,/*val20*/
        /*[47][103]*/         KEY_|SCLR|PLAI|PEX2, 68,5,/*key11*/
        /*[48][106]*/         VAL_|SCLR|PLAI|PEX2, 75,5,/*val11*/
        /*[49][109]*/         KEY_|SCLR|PLAI|PEX2, 97,5,/*key12*/
        /*[50][112]*/         VAL_|SCLR|PLAI|PEX2, 104,5,/*val12*/
        /*[51][115]*/         KEY_|SCLR|PLAI|PEX2, 152,5,/*key22*/
        /*[52][118]*/         VAL_|SCLR|PLAI|PEX2, 159,5,/*val22*/
        /*[53][121]*/         RREF|PEX2,137,171,
        /*[54][124]*/         PEX2,0,0,0,0,0,0,0,0,
        /*[63][133]*/       EMAP,
        /*[64][134]*/     EMAP,
        /*[65][135]*/   EDOC,
        /*[66][136]*/ ESTR,
        /*[67][137]*/ KEY_|SCLR|PLAI, 202,4,/*seq0*/
        /*[68][140]*/ VAL_|BSEQ|PEX2,
        /*[69][141]*/   VAL_|SCLR|PLAI, 214,6,/*sval00*/
        /*[70][144]*/   PEX2,0,0,
        /*[73][147]*/   VAL_|SCLR|PLAI, 235,6,/*sval01*/
        /*[74][150]*/ ESEQ|PEX2,
        /*[75][151]*/ KEY_|SCLR|PLAI, 244,4,/*seq1*/
        /*[76][154]*/ VAL_|BSEQ|PEX2,
        /*[77][155]*/   VAL_|SCLR|PLAI, 256,6,/*sval10*/
        /*[78][158]*/   PEX2,0,0,
        /*[81][161]*/   VAL_|SCLR|PLAI, 277,6,/*sval11*/
        /*[82][164]*/ ESEQ|PEX2,
        /*[83][165]*/ KEY_|SCLR|PLAI, 235,6,/*sval01*/
        /*[84][168]*/ VAL_|SCLR|PLAI|PEX2, 277,6,/*sval11*/
        /*[85][171]*/ PEX2,
    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 11_inherit_merge_2_have_to_move_forward_seqs)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n  key0: [val00,val00,val00,val00]\n  key1: [val10,val10,val10]\n  key2: [val20,val20,val20]\n"
        "map1: &map1\n  key11: [val11,val11,val11,val11]\n  key1: [val11,val11,val11]\n  key12: [val12,val12,val12]\n"
        "map2: &map2\n  key2: [val22,val22,val22,val22]\n  key1: [val21,val21,val21]\n  key22: [val22,val22,val22]\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n" // causes move fwd
        "  seq0:\n    - sval00\n    - &sval01 sval01\n"
        "  seq1:\n    - sval10\n    - &sval11 sval11\n"
        "  *sval01 : *sval11\n"
        "";
    std::string resolved = ""
        "map0:\n  key0: [val00,val00,val00,val00]\n  key1: [val10,val10,val10]\n  key2: [val20,val20,val20]\n"
        "map1:\n  key11: [val11,val11,val11,val11]\n  key1: [val11,val11,val11]\n  key12: [val12,val12,val12]\n"
        "map2:\n  key2: [val22,val22,val22,val22]\n  key1: [val21,val21,val21]\n  key22: [val22,val22,val22]\n"
        "inherit:\n"
        "  key0: [val00,val00,val00,val00]\n"
        "  key1: [val10,val10,val10]\n"
        "  key2: [val20,val20,val20]\n"
        "  key11: [val11,val11,val11,val11]\n"
        "  key12: [val12,val12,val12]\n"
        "  key22: [val22,val22,val22]\n"
        "  seq0:\n    - sval00\n    - sval01\n"
        "  seq1:\n    - sval10\n    - sval11\n"
        "  sval01: sval11\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": [\"val00\",\"val00\",\"val00\",\"val00\"],\n"
        "    \"key1\": [\"val10\",\"val10\",\"val10\"],\n"
        "    \"key2\": [\"val20\",\"val20\",\"val20\"]\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key11\": [\"val11\",\"val11\",\"val11\",\"val11\"],\n"
        "    \"key1\": [\"val11\",\"val11\",\"val11\"],\n"
        "    \"key12\": [\"val12\",\"val12\",\"val12\"]\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": [\"val22\",\"val22\",\"val22\",\"val22\"],\n"
        "    \"key1\": [\"val21\",\"val21\",\"val21\"],\n"
        "    \"key22\": [\"val22\",\"val22\",\"val22\"]\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"],\n"
        "    \"seq0\": [\n"
        "      \"sval00\",\n"
        "      \"sval01\"\n"
        "    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"*sval01\": \"*sval11\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": [\"val00\",\"val00\",\"val00\",\"val00\"],\n"
        "    \"key1\": [\"val10\",\"val10\",\"val10\"],\n"
        "    \"key2\": [\"val20\",\"val20\",\"val20\"]\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key11\": [\"val11\",\"val11\",\"val11\",\"val11\"],\n"
        "    \"key1\": [\"val11\",\"val11\",\"val11\"],\n"
        "    \"key12\": [\"val12\",\"val12\",\"val12\"]\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": [\"val22\",\"val22\",\"val22\",\"val22\"],\n"
        "    \"key1\": [\"val21\",\"val21\",\"val21\"],\n"
        "    \"key22\": [\"val22\",\"val22\",\"val22\"]\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": [\"val00\",\"val00\",\"val00\",\"val00\"],\n"
        "    \"key1\": [\"val10\",\"val10\",\"val10\"],\n"
        "    \"key2\": [\"val20\",\"val20\",\"val20\"],\n"
        "    \"key11\": [\"val11\",\"val11\",\"val11\",\"val11\"],\n"
        "    \"key12\": [\"val12\",\"val12\",\"val12\"],\n"
        "    \"key22\": [\"val22\",\"val22\",\"val22\"],\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"sval01\": \"sval11\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {

        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[8][14]*/           VAL_|SCLR|PLAI, 21,5,/*val00*/
        /*[9][17]*/           VAL_|SCLR|PLAI|PEX2, 27,5,/*val00*/
        /*[10][20]*/           VAL_|SCLR|PLAI|PEX2, 33,5,/*val00*/
        /*[11][23]*/           VAL_|SCLR|PLAI|PEX2, 39,5,/*val00*/
        /*[12][26]*/         ESEQ|PEX2,
        /*[13][27]*/         KEY_|SCLR|PLAI, 48,4,/*key1*/
        /*[14][30]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[15][31]*/           VAL_|SCLR|PLAI, 55,5,/*val10*/
        /*[16][34]*/           VAL_|SCLR|PLAI|PEX2, 61,5,/*val10*/
        /*[17][37]*/           VAL_|SCLR|PLAI|PEX2, 67,5,/*val10*/
        /*[18][40]*/         ESEQ|PEX2,
        /*[19][41]*/         KEY_|SCLR|PLAI, 76,4,/*key2*/
        /*[20][44]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[21][45]*/           VAL_|SCLR|PLAI, 83,5,/*val20*/
        /*[22][48]*/           VAL_|SCLR|PLAI|PEX2, 89,5,/*val20*/
        /*[23][51]*/           VAL_|SCLR|PLAI|PEX2, 95,5,/*val20*/
        /*[24][54]*/         ESEQ|PEX2,
        /*[25][55]*/       EMAP,
        /*[26][56]*/       KEY_|SCLR|PLAI, 102,4,/*map1*/
        /*[27][59]*/       VAL_|ANCH|PEX2, 109,4,/*map1*/
        /*[28][62]*/       VAL_|BMAP|PEX2,
        /*[29][63]*/         KEY_|SCLR|PLAI, 116,5,/*key11*/
        /*[30][66]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[31][67]*/           VAL_|SCLR|PLAI, 124,5,/*val11*/
        /*[32][70]*/           VAL_|SCLR|PLAI|PEX2, 130,5,/*val11*/
        /*[33][73]*/           VAL_|SCLR|PLAI|PEX2, 136,5,/*val11*/
        /*[34][76]*/           VAL_|SCLR|PLAI|PEX2, 142,5,/*val11*/
        /*[35][79]*/         ESEQ|PEX2,
        /*[36][80]*/         KEY_|SCLR|PLAI, 151,4,/*key1*/
        /*[37][83]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[38][84]*/           VAL_|SCLR|PLAI, 158,5,/*val11*/
        /*[39][87]*/           VAL_|SCLR|PLAI|PEX2, 164,5,/*val11*/
        /*[40][90]*/           VAL_|SCLR|PLAI|PEX2, 170,5,/*val11*/
        /*[41][93]*/         ESEQ|PEX2,
        /*[42][94]*/         KEY_|SCLR|PLAI, 179,5,/*key12*/
        /*[43][97]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[44][98]*/           VAL_|SCLR|PLAI, 187,5,/*val12*/
        /*[45][101]*/           VAL_|SCLR|PLAI|PEX2, 193,5,/*val12*/
        /*[46][104]*/           VAL_|SCLR|PLAI|PEX2, 199,5,/*val12*/
        /*[47][107]*/         ESEQ|PEX2,
        /*[48][108]*/       EMAP,
        /*[49][109]*/       KEY_|SCLR|PLAI, 206,4,/*map2*/
        /*[50][112]*/       VAL_|ANCH|PEX2, 213,4,/*map2*/
        /*[51][115]*/       VAL_|BMAP|PEX2,
        /*[52][116]*/         KEY_|SCLR|PLAI, 220,4,/*key2*/
        /*[53][119]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[54][120]*/           VAL_|SCLR|PLAI, 227,5,/*val22*/
        /*[55][123]*/           VAL_|SCLR|PLAI|PEX2, 233,5,/*val22*/
        /*[56][126]*/           VAL_|SCLR|PLAI|PEX2, 239,5,/*val22*/
        /*[57][129]*/           VAL_|SCLR|PLAI|PEX2, 245,5,/*val22*/
        /*[58][132]*/         ESEQ|PEX2,
        /*[59][133]*/         KEY_|SCLR|PLAI, 254,4,/*key1*/
        /*[60][136]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[61][137]*/           VAL_|SCLR|PLAI, 261,5,/*val21*/
        /*[62][140]*/           VAL_|SCLR|PLAI|PEX2, 267,5,/*val21*/
        /*[63][143]*/           VAL_|SCLR|PLAI|PEX2, 273,5,/*val21*/
        /*[64][146]*/         ESEQ|PEX2,
        /*[65][147]*/         KEY_|SCLR|PLAI, 282,5,/*key22*/
        /*[66][150]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[67][151]*/           VAL_|SCLR|PLAI, 290,5,/*val22*/
        /*[68][154]*/           VAL_|SCLR|PLAI|PEX2, 296,5,/*val22*/
        /*[69][157]*/           VAL_|SCLR|PLAI|PEX2, 302,5,/*val22*/
        /*[70][160]*/         ESEQ|PEX2,
        /*[71][161]*/       EMAP,
        /*[72][162]*/       KEY_|SCLR|PLAI, 309,7,/*inherit*/
        /*[73][165]*/       VAL_|BMAP|PEX2,
        /*[74][166]*/         KEY_|SCLR|PLAI, 320,2,/*<<*/
        /*[75][169]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[76][170]*/           VAL_|ALIA, 326,4,/*map0*/
        /*[77][173]*/           VAL_|ALIA|PEX2, 332,4,/*map1*/
        /*[78][176]*/           VAL_|ALIA|PEX2, 338,4,/*map2*/
        /*[79][179]*/         ESEQ|PEX2,
        /*[80][180]*/         KEY_|SCLR|PLAI, 346,4,/*seq0*/
        /*[81][183]*/         VAL_|BSEQ|PEX2,
        /*[82][184]*/           VAL_|SCLR|PLAI, 358,6,/*sval00*/
        /*[83][187]*/           VAL_|ANCH|PEX2, 372,6,/*sval01*/
        /*[84][190]*/           VAL_|SCLR|PLAI|PEX2, 379,6,/*sval01*/
        /*[85][193]*/         ESEQ|PEX2,
        /*[86][194]*/         KEY_|SCLR|PLAI, 388,4,/*seq1*/
        /*[87][197]*/         VAL_|BSEQ|PEX2,
        /*[88][198]*/           VAL_|SCLR|PLAI, 400,6,/*sval10*/
        /*[89][201]*/           VAL_|ANCH|PEX2, 414,6,/*sval11*/
        /*[90][204]*/           VAL_|SCLR|PLAI|PEX2, 421,6,/*sval11*/
        /*[91][207]*/         ESEQ|PEX2,
        /*[92][208]*/         KEY_|ALIA, 431,6,/*sval01*/
        /*[93][211]*/         VAL_|ALIA|PEX2, 441,6,/*sval11*/
        /*[94][214]*/       EMAP|PEX2,
        /*[95][215]*/     EMAP,
        /*[96][216]*/   EDOC,
        /*[97][217]*/ ESTR,

    };
    const evt_bits evts_resolved[] = {

        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,0,0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[10][14]*/           VAL_|SCLR|PLAI, 21,5,/*val00*/
        /*[11][17]*/           VAL_|SCLR|PLAI|PEX2, 27,5,/*val00*/
        /*[12][20]*/           VAL_|SCLR|PLAI|PEX2, 33,5,/*val00*/
        /*[13][23]*/           VAL_|SCLR|PLAI|PEX2, 39,5,/*val00*/
        /*[14][26]*/         ESEQ|PEX2,
        /*[15][27]*/         KEY_|SCLR|PLAI, 48,4,/*key1*/
        /*[16][30]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[17][31]*/           VAL_|SCLR|PLAI, 55,5,/*val10*/
        /*[18][34]*/           VAL_|SCLR|PLAI|PEX2, 61,5,/*val10*/
        /*[19][37]*/           VAL_|SCLR|PLAI|PEX2, 67,5,/*val10*/
        /*[20][40]*/         ESEQ|PEX2,
        /*[21][41]*/         KEY_|SCLR|PLAI, 76,4,/*key2*/
        /*[22][44]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[23][45]*/           VAL_|SCLR|PLAI, 83,5,/*val20*/
        /*[24][48]*/           VAL_|SCLR|PLAI|PEX2, 89,5,/*val20*/
        /*[25][51]*/           VAL_|SCLR|PLAI|PEX2, 95,5,/*val20*/
        /*[26][54]*/         ESEQ|PEX2,
        /*[27][55]*/       EMAP,
        /*[28][56]*/       KEY_|SCLR|PLAI, 102,4,/*map1*/
        /*[29][59]*/       PEX2,0,0,
        /*[32][62]*/       VAL_|BMAP,
        /*[33][63]*/         KEY_|SCLR|PLAI, 116,5,/*key11*/
        /*[34][66]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[35][67]*/           VAL_|SCLR|PLAI, 124,5,/*val11*/
        /*[36][70]*/           VAL_|SCLR|PLAI|PEX2, 130,5,/*val11*/
        /*[37][73]*/           VAL_|SCLR|PLAI|PEX2, 136,5,/*val11*/
        /*[38][76]*/           VAL_|SCLR|PLAI|PEX2, 142,5,/*val11*/
        /*[39][79]*/         ESEQ|PEX2,
        /*[40][80]*/         KEY_|SCLR|PLAI, 151,4,/*key1*/
        /*[41][83]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[42][84]*/           VAL_|SCLR|PLAI, 158,5,/*val11*/
        /*[43][87]*/           VAL_|SCLR|PLAI|PEX2, 164,5,/*val11*/
        /*[44][90]*/           VAL_|SCLR|PLAI|PEX2, 170,5,/*val11*/
        /*[45][93]*/         ESEQ|PEX2,
        /*[46][94]*/         KEY_|SCLR|PLAI, 179,5,/*key12*/
        /*[47][97]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[48][98]*/           VAL_|SCLR|PLAI, 187,5,/*val12*/
        /*[49][101]*/           VAL_|SCLR|PLAI|PEX2, 193,5,/*val12*/
        /*[50][104]*/           VAL_|SCLR|PLAI|PEX2, 199,5,/*val12*/
        /*[51][107]*/         ESEQ|PEX2,
        /*[52][108]*/       EMAP,
        /*[53][109]*/       KEY_|SCLR|PLAI, 206,4,/*map2*/
        /*[54][112]*/       PEX2,0,0,
        /*[57][115]*/       VAL_|BMAP,
        /*[58][116]*/         KEY_|SCLR|PLAI, 220,4,/*key2*/
        /*[59][119]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[60][120]*/           VAL_|SCLR|PLAI, 227,5,/*val22*/
        /*[61][123]*/           VAL_|SCLR|PLAI|PEX2, 233,5,/*val22*/
        /*[62][126]*/           VAL_|SCLR|PLAI|PEX2, 239,5,/*val22*/
        /*[63][129]*/           VAL_|SCLR|PLAI|PEX2, 245,5,/*val22*/
        /*[64][132]*/         ESEQ|PEX2,
        /*[65][133]*/         KEY_|SCLR|PLAI, 254,4,/*key1*/
        /*[66][136]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[67][137]*/           VAL_|SCLR|PLAI, 261,5,/*val21*/
        /*[68][140]*/           VAL_|SCLR|PLAI|PEX2, 267,5,/*val21*/
        /*[69][143]*/           VAL_|SCLR|PLAI|PEX2, 273,5,/*val21*/
        /*[70][146]*/         ESEQ|PEX2,
        /*[71][147]*/         KEY_|SCLR|PLAI, 282,5,/*key22*/
        /*[72][150]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[73][151]*/           VAL_|SCLR|PLAI, 290,5,/*val22*/
        /*[74][154]*/           VAL_|SCLR|PLAI|PEX2, 296,5,/*val22*/
        /*[75][157]*/           VAL_|SCLR|PLAI|PEX2, 302,5,/*val22*/
        /*[76][160]*/         ESEQ|PEX2,
        /*[77][161]*/       EMAP,
        /*[78][162]*/       KEY_|SCLR|PLAI, 309,7,/*inherit*/
        /*[79][165]*/       VAL_|BMAP|PEX2,
        /*[80][166]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[81][169]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[82][170]*/           VAL_|SCLR|PLAI, 21,5,/*val00*/
        /*[83][173]*/           VAL_|SCLR|PLAI|PEX2, 27,5,/*val00*/
        /*[84][176]*/           VAL_|SCLR|PLAI|PEX2, 33,5,/*val00*/
        /*[85][179]*/           VAL_|SCLR|PLAI|PEX2, 39,5,/*val00*/
        /*[86][182]*/         ESEQ|PEX2,
        /*[87][183]*/         KEY_|SCLR|PLAI, 48,4,/*key1*/
        /*[88][186]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[89][187]*/           VAL_|SCLR|PLAI, 55,5,/*val10*/
        /*[90][190]*/           VAL_|SCLR|PLAI|PEX2, 61,5,/*val10*/
        /*[91][193]*/           VAL_|SCLR|PLAI|PEX2, 67,5,/*val10*/
        /*[92][196]*/         ESEQ|PEX2,
        /*[93][197]*/         RREF,218,311,
        /*[94][200]*/         PEX2,0,0,0,0,0,0,0,0,0,0,0,0,0,
        /*[108][214]*/       EMAP,
        /*[109][215]*/     EMAP,
        /*[110][216]*/   EDOC,
        /*[111][217]*/ ESTR,
        /*[112][218]*/ KEY_|SCLR|PLAI, 76,4,/*key2*/
        /*[113][221]*/ VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[114][222]*/   VAL_|SCLR|PLAI, 83,5,/*val20*/
        /*[115][225]*/   VAL_|SCLR|PLAI|PEX2, 89,5,/*val20*/
        /*[116][228]*/   VAL_|SCLR|PLAI|PEX2, 95,5,/*val20*/
        /*[117][231]*/ ESEQ|PEX2,
        /*[118][232]*/ KEY_|SCLR|PLAI, 116,5,/*key11*/
        /*[119][235]*/ VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[120][236]*/   VAL_|SCLR|PLAI, 124,5,/*val11*/
        /*[121][239]*/   VAL_|SCLR|PLAI|PEX2, 130,5,/*val11*/
        /*[122][242]*/   VAL_|SCLR|PLAI|PEX2, 136,5,/*val11*/
        /*[123][245]*/   VAL_|SCLR|PLAI|PEX2, 142,5,/*val11*/
        /*[124][248]*/ ESEQ|PEX2,
        /*[125][249]*/ KEY_|SCLR|PLAI, 179,5,/*key12*/
        /*[126][252]*/ VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[127][253]*/   VAL_|SCLR|PLAI, 187,5,/*val12*/
        /*[128][256]*/   VAL_|SCLR|PLAI|PEX2, 193,5,/*val12*/
        /*[129][259]*/   VAL_|SCLR|PLAI|PEX2, 199,5,/*val12*/
        /*[130][262]*/ ESEQ|PEX2,
        /*[131][263]*/ KEY_|SCLR|PLAI, 282,5,/*key22*/
        /*[132][266]*/ VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[133][267]*/   VAL_|SCLR|PLAI, 290,5,/*val22*/
        /*[134][270]*/   VAL_|SCLR|PLAI|PEX2, 296,5,/*val22*/
        /*[135][273]*/   VAL_|SCLR|PLAI|PEX2, 302,5,/*val22*/
        /*[136][276]*/ ESEQ|PEX2,
        /*[137][277]*/ KEY_|SCLR|PLAI, 346,4,/*seq0*/
        /*[138][280]*/ VAL_|BSEQ|PEX2,
        /*[139][281]*/   VAL_|SCLR|PLAI, 358,6,/*sval00*/
        /*[140][284]*/   PEX2,0,0,
        /*[143][287]*/   VAL_|SCLR|PLAI, 379,6,/*sval01*/
        /*[144][290]*/ ESEQ|PEX2,
        /*[145][291]*/ KEY_|SCLR|PLAI, 388,4,/*seq1*/
        /*[146][294]*/ VAL_|BSEQ|PEX2,
        /*[147][295]*/   VAL_|SCLR|PLAI, 400,6,/*sval10*/
        /*[148][298]*/   PEX2,0,0,
        /*[151][301]*/   VAL_|SCLR|PLAI, 421,6,/*sval11*/
        /*[152][304]*/ ESEQ|PEX2,
        /*[153][305]*/ KEY_|SCLR|PLAI, 379,6,/*sval01*/
        /*[154][308]*/ VAL_|SCLR|PLAI|PEX2, 421,6,/*sval11*/
        /*[155][311]*/ PEX2,

    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}

TEST(ints_resolve, 11_inherit_merge_2_have_to_move_forward_maps)
{
    using namespace ievt;
    std::string src = ""
        "map0: &map0\n  key0: {val00: val00,val00: val00}\n  key1: {val10: val10}\n  key2: {val20: val20}\n"
        "map1: &map1\n  key11: {val11: val11,val11: val11}\n  key1: {val11: val11}\n  key12: {val12: val12}\n"
        "map2: &map2\n  key2: {val22: val22,val22: val22}\n  key1: {val21: val21}\n  key22: {val22: val22}\n"
        "inherit:\n"
        "  <<: [*map0,*map1,*map2]\n" // causes move fwd
        "  seq0:\n    - sval00\n    - &sval01 sval01\n"
        "  seq1:\n    - sval10\n    - &sval11 sval11\n"
        "  *sval01 : *sval11\n"
        "";
    std::string resolved = ""
        "map0:\n  key0: {val00: val00,val00: val00}\n  key1: {val10: val10}\n  key2: {val20: val20}\n"
        "map1:\n  key11: {val11: val11,val11: val11}\n  key1: {val11: val11}\n  key12: {val12: val12}\n"
        "map2:\n  key2: {val22: val22,val22: val22}\n  key1: {val21: val21}\n  key22: {val22: val22}\n"
        "inherit:\n"
        "  key0: {val00: val00,val00: val00}\n"
        "  key1: {val10: val10}\n"
        "  key2: {val20: val20}\n"
        "  key11: {val11: val11,val11: val11}\n"
        "  key12: {val12: val12}\n"
        "  key22: {val22: val22}\n"
        "  seq0:\n    - sval00\n    - sval01\n"
        "  seq1:\n    - sval10\n    - sval11\n"
        "  sval01: sval11\n"
        "";
    std::string json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": {\"val00\": \"val00\",\"val00\": \"val00\"},\n"
        "    \"key1\": {\"val10\": \"val10\"},\n"
        "    \"key2\": {\"val20\": \"val20\"}\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key11\": {\"val11\": \"val11\",\"val11\": \"val11\"},\n"
        "    \"key1\": {\"val11\": \"val11\"},\n"
        "    \"key12\": {\"val12\": \"val12\"}\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": {\"val22\": \"val22\",\"val22\": \"val22\"},\n"
        "    \"key1\": {\"val21\": \"val21\"},\n"
        "    \"key22\": {\"val22\": \"val22\"}\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"<<\": [\"*map0\",\"*map1\",\"*map2\"],\n"
        "    \"seq0\": [\n"
        "      \"sval00\",\n"
        "      \"sval01\"\n"
        "    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"*sval01\": \"*sval11\"\n"
        "  }\n"
        "}\n"
        "";
    std::string resolved_json = ""
        "{\n"
        "  \"map0\": {\n"
        "    \"key0\": {\"val00\": \"val00\",\"val00\": \"val00\"},\n"
        "    \"key1\": {\"val10\": \"val10\"},\n"
        "    \"key2\": {\"val20\": \"val20\"}\n"
        "  },\n"
        "  \"map1\": {\n"
        "    \"key11\": {\"val11\": \"val11\",\"val11\": \"val11\"},\n"
        "    \"key1\": {\"val11\": \"val11\"},\n"
        "    \"key12\": {\"val12\": \"val12\"}\n"
        "  },\n"
        "  \"map2\": {\n"
        "    \"key2\": {\"val22\": \"val22\",\"val22\": \"val22\"},\n"
        "    \"key1\": {\"val21\": \"val21\"},\n"
        "    \"key22\": {\"val22\": \"val22\"}\n"
        "  },\n"
        "  \"inherit\": {\n"
        "    \"key0\": {\"val00\": \"val00\",\"val00\": \"val00\"},\n"
        "    \"key1\": {\"val10\": \"val10\"},\n"
        "    \"key2\": {\"val20\": \"val20\"},\n"
        "    \"key11\": {\"val11\": \"val11\",\"val11\": \"val11\"},\n"
        "    \"key12\": {\"val12\": \"val12\"},\n"
        "    \"key22\": {\"val22\": \"val22\"},\n"
        "    \"seq0\": [\n      \"sval00\",\n      \"sval01\"\n    ],\n"
        "    \"seq1\": [\n      \"sval10\",\n      \"sval11\"\n    ],\n"
        "    \"sval01\": \"sval11\"\n"
        "  }\n"
        "}\n"
        "";
    const evt_bits evts_expected[] = {

        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       VAL_|ANCH|PEX2, 7,4,/*map0*/
        /*[5][9]*/       VAL_|BMAP|PEX2,
        /*[6][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[7][13]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[8][14]*/           KEY_|SCLR|PLAI, 21,5,/*val00*/
        /*[9][17]*/           VAL_|SCLR|PLAI|PEX2, 28,5,/*val00*/
        /*[10][20]*/           KEY_|SCLR|PLAI|PEX2, 34,5,/*val00*/
        /*[11][23]*/           VAL_|SCLR|PLAI|PEX2, 41,5,/*val00*/
        /*[12][26]*/         EMAP|PEX2,
        /*[13][27]*/         KEY_|SCLR|PLAI, 50,4,/*key1*/
        /*[14][30]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[15][31]*/           KEY_|SCLR|PLAI, 57,5,/*val10*/
        /*[16][34]*/           VAL_|SCLR|PLAI|PEX2, 64,5,/*val10*/
        /*[17][37]*/         EMAP|PEX2,
        /*[18][38]*/         KEY_|SCLR|PLAI, 73,4,/*key2*/
        /*[19][41]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[20][42]*/           KEY_|SCLR|PLAI, 80,5,/*val20*/
        /*[21][45]*/           VAL_|SCLR|PLAI|PEX2, 87,5,/*val20*/
        /*[22][48]*/         EMAP|PEX2,
        /*[23][49]*/       EMAP,
        /*[24][50]*/       KEY_|SCLR|PLAI, 94,4,/*map1*/
        /*[25][53]*/       VAL_|ANCH|PEX2, 101,4,/*map1*/
        /*[26][56]*/       VAL_|BMAP|PEX2,
        /*[27][57]*/         KEY_|SCLR|PLAI, 108,5,/*key11*/
        /*[28][60]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[29][61]*/           KEY_|SCLR|PLAI, 116,5,/*val11*/
        /*[30][64]*/           VAL_|SCLR|PLAI|PEX2, 123,5,/*val11*/
        /*[31][67]*/           KEY_|SCLR|PLAI|PEX2, 129,5,/*val11*/
        /*[32][70]*/           VAL_|SCLR|PLAI|PEX2, 136,5,/*val11*/
        /*[33][73]*/         EMAP|PEX2,
        /*[34][74]*/         KEY_|SCLR|PLAI, 145,4,/*key1*/
        /*[35][77]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[36][78]*/           KEY_|SCLR|PLAI, 152,5,/*val11*/
        /*[37][81]*/           VAL_|SCLR|PLAI|PEX2, 159,5,/*val11*/
        /*[38][84]*/         EMAP|PEX2,
        /*[39][85]*/         KEY_|SCLR|PLAI, 168,5,/*key12*/
        /*[40][88]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[41][89]*/           KEY_|SCLR|PLAI, 176,5,/*val12*/
        /*[42][92]*/           VAL_|SCLR|PLAI|PEX2, 183,5,/*val12*/
        /*[43][95]*/         EMAP|PEX2,
        /*[44][96]*/       EMAP,
        /*[45][97]*/       KEY_|SCLR|PLAI, 190,4,/*map2*/
        /*[46][100]*/       VAL_|ANCH|PEX2, 197,4,/*map2*/
        /*[47][103]*/       VAL_|BMAP|PEX2,
        /*[48][104]*/         KEY_|SCLR|PLAI, 204,4,/*key2*/
        /*[49][107]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[50][108]*/           KEY_|SCLR|PLAI, 211,5,/*val22*/
        /*[51][111]*/           VAL_|SCLR|PLAI|PEX2, 218,5,/*val22*/
        /*[52][114]*/           KEY_|SCLR|PLAI|PEX2, 224,5,/*val22*/
        /*[53][117]*/           VAL_|SCLR|PLAI|PEX2, 231,5,/*val22*/
        /*[54][120]*/         EMAP|PEX2,
        /*[55][121]*/         KEY_|SCLR|PLAI, 240,4,/*key1*/
        /*[56][124]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[57][125]*/           KEY_|SCLR|PLAI, 247,5,/*val21*/
        /*[58][128]*/           VAL_|SCLR|PLAI|PEX2, 254,5,/*val21*/
        /*[59][131]*/         EMAP|PEX2,
        /*[60][132]*/         KEY_|SCLR|PLAI, 263,5,/*key22*/
        /*[61][135]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[62][136]*/           KEY_|SCLR|PLAI, 271,5,/*val22*/
        /*[63][139]*/           VAL_|SCLR|PLAI|PEX2, 278,5,/*val22*/
        /*[64][142]*/         EMAP|PEX2,
        /*[65][143]*/       EMAP,
        /*[66][144]*/       KEY_|SCLR|PLAI, 285,7,/*inherit*/
        /*[67][147]*/       VAL_|BMAP|PEX2,
        /*[68][148]*/         KEY_|SCLR|PLAI, 296,2,/*<<*/
        /*[69][151]*/         VAL_|BSEQ|FLOW|FSL_|PEX2,
        /*[70][152]*/           VAL_|ALIA, 302,4,/*map0*/
        /*[71][155]*/           VAL_|ALIA|PEX2, 308,4,/*map1*/
        /*[72][158]*/           VAL_|ALIA|PEX2, 314,4,/*map2*/
        /*[73][161]*/         ESEQ|PEX2,
        /*[74][162]*/         KEY_|SCLR|PLAI, 322,4,/*seq0*/
        /*[75][165]*/         VAL_|BSEQ|PEX2,
        /*[76][166]*/           VAL_|SCLR|PLAI, 334,6,/*sval00*/
        /*[77][169]*/           VAL_|ANCH|PEX2, 348,6,/*sval01*/
        /*[78][172]*/           VAL_|SCLR|PLAI|PEX2, 355,6,/*sval01*/
        /*[79][175]*/         ESEQ|PEX2,
        /*[80][176]*/         KEY_|SCLR|PLAI, 364,4,/*seq1*/
        /*[81][179]*/         VAL_|BSEQ|PEX2,
        /*[82][180]*/           VAL_|SCLR|PLAI, 376,6,/*sval10*/
        /*[83][183]*/           VAL_|ANCH|PEX2, 390,6,/*sval11*/
        /*[84][186]*/           VAL_|SCLR|PLAI|PEX2, 397,6,/*sval11*/
        /*[85][189]*/         ESEQ|PEX2,
        /*[86][190]*/         KEY_|ALIA, 407,6,/*sval01*/
        /*[87][193]*/         VAL_|ALIA|PEX2, 417,6,/*sval11*/
        /*[88][196]*/       EMAP|PEX2,
        /*[89][197]*/     EMAP,
        /*[90][198]*/   EDOC,
        /*[91][199]*/ ESTR,

    };
    const evt_bits evts_resolved[] = {

        /*[0][0]*/ BSTR,
        /*[1][1]*/   BDOC,
        /*[2][2]*/     VAL_|BMAP,
        /*[3][3]*/       KEY_|SCLR|PLAI, 0,4,/*map0*/
        /*[4][6]*/       PEX2,0,0,
        /*[7][9]*/       VAL_|BMAP,
        /*[8][10]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[9][13]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[10][14]*/           KEY_|SCLR|PLAI, 21,5,/*val00*/
        /*[11][17]*/           VAL_|SCLR|PLAI|PEX2, 28,5,/*val00*/
        /*[12][20]*/           KEY_|SCLR|PLAI|PEX2, 34,5,/*val00*/
        /*[13][23]*/           VAL_|SCLR|PLAI|PEX2, 41,5,/*val00*/
        /*[14][26]*/         EMAP|PEX2,
        /*[15][27]*/         KEY_|SCLR|PLAI, 50,4,/*key1*/
        /*[16][30]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[17][31]*/           KEY_|SCLR|PLAI, 57,5,/*val10*/
        /*[18][34]*/           VAL_|SCLR|PLAI|PEX2, 64,5,/*val10*/
        /*[19][37]*/         EMAP|PEX2,
        /*[20][38]*/         KEY_|SCLR|PLAI, 73,4,/*key2*/
        /*[21][41]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[22][42]*/           KEY_|SCLR|PLAI, 80,5,/*val20*/
        /*[23][45]*/           VAL_|SCLR|PLAI|PEX2, 87,5,/*val20*/
        /*[24][48]*/         EMAP|PEX2,
        /*[25][49]*/       EMAP,
        /*[26][50]*/       KEY_|SCLR|PLAI, 94,4,/*map1*/
        /*[27][53]*/       PEX2,0,0,
        /*[30][56]*/       VAL_|BMAP,
        /*[31][57]*/         KEY_|SCLR|PLAI, 108,5,/*key11*/
        /*[32][60]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[33][61]*/           KEY_|SCLR|PLAI, 116,5,/*val11*/
        /*[34][64]*/           VAL_|SCLR|PLAI|PEX2, 123,5,/*val11*/
        /*[35][67]*/           KEY_|SCLR|PLAI|PEX2, 129,5,/*val11*/
        /*[36][70]*/           VAL_|SCLR|PLAI|PEX2, 136,5,/*val11*/
        /*[37][73]*/         EMAP|PEX2,
        /*[38][74]*/         KEY_|SCLR|PLAI, 145,4,/*key1*/
        /*[39][77]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[40][78]*/           KEY_|SCLR|PLAI, 152,5,/*val11*/
        /*[41][81]*/           VAL_|SCLR|PLAI|PEX2, 159,5,/*val11*/
        /*[42][84]*/         EMAP|PEX2,
        /*[43][85]*/         KEY_|SCLR|PLAI, 168,5,/*key12*/
        /*[44][88]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[45][89]*/           KEY_|SCLR|PLAI, 176,5,/*val12*/
        /*[46][92]*/           VAL_|SCLR|PLAI|PEX2, 183,5,/*val12*/
        /*[47][95]*/         EMAP|PEX2,
        /*[48][96]*/       EMAP,
        /*[49][97]*/       KEY_|SCLR|PLAI, 190,4,/*map2*/
        /*[50][100]*/       PEX2,0,0,
        /*[53][103]*/       VAL_|BMAP,
        /*[54][104]*/         KEY_|SCLR|PLAI, 204,4,/*key2*/
        /*[55][107]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[56][108]*/           KEY_|SCLR|PLAI, 211,5,/*val22*/
        /*[57][111]*/           VAL_|SCLR|PLAI|PEX2, 218,5,/*val22*/
        /*[58][114]*/           KEY_|SCLR|PLAI|PEX2, 224,5,/*val22*/
        /*[59][117]*/           VAL_|SCLR|PLAI|PEX2, 231,5,/*val22*/
        /*[60][120]*/         EMAP|PEX2,
        /*[61][121]*/         KEY_|SCLR|PLAI, 240,4,/*key1*/
        /*[62][124]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[63][125]*/           KEY_|SCLR|PLAI, 247,5,/*val21*/
        /*[64][128]*/           VAL_|SCLR|PLAI|PEX2, 254,5,/*val21*/
        /*[65][131]*/         EMAP|PEX2,
        /*[66][132]*/         KEY_|SCLR|PLAI, 263,5,/*key22*/
        /*[67][135]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[68][136]*/           KEY_|SCLR|PLAI, 271,5,/*val22*/
        /*[69][139]*/           VAL_|SCLR|PLAI|PEX2, 278,5,/*val22*/
        /*[70][142]*/         EMAP|PEX2,
        /*[71][143]*/       EMAP,
        /*[72][144]*/       KEY_|SCLR|PLAI, 285,7,/*inherit*/
        /*[73][147]*/       VAL_|BMAP|PEX2,
        /*[74][148]*/         KEY_|SCLR|PLAI, 14,4,/*key0*/
        /*[75][151]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[76][152]*/           KEY_|SCLR|PLAI, 21,5,/*val00*/
        /*[77][155]*/           VAL_|SCLR|PLAI|PEX2, 28,5,/*val00*/
        /*[78][158]*/           KEY_|SCLR|PLAI|PEX2, 34,5,/*val00*/
        /*[79][161]*/           VAL_|SCLR|PLAI|PEX2, 41,5,/*val00*/
        /*[80][164]*/         EMAP|PEX2,
        /*[81][165]*/         KEY_|SCLR|PLAI, 50,4,/*key1*/
        /*[82][168]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[83][169]*/           KEY_|SCLR|PLAI, 57,5,/*val10*/
        /*[84][172]*/           VAL_|SCLR|PLAI|PEX2, 64,5,/*val10*/
        /*[85][175]*/         EMAP|PEX2,
        /*[86][176]*/         KEY_|SCLR|PLAI, 73,4,/*key2*/
        /*[87][179]*/         VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[88][180]*/           KEY_|SCLR|PLAI, 80,5,/*val20*/
        /*[89][183]*/           VAL_|SCLR|PLAI|PEX2, 87,5,/*val20*/
        /*[90][186]*/         EMAP|PEX2,
        /*[91][187]*/         RREF,200,273,
        /*[92][190]*/         PEX2,0,0,0,0,0,
        /*[98][196]*/       EMAP,
        /*[99][197]*/     EMAP,
        /*[100][198]*/   EDOC,
        /*[101][199]*/ ESTR,
        /*[102][200]*/ KEY_|SCLR|PLAI, 108,5,/*key11*/
        /*[103][203]*/ VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[104][204]*/   KEY_|SCLR|PLAI, 116,5,/*val11*/
        /*[105][207]*/   VAL_|SCLR|PLAI|PEX2, 123,5,/*val11*/
        /*[106][210]*/   KEY_|SCLR|PLAI|PEX2, 129,5,/*val11*/
        /*[107][213]*/   VAL_|SCLR|PLAI|PEX2, 136,5,/*val11*/
        /*[108][216]*/ EMAP|PEX2,
        /*[109][217]*/ KEY_|SCLR|PLAI, 168,5,/*key12*/
        /*[110][220]*/ VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[111][221]*/   KEY_|SCLR|PLAI, 176,5,/*val12*/
        /*[112][224]*/   VAL_|SCLR|PLAI|PEX2, 183,5,/*val12*/
        /*[113][227]*/ EMAP|PEX2,
        /*[114][228]*/ KEY_|SCLR|PLAI, 263,5,/*key22*/
        /*[115][231]*/ VAL_|BMAP|FLOW|FSL_|PEX2,
        /*[116][232]*/   KEY_|SCLR|PLAI, 271,5,/*val22*/
        /*[117][235]*/   VAL_|SCLR|PLAI|PEX2, 278,5,/*val22*/
        /*[118][238]*/ EMAP|PEX2,
        /*[119][239]*/ KEY_|SCLR|PLAI, 322,4,/*seq0*/
        /*[120][242]*/ VAL_|BSEQ|PEX2,
        /*[121][243]*/   VAL_|SCLR|PLAI, 334,6,/*sval00*/
        /*[122][246]*/   PEX2,0,0,
        /*[125][249]*/   VAL_|SCLR|PLAI, 355,6,/*sval01*/
        /*[126][252]*/ ESEQ|PEX2,
        /*[127][253]*/ KEY_|SCLR|PLAI, 364,4,/*seq1*/
        /*[128][256]*/ VAL_|BSEQ|PEX2,
        /*[129][257]*/   VAL_|SCLR|PLAI, 376,6,/*sval10*/
        /*[130][260]*/   PEX2,0,0,
        /*[133][263]*/   VAL_|SCLR|PLAI, 397,6,/*sval11*/
        /*[134][266]*/ ESEQ|PEX2,
        /*[135][267]*/ KEY_|SCLR|PLAI, 355,6,/*sval01*/
        /*[136][270]*/ VAL_|SCLR|PLAI|PEX2, 397,6,/*sval11*/
        /*[137][273]*/ PEX2,

    };
    test_ints_resolve(src, src, json, resolved, resolved_json, evts_expected, evts_resolved);
}


} // namespace extra


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// The other test executables are written to contain the declarative-style
// YmlTestCases. This executable does not have any but the build setup
// assumes it does, and links with the test lib, which requires an existing
// get_case() function. So this is here to act as placeholder until (if?)
// proper test cases are added here. This was detected in #47 (thanks
// @cburgard).
Case const* get_case(csubstr)
{
    return nullptr;
}

} // namespace yml
} // namespace c4

// NOLINTEND(hicpp-signed-bitwise)
